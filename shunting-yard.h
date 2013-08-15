#ifndef SHUNTING_YARD_H_
#define SHUNTING_YARD_H_

#include <stack>
#include <map>
#include <cstring>
#include <cstdarg>
#include <memory>
#include "reverse-polish.h"
#include "base/build_config.h"
#include "map-utils.h"
#include "operator.h"

struct BadOperator : ParseException {
  BadOperator() : ParseException("Unknown operator.") {}
};

struct MismatchedParenthesis : ParseException {
  MismatchedParenthesis() : ParseException("Mismatched Parenthesis.") {}
};

struct NotEnoughParams : ParseException {
  NotEnoughParams()
      : ParseException("Not enough parameters/Too many operators.") {}
};

struct UnknownAlpha : ParseException {
  std::string token;
  UnknownAlpha(const std::string& alpha)
      : token(alpha),
        ParseException(std::string("Unrecognized alphanumeric symbol: '") +
                       alpha + '\'') {}
};

#if defined(OS_WIN)
#define stricmp _stricmp
#else
#define stricmp strcasecmp
#endif  // OS_WIN

#ifdef DEBUG
#define DEBUGOUT print()
#else
#define DEBUGOUT
#endif

template <class Symbol, class Result = Symbol,
          class SymbolMap = StreamMap<std::string, Symbol>,
          // class FunctionMap = StreamMap<std::string, Operator>>
          class FunctionMap = std::map<std::string, const Operator, StreamCompare>>
class ShuntingYard {
 public:
  typedef Symbol symbol_type;
  typedef Result result_type;
  typedef ReversePolish<Symbol, Result> calculator_type;
  typedef std::stack<const Operator*> operator_stack;
  typedef FunctionMap function_map;
  typedef SymbolMap symbol_map;
  // TODO: I can choose whether or not I want case handled here. I should be
  // able to add functionality that chooses case by case whether or not to use
  // case sensitivity, but do I need it?
  typedef const char* Iterator;

  ShuntingYard(ReversePolish<Symbol, Result>& calculator);
  ShuntingYard(ReversePolish<Symbol, Result>& calculator,
               std::shared_ptr<symbol_map> symbols,
               std::shared_ptr<function_map> functions);

  void SetCalculator(ReversePolish<Symbol, Result>& calculator);
  void SetSymbolMap(std::shared_ptr<symbol_map> map);
  void SetFunctionMap(std::shared_ptr<function_map> map);

  result_type Parse(Iterator& start, Iterator end = NULL);

  void Print() {
    printf("Symbols[%lu]:\n", symbols_->size());
    for (typename symbol_map::iterator idx = symbols_->begin();
         idx != symbols_->end(); ++idx)
      printf("[%s]=%.15g\n", idx->first, idx->second);
    printf("Functions[%lu]:\n", functions_->size());
    for (std::map<const char*, Operator>::iterator idx = functions_->begin();
         idx != functions_->end(); ++idx)
      printf("[%s]=%s\n", idx->first, idx->second.name);
  }

  void Clear() {
    operator_stack().swap(operators_);
    calculator_.Clear();
  }

  void print() {
    fprintf(stderr, "State:\t\t\tOperators.Top(");
    if (!operators_.empty()) fprintf(stderr, "%s", operators_.top()->name);
    fprintf(stderr, ");\tCalculator.Top(");
//#ifdef EXPRESSION
//    if (!calculator_.empty()) std::cerr << calculator_.top()->Process();
//#else
    if (!calculator_.empty()) std::cerr << calculator_.top();
//#endif
    fputs(")\n", stderr);
  }

 private:
  void CloseParenthesis();
  void FunctionArgumentSeparator();
  void ProcessOperator(const Operator* operation);
  void PopOperator();

  // TODO: Should it maintain a copy?
  //  const symbol_map& symbols_;
  //  const function_map& functions_;
  // symbol_map& symbols_;
  // function_map& functions_;
  std::shared_ptr<symbol_map> symbols_;
  std::shared_ptr<function_map> functions_;
  operator_stack operators_;
  calculator_type& calculator_;
};

#include "shunting-yard-impl.hpp"

#endif  // SHUNTING_YARD_H_
