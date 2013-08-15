#ifndef NUMBER_UTILITIES_H_
#define NUMBER_UTILITIES_H_

template<class T>
bool representable(T) {
  return false;
}

template<>
bool representable(double x) {
    //  Warning!!! Not fully C-compliant!
  union{
    double f;
    uint64_t i;
  } val;

  val.f = x;

  uint64_t exp = val.i & 0x7ff0000000000000ull;
  uint64_t man = val.i & 0x000fffffffffffffull;
    
  int shift = (exp >> 52) - 1075;
    
    //  Out of range
  if (shift < -63 || shift > 63)
      return exp == 0 && man == 0;  // 0 is an integer, IMO.
    
  man |= 0x0010000000000000ull;  //  Implicit leading 1-bit.
    //  Test mantissa
  if (shift < 0) {
    shift = -shift;
    return ((man >> shift) << shift) == man;
  }
  return ((man << shift) >> shift) == man;
}

template<>
bool representable(int x) {
  return true;
}

#endif  // NUMBER_UTILITIES_H_
