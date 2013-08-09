#ifndef __UNITS_H__
#define __UNITS_H__

#include <complex>
#include <stdexcept>

typedef std::complex<double> unit_value;

struct Dimensions {
  typedef signed char value;

  inline bool DimensionLess() const {
    return !(charge || mass || time || length);
  }

  value charge;
  value mass;
  value time;
  value length;
  // value temperature;
};

#define UNITS_COMP_MEMBER(op)                   \
  bool operator op(const Units& b) const {      \
    CheckCompatibility(b);                      \
    return value op b.value;                    \
  }// \
  // bool operator op(const T& b) const {          \
  //   return value op b;                          \
  // }

#define UNITS_COMP_GLOBAL(op)                                   \
  template<class T>                                             \
  bool operator op(const Units<T>& a, const Units<T>& b) {      \
    a.CheckCompatibility(b);                                    \
    return a.value op b.value;                                  \
  }
  // template<class T>                                             \
  // bool operator op(const Units<T>& a, const T& b) {             \
  //   return a.value op b;                                        \
  // }                                                             \
  // template<class T>                                             \
  // bool operator op(const T& a, const Units<T>& b) {             \
  //   return b.value op a;                                        \
  // }

template<class T>
struct Units {
  typedef T value_type;

  Units() {
    dimensions.charge = 0;
    dimensions.mass = 0;
    dimensions.time = 0;
    dimensions.length = 0;
    // value = 1;
  }

  Units(const double& val) {
    value = val;
    dimensions = {0};
  }
  Units(const T& val) {
    value = val;
    dimensions = {0};
  }

  Units(const T& val, const Dimensions& dim) {
    value = val;
    dimensions = dim;
  }

  Units(const T& val,
        Dimensions::value charge,
        Dimensions::value mass,
        Dimensions::value time,
        Dimensions::value length) {
    value = val;
    dimensions.charge = charge;
    dimensions.mass = mass;
    dimensions.time = time;
    dimensions.length = length;
  }

  Units& operator-() {
    value = -value;
    return *this;
  }

  Units& operator*=(const T& b) {
    value *= b;
    return *this;
  }

  Units& operator/=(const T& b) {
    value /= b;
    return *this;
  }

  Units operator*(const T& b) {
    Units temp = *this;
    return temp *= b;
  }

  Units operator/(const T& b) {
    Units temp = *this;
    return temp /= b;
  }

  Units& operator*=(const int& b) {
    value *= b;
    return *this;
  }

  Units& operator/=(const int& b) {
    value /= b;
    return *this;
  }

  Units operator*(const int& b) {
    Units temp = *this;
    return temp *= b;
  }

  Units operator/(const int& b) {
    Units temp = *this;
    return temp /= (T)b;
  }

  Units& operator*=(const double& b) {
    value *= b;
    return *this;
  }

  Units& operator/=(const double& b) {
    value /= b;
    return *this;
  }

  Units operator*(const double& b) {
    Units temp = *this;
    return temp *= b;
  }

  Units operator/(const double& b) {
    Units temp = *this;
    return temp /= (T)b;
  }

  Units& operator+=(const Units& b) {
    CheckCompatibility(b);
    value += b.value;
    return *this;
  }

  Units& operator-=(const Units& b) {
    CheckCompatibility(b);
    value -= b.value;
    return *this;
  }

  Units& operator*=(const Units& b) {
    value *= b.value;
    dimensions.charge += b.dimensions.charge;
    dimensions.mass += b.dimensions.mass;
    dimensions.time += b.dimensions.time;
    dimensions.length += b.dimensions.length;
    return *this;
  }

  Units& operator/=(const Units& b) {
    value /= b.value;
    dimensions.charge -= b.dimensions.charge;
    dimensions.mass -= b.dimensions.mass;
    dimensions.time -= b.dimensions.time;
    dimensions.length -= b.dimensions.length;
    return *this;
  }

  Units operator*(const Units& b) const {
    Units temp = *this;
    return temp *= b;
  }

  Units operator/(const Units& b) const {
    Units temp = *this;
    return temp /= b;
  }

  Units operator+(const Units& b) const {
    Units temp = *this;
    return temp += b;
  }

  Units operator-(const Units& b) const {
    Units temp = *this;
    return temp -= b;
  }

  Units operator-() const {
    Units temp = *this;
    temp.value = -temp.value;
    return temp;
  }

  Units operator+() const {
    return *this;
  }

  UNITS_COMP_MEMBER(==);
  UNITS_COMP_MEMBER(>=);
  UNITS_COMP_MEMBER(<=);
  UNITS_COMP_MEMBER(>);
  UNITS_COMP_MEMBER(<);

  bool CheckCompatibility(const Units& b) const {
    // TODO: Hacky, should find better way to compare dimensionless.
    if (value == T(0) || b.value == T(0))
      return true;
    // if (b.dimensions.DimensionLess() || b.value == T(0))
    //   return b.dimensions.DimensionLess();
    bool compatible =
        (dimensions.charge == b.dimensions.charge)
        && (dimensions.mass == b.dimensions.mass)
        && (dimensions.time == b.dimensions.time)
        && (dimensions.length == b.dimensions.length);
    if (!compatible)
      throw std::runtime_error("Incompatible types!");
    return compatible;
  }

  T value;
  Dimensions dimensions;
};

UNITS_COMP_GLOBAL(==);
UNITS_COMP_GLOBAL(>=);
UNITS_COMP_GLOBAL(<=);
UNITS_COMP_GLOBAL(>);
UNITS_COMP_GLOBAL(<);

#undef UNITS_COMP_GLOBAL
#undef UNITS_COMP_MEMBER

template<class T>
Units<T> operator+(const Units<T>& lhs, const Units<T>& rhs) {
  Units<T> temp = lhs;
  return temp += rhs;
}

template<class T>
Units<T> operator-(const Units<T>& lhs, const Units<T>& rhs) {
  Units<T> temp = lhs;
  return temp -= rhs;
}

template<class T>
Units<T> operator*(const Units<T>& lhs, const Units<T>& rhs) {
  Units<T> temp = lhs;
  return temp *= rhs;
}

template<class T>
Units<T> operator*(const Units<T>& lhs, const T& rhs) {
  Units<T> temp = lhs;
  return temp *= rhs;
}

template<class T>
Units<T> operator*(const T& lhs, const Units<T>& rhs) {
  Units<T> temp = lhs;
  return temp *= lhs;
}

template<class T>
Units<T> operator/(const Units<T>& lhs, const Units<T>& rhs) {
  Units<T> temp = lhs;
  return temp /= rhs;
}

template<class T>
Units<T> operator/(const Units<T>& lhs, const T& rhs) {
  Units<T> temp = lhs;
  return temp /= rhs;
}

template<class T>
Units<T> operator/(const T& lhs, const Units<T>& rhs) {
  Units<T> temp;
  temp.value = lhs / rhs.value;
  temp.charge = -rhs.charge;
  temp.mass = -rhs.mass;
  temp.time = -rhs.time;
  temp.length = -rhs.length;
  return temp;
}

template<class T>
Units<T> operator*(const Units<T>& lhs, const int& rhs) {
  return lhs * (T)rhs;
}

template<class T>
Units<T> operator*(const int& lhs, const Units<T>& rhs) {
  return rhs * (T)lhs;
}

template<class T>
Units<T> operator/(const Units<T>& lhs, const int& rhs) {
  return lhs / (T)rhs;
}

template<class T>
Units<T> operator/(const int& lhs, const Units<T>& rhs) {
  Units<T> temp;
  temp.value = lhs / rhs.value;
  temp.charge = -rhs.charge;
  temp.mass = -rhs.mass;
  temp.time = -rhs.time;
  temp.length = -rhs.length;
  return temp;
}

template<class T>
Units<T> operator*(const Units<T>& lhs, const double& rhs) {
  return lhs * (T)rhs;
}

template<class T>
Units<T> operator*(const double& lhs, const Units<T>& rhs) {
  return rhs * (T)lhs;
}

template<class T>
Units<T> operator/(const Units<T>& lhs, const double& rhs) {
  return lhs / (T)rhs;
}

template<class T>
Units<T> operator/(const double& lhs, const Units<T>& rhs) {
  Units<T> temp;
  temp.value = lhs / rhs.value;
  temp.charge = -rhs.charge;
  temp.mass = -rhs.mass;
  temp.time = -rhs.time;
  temp.length = -rhs.length;
  return temp;
}

template<class T>
bool CheckCompatibility(const Units<T>& a, const Units<T>& b) {
  bool compatible =
      (a.dimensions.charge == b.dimensions.charge)
      && (a.dimensions.mass == b.dimensions.mass)
      && (a.dimensions.time == b.dimensions.time)
      && (a.dimensions.length == b.dimensions.length);
  if (!compatible)
    throw std::runtime_error("Incompatible types!");
  return compatible;
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const Units<T>& unit) {
  stream << unit.value;
  Dimensions dim = unit.dimensions;
  if (!dim.DimensionLess()) {
    stream << " [[ ";
    if (dim.charge) {
      // if (dim.charge == -1)
      //   stream << "/ ";
      stream << "coulomb";
      if (dim.charge != 1)
        stream << '^' << (int)dim.charge;
      stream << ' ';
    }
    if (dim.length) {
      // if (dim.length == -1)
      //   stream << "/ ";
      stream << "meter";
      // if (dim.length != 1 && dim.length != -1)
      if (dim.length != 1)
        stream << '^' << (int)dim.length;
      stream << ' ';
    }
    if (dim.mass) {
      // if (dim.mass == -1)
      //   stream << "/ ";
      stream << "kilogram";
      // if (dim.mass != 1 && dim.mass != -1)
      if (dim.mass != 1)
        stream << '^' << (int)dim.mass;
      stream << ' ';
    }
    if (dim.time) {
      // if (dim.time == -1)
      //   stream << "/ ";
      stream << "second";
      // if (dim.time != 1 && dim.time != -1)
      if (dim.time != 1)
        stream << '^' << (int)dim.time;
      stream << ' ';
    }
    stream << "]]";
  }
  return stream;
}

template<class T>
Units<T> ConvertUnits(const Units<T> &from, const Units<T> &to) {
  CheckCompatibility(from, to);
  return Units<T>(from.value / to.value, from.dimensions);
}

// const static Units<unit_value> kDimensionLess (1, 0, 0, 0, 0);

// namespace SI {
// namespace Prefix {
// const static Units<unit_value> kYocto ( 1e-24, 0, 0, 0, 0);
// const static Units<unit_value> kZepto ( 1e-21, 0, 0, 0, 0);
// const static Units<unit_value> kAtto  ( 1e-18, 0, 0, 0, 0);
// const static Units<unit_value> kFemto ( 1e-15, 0, 0, 0, 0);
// const static Units<unit_value> kPico  ( 1e-12, 0, 0, 0, 0);
// const static Units<unit_value> kNano  ( 1e-9,  0, 0, 0, 0);
// const static Units<unit_value> kMicro ( 1e-6,  0, 0, 0, 0);
// const static Units<unit_value> kMilli ( 1e-3,  0, 0, 0, 0);
// const static Units<unit_value> kCenti ( 1e-2,  0, 0, 0, 0);
// const static Units<unit_value> kDeci  ( 1e-1,  0, 0, 0, 0);
// const static Units<unit_value> kDeca  ( 1e1,   0, 0, 0, 0);
// const static Units<unit_value> kHecto ( 1e2,   0, 0, 0, 0);
// const static Units<unit_value> kKilo  ( 1e3,   0, 0, 0, 0);
// const static Units<unit_value> kMega  ( 1e6,   0, 0, 0, 0);
// const static Units<unit_value> kGiga  ( 1e9,   0, 0, 0, 0);
// const static Units<unit_value> kTera  ( 1e12,  0, 0, 0, 0);
// const static Units<unit_value> kPeta  ( 1e15,  0, 0, 0, 0);
// const static Units<unit_value> kExa   ( 1e18,  0, 0, 0, 0);
// const static Units<unit_value> kZetta ( 1e21,  0, 0, 0, 0);
// const static Units<unit_value> kYotta ( 1e24,  0, 0, 0, 0);
// }
// namespace Length {
// const static Units<unit_value> kMeter ( 1, 0, 0, 0, 1);
// }
// namespace Mass {
// const static Units<unit_value> kGram ( 1e-3, 0, 1, 0, 0);
// }
// namespace Force {
// const static Units<unit_value> kNewton = Length::kMeter * Mass::kGram * Prefix::kKilo / Time
//     }
// }




// namespace Imperial {
// // All in terms of meters.
// namespace Length {
// const static Units<unit_value> kThou         ( 2.54e-5,       0, 0, 0, 1 );
// const static Units<unit_value> kInch         = 1000 * kThou;
// const static Units<unit_value> kFoot         = 12 * kInch;
// const static Units<unit_value> kYard         = 3 * kFoot;
// const static Units<unit_value> kChain        = 22 * kYard;
// const static Units<unit_value> kFurlong      = 10 * kChain;
// const static Units<unit_value> kMile         = 8 * kFurlong;
// const static Units<unit_value> kLeague       = 3 * kMile;
// const static Units<unit_value> kFathom       (1.853184,       0, 0, 0, 1 );
// const static Units<unit_value> kCable        = 100 * kFathom;
// const static Units<unit_value> kNauticalMile = 10 * kCable;
// }
// // All in terms of meters^2.
// namespace Area {
// const static Units<unit_value> kAcre         = Length::kFurlong * Length::kChain;
// }
// // All in terms of meters^3.
// namespace Volume {
// const static Units<unit_value> kFluidOunce ( 2.95735296e-5,     0, 0, 0, 3);
// const static Units<unit_value> kGill       = 5 * kFluidOunce;
// const static Units<unit_value> kPint       = 20 * kFluidOunce;
// const static Units<unit_value> kQuart      = 40 * kFluidOunce;
// const static Units<unit_value> kGallon     = 160 * kFluidOunce;
// }
// // All in terms of kilograms.
// namespace Mass {
// const static Units<unit_value> kPound         ( 4.5359237e-1,  0, 1, 0, 0);
// const static Units<unit_value> kGrain         = kPound / 7000;
// const static Units<unit_value> kDrachm        = kPound / 256;
// const static Units<unit_value> kOunce         = kPound / 16;
// const static Units<unit_value> kStone         = 14 * kPound;
// const static Units<unit_value> kQuarter       = 28 * kPound;
// const static Units<unit_value> kHundredWeight = 112 * kPound;
// const static Units<unit_value> kTon           = 2240 * kPound;
// }
// }
// // All in terms of seconds.
// namespace Time {
// const static Units<unit_value> kSecond ( 1,            0, 0, 1, 0);
// const static Units<unit_value> kMinute = 60 * kSecond;
// const static Units<unit_value> kHour   = 60 * kMinute;
// const static Units<unit_value> kDay    = 24 * kHour;
// const static Units<unit_value> kYear   = 365 * kDay;
// }



#endif  // __UNITS_H__
