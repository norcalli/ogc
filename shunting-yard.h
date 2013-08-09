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
  const char* token;
  UnknownAlpha(const char* alpha)
  : token(alpha), ParseException((std::string("Unrecognized alphanumeric symbol: '") + token + '\'').c_str()) {}
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

template<class T>
// template<class T, class RPNHandler = ReversePolish<T> >
class ShuntingYard {
 public:
  typedef std::stack<Operator*> operator_stack;
  typedef StreamMap<const char*, Operator> function_map;
  // TODO: I can choose whether or not I want case handled here. I should be able to add functionality that chooses case by case whether or not to use case sensitivity, but do I need it?
  // typedef StreamMap<const char*, base_type, StreamCaseCompare> symbol_map;
  typedef StreamMap<std::string, base_type, StreamCaseCompare> symbol_map;
  // typedef StreamMap<const char*, base_type> symbol_map;
  typedef T type;
  typedef const char* Iterator;

  ShuntingYard(ReversePolish<T>& calculator);
  ShuntingYard(ReversePolish<T>& calculator,
   std::shared_ptr<symbol_map> symbols,
   std::shared_ptr<function_map> functions);
  
  void SetCalculator(ReversePolish<T>& calculator);
  // void SetSymbolMap(symbol_map& map);
  // void SetFunctionMap(function_map& map);
  void SetSymbolMap(std::shared_ptr<symbol_map> map);
  void SetFunctionMap(std::shared_ptr<function_map> map);

  type Parse(Iterator& start, Iterator end = NULL);

//  // inline MapWrap<const char*, base_type> AddSymbol(const char* key, const base_type& val) {
//  //   return MapWrap<const char*, base_type>(symbols_)(key, val);
//  // inline MapWrap<const char*, base_type, StreamCaseCompare> AddSymbol(const char* key, const base_type& val) {
//    // return MapWrap<const char*, base_type, StreamCaseCompare>(symbols_)(key, val);
//  inline MapWrap<symbol_map> AddSymbol(const char* key, const base_type& val) {
//    return WrapMap(symbols_)(key, val);
//  // inline MapWrap<const char*, type, StreamCaseCompare> AddSymbol(const char* key, const type& val) {
//  //   return MapWrap<const char*, type, StreamCaseCompare>(symbols_)(key, val);
//  }
//
//  // inline MapWrap<const char*, Operator, StreamCompare> AddFunction(const char* key, Operator val) {
//  //   return MapWrap<const char*, Operator, StreamCompare>(functions_)(key, val);
//  inline MapWrap<function_map> AddFunction(const char* key, Operator val) {
//    return WrapMap(functions_)(key, val);
//  }

  void Print() {
    printf("Symbols[%lu]:\n", symbols_->size());
    // for (std::map<const char*, base_type>::iterator idx = symbols_->begin();
    for (typename std::map<const char*, type>::iterator idx = symbols_->begin();
         idx != symbols_->end();
         ++idx)
      printf("[%s]=%.15g\n", idx->first, idx->second);
    printf("Functions[%lu]:\n", functions_->size());
    for (std::map<const char*, Operator>::iterator idx = functions_->begin();
         idx != functions_->end();
         ++idx)
      printf("[%s]=%s\n", idx->first, idx->second.name);
  }

  void Clear() {
    operator_stack().swap(operators_);
    calculator_.Clear();
  }

  void print() {
    fprintf(stderr, "State:\t\t\tOperators.Top(");
    if (!operators_.empty())
      fprintf(stderr, "%s", operators_.top()->name);
    //   if (operators_.top() >= 0x100)
    //     fprintf(stderr, "%d", operators_.top());
    //   else
    //     fprintf(stderr, "%c", (char)operators_.top());
    fprintf(stderr, ");\tCalculator.Top(");
    if (!calculator_.empty())
      // fprintf(stderr, "%.15g", calculator_.top());
      std::cerr << calculator_.top();
    fputs(")\n", stderr);
  }
 private:
  void CloseParenthesis();
  void FunctionArgumentSeparator();
  // void ProcessOperator(int operation);
  void ProcessOperator(Operator* operation);
  void PopOperator();

  // StreamMap<const char*, int> functions_;
  // StreamMap<const char*, Operator> functions_;

  // StreamMap<const char*, base_type> symbols_;
  // StreamMap<const char*, base_type, StreamCaseCompare> symbols_;
  // TODO: Should it maintain a copy?
//  const symbol_map& symbols_;
//  const function_map& functions_;
  // symbol_map& symbols_;
  // function_map& functions_;
  std::shared_ptr<symbol_map> symbols_;
  std::shared_ptr<function_map> functions_;
  // StreamMap<const char*, type, StreamCaseCompare> symbols_;
  operator_stack operators_;
  // std::stack<int> operators_;

  // std::stack<char> operators_;
  ReversePolish<type>& calculator_;
  // RPNHandler calculator_;
};

#include "shunting-yard-impl.hpp"

#endif  // SHUNTING_YARD_H_

