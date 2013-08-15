#ifndef REVERSE_POLISH_H_
#define REVERSE_POLISH_H_

// Only neccessary if you define the virtual Perform* functions.
#include <cmath>
// TODO: Delete after debuggin.
#include <string>
#include <cstdint>
#include "calc-utils.h"
#include "operator.h"
#include "debug-out.h"

// typedef unsigned char uint8;

// // TODO: Should I have this or just for testing?
// enum Funcs { kNeg = 200 };

struct ParseException {
  ParseException(const std::string& wat) : what(wat) {}
  const std::string what;
};

struct NoParam : ParseException {
  NoParam() : ParseException("Not enough parameters for operator.") {}
};

struct NotEnoughOperators : ParseException {
  NotEnoughOperators() : ParseException("Not enough operators.") {}
};

struct WTFHappened : ParseException {
  WTFHappened() : ParseException("I don't know what happened.") {}
};

template <class Symbol, class Result = Symbol>
class ReversePolish {
 public:
  typedef Symbol symbol_type;
  typedef Result result_type;

  virtual void HandleValue(double value) = 0;

  virtual void HandleConstant(const std::string& name,
                              const symbol_type& value) = 0;

  virtual void HandleParenthesis() {}

  void HandleOperator(const Operator* operation);

  const result_type& GetValue() const;

  void Clear() { std::stack<result_type>().swap(values_); }

#ifdef DEBUG
  bool empty() { return values_.empty(); }
  result_type top() { return values_.top(); }
#endif

 protected:
  void PushValue(const result_type& value);
  result_type PopValue();

 private:
  std::stack<result_type> values_;
};

template <class Symbol, class Result = Symbol>
class DefaultRPN : public ReversePolish<Symbol, Result> {
 public:
  typedef typename ReversePolish<Symbol, Result>::symbol_type symbol_type;
  typedef typename ReversePolish<Symbol, Result>::result_type result_type;

  void HandleValue(double value);
  void HandleConstant(const std::string& name, const symbol_type& value);

  // private:
  //  T BinaryOperator(int operation, const T& left, const T& right);
  //  T UnaryOperator(int operation, const T& value);
  //  T FunctionOperator(int operation);
};

#include "reverse-polish-impl.hpp"

#endif  // REVERSE_POLISH_H_
