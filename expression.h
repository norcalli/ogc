#ifndef EXPRESSION_H_
#define EXPRESSION_H_

// #include <cstdio>
#include <iostream>
#include <cmath>
#include <complex>
#include "operator.h"

struct ExpressionException {
  ExpressionException(const char* value) : what(value) {}
  
  const char* what;
};

struct DivideByZero : ExpressionException {
  DivideByZero() : ExpressionException("Divided by zero!") {}
};

template<class T>
struct Expression {
  typedef T type;

  virtual ~Expression() {}

  virtual type Process() = 0;
  virtual type Process(std::ostream&) = 0;

  // TODO: Experimental.
  virtual bool paren() const { return false; }
  // virtual bool mult() const { return false; }
  virtual Expression* mult() { return this; }
};


template<class T>
struct NumberExpr : Expression<T> {
  typedef typename Expression<T>::type type;
  NumberExpr(type val) : value(val) { }

  type Process() { return value; }

  type Process(std::ostream& stream) {
    stream << value;
    return value;
  }

  type value;
};

template<class T>
struct ConstantExpr : Expression<T> {
  typedef typename Expression<T>::type type;

  ConstantExpr(const std::string& nam, type val) : value(val), name(nam) {}

  type Process() { return value; }

  type Process(std::ostream& stream) {
    stream << name;
    return value;
  }

  std::string name;
  type value;
};

template<class T>
struct UnaryExpr : Expression<T> {
  typedef typename Expression<T>::type type;

  virtual ~UnaryExpr() { delete expr; }

  UnaryExpr(Expression<T>* e) : expr(e->mult()) {}

  Expression<T>* expr;
};

template<class T>
struct Parenthesis : UnaryExpr<T> {
  typedef typename Expression<T>::type type;

  Parenthesis(Expression<T>* val) : UnaryExpr<T>(val) {
    // if (this->expr->paren()) {
    //   Parenthesis* p = reinterpret_cast<Parenthesis*>(this->expr);
    //   this->expr = p->expr;
    // }
  }
  type Process() { return this->expr->Process(); }

  type Process(std::ostream& stream) {
    type value = this->expr->Process(stream << '(');
    stream << ')';
    return value;
  }

  bool paren() const { return true; }
};

// #define UNARY_OPERATION_EXPRESSION(name, op)\
// template<class T>\
// struct name##Expr : UnaryExpr<T> {\
//   typedef typename Expression<T>::type type;\
//   name##Expr(Expression<T>* val) : UnaryExpr<T>(val) { }\
//   type Process() {\
//     return op this->expr->Process();\
//   }\
// \
//   type Process(std::ostream& stream) {\
//     return this->expr->Process(stream << '-');\
//   }\
// };
template<class T>
Expression<T>* WrapInParenthesis(Expression<T>* expr) {
  return expr->paren() ? expr : new Parenthesis<T>(expr);
}

template<class T>
T WrapInParenthesis(T expr) {
  return expr;
}

template<class T>
struct NegateExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  #ifdef DEBUG
  NegateExpr(Expression<T>* val) : UnaryExpr<T>(WrapInParenthesis(val)) { }
  #else
  NegateExpr(Expression<T>* val) : UnaryExpr<T>(val) { }
  #endif  // DEBUG

  type Process() { return -this->expr->Process(); }

  type Process(std::ostream& stream) {
    return -this->expr->Process(stream << '-');
  }
};

template<class T>
struct PosExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;

  type Process() { return this->expr->Process(); }

  type Process(std::ostream& stream) {
    return this->expr->Process(stream << '+');
  }
};

// typedef Expression::type (*UnaryFunction)(Expression::type);
// typedef Expression<T>::type (*UnaryFunction)(const Expression::type&);


template<class T>
struct FuncExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  typedef type (*UnaryFunction)(const type&);

  FuncExpr(const char* name, UnaryFunction function, Expression<T>* e)
  : name_(name), function_(function), UnaryExpr<T>(e) {
    #ifdef DEBUG
   this->expr = WrapInParenthesis(e);
   #endif  // DEBUG
  }

  // UnaryExpr<T>* operator()(Expression<T>* e) {
  //   this->expr = WrapInParenthesis(e); return this;
  // }
  type Process() {
    return function_(this->expr->Process());
  }
  type Process(std::ostream& stream) {
    return function_(this->expr->Process(stream << name_));
  }
 private:
  UnaryFunction function_;
  const char* name_;
};

template<class T>
struct BinaryExpr : Expression<T> {
  virtual ~BinaryExpr() {
    delete left;
    delete right;
  }
  BinaryExpr(Expression<T>* l, Expression<T>* r)
  #ifdef DEBUG
   : left(WrapInParenthesis(l)), right(WrapInParenthesis(r)) { }
  #else
   : left(l->mult()), right(r->mult()) { }
  #endif  // DEBUG

  Expression<T>* left, *right;
};

template<class T>
struct PowExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;
  PowExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    return pow(this->left->Process(), this->right->Process());
  }
  type Process(std::ostream& stream) {
    type left = this->left->Process(stream);
    return pow(left,
               this->right->Process(stream << " ^ "));
  }
};

template<class T>
struct ModuloExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;
  ModuloExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    return modulo(this->left->Process(), this->right->Process());
  }
  type Process(std::ostream& stream) {
    type left = this->left->Process(stream);
    return modulo(left,
                  this->right->Process(stream << " % "));
  }
};

#define BINARY_OPERATION_EXPRESSION(name, op)\
template<class T>\
struct name##Expr : BinaryExpr<T> {\
  typedef typename Expression<T>::type type;\
  name##Expr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}\
  \
  type Process() {\
    return this->left->Process() op this->right->Process();\
  }\
  type Process(std::ostream& stream) {\
    type left = this->left->Process(stream);\
    return left op this->right->Process(stream << " " #op " ");\
  }\
};

BINARY_OPERATION_EXPRESSION(Minus, -);
BINARY_OPERATION_EXPRESSION(Plus, +);
BINARY_OPERATION_EXPRESSION(Mult, *);
BINARY_OPERATION_EXPRESSION(Div, /);


template<class T>
struct Operations<Expression<T>*> {
  using type   = Expression<T>*;
  // using base   = double;
  // using base   = std::complex<double>;
  // using base = typename type::type;
  using base   = T;
  using Unary  = type(type);
  using Binary = type(type, type);
  using Nary   = type(const std::vector<type>&);

  inline static type plus(type a, type b) { return new PlusExpr<T>(a, b); }
  inline static type minus(type a, type b) { return new MinusExpr<T>(a, b); }
  inline static type multiply(type a, type b) { return new MultExpr<T>(a, b); }
  inline static type divide(type a, type b) { return new DivExpr<T>(a, b); }
  inline static type negate(type a) { return new NegateExpr<T>(a); }
  inline static type modulo(type a, type b) { return new ModuloExpr<T>(a, b); }
  inline static type pow(type a, type b) { return new PowExpr<T>(a, b); }

  inline static type sin(type a) { return new FuncExpr<T>("sin", Operations<base>::sin, a); }
  inline static type cos(type a) { return new FuncExpr<T>("cos", Operations<base>::cos, a); }
  inline static type tan(type a) { return new FuncExpr<T>("tan", Operations<base>::tan, a); }
  inline static type asin(type a) { return new FuncExpr<T>("asin", Operations<base>::asin, a); }
  inline static type acos(type a) { return new FuncExpr<T>("acos", Operations<base>::acos, a); }
  inline static type atan(type a) { return new FuncExpr<T>("atan", Operations<base>::atan, a); }
  inline static type exp(type a) { return new FuncExpr<T>("exp", Operations<base>::exp, a); }
  inline static type log(type a) { return new FuncExpr<T>("log", Operations<base>::log, a); }
  inline static type log10(type a) { return new FuncExpr<T>("log10", Operations<base>::log10, a); }
  inline static type log2(type a) { return new FuncExpr<T>("log2", Operations<base>::log2, a); }
  inline static type sqrt(type a) { return new FuncExpr<T>("sqrt", Operations<base>::sqrt, a); }

  inline static type paren(type a) { return new Parenthesis<T>(a); }
};

template<class T>
struct Operations<Expression<std::complex<T> >*> {
  // using type   = Expression*;
  using type = Expression<std::complex<T> >*;
  // using base   = double;
  using base   = std::complex<T>;
  // using base = typename std::compl::type;
  using Unary  = type(type);
  using Binary = type(type, type);
  using Nary   = type(const std::vector<type>&);

  inline static type plus(type a, type b) { return new PlusExpr<base>(a, b); }
  inline static type minus(type a, type b) { return new MinusExpr<base>(a, b); }
  inline static type multiply(type a, type b) { return new MultExpr<base>(a, b); }
  inline static type divide(type a, type b) { return new DivExpr<base>(a, b); }
  inline static type negate(type a) { return new NegateExpr<base>(a); }
  // inline static type modulo(type a, type b) { return new ModuloExpr<base>(a, b); }
  inline static type pow(type a, type b) { return new PowExpr<base>(a, b); }

  inline static type sin(type a) { return new FuncExpr<base>("sin", Operations<base>::sin, a); }
  inline static type cos(type a) { return new FuncExpr<base>("cos", Operations<base>::cos, a); }
  inline static type tan(type a) { return new FuncExpr<base>("tan", Operations<base>::tan, a); }
  // inline static type asin(type a) { return new FuncExpr<base>("asin", Operations<base>::asin, a); }
  // inline static type acos(type a) { return new FuncExpr<base>("acos", Operations<base>::acos, a); }
  // inline static type atan(type a) { return new FuncExpr<base>("atan", Operations<base>::atan, a); }
  inline static type exp(type a) { return new FuncExpr<base>("exp", Operations<base>::exp, a); }
  inline static type log(type a) { return new FuncExpr<base>("log", Operations<base>::log, a); }
  inline static type log10(type a) { return new FuncExpr<base>("log10", Operations<base>::log10, a); }
  inline static type log2(type a) { return new FuncExpr<base>("log2", Operations<base>::log2, a); }
  inline static type sqrt(type a) { return new FuncExpr<base>("sqrt", Operations<base>::sqrt, a); }

  inline static type paren(type a) { return new Parenthesis<base>(a); }
};

template<class T>
struct Operations<Expression<Units<T> >*> {
  // using type   = Expression*;
  using type = Expression<Units<T> >*;
  // using base   = double;
  using base   = Units<T>;
  // using base = typename std::compl::type;
  using Unary  = type(type);
  using Binary = type(type, type);
  using Nary   = type(const std::vector<type>&);

  inline static type plus(type a, type b) { return new PlusExpr<base>(a, b); }
  inline static type minus(type a, type b) { return new MinusExpr<base>(a, b); }
  inline static type multiply(type a, type b) { return new MultExpr<base>(a, b); }
  inline static type divide(type a, type b) { return new DivExpr<base>(a, b); }
  // inline static type floor(type a) { return new NegateExpr<base>(a); }
  inline static type negate(type a) { return new NegateExpr<base>(a); }
  inline static type modulo(type a, type b) { return new ModuloExpr<base>(a, b); }
  inline static type pow(type a, type b) { return new PowExpr<base>(a, b); }

  inline static type floor(type a) { return new FuncExpr<base>("floor", Operations<base>::floor, a); }
  inline static type ceil(type a) { return new FuncExpr<base>("ceil", Operations<base>::ceil, a); }

  inline static type sin(type a) { return new FuncExpr<base>("sin", Operations<base>::sin, a); }
  inline static type cos(type a) { return new FuncExpr<base>("cos", Operations<base>::cos, a); }
  inline static type tan(type a) { return new FuncExpr<base>("tan", Operations<base>::tan, a); }
  inline static type asin(type a) { return new FuncExpr<base>("asin", Operations<base>::asin, a); }
  inline static type acos(type a) { return new FuncExpr<base>("acos", Operations<base>::acos, a); }
  inline static type atan(type a) { return new FuncExpr<base>("atan", Operations<base>::atan, a); }
  inline static type exp(type a) { return new FuncExpr<base>("exp", Operations<base>::exp, a); }
  inline static type log(type a) { return new FuncExpr<base>("log", Operations<base>::log, a); }
  inline static type log10(type a) { return new FuncExpr<base>("log10", Operations<base>::log10, a); }
  inline static type log2(type a) { return new FuncExpr<base>("log2", Operations<base>::log2, a); }
  inline static type sqrt(type a) { return new FuncExpr<base>("sqrt", Operations<base>::sqrt, a); }

  inline static type paren(type a) { return new Parenthesis<base>(a); }
};


#endif  // EXPRESSION_H_
