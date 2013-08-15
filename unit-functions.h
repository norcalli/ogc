#ifndef __UNIT_FUNCTIONS_H__
#define __UNIT_FUNCTIONS_H__

#include "number-utilities.h"
#include "number-parse.h"
#include "units.h"
#include <cmath>
#include <complex>

namespace std {

template<class T>
inline std::complex<T> ceil(std::complex<T> a) {
  return std::complex<T>(ceil(a.real()), ceil(a.imag()));
}

template<class T>
inline std::complex<T> floor(std::complex<T> a) {
  return std::complex<T>(floor(a.real()), floor(a.imag()));
}

template<class T>
inline T round(const T& a) {
  return floor(a + static_cast<T>(0.5));
  // return floor(a + 0.5);
}

inline bool is_int(int value) {
  return true;
}

inline bool is_int(float value) {
  return fabs(value - round(value)) < 1e-14;
}

inline bool is_int(double value) {
  return fabs(value - round(value)) < 1e-14;
}

inline bool is_int(const std::complex<int>& value) {
  return true;
}

inline bool is_int(const std::complex<float>& value) {
  return abs(value - round(value)) < 1e-14;
}

inline bool is_int(const std::complex<double>& value) {
  return abs(value - round(value)) < 1e-14;
}

// template<class T>
// Units<std::complex<T> > pow(const std::complex<T>& x, int y) {
//   if (x.value.imag() == 0)
//     // return ipow(x.value.real(), y);
//     return pow(x.value.real(), y);
//   // return ipow(x.value, y);
//   return pow(x.value, y);
// }

// template<class T>
// Units<std::complex<T> > pow(const std::complex<T>& x, double y) {
//   if (x.value.imag() == 0)
//     // return ipow(x.value.real(), y);
//     return pow(x.value.real(), y);
//   // return ipow(x.value, y);
//   return pow(x.value, y);
// }

template<class T>
Units<std::complex<T> > pow(const Units<std::complex<T>>& x, int y) {
  Dimensions dim = x.dimensions;
  dim.charge *= y;
  dim.mass *= y;
  dim.time *= y;
  dim.length *= y;
  return {pow(x.value, y), dim};
}

template<class T>
Units<std::complex<T> > pow(const Units<std::complex<T>>& x, double y) {
  if (representable(y))
    return pow(x, int(y));
  Dimensions dim = x.dimensions;
  T charge = dim.charge * y;
  T mass = dim.mass * y;
  T time = dim.time * y;
  T length = dim.length * y;
  if (representable(charge) && representable(mass)
      && representable(time) && representable(length)) {
    dim.charge = static_cast<Dimensions::value>(charge);
    dim.mass = static_cast<Dimensions::value>(mass);
    dim.time = static_cast<Dimensions::value>(time);
    dim.length = static_cast<Dimensions::value>(length);
  }
  return {pow(x.value, y), dim};
}

// template<class T>
// Units<std::complex<T> > pow(const std::complex<T>& x, const std::complex<T>& y) {
//   if (y.imag() == 0)
//     return pow(x, y.real());
//   return exp(y*log(x));
// }

template<class T>
Units<std::complex<T> > pow(const Units<std::complex<T>>& x, const std::complex<T>& y) {
  if (y.imag() == 0)
    return pow(x, y.real());
  if (!x.DimensionLess())
    throw std::runtime_error("Applying imaginary power to dimensional base");
  return pow(x.value, y.real());
}

template<class T>
Units<std::complex<T> > pow(const Units<std::complex<T> >& x, const Units<std::complex<T> >& y) {
  if (!y.dimensions.DimensionLess())
    throw std::runtime_error("Applying pow to non-dimensionless exponent");
  return pow(x, y.value);

  // if (!y.dimensions.DimensionLess())
  //   throw std::runtime_error("Applying pow to non-dimensionless exponent");
  // if (!x.dimensions.DimensionLess() && y.value.imag() != 0)
  //   throw std::runtime_error("Applying imaginary power to dimensional base");

  // // if (representable(y.value.real()))
  // Dimensions dim = x.dimensions;
  // T charge = dim.charge * y.value.real();
  // T mass = dim.mass * y.value.real();
  // T time = dim.time * y.value.real();
  // T length = dim.length * y.value.real();
  // if (is_int(charge) && is_int(mass) && is_int(time) && is_int(length)) {
  //   dim.charge = static_cast<Dimensions::value>(charge);
  //   dim.mass = static_cast<Dimensions::value>(mass);
  //   dim.time = static_cast<Dimensions::value>(time);
  //   dim.length = static_cast<Dimensions::value>(length);
  // } else {
  //   throw std::runtime_error("Pow results in non-integer units.");
  // }

  // if (y.value.imag() == 0 && representable(y.value.real()))
  //   return Units<std::complex<T> >(ipow(x.value, int(y.value.real())), dim);
  // return Units<std::complex<T> >(std::pow(x.value, y.value), dim);
}
template<class T>
Units<T> pow(const Units<T>& x, const Units<T>& y) {
  if (!y.dimensions.DimensionLess())
    throw std::runtime_error("Applying pow to non-dimensionless exponent");
  Dimensions dim = x.dimensions;
  T charge = dim.charge * y.value;
  T mass = dim.mass * y.value;
  T time = dim.time * y.value;
  T length = dim.length * y.value;
  if (is_int(charge) && is_int(mass) && is_int(time) && is_int(length)) {
    dim.charge = static_cast<Dimensions::value>(charge);
    dim.mass = static_cast<Dimensions::value>(mass);
    dim.time = static_cast<Dimensions::value>(time);
    dim.length = static_cast<Dimensions::value>(length);
  } else {
    throw std::runtime_error("Pow results in non-integer units.");
  }

  return Units<T>(std::pow(x.value, y.value), dim);
}

template<class T>
Units<T> pow(const Units<T>& x, int y) {
  Dimensions dim = x.dimensions;
  dim.charge *= y;
  dim.mass *= y;
  dim.time *= y;
  dim.length *= y;
  return Units<T>(std::pow(x.value, y), dim);
}

template<class T>
Units<T> sqrt(const Units<T>& val) {
  Dimensions dim = val.dimensions;
  if (dim.charge % 2)
    throw std::runtime_error("sqrt: charge is not even");
  dim.charge /= 2;
  if (dim.mass % 2)
    throw std::runtime_error("sqrt: mass is not even");
  dim.mass /= 2;
  if (dim.time % 2)
    throw std::runtime_error("sqrt: time is not even");
  dim.time /= 2;
  if (dim.length % 2)
    throw std::runtime_error("sqrt: length is not even");
  dim.length /= 2;
  return Units<T>(std::sqrt(val.value), dim);
}


// TODO: Can you call floor on a non dimensionless? I think so...
template<class T>
Units<T> floor(const Units<T>& val) {
  // if (!val.dimensions.DimensionLess())
  //   throw std::runtime_error("Applying floor to non-dimensionless value");
  return Units<T>(std::floor(val.value), val.dimensions);
}

template<class T>
Units<T> ceil(const Units<T>& val) {
  // if (!val.dimensions.DimensionLess())
  //   throw std::runtime_error("Applying ceil to non-dimensionless value");
  return Units<T>(std::ceil(val.value), val.dimensions);
}

#define DIMENSIONLESS_STD_ADAPTER(fn)\
template<class T>\
Units<T> fn(const Units<T>& val) {\
  if (!val.dimensions.DimensionLess())\
    throw std::runtime_error("Applying " #fn " to non-dimensionless value");\
  return Units<T>(std::fn(val.value), val.dimensions);\
}

DIMENSIONLESS_STD_ADAPTER(exp);
DIMENSIONLESS_STD_ADAPTER(log10);
DIMENSIONLESS_STD_ADAPTER(log);
DIMENSIONLESS_STD_ADAPTER(cos);
DIMENSIONLESS_STD_ADAPTER(sin);
DIMENSIONLESS_STD_ADAPTER(tan);
DIMENSIONLESS_STD_ADAPTER(acos);
DIMENSIONLESS_STD_ADAPTER(asin);
DIMENSIONLESS_STD_ADAPTER(atan);
DIMENSIONLESS_STD_ADAPTER(cosh);
DIMENSIONLESS_STD_ADAPTER(sinh);
DIMENSIONLESS_STD_ADAPTER(tanh);

}  // std

// TODO: I could just make it call log10() and scale by log2, but iono.
template<class T>
Units<T> log2(const Units<T>& val) {
  const static double log10_2 = std::log10(2);
  if (!val.dimensions.DimensionLess())
    throw std::runtime_error("Applying log2 to non-dimensionless value");
  if (val.value.imag() != 0)
    // throw std::runtime_error("Applying log2 to a complex value.")
    return Units<T>(::log2(val.value.real()), val.dimensions);
  else
    return std::log10(val) / log10_2;
}


#endif  // __UNIT_FUNCTIONS_H__
