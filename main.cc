#include <cmath>
#include <cstdio>
#include <cstdlib>  // for std::exit
#include <cassert>
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
typedef Units<std::complex<double>> unit_type;

#include "expression.h"
#include "arguments.h"
#include "shunting-yard.h"
#include "base/scoped_ptr.h"

using namespace std;

// #define EXPRESSION
#define EXPERIMENTAL

template <class T>
class ExpressionCalculator : public ReversePolish<T, Expression<T>*> {
 public:
  typedef T type;

  void HandleValue(double value) { this->PushValue(new NumberExpr<T>(value)); }

  void HandleConstant(const std::string& name, const type& value) {
    this->PushValue(new ConstantExpr<T>(name, value));
  }
};

template <class T>
struct Constant {
  const char* display_name;
  T value;
};

static const double kE = exp(1);
static const double kPi = atan(1) * 4;
static const double kPhi = (sqrt(5) + 1) / 2;

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
template <class T, int n>
struct NaryFunction {
  typedef T (*type)(const std::vector<T>&);
};

template <class T>
struct NaryFunction<T, 1> {
  typedef T (*type)(const T&);
};

template <class T>
struct NaryFunction<T, 2> {
  typedef T (*type)(const T&, const T&);
};

// template<class T>
// inline T combination(T n, T r) {
//  if (r > n) return 0;
//  T result = 1;
//  // r = std::min(r, n - r); // would be useful because combination is
// symmetric across r
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

// template<>
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

template <class T>
inline T permutation(T n, T r) {
  if (r > n) return 0;
  T result = 1;
  for (; r > 0; --r, --n) {
    result *= n;
  }
  return result;
}

template <class T>
inline T factorial(T a) {
  T result = 1;
  for (; a > 0; --a) {
    result *= a;
  }
  return result;
}

struct OperationsDouble : Operations<double> {
  inline static type nCr(const type& a, const type& b) { return a * b; }
};

template<class Table, class T>
T CallOperator(const Table& operators, const char* n, std::stack<T>& values) {
  typename Table::const_iterator it = operators.Find(n);
  if (it == operators.end())
    throw "Couldn't find that bitch";
  const Operator& op = it->second;
  // printf("CallOperator: name: %s\n", op.display_name);
  printf("CallOperator: name: %s\n", op.name);
  if (op.argument_count == 1) {
    // typename Operations<T>::Unary unary = op.function;
    typename Operations<T>::Unary* unary = op.function;
    T value = values.top(); values.pop();
    return unary(value);
  }
  if (op.argument_count == 2) {
    // typename Operations<T>::Binary binary = op.function;
    typename Operations<T>::Binary* binary = op.function;
    T right = values.top(); values.pop();
    T left  = values.top(); values.pop();
    return binary(left, right);
  }
  // typename Operations<T>::Nary nary = op.function;
  typename Operations<T>::Nary* nary = op.function;
  std::vector<T> arguments(op.argument_count);
  for (int i = arguments.size() - 1; i >= 0; --i) {
    arguments[i] = values.top(); values.pop();
  }
  return nary(arguments);
}

template <class Table, class T>
T CallBinary(const Table& operators, const char* n, const T& l, const T& r) {
  typename NaryFunction<T, 2>::type fn = find(operators, n).function;
  return fn(l, r);
}

inline void PrintIndicator(int indent, long pos, long length) {
  for (int i = 0; i < indent; ++i) {
    fputc(' ', stderr);
  }
  for (long i = 0, end = pos - 1; i < end; ++i) {
    fputc('~', stderr);
  }
  fputc('^', stderr);
  for (long i = pos, end = length; i < end; ++i) {
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

inline void Error(const std::string& msg, const std::string& input,
                  const char* str) {
  fprintf(stderr,
          "Error: %s\n"
          "Input: %s\n",
          msg.c_str(), input.c_str());
  PrintIndicator(strlen("Input: "), str - input.c_str(), input.size());
  std::exit(1);
}

template <class T, class Calculator = DefaultRPN<T>,
          class Parser = ShuntingYard<T>>
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
  }
  catch (const ParseException & e) {
    Error(e.what, content.c_str(), test_str);
  }
  catch (const ExpressionException & e) {
    Error(e.what, content.c_str(), test_str);
  }
  catch (const char * w) {
    Error(w, content.c_str(), test_str);
  }
  catch (const std::runtime_error & e) {
    Error(e.what(), content.c_str(), test_str);
  }
  catch (const std::string & s) {
    Error(s.c_str(), content.c_str(), test_str);
  }
  return value;
}

template <class T, class SymbolMap, class FunctionMap,
          class Calculator = DefaultRPN<T>, class Parser = ShuntingYard<T>>
void LoadSymbols(std::fstream& file, std::shared_ptr<SymbolMap> symbols,
                 std::shared_ptr<FunctionMap> functions) {
  Calculator calculator;
  Parser parser(calculator, symbols, functions);
  std::string line;
  while (std::getline(file, line)) {
    // TODO: Add semi-colon separator?
    if (line.empty() || line[0] == '#') continue;
    auto eq = ::string::SplitInTwo(line, "=");
    auto it = eq.second.c_str();
    T value;
    try {
      value = parser.Parse(it);
    }
    catch (UnknownAlpha & e) {
      Error(e.what, line, it);
    }
    (*symbols)[eq.first] = value;
    parser.Clear();
  }
}

template <class K, class T, class Compare>
std::ostream& operator<<(std::ostream& stream,
                         const StreamMap<K, T, Compare>& map) {
  return stream << *reinterpret_cast<const std::map<K, T, Compare>*>(&map);
}

// template<class FunctionMap>
// void CreateExpressionMap(class Fun)

int main(int argc, char* argv[]) {
  // double n = 150.3;
  // double r = 146;
  // printf("%.15g choose %.15g = %.15g\n", n, r, (double)combination<double>(n, r));
  // return 0;


  setvbuf(stdout, NULL, _IONBF, 0);
  std::cout.precision(14);
  // std::cout.sync_with_stdio(false);
  Arguments args(argc, argv);
  if (!args[0]) {
    printf("Usage: %s <expression>\n", argv[0]);
    return 1;
  }
  std::string content = (const char*)args[0];

  typedef unit_type base_type;
#ifdef EXPRESSION
  using calculator_type = ExpressionCalculator<base_type>;
  using parser_type = ShuntingYard<base_type, Expression<base_type>*>;

  std::auto_ptr<Expression<base_type>> expr;
  typedef Expression<base_type>* op_type;
#else
  using calculator_type = DefaultRPN<base_type>;
  using parser_type = ShuntingYard<base_type>;
  typedef base_type op_type;
#endif  // EXPRESSION

  // TODO: Should I make sure that "i" works, but "I" doesn't? How to handle
  // case?
  //  parser.AddSymbol
  // parser_type::symbol_map symbol_map;
  auto symbol_map = std::make_shared<parser_type::symbol_map>();

  WrapMap(*symbol_map)
  ("i"                     , complex_type(0, 1))
  ("g"                     , SI::Mass::kGram)
  ("s"                     , SI::Time::kSecond)
  ("m"                     , SI::Length::kMeter)
  ("C"                     , SI::Charge::kCoulomb)
  ("pi"                    , kDimensionLess * kPi)
  ("phi"                   , kDimensionLess * kPhi)
  ("e"                     , kDimensionLess * kE)
  // ("g"                     , SI::Mass::kGram)
  // ("coulomb"               , SI::Charge::kCoulomb)
  // ("the golden ratio"      , kDimensionLess * kPhi)
  // ("golden ratio"          , kDimensionLess * kPhi)
  // ("golden"                , kDimensionLess * kPhi)
  // ("km"                    , 1000 * SI::Length::kMeter)
  // ("meter"                 , SI::Length::kMeter)
  // ("meters"                , SI::Length::kMeter)
  // ("metre"                 , SI::Length::kMeter)
  // ("metres"                , SI::Length::kMeter)
  // ("second"                , SI::Time::kSecond)
  // ("seconds"               , SI::Time::kSecond)
  // ("mile"                  , Imperial::Length::kMile)
  // ("miles"                 , Imperial::Length::kMile)
  // ("minute"                , Time::kMinute)
  // ("minutes"               , Time::kMinute)
  // ("hour"                  , Time::kHour)
  // ("hours"                 , Time::kHour)
  // ("foot"                  , Imperial::Length::kFoot)
  // ("ft"                    , Imperial::Length::kFoot)
  // ("feet"                  , Imperial::Length::kFoot)
  // ("foots"                 , Imperial::Length::kFoot)
  // ("mph"                   , Imperial::Length::kMile / Time::kHour)
  // ("c"                     , 299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("speed of light"        , 299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("the speed of light"    , 299792458 * SI::Length::kMeter / SI::Time::kSecond)
  // ("pa"                    , SI::Pressure::kPascal)
  // ("pascal"                , SI::Pressure::kPascal)
  // ("pascals"               , SI::Pressure::kPascal)
  // ("elementary charge"     , 1.60217646e-19 * SI::Charge::kCoulomb)
  // ("charge of electron"    , 1.60217646e-19 * SI::Charge::kCoulomb)
  // ("the elementary charge" , 1.60217646e-19 * SI::Charge::kCoulomb)
  // ("the charge of electron", 1.60217646e-19 * SI::Charge::kCoulomb)
      ;

// parser_type::function_map function_map;
#ifdef EXPRESSION
  auto function_map = std::make_shared<parser_type::function_map>();
  WrapMap(*function_map)
   ("sin",   {"Sin",  Operations<op_type>::sin,      1, 9, false, false})
   ("cos",   {"Cos",  Operations<op_type>::cos,      1, 9, false, false})
   ("tan",   {"Tan",  Operations<op_type>::tan,      1, 9, false, false})
   ("asin",  {"Asin", Operations<op_type>::asin,     1, 9, false, false})
   ("acos",  {"Acos", Operations<op_type>::acos,     1, 9, false, false})
   ("atan",  {"Atan", Operations<op_type>::atan,     1, 9, false, false})
   ("exp",   {"Exp",  Operations<op_type>::exp,      1, 9, false, false})
   ("ln",    {"Ln",   Operations<op_type>::log,      1, 9, false, false})
   ("log10", {"Log",  Operations<op_type>::log10,    1, 9, false, false})
   ("log",   {"Log",  Operations<op_type>::log10,    1, 9, false, false})
   ("log2",  {"Lg",   Operations<op_type>::log2,     1, 9, false, false})
   ("lg",    {"Lg",   Operations<op_type>::log2,     1, 9, false, false})
   ("sqrt",  {"Sqrt", Operations<op_type>::sqrt,     1, 9, false, false})
   ("_",     {"-",    Operations<op_type>::negate,   1, 9, true,  false})
   ("^",     {"^",    Operations<op_type>::pow,      2, 8, true,  false})
   ("*",     {"*",    Operations<op_type>::multiply, 2, 7, true,  true})
   ("/",     {"/",    Operations<op_type>::divide,   2, 7, true,  true})
   ("%",     {"%",    Operations<op_type>::modulo,   2, 7, true,  true})
   ("+",     {"+",    Operations<op_type>::plus,     2, 5, true,  true})
   ("-",     {"-",    Operations<op_type>::minus,    2, 5, true,  true})
   ("(",     {"(",    Operations<op_type>::paren,    1, 1, false, true})
   (")",     {")",    Operations<op_type>::paren,    1, 1, false, true})
   ("per",   {"per",  Operations<op_type>::divide,   2, 7, true,  true})
  ;
#endif  // EXPRESSION

  auto base_type_function_map =
      std::make_shared<StreamMap<std::string, Operator>>();
  WrapMap(*base_type_function_map)
   ("sin",   {"Sin",   Operations<base_type>::sin,      1, 9, false, false})
   ("cos",   {"Cos",   Operations<base_type>::cos,      1, 9, false, false})
   ("tan",   {"Tan",   Operations<base_type>::tan,      1, 9, false, false})
   ("asin",  {"Asin",  Operations<base_type>::asin,     1, 9, false, false})
   ("acos",  {"Acos",  Operations<base_type>::acos,     1, 9, false, false})
   ("atan",  {"Atan",  Operations<base_type>::atan,     1, 9, false, false})
   ("exp",   {"Exp",   Operations<base_type>::exp,      1, 9, false, false})
   ("ln",    {"Ln",    Operations<base_type>::log,      1, 9, false, false})
   ("log10", {"Log",   Operations<base_type>::log10,    1, 9, false, false})
   ("log",   {"Log",   Operations<base_type>::log10,    1, 9, false, false})
   ("log2",  {"Lg",    Operations<base_type>::log2,     1, 9, false, false})
   ("lg",    {"Lg",    Operations<base_type>::log2,     1, 9, false, false})
   ("sqrt",  {"Sqrt",  Operations<base_type>::sqrt,     1, 9, false, false})
   ("ceil",  {"ceil",  Operations<base_type>::ceil,     1, 9, false, false})
   ("floor", {"floor", Operations<base_type>::floor,    1, 9, false, false})
   ("_",     {"-",     Operations<base_type>::negate,   1, 9, true,  false})
   ("^",     {"^",     Operations<base_type>::pow,      2, 8, true,  false})
   ("*",     {"*",     Operations<base_type>::multiply, 2, 7, true,  true})
   ("/",     {"/",     Operations<base_type>::divide,   2, 7, true,  true})
   ("%",     {"%",     Operations<base_type>::modulo,   2, 7, true,  true})
   ("+",     {"+",     Operations<base_type>::plus,     2, 5, true,  true})
   ("-",     {"-",     Operations<base_type>::minus,    2, 5, true,  true})
   ("per",   {"per",   Operations<base_type>::divide,   2, 7, true,  true})
   ("(",     {"(",     NULL,                            1, 1, false, true})
   (")",     {")",     NULL,                            1, 1, false, true})
   ;
#ifndef EXPRESSION
  auto& function_map = base_type_function_map;
#endif
  // const char* name = "sin";
  // std::stack<double> values;
  // values.push(2);
  // values.push(1);
  // values.push(kPi);
  // values.push(kE);
  // values.push(kPhi);
  // try {
  //   printf("%s: %.15g\n", name, CallOperator(*function_map, name, values));
  // } catch (const char* e) {
  //   printf("Error: %s\n", e);
  //   std::exit(1);
  // }
  // return 0;


  std::fstream file("symbols.txt", std::fstream::in);
  /*
  TODO:
  -Prefix modifier support (Space vs Time):
    - Check if symbol is part of table, then try matching prefix and then
  symbol.
    - Add a prefix to every symbol you load and add it to the table.
  - Suffix plurality (Space vs Time):
    - Check if symbol is part of table, then try matching prefix and then
  symbol.
    - Add a suffix to every symbol you load and add it to the table.
  - For conversion, if one conversion doesn't work, or another conversion may be
    better, you should be able to check through all the possibilities. As such,
    you should probably use equal_range and a std::map with a good compare. I
    don't think StreamMap is neccesary.
  - Go through and make everything std::string.
  - Figure out a proper way of specifying base_type and op_type automatically.
    Figure out the most elegant way of specifying both an Expression* and
  double.
    ****Probably by specifying a numeric_type, which is for constants, numbers,
  etc,
    and also a result_type=numeric_type which is the result of expressions.
  - Why did I make Operations in the first place?
  - Update number-parse to use the faster uint loop in the ufloat loop.

  - Don't really need to specify specific *Expr's for each function, you
    can just take a std::function<T(T,T)> for binary and .. for others, and
    create the Exprs at run time. Which would be faster? Probably the former.
  - Instead of using a void* and casting to a function, you can have multiple
  maps.
  */
  LoadSymbols<base_type>(file, symbol_map, base_type_function_map);

  auto half = ::string::SplitInTwo(content, "to ");
#ifdef EXPRESSION
  expr.reset(Parse<op_type, calculator_type, parser_type>(
      half.first, symbol_map, function_map));

  auto lhs = expr->Process();
  cout << " = " << lhs << endl;
  if (half.second.empty()) return 0;

  expr.reset(Parse<op_type, calculator_type, parser_type>(
      half.second, symbol_map, function_map));
  auto rhs = expr->Process();
  cout << " = " << rhs << endl;
  cout << "Result: " << ConvertUnits(lhs, rhs) << endl;
  return 0;
#else
  auto lhs = Parse<base_type>(half.first, symbol_map, function_map);
  cout << half.first << "= " << lhs;
  if (half.second.empty()) return 0;
  auto rhs = Parse<base_type>(half.second, symbol_map, function_map);
  cout << " = " << ConvertUnits(lhs, rhs).value << " " << half.second;
  return 0;
#endif  // EXPRESSION
}
