#ifndef REVERSE_POLISH_H_
#define REVERSE_POLISH_H_

// Only neccessary if you define the virtual Perform* functions.
#include <cmath>
// TODO: Delete after debuggin.
#include <string>
#include "calc-utils.h"
#include "operator.h"
#include "debug-out.h"

typedef unsigned char uint8;

// typedef double base_type;
// typedef std::complex<double> base_type;

// // TODO: Should I have this or just for testing?
// enum Funcs { kNeg = 200 };

// struct ParseException {
//   // ParseException(int code, const char* what) : what_(what), code_(code) {}
//   ParseException(const char* what) : what_(what) {}
//   // int code() { return code_; }
//   const char* what() { return what_; }
//  private:
//   // int code_;
//   const char* what_;
// };

struct ParseException {
  ParseException(const char* wat) : what(wat) {}
  const char* const what;
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

template<class T>
class ReversePolish {
 public:
  typedef T type;

  virtual void HandleValue(const base_type& value) = 0;
  // virtual void HandleConstant(const char* name, const base_type& value) = 0;
  virtual void HandleConstant(const std::string& name, const base_type& value) = 0;
  // virtual void HandleValue(const base_type& value);
  // virtual void HandleConstant(const char* name, const base_type& value);
  virtual void HandleParenthesis() { }
  void HandleOperator(Operator* operation);
  const T& GetValue() const;

  void Clear() {
    std::stack<T>().swap(values_);
  }

  #ifdef DEBUG
  bool empty() { return values_.empty(); }
  T top() { return values_.top(); }
  #endif

 protected:
  void PushValue(const T& value);
  // void PopValue(T* value);
  // T& PopValue();
  T PopValue();

 private:
  std::stack<T> values_;
};

template<class T>
class DefaultRPN : public ReversePolish<T> {
 public:
  void HandleValue(const base_type& value);
  void HandleConstant(const char* name, const base_type& value);
  void HandleConstant(const std::string& name, const base_type& value);

 // private:
 //  T BinaryOperator(int operation, const T& left, const T& right);
 //  T UnaryOperator(int operation, const T& value);
 //  T FunctionOperator(int operation);
};

#include "reverse-polish-impl.hpp"

#endif  // REVERSE_POLISH_H_
