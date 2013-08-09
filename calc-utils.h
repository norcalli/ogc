#ifndef CALC_UTILS_H_
#define CALC_UTILS_H_

#include "string-utils.h"

static const int kFunctionStart = 0x100;
static const int kFunctionLast  = 0x300;
enum BaseFunctions { kNeg = kFunctionStart, kSin, kCos, kTan, kExp, kLog, kAsin, kAcos, kAtan, kLog10 };
// enum Functions { kStart, kNeg = kStart, kLast };

static inline bool InRange(int value, int minimum, int maximum) {
  return static_cast<unsigned>(value - minimum) <= (maximum - minimum);
}

inline bool IsFunctionSeparator(char c) {
  return c == ',';
}

inline bool IsGeometric(char c) {
  return c == '*' || c == '/' || c == '%';
}

inline bool IsArithmetic(char c) {
  return c == '-' || c == '+';
}

inline bool IsParenthesis(char c) {
  return c == '(' || c == ')';
}

inline bool IsCommutative(int c) {
  return c == '*' || IsArithmetic(c);
}

inline bool IsBinary(char c) {
  return IsArithmetic(c) || IsGeometric(c) || c == '^';
}

inline bool IsUnary(int c) {
  return InRange(c, kFunctionStart, kFunctionLast);
  // return static_cast<unsigned>(c - kStart) < kLast;// = kNeg;
}

inline bool IsFunction(int c) {
  return IsUnary(c);
}

inline bool IsOperator(char c) {
  return IsArithmetic(c) || IsGeometric(c) || IsFunction(c)
      || c == '^' || IsParenthesis(c);
}

inline bool LeftAssociated(char c) {
  return IsGeometric(c) || IsArithmetic(c);
}

inline int Precedence(int c) {
  // If Precedence(kNeg) == Precedence('^'), then -4^2 = -16.
  // If Precedence(kNeg) > Precedence('^'), then -4^2 = (-4)^2 = 16.
  if (IsFunction(c))    return 9;
  if (c == '^')         return 9;
  if (IsGeometric(c))   return 8;
  if (IsArithmetic(c))  return 5;
  if (IsParenthesis(c)) return 1;
  return -1;
}

#endif  // CALC_UTILS_H_
