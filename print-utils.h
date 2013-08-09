#ifndef PRINT_UTILS_H_
#define PRINT_UTILS_H_

#include <complex>

template<class T>
std::ostream& operator<<(std::ostream& stream, std::complex<T> value) {
  if (value.real() != 0) {
    // stream << value.real() << ' ';
    stream << value.real();
    if (value.imag() != 0) {
      stream << ' ';
      double temp;
      if (std::signbit(value.imag())) {
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
  } else if (value.imag() != 0) {
    if (value.imag() != 1) {
      if (value.imag() != -1)
        stream << value.imag() << ' ';
      else
        stream << "- ";
    }
    stream << "i ";
  } else {
    stream << '0';
  }
  return stream;
}

#endif  // PRINT_UTILS_H_
