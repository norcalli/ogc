#ifndef EXPR_H_
#define EXPR_H_

#include <cmath>

namespace math {

template<class T>
inline T modulo(const T& a, const T& b) {
  return a % b;
}

template<>
inline double modulo<double>(const double& a, const double& b) {
  return fmod(a, b);
}

template<class T>
struct Expression {
  typedef T type;
  // virtual ~Expression() = delete;
  virtual void destroy() { delete this; }
  virtual T Process() const = 0;
};

template<class T>
struct Expr {
  struct Number : Expression<T> {
    Number(const T& value) : value_(value) {}
    T Process() const { return this->value_; }
   private:
    T value_;
  };
  struct Unary : Expression<T> {
    Unary(Expression<T>* value) : value_(value) {}
    void value(Expression<T>* value) { value_ = value; }
    inline Expression<T>* value() const { return value_; }
    void destroy() { delete value_; delete this; }
   protected:
    Expression<T>* value_;
  };
  struct Negate : Unary {
    Negate(Expression<T>* value) : Unary(value) {}
    T Process() const { return -this->value_->Process(); }
  };
  struct Parenthesis : Unary {
    Parenthesis(Expression<T>* value) : Unary(value) {}
    T Process() const { return this->value_->Process(); }
  };
  struct Binary : Expression<T> {
    Binary(Expression<T>* left, Expression<T>* right) : left_(left), right_(right) {}
    void left(Expression<T>* value) { left_ = value; }
    Expression<T>* left() const { return left_; }
    void right(Expression<T>* value) { right_ = value; }
    Expression<T>* right() const { return right_; }
    void destroy() { delete left_; delete right_; delete this; }
   protected:
    Expression<T>* left_, *right_;
  };
  struct Addition : Binary {
    Addition(Expression<T>* left, Expression<T>* right) : Binary(left, right) {}
    T Process() const { return this->left_->Process() + this->right_->Process(); }
  };
  struct Subtraction : Binary {
    Subtraction(Expression<T>* left, Expression<T>* right) : Binary(left, right) {}
    T Process() const { return this->left_->Process() - this->right_->Process(); }
  };
  struct Multiplication : Binary {
    Multiplication(Expression<T>* left, Expression<T>* right) : Binary(left, right) {}
    T Process() const { return this->left_->Process() * this->right_->Process(); }
  };
  struct Division : Binary {
    Division(Expression<T>* left, Expression<T>* right) : Binary(left, right) {}
    T Process() const { return this->left_->Process() / this->right_->Process(); }
  };
  struct Modulo : Binary {
    Modulo(Expression<T>* left, Expression<T>* right) : Binary(left, right) {}
    T Process() const { return modulo(this->left_->Process(), this->right_->Process()); }
  };
};
}


#endif  // EXPR_H_
