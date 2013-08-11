#ifndef SHUNTING_YARD_INL_H_
#define SHUNTING_YARD_INL_H_

#include "number-parse.h"
#include "map-utils.h"
#include "string-utils.h"

enum Token {
  kNothing = 0x00,
  kBad = 0x01,
  kValue = 0x02,
  kAlpha = 0x04,
  kOperator = 0x08,
  kLeftParenthesis = 0x10,
  kRightParenthesis = 0x20,
  kFunctionSeparator = 0x40,
  kConstant = kAlpha | kValue,
  kFunction = kAlpha | kOperator
};

#define CRS(x) \
  case x:      \
    return #x;

const char* TokenToString(Token token) {
  switch (token) {
    CRS(kNothing);
    CRS(kBad);
    CRS(kValue);
    CRS(kAlpha);
    CRS(kOperator);
    CRS(kLeftParenthesis);
    CRS(kRightParenthesis);
    CRS(kFunctionSeparator);
    CRS(kConstant);
    CRS(kFunction);
  }
}

// struct Token {
//   union {
//     double value;
//     int operation;
//   } data;
//   TokenID id;
// };

static /*TokenID*/ Token GetToken(const char*& str, size_t* length) {
  while (string::IsSpace(*str)) ++str;
  *length = 0;
  if (!*str) return kNothing;
  const char* start = str;
  // TODO: Is this necessary?
  *length = 1;
  if (IsFunctionSeparator(*str)) return kFunctionSeparator;
  if (*str == '(') return kLeftParenthesis;
  if (*str == ')') return kRightParenthesis;
  if (*str == '.' || IsDigit(*str)) return kValue;
  // TODO: Fix this.
  if (IsOperator(*str)) return kOperator;
  if (string::IsAlpha(*str)) return kAlpha;
  // TODO: Add function support.
  return kBad;
}

// Move the pointer forward accordingly.
static int GetOperator(const char*& str) { return *str++; }

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::ShuntingYard(
    calculator_type& calculator)
    : calculator_(calculator) {}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::ShuntingYard(
    calculator_type& calculator, std::shared_ptr<symbol_map> symbols,
    std::shared_ptr<function_map> functions)
    : calculator_(calculator), symbols_(symbols), functions_(functions) {}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
void ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::SetCalculator(
    calculator_type& calculator) {
  calculator_ = calculator;
}
template <class Symbol, class Result, class SymbolMap, class FunctionMap>
void ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::SetFunctionMap(
    std::shared_ptr<function_map> map) {
  functions_ = map;
}
template <class Symbol, class Result, class SymbolMap, class FunctionMap>
void ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::SetSymbolMap(
    std::shared_ptr<symbol_map> map) {
  symbols_ = map;
}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
void ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::CloseParenthesis() {
  // template<class T, class RPNHandler>
  // void ShuntingYard<T, RPNHandler>::CloseParenthesis() {
  while (!operators_.empty() && *operators_.top()->name != '(') {
    DEBUGOUT;
    PopOperator();
    // calculator_.HandleOperator(operators_.top());
    // operators_.pop();
  }
  if (operators_.empty()) {
    fputs("Close: ", stderr);
    throw MismatchedParenthesis();
  }
  operators_.pop();
  // TODO: Experimental.
  calculator_.HandleOperator(&functions_->at("("));
  // calculator_.HandleParenthesis();

  // if (!operators_.empty() && IsFunction(operators_.top())) {
  if (!operators_.empty() && !IsParenthesis(*operators_.top()->name)) {
    dprintf("Token:\t\t\tFunction(%s)\n", operators_.top()->name);
    // TODO: Should I add direct FunctionOperator access?
    PopOperator();
    // calculator_.HandleOperator(operators_.top());
    // // TODO: Make sure this works.
    // operators_.pop();
  }
  // PerformFunction(operators_.top());
}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
inline void
ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::PopOperator() {
  calculator_.HandleOperator(operators_.top());
  operators_.pop();
}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
void ShuntingYard<Symbol, Result, SymbolMap,
                  FunctionMap>::FunctionArgumentSeparator() {
  while (!operators_.empty() && *operators_.top()->name != '(') {
    // Should this be `HandleOperator` or `ProcessOperator`.
    PopOperator();
    // calculator_.HandleOperator(operators_.top());
    // operators_.pop();
  }
  if (operators_.empty())
    throw "Misplaced parenthesis or function argument separator.";
}

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
// void ShuntingYard<T>::ProcessOperator(int operation) {
    void ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::ProcessOperator(
        Operator* operation) {
  // template<class T, class RPNHandler>
  // void ShuntingYard<T, RPNHandler>::ProcessOperator(int operation) {

  int precedence = operation->precedence - operation->left_assoc;
  dprintf("ProcessOperator:\tOperator(%s);\t\tPrecedence(%d)\n",
          operation->name, precedence);
  while (!operators_.empty() && precedence < operators_.top()->precedence) {
    dprintf("ProcessOperatorLoop:\tOperator(%s vs %s);\tPrecedence(%d vs %d)\n",
            operation->name, operators_.top()->name, precedence,
            operators_.top()->precedence);
    // DEBUGOUT;
    PopOperator();
    // calculator_.HandleOperator(operators_.top());
    // operators_.pop();
  }
  operators_.push(operation);
}

class State {
 public:
  State() {
    negate_ = true;
    impl_mult_ = false;
  }
  inline bool Negate() const { return negate_; }
  inline bool ImplicitMultiplication() const { return impl_mult_; }
  void SetToken(Token token) {
    negate_ = token & (kLeftParenthesis | kOperator);
    // impl_mult_ = token & kValue;
    impl_mult_ = token & (kValue | kRightParenthesis);
  }

 private:
  bool negate_;
  bool impl_mult_;
  // Token previous_;
};

template <class Symbol, class Result, class SymbolMap, class FunctionMap>
Result
ShuntingYard<Symbol, Result, SymbolMap, FunctionMap>::Parse(Iterator& str,
                                                            Iterator end) {
  // template<class T, class RPNHandler>
  // T ShuntingYard<T, RPNHandler>::Parse(const char* str, const char* end) {
  State state;

  size_t length;
  while (str != end && *str) {
#ifdef DEBUG
    const char* start = str;
#endif  // DEBUG
        // while (*str) {
    Token token = GetToken(str, &length);

    dprintf("Token:\t\t\t");
    switch (token) {
      // switch (token.id) {
      case kFunctionSeparator:
        dprintf("FunctionArgumentSeparator\n");
        FunctionArgumentSeparator();
        ++str;
        break;
      case kValue: {
        // base_type value = ParseUFloat<base_type>(str);
        double value = ParseUFloat<double>(str);
        dprintf("Value(%.15g)\n", value);
        if (string::BitLower(*(str - 1)) == 'e') --str;
        calculator_.HandleValue(value);

        // If preceded a number (not a constant), you should disable
        // implicit multiplication. Constants however, should always
        // enable implicit multiplication.
      } break;
      case kAlpha: {
        {
          // std::map<const char*, int>::iterator idx =
          //     UnGreedyFind(functions_, str, string::StreamCompare);

          // StreamMap<const char*, int>::iterator idx = functions_->Find(str);
          typename function_map::iterator idx = functions_->Find(str);
          if (idx != functions_->end()) {
            dprintf("Function(%s)", idx->first.c_str());
            if (!idx->second.is_operator && state.ImplicitMultiplication()) {
              dprintf(" with Implicit Multiplication\n");
              ProcessOperator(&functions_->at("*"));
            } else
              dprintf("\n");
            // calculator_.HandleOperator(idx->first, idx->second);
            // calculator_.HandleOperator(idx->second);
            ProcessOperator(&idx->second);
            str += string::ssize(idx->first);
            token = kFunction;
            break;
          }
        }
        {
          // std::map<const char*, double>::iterator idx =
          //     UnGreedyFind(symbols_, str, string::StreamCaseCompare);
          // StreamMap<const char*, base_type, StreamCaseCompare>::iterator idx
          // = symbols_->Find(str);
          typename symbol_map::iterator idx = symbols_->Find(str);
          if (idx != symbols_->end()) {
            // dprintf("Constant(%s=%.15g)", idx->first, idx->second);
            dexec(std::cout << "Constant(" << idx->first << "=" << idx->second
                            << ")");
            if (state.ImplicitMultiplication()) {
              dprintf(" with Implicit Multiplication\n");
              ProcessOperator(&functions_->at("*"));
            } else
              dprintf("\n");
            calculator_.HandleConstant(idx->first, idx->second);
            str += string::ssize(idx->first);
            token = kConstant;
            break;
          }
        }
        throw UnknownAlpha(str);
        // throw std::string("Unrecognized alphanumeric symbol: ") + str;
      } break;
      case kLeftParenthesis:
        dprintf("LeftParenthesis");
        if (state.ImplicitMultiplication()) {
          dprintf(" with Implicit Multiplication\n");
          ProcessOperator(&functions_->at("*"));
        } else
          dprintf("\n");
        operators_.push(&functions_->at("("));
        ++str;
        break;
      case kRightParenthesis:
        dprintf("RightParenthesis\n");
        CloseParenthesis();
        ++str;
        break;
      case kOperator: {
        // const char* start = str;
        char op[2] = {*str, '\0'};
        dprintf("Operator(%c)\n", *op);
        if (*op == '-' && state.Negate()) *op = '_';
        typename function_map::iterator it = functions_->find(op);
        if (it == functions_->end()) throw BadOperator();
        ProcessOperator(&it->second);
        ++str;
      } break;
      case kBad:
        throw "Unknown symbol.";
      default:
        break;
    }
    DEBUGOUT;
    dprintf("\n");
    state.SetToken(token);
#ifdef DEBUG
    if (str == start) throw "Forgot to move str iterator forward...";
#endif  // DEBUG
  }

  // Finish off remaining stack.
  while (!operators_.empty()) {
    if (IsParenthesis(*operators_.top()->name)) {
      fputs("Remainder: ", stderr);
      throw MismatchedParenthesis();
    }
    PopOperator();
    // calculator_.HandleOperator(operators_.top());
    // operators_.pop();
  }
  return calculator_.GetValue();
}

#endif  // SHUNTING_YARD_INL_H_
