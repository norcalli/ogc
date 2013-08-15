#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <complex>
#include "floating-point.h"

template<class T>
std::ostream& operator<<(std::ostream& stream, std::complex<T> value) {
  // return stream << '(' << value.real() << ", " << value.imag() << ')';
  const FloatingPoint<T> real(value.real()), imag(value.imag()), zero(0);
  // if (value.real() != 0) {
  if (!real.AlmostEquals(zero)) {
    // stream << value.real() << ' ';
    stream << value.real();
    // if (value.imag() != 0) {
    if (!imag.AlmostEquals(zero)) {
      stream << ' ';
      double temp;
      // if (std::signbit(value.imag())) {
      if (imag.sign_bit()) {
        temp = -value.imag();
        stream << "- ";
      } else {
        temp = value.imag();
        stream << "+ ";
      }
      // double temp = fabs(value.imag());
      if (temp != 1)
        stream << temp << ' ';
      stream << "i ";
    }
  // } else if (value.imag() != 0) {
  } else if (!imag.AlmostEquals(zero)) {
    const FloatingPoint<T> one(1.0), none(-1.0);
    if (!imag.AlmostEquals(one)) {
      // if (imag.sign_bit())
      if (!imag.AlmostEquals(none))
        stream << '-';
      else
        stream << value.imag() << ' ';
    }
    // if (value.imag() != 1) {
    //   if (value.imag() != -1)
    //     stream << value.imag() << ' ';
    //   else
    //     stream << "- ";
    // }
    stream << "i ";
  } else {
    stream << '0';
  }
  return stream;
}

template<class T>
class Expression;
    
template<class T>
std::ostream& operator<<(std::ostream& stream, Expression<T>* value) {
  T v = value->Process(stream);
  return stream << " = " << v;
}

#endif  // PRINT_UTILS_H_
