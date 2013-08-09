#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <cstdio>
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
  // typedef double type;
  // typedef std::complex<double> type;
  typedef T type;

  virtual ~Expression() {}

  virtual type Process() = 0;

  // TODO: Experimental.
  virtual bool paren() const { return false; }
  // virtual bool mult() const { return false; }
  virtual Expression* mult() { return this; }
};


template<class T>
struct NumberExpr : Expression<T> {
  typedef typename Expression<T>::type type;
  NumberExpr(type val) : value(val) { }

  type Process() {
    // printf("%.15g", value);
    std::cout << value;
    return value;
  }

  /*Expression::*/type value;
};

// template<class T>
// struct NumberExpr : Expression<std::complex<T> > {
//   typedef typename Expression<T>::type type;
//   NumberExpr(type val) : value(val) { }

//   type Process() {
//     printf("%.15g", value);
//     return value;
//   }

//   /*Expression::*/type value;
// };

template<class T>
struct ConstantExpr : Expression<T> {
  typedef typename Expression<T>::type type;
  // ConstantExpr(const char* nam, type val) : value(val), name(nam) {}
  ConstantExpr(const std::string& nam, type val) : value(val), name(nam) {}

  type Process() {
    printf("%s", name.c_str());
    return value;
  }

  // const char* name;
  std::string name;
  type value;
};

template<class T>
struct UnaryExpr : Expression<T> {
  typedef typename Expression<T>::type type;

  virtual ~UnaryExpr() {
    delete expr;
  }
  // TODO: Experimental.
  UnaryExpr() : expr(NULL) {}
  // UnaryExpr(Expression* e) : expr(e) {}
  UnaryExpr(Expression<T>* e) : expr(e->mult()) {}

  Expression<T>* expr;
};

// struct Parenthesis : UnaryExpr {
//   Parenthesis(Expression* val) : disable(false) {
//     if (val->paren())
//       reinterpret_cast<Parenthesis*>(val)->disable = true;
//     expr = val;
//   }
//   type Process() {
//     type value;
//     if (disable)
//       value = expr->Process();
//     else {
//       printf("(");
//       value = expr->Process();
//       printf(")");
//     }
//     return value;
//     // return expr->Process();
//   }
//   bool paren() const { return true; }

//   bool disable;
// };

template<class T>
struct Parenthesis : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  Parenthesis() {}
  Parenthesis(Expression<T>* val) : UnaryExpr<T>(val) {
    if (this->expr->paren()) {
      Parenthesis* p = reinterpret_cast<Parenthesis*>(this->expr);
      this->expr = p->expr;
    }
  }
  type Process() {
    printf("(");
    type value = this->expr->Process();
    printf(")");
    return value;
    // return expr->Process();
  }
  bool paren() const { return true; }
};

template<class T>
struct NegateExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  NegateExpr(Expression<T>* val) : UnaryExpr<T>(val) { }
  type Process() {
    putchar('-');
    // printf("(-");
    type value = this->expr->Process();
    // putchar(')');
    return -value;
    // return -expr->Process();
  }
};

template<class T>
struct PosExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  type Process() {
    return this->expr->Process();
  }
};

// typedef Expression::type (*UnaryFunction)(Expression::type);
// typedef Expression<T>::type (*UnaryFunction)(const Expression::type&);

template<class T>
Expression<T>* WrapInParenthesis(Expression<T>* expr) {
  return expr->paren() ? expr : new Parenthesis<T>(expr);
}

template<class T>
struct FuncExpr : UnaryExpr<T> {
  typedef typename Expression<T>::type type;
  typedef type (*UnaryFunction)(const type&);

  // FuncExpr(const char* name, UnaryFunction function, Expression<T>* e) : name_(name), function_(function) { expr = e; }
  FuncExpr(const char* name, UnaryFunction function, Expression<T>* e) : name_(name), function_(function) { this->expr = WrapInParenthesis(e); }
  // UnaryExpr* operator()(Expression<T>* e) { expr = e; return this; }
  UnaryExpr<T>* operator()(Expression<T>* e) { this->expr = WrapInParenthesis(e); return this; }
  type Process() {
    printf("%s", name_);
    return function_(this->expr->Process());
    // return function_(new Parenthesis(expr->Process());
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
  // TODO: Experimental
  BinaryExpr() : left(NULL), right(NULL) {}
  // BinaryExpr(Expression<T>* l, Expression<T>* r) : left(l), right(r) { }
  BinaryExpr(Expression<T>* l, Expression<T>* r) : left(l->mult()), right(r->mult()) { }

  Expression<T>* left, *right;
};

template<class T>
struct PowExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;
  PowExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    // printf("(");
    type lval = this->left->Process();
    printf(" ^ ");
    type rval = this->right->Process();
    // printf(")");
    return pow(lval, rval);
  }
};

template<class T>
struct MinusExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;
  MinusExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    type lval = this->left->Process();
    printf(" - ");
    type rval = this->right->Process();
    return lval - rval;
    // return left->Process() - right->Process();
  }
};

template<class T>
struct PlusExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;
  PlusExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    type lval = this->left->Process();
    printf(" + ");
    type rval = this->right->Process();
    return lval + rval;
    // return left->Process() + right->Process();
  }
};

template<class T>
struct MultExpr : BinaryExpr<T> {
  typedef typename Expression<T>::type type;

  // MultExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr(l, r) {}
  MultExpr(Expression<T>* l, Expression<T>* r) {
    this->left = l;
    this->right = r;
  }

  type Process() {
    // printf("(");
    type lval = this->left->Process();
    printf(" * ");
    type rval = this->right->Process();
    // printf(")");
    return lval * rval;
    // return left->Process() * right->Process();
  }
  // bool mult() const { return true; }
  // Expression<T>* mult() { return this; }
  Expression<T>* mult() {
    return this;
    //return WrapInParenthesis(this);
    // Parenthesis<T>* result = new Parenthesis<T>;
    // result->expr = this;
    // return result;
//    return new Parenthesis<T>(this);
  }
};

template<class T>
struct DivExpr : BinaryExpr<T> {
  // typedef typename Expression<T>::type type;
  typedef T type;
  DivExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    // printf("(");
    type lval = this->left->Process();
    printf(" / ");
    type rval = this->right->Process();
    // printf(")");
    // if (rval == static_cast<type>(0))
    if (rval == 0)
    // if (rval == type())
      throw DivideByZero();
    // return left->Process() / rval;
    return lval / rval;
  }
};

template<class T>
struct ModuloExpr : BinaryExpr<T> {
  // typedef typename Expression<T>::type type;
  typedef T type;
  ModuloExpr(Expression<T>* l, Expression<T>* r) : BinaryExpr<T>(l, r) {}

  type Process() {
    // printf("(");
    type lval = this->left->Process();
    printf(" %% ");
    type rval = this->right->Process();
    // printf(")");
    if (rval == static_cast<type>(0))
      throw DivideByZero();
    // return left->Process() / rval;
    return modulo(lval, rval);
  }
};

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
  inline static type negate(type a) { return new NegateExpr<base>(a); }
  inline static type modulo(type a, type b) { return new ModuloExpr<base>(a, b); }
  inline static type pow(type a, type b) { return new PowExpr<base>(a, b); }

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
