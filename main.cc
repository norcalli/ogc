// void* __gxx_personality_v0;
// void (*__cxa_terminate_handler)();
// void (*__cxa_unexpected_handler)();az
#include <cmath>
#include <cstdio>
#include <cstdlib> // for std::exit
#include <cassert>
// #include <list>
#include <fstream>
#include <vector>

#include <iostream>
#include <complex>
#include "print-utils.h"
#include "print.h"
#include "units.h"
#include "unit-functions.h"
#include "units-common.h"

typedef std::complex<double> complex_type;
typedef Units<std::complex<double> > unit_type;
typedef unit_type base_type;

#include "expression.h"
#include "arguments.h"
#include "shunting-yard.h"
#include "base/scoped_ptr.h"

using namespace std;

// #define EXPRESSION
#define EXPERIMENTAL


template<class T>
class ExpressionCalculator : public ReversePolish<Expression<T>*> {
 public:
  typedef typename Expression<T>::type type;

  void HandleValue(const type& value) {
    this->PushValue(new NumberExpr<T>(value));
  }

  void HandleConstant(const std::string& name, const type& value) {
    this->PushValue(new ConstantExpr<T>(name, value));
  }
};

template<class T>
struct Constant {
  const char* display_name;
  T           value;
};

static const double kE = exp(1);
static const double kPi = atan(1)*4;
static const double kPhi = (sqrt(5) + 1)/2;

// #ifndef DEBUG
// #endif  // DEBUG


// template<class T>
// inline T modulo(const T& a, const T& b) {
//   return a % b;
// }

// template<>
// inline double modulo<double>(const double& a, const double& b) {
//   return fmod(a, b);
// }

// TODO: I could define a fixed number of defines that support
// up to `N` function arguments, or just make functions with
// more than 2 arguments accept an array, and let them deliberate.
// #define FIXED_NARY
template<class T, int n>
struct NaryFunction { typedef T (*type)(const std::vector<T>&); };

template<class T>
struct NaryFunction<T, 1> { typedef T (*type)(const T&); };

template<class T>
struct NaryFunction<T, 2> { typedef T (*type)(const T&, const T&); };

//template<class T>
//inline T combination(T n, T r) {
//  if (r > n) return 0;
//  T result = 1;
//  // r = std::min(r, n - r); // would be useful because combination is symmetric across r
//  r = std::max(r, n-r); // useful only for double's
//  for ( ; r > 0; --r, --n) {
//    // for (T i = 1; i <= r; ++i, --n) {
//    std::cout << "result(" << result << ")\n";
//    result *= n;
//    result /= r;
//    // result /= i;
//  }
//  return result;
//}

//template<>
inline uint64 combination(uint64 n, uint64 r) {
  if (r > n) return 0;
  uint64 result = 1;
  r = std::min(r, n - r);
  for (uint64 i = 1; i <= r; ++i, --n) {
    result *= n;
    result /= i;
  }
  return result;
}

template<class T>
inline T permutation(T n, T r) {
  if (r > n) return 0;
  T result = 1;
  for ( ; r > 0; --r, --n) {
    result *= n;
  }
  return result;
}

template<class T>
inline T factorial(T a) {
  T result = 1;
  for ( ; a > 0; --a) {
    result *= a;
  }
  return result;
}

struct OperationsDouble : Operations<double> {
  inline static type nCr(const type& a, const type& b) { return a*b; }
};

// template<class Table, class T>
// T CallOperator(const Table& operators, const char* n, std::stack<T>& values) {
//   typename Table::const_iterator it = operators.Find(n);
//   if (it == operators.end())
//     throw "Couldn't find that bitch";
//   const Operator& op = it->second;
//   // printf("CallOperator: name: %s\n", op.display_name);
//   printf("CallOperator: name: %s\n", op.name);
//   if (op.argument_count == 1) {
//     // typename Operations<T>::Unary unary = op.function;
//     typename Operations<T>::Unary* unary = op.function;
//     T value = values.top(); values.pop();
//     return unary(value);
//   }
//   if (op.argument_count == 2) {
//     // typename Operations<T>::Binary binary = op.function;
//     typename Operations<T>::Binary* binary = op.function;
//     T right = values.top(); values.pop();
//     T left  = values.top(); values.pop();
//     return binary(left, right);
//   }
//   // typename Operations<T>::Nary nary = op.function;
//   typename Operations<T>::Nary* nary = op.function;
//   std::vector<T> arguments(op.argument_count);
//   for (int i = arguments.size() - 1; i >= 0; --i) {
//     arguments[i] = values.top(); values.pop();
//   }
//   return nary(arguments);
// }

template<class Table, class T>
T CallBinary(const Table& operators, const char* n, const T& l, const T& r) {
  typename NaryFunction<T, 2>::type fn = find(operators, n).function;
  return fn(l, r);
}

inline void PrintIndicator(int indent, size_t pos, size_t length) {
  for (int i = 0; i < indent; ++i) {
    fputc(' ', stderr);
  }
  for (size_t i = 0, end = pos - 1; i < end; ++i) {
    fputc('~', stderr);
  }
  fputc('^', stderr);
  for (size_t i = pos, end = length; i < end; ++i) {
    fputc('~', stderr);
  }
}

inline void Error(const std::string& msg, const char* input, const char* str) {
  fprintf(stderr,
          "Error: %s\n"
          "Input: %s\n",
          msg.c_str(), input);
 PrintIndicator(strlen("Input: "), str - input, strlen(input));
  std::exit(1);
}

inline void Error(const std::string& msg, const std::string& input, const char* str) {
  fprintf(stderr,
          "Error: %s\n"
          "Input: %s\n",
          msg.c_str(), input.c_str());
   PrintIndicator(strlen("Input: "), input.find(str), input.size());
  std::exit(1);
}

//template<class T>
using symbol_map = StreamMap<const char*, base_type, StreamCaseCompare>;

using function_map = StreamMap<const char*, Operator>;

template<class T, class Calculator = ReversePolish<T>, class Parser = ShuntingYard<T>>
T Parse(const std::string& content,
 std::shared_ptr<typename Parser::symbol_map> symbols,
 std::shared_ptr<typename Parser::function_map> functions) {
  Calculator calculator;
  Parser parser(calculator);
  parser.SetSymbolMap(symbols);
  parser.SetFunctionMap(functions);
  T value;
  const char* test_str = content.c_str();
  try {
    value = parser.Parse(test_str);
    return value;
  } catch (const ParseException& e) {
    Error(e.what, content.c_str(), test_str);
  } catch (const ExpressionException& e) {
    Error(e.what, content.c_str(), test_str);
  } catch (const char* w) {
    Error(w, content.c_str(), test_str);
  } catch (const std::runtime_error& e) {
    Error(e.what(), content.c_str(), test_str);
  } catch (const std::string& s) {
    Error(s.c_str(), content.c_str(), test_str);
  }
  return value;
}

// template<class FunctionMap, class SymbolMap, class T, class Calculator = ReversePolish<T>, class Parser = ShuntingYard<T>>
// void LoadSymbols(std::ifstream& file, std::shared_ptr<SymbolMap> symbols, std::shared_ptr<FunctionMap> functions) {
//   Calculator calculator;
//   Parser parser(calculator, symbols, functions);
//   std::string line;
//   while (std::getline(file, line)) {
//     auto eq = ::string::SplitInTwo(line);

//     symbol_map.at(eq.first.c_str()) = eq.second;
//   }
// }

// template<class FunctionMap, class SymbolMap, class T, class Calculator = ReversePolish<T>, class Parser = ShuntingYard<T>>
template<class T, class SymbolMap, class FunctionMap, class Calculator = DefaultRPN<T>, class Parser = ShuntingYard<T>>
void LoadSymbols(std::fstream& file, std::shared_ptr<SymbolMap> symbols, std::shared_ptr<FunctionMap> functions) {
  Calculator calculator;
  Parser parser(calculator, symbols, functions);
  std::string line;
  // std::auto_ptr<Expression<T>> expr;
  while (std::getline(file, line)) {
    // TODO: Add semi-colon separator?
    if (line.empty() || line[0] == '#')
      continue;
    auto eq = ::string::SplitInTwo(line, "=");
    auto it = eq.second.c_str();
    T value;
    try {
      value = parser.Parse(it);
    } catch (UnknownAlpha& e) {
      Error(e.what, line, it);
      // std::cout << e.what << std::endl;
      // exit(1);
    }
//    expr.reset(parser.Parse(it));
//    auto value = expr->Process();
    (*symbols)[eq.first] = value;
    parser.Clear();
  }
}

template<class K, class T, class Compare>
std::ostream& operator<<(std::ostream& stream, const StreamMap<K, T, Compare>& map) {
  return stream << *reinterpret_cast<const std::map<K, T, Compare>*>(&map);
}


int main(int argc, char *argv[]) {
  // double n = 150.3;
  // double r = 146;
  // printf("%.15g choose %.15g = %.15g\n", n, r, (double)combination<double>(n, r));
  // return 0;

  // StreamMap<const char*, Operator> operators;
  // sed -r 's/operators\[("[^"]*")\] *= ([^;]*);/(\1, \2)/g'
  // sed -r 's/\(("[^"]*") *, *(\{[^}]*\})\)/operators[\1] = \2\;/g'

  // sed -r 's/\{([^}]*)\}/Operator(\1)/g'
  // sed -r 's/Operator\(([^)]*)\)/{\1}/g'
  // operator        = Operator(name,   function,                     nargs, prec, left_assoc);
  // operators["sin"]   = Operator("Sin",  Operations<double>::sin,      1,     9,    true);
  // operators["cos"]   = Operator("Cos",  Operations<double>::cos,      1,     9,    true);
  // operators["tan"]   = Operator("Tan",  Operations<double>::tan,      1,     9,    true);
  // operators["asin"]  = Operator("Asin", Operations<double>::asin,     1,     9,    true);
  // operators["acos"]  = Operator("Acos", Operations<double>::acos,     1,     9,    true);
  // operators["atan"]  = Operator("Atan", Operations<double>::atan,     1,     9,    true);
  // operators["exp"]   = Operator("Exp",  Operations<double>::exp,      1,     9,    true);
  // operators["ln"]    = Operator("Ln",   Operations<double>::log,      1,     9,    true);
  // operators["log10"] = Operator("Log",  Operations<double>::log10,    1,     9,    true);
  // operators["log"]   = Operator("Log",  Operations<double>::log10,    1,     9,    true);
  // operators["log2"]  = Operator("Lg",   Operations<double>::log2,     1,     9,    true);
  // operators["lg"]    = Operator("Lg",   Operations<double>::log2,     1,     9,    true);
  // operators["sqrt"]  = Operator("Sqrt", Operations<double>::sqrt,     1,     9,    true);
  // operators["_"]     = Operator("-",    Operations<double>::negate,   1,     9,    true);
  // operators["^"]     = Operator("^",    Operations<double>::pow,      2,     8,    true);
  // operators["*"]     = Operator("*",    Operations<double>::multiply, 2,     7,    false);
  // operators["/"]     = Operator("/",    Operations<double>::divide,   2,     7,    false);
  // operators["%"]     = Operator("%",    Operations<double>::modulo,   2,     7,    false);
  // operators["+"]     = Operator("+",    Operations<double>::plus,     2,     5,    false);
  // operators["-"]     = Operator("-",    Operations<double>::minus,    2,     5,    false);
  // operators["("]     = Operator("(",    NULL,                         0,     1,    false);
  // operators[")"]     = Operator(")",    NULL,                         0,     1,    false);

  // const char* name = "sin";
  // std::stack<double> values;
  // values.push(2);
  // values.push(1);
  // values.push(kPi);
  // values.push(kE);
  // values.push(kPhi);
  // try {
  //   printf("%s: %.15g\n", name, CallOperator(operators, name, values));
  // } catch (const char* e) {
  //   printf("Error: %s\n", e);
  //   std::exit(1);
  // }
  
  setvbuf(stdout, NULL, _IONBF, 0);
  // std::cout << std::setprecision(14);
  std::cout.precision(14);
  // std::cout.sync_with_stdio(false);
  Arguments args(argc, argv);
  if (!args[0]) {
    printf("Usage: %s <expression>\n", argv[0]);
    return 1;
  }
  std::string content = (const char*)args[0];
//  const char* test_str = content.c_str();

#if defined(EXPRESSION)
//  // using type = std::complex<double>;
//  // using type = double;
  using calculator_type = ExpressionCalculator<base_type>;
  using parser_type = ShuntingYard<Expression<base_type>*>;
//  calculator_type calculator;
//  parser_type parser(calculator);
//
 std::auto_ptr<Expression<base_type> > expr;
//
 // typedef Expression<base_type>* op_type;
 typedef base_type op_type;
#else
 using calculator_type = DefaultRPN<base_type>;
 using parser_type = ShuntingYard<base_type>;
 typedef base_type op_type;
//  DefaultRPN<double> calculator;
//  ShuntingYard<double> parser(calculator);
//  typedef base_type op_type;
#endif  // EXPRESSION

  // TODO: Should I make sure that "i" works, but "I" doesn't? How to handle case?
//  parser.AddSymbol
  // parser_type::symbol_map symbol_map;
  auto symbol_map = std::make_shared<parser_type::symbol_map>();

  WrapMap(*symbol_map)
  ("i",                      complex_type(0, 1))
  ("g",                      SI::Mass::kGram)
  ("s",                      SI::Time::kSecond)
  ("m",                      SI::Length::kMeter)
  ("C",                      SI::Charge::kCoulomb)
  // ("g",                      SI::Mass::kGram)
  // ("coulomb",                SI::Charge::kCoulomb)
  ("pi",                     kDimensionLess * kPi)
  ("phi",                    kDimensionLess * kPhi)
  ("e",                      kDimensionLess * kE)
  // ("the golden ratio",       kDimensionLess * kPhi)
  // ("golden ratio",           kDimensionLess * kPhi)
  // ("golden",                 kDimensionLess * kPhi)
  // ("km",                     1000 * SI::Length::kMeter)
  // ("meter",                  SI::Length::kMeter)
  // ("meters",                 SI::Length::kMeter)
  // ("metre",                  SI::Length::kMeter)
  // ("metres",                 SI::Length::kMeter)
  // ("second",                 SI::Time::kSecond)
  // ("seconds",                SI::Time::kSecond)
  // ("mile",                   Imperial::Length::kMile)
  // ("miles",                  Imperial::Length::kMile)
  // ("minute",                 Time::kMinute)
  // ("minutes",                Time::kMinute)
  // ("hour",                   Time::kHour)
  // ("hours",                  Time::kHour)
  // ("foot",                   Imperial::Length::kFoot)
  // ("ft",                     Imperial::Length::kFoot)
  // ("feet",                   Imperial::Length::kFoot)
  // ("foots",                  Imperial::Length::kFoot)
  // ("mph",                    Imperial::Length::kMile / Time::kHour)
  // ("c",                      299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("speed of light",         299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("the speed of light",     299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("pa",                     SI::Pressure::kPascal)
  // ("pascal",                 SI::Pressure::kPascal)
  // ("pascals",                SI::Pressure::kPascal)
  // ("elementary charge",      1.60217646e-19 * SI::Charge::kCoulomb)
  // ("charge of electron",     1.60217646e-19 * SI::Charge::kCoulomb)
  // ("the elementary charge",  1.60217646e-19 * SI::Charge::kCoulomb)
  // ("the charge of electron", 1.60217646e-19 * SI::Charge::kCoulomb)
  ;

  // using base_type = Expression<type>*;
//  parser.AddFunction
  // parser_type::function_map function_map;
  auto function_map = std::make_shared<parser_type::function_map>();
  WrapMap(*function_map)
      ("sin",   Operator("Sin",  Operations<op_type>::sin,      1, 9, false,  false))
      ("cos",   Operator("Cos",  Operations<op_type>::cos,      1, 9, false,  false))
      ("tan",   Operator("Tan",  Operations<op_type>::tan,      1, 9, false,  false))
      // ("asin",  Operator("Asin", Operations<op_type>::asin,     1, 9, false,  false))
      // ("acos",  Operator("Acos", Operations<op_type>::acos,     1, 9, false,  false))
      // ("atan",  Operator("Atan", Operations<op_type>::atan,     1, 9, false,  false))
      ("exp",   Operator("Exp",  Operations<op_type>::exp,      1, 9, false,  false))
      ("ln",    Operator("Ln",   Operations<op_type>::log,      1, 9, false,  false))
      ("log10", Operator("Log",  Operations<op_type>::log10,    1, 9, false,  false))
      ("log",   Operator("Log",  Operations<op_type>::log10,    1, 9, false,  false))
      // ("log2",  Operator("Lg",   Operations<op_type>::log2,     1, 9, false,  false))
      // ("lg",    Operator("Lg",   Operations<op_type>::log2,     1, 9, false,  false))
      ("sqrt",  Operator("Sqrt", Operations<op_type>::sqrt,     1, 9, false,  false))
      ("_",     Operator("-",    Operations<op_type>::negate,   1, 9, true,   false))
      ("^",     Operator("^",    Operations<op_type>::pow,      2, 8, true,   false))
      ("*",     Operator("*",    Operations<op_type>::multiply, 2, 7, true,   true))
      ("/",     Operator("/",    Operations<op_type>::divide,   2, 7, true,   true))
      // ("%",     Operator("%",    Operations<op_type>::modulo,   2, 7, true,   true))
      ("+",     Operator("+",    Operations<op_type>::plus,     2, 5, true,   true))
      ("-",     Operator("-",    Operations<op_type>::minus,    2, 5, true,   true))
#ifdef EXPRESSION
      ("(",     Operator("(",    Operations<op_type>::paren,    1, 1, false,  true))
      (")",     Operator(")",    Operations<op_type>::paren,    1, 1, false,  true))
#endif  // EXPRESSION
      ("per",   Operator("per",  Operations<op_type>::divide,   2, 7, true,   true))
      // ("(",     Operator("(",    NULL,                            0,     1,    false))
      // (")",     Operator(")",    NULL,                            0,     1,    false))
      ;

//  expr.reset(Parse<op_type, calculator_type, parser_type>(content, symbol_map, function_map));
//  auto result = expr->Process();
//  cout << " = " << result << endl;
//  return 0;

  std::fstream file("symbols.txt", std::fstream::in);

  // LoadSymbols<base_type,
  // parser_type::symbol_map,
  // parser_type::function_map,
  // calculator_type,
  // parser_type>(file, symbol_map, function_map);
  /*
  TODO:
  -Prefix modifier support (Space vs Time):
    - Check if symbol is part of table, then try matching prefix and then symbol.
    - Add a prefix to every symbol you load and add it to the table.
  - Suffix plurality (Space vs Time):
    - Check if symbol is part of table, then try matching prefix and then symbol.
    - Add a suffix to every symbol you load and add it to the table.
  - For conversion, if one conversion doesn't work, or another conversion may be
    better, you should be able to check through all the possibilities. As such,
    you should probably use equal_range and a std::map with a good compare. I
    don't think StreamMap is neccesary.
  - Go through and make everything std::string.
  - Figure out a proper way of specifying base_type and op_type automatically.
    Figure out the most elegant way of specifying both an Expression* and double.
    ****Probably by specifying a numeric_type, which is for constants, numbers, etc,
    and also a result_type=numeric_type which is the result of expressions.
  - Why did I make Operations in the first place?
  - Update number-parse to use the faster uint loop in the ufloat loop.
  */
  LoadSymbols<base_type>(file, symbol_map, function_map);
  std::cout << *symbol_map << std::endl;
  return 0;

#ifdef EXPRESSION
  auto half = ::string::SplitInTwo(content, "to ");
  expr.reset(Parse<op_type, calculator_type, parser_type>(half.first, symbol_map, function_map));

  auto lhs = expr->Process();
  cout << " = " << lhs << endl;
  if (half.second.empty())
  return 0;

  expr.reset(Parse<op_type, calculator_type, parser_type>(half.second, symbol_map, function_map));
  auto rhs = expr->Process();
  cout << " = " << rhs << endl;
  cout << "Result: " << ConvertUnits(lhs, rhs) << endl;
  return 0;
#endif  // EXPRESSION

  // std::string lhs_string, rhs_string;
  // auto pos = content.find("to ");
  // lhs_string = content.substr(0, pos);
  // expr.reset(Parse<op_type, calculator_type, parser_type>(lhs_string, symbol_map, function_map));
  // auto lhs = expr->Process();
  // cout << " = " << lhs << endl;

  // if (pos == std::string::npos)
  //   return 0;

  // rhs_string = content.substr(pos+3);
  // if (rhs_string.empty())
  //   return 13;
  // cout << "Converting to ";
  // expr.reset(Parse<op_type, calculator_type, parser_type>(rhs_string, symbol_map, function_map));
  // auto rhs = expr->Process();
  // cout << " = " << rhs << endl;
  // cout << "Result: " << ConvertUnits(lhs, rhs) << endl;

  // return 0;
  

//   // parser.Print();
//   // parser.SetSymbolMap(symbol_map);
//   // parser.SetFunctionMap(function_map);


//   base_type value;
//   try {
// #ifdef EXPRESSION
//     expr.reset(parser.Parse(test_str));
//     // Expression::type value = expr->Process();
//     // Expression<std::complex<double> >::type value;
//     // if (expr->paren()) {
//     //   Parenthesis* p = (blind_ptr)expr.get();
//     //   value = p->expr->Process();
//     //   // value = reinterpret_cast<Parenthesis*>(expr.get())->expr->Process();
//     // } else
//     setvbuf(stdout, NULL, _IONBF, 0);
// //    dprintf("%s: %d: here.\n", __FILE__, __LINE__);
//     value = expr->Process();
// //    dprintf("%s: %d: here.\n", __FILE__, __LINE__);
// #else
//     double value = parser.Parse(test_str);
//     printf("%s", test_str);
// #endif  // EXPRESSION
//     // printf(" = %.14g\n", value);
//     // printf(" = (%.14g, %.14g)\n", value.real(), value.imag());
//     std::cout << " = " << value << std::endl;
//   } catch (const ParseException& e) {
//     Error(e.what, content.c_str(), test_str);
//   } catch (const ExpressionException& e) {
//     Error(e.what, content.c_str(), test_str);
//   } catch (const char* w) {
//     Error(w, content.c_str(), test_str);
//   } catch (const std::runtime_error& e) {
//     Error(e.what(), content.c_str(), test_str);
//   } catch (const std::string& s) {
//     while (*test_str && isspace(*test_str))
//       ++test_str;
//     // printf("%s:%d: %s\n", __FILE__, __LINE__, test_str);
//     if (*test_str && string::StreamCaseCompare("to", test_str)) {
//       calculator_type crhs;
//       parser_type prhs(crhs);
//       prhs.SetSymbolMap(symbol_map);
//       prhs.SetFunctionMap(function_map);
// //      parser.SetCalculator(crhs);
//       std::cout << "Converting to ";
//       test_str += 2;
//       // printf("%s:%d: %s\n", __FILE__, __LINE__, test_str);
// //      base_type rhs = parser.Parse(test_str);
//       // base_type rhs = expr->Process();
//       expr.reset(prhs.Parse(test_str));
//       base_type rhs = expr->Process();
// //      printf("%s:%d: %s\n", __FILE__, __LINE__, test_str);
//       std::cout << " = " << rhs << '\n';
//       try {
//         std::cout << "Result: " << ConvertUnits(value, rhs) << '\n';
//       } catch (std::runtime_error w) {
//         Error(w.what(), content.c_str(), test_str);
//       }
//     } else
//       Error(s.c_str(), content.c_str(), test_str);
//   }
//   return 0;
}
