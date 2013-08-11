#ifndef OPERATOR_H_
#define OPERATOR_H_

class blind_ptr {
 public:
  blind_ptr() : ptr_(NULL) {}

  template<class Pointer>
  blind_ptr(Pointer ptr) : ptr_(reinterpret_cast<void*>(ptr)) {}

  template<class U>
  inline U* as() const { return reinterpret_cast<U*>(ptr_); }

  template<class U>
  inline operator U() const { return reinterpret_cast<U>(ptr_); }

  // Useful if you have C++0x variadic templates and a pointer to a function
  // template<class T, class... Args>
  // T operator()(Args... args) const {
  //   return reinterpret_cast<T (*)(Args...)>(ptr_)(args...);
  // }
  
 private:
  void* ptr_;
};

template<class T>
inline T modulo(const T& a, const T& b) {
  // return a % b;
  return a - std::floor(a/b)*b;
}

template<>
inline double modulo<double>(const double& a, const double& b) {
  return fmod(a, b);
}

struct Operator {
  Operator(const char* d, blind_ptr f, size_t a, int p, bool i, bool l)
      : name(d), function(f), argument_count(a),
        precedence(p), is_operator(i), left_assoc(l) {}

  Operator() = default;
  // Operator()
  //     : display_name(NULL), function(NULL), argument_count(0), precedence(0), left_assoc(false) {}
  // const char* parse_name;
  // const char* display_name   = NULL;
  const char* name           = NULL;
  blind_ptr   function       = NULL;
  size_t      argument_count = 0;
  int         precedence     = 0;
  bool        is_operator    = false;
  bool        left_assoc     = false;
};

template<class T>
struct Operations {
  using type        = T;
  using arg_type    = const T&;
  // using arg_type    = T;
  // using Unary       = T(arg_type);
  // using Binary      = T(arg_type, arg_type);
  // using Nary        = T(const std::vector<T>&);
  using Unary       = std::function<T(arg_type)>;
  using Binary      = std::function<T(arg_type, arg_type)>;
  using Nary        = std::function<T(const std::vector<T>&)>;
  // using Unary  = T (*)(arg_type);
  // using Binary = T (*)(arg_type, arg_type);
  // using Nary   = T (*)(const std::vector<T>&);

  // typedef T type;
  // typedef T (*Unary)(arg_type);
  // typedef T (*Binary)(arg_type, arg_type);
  // typedef T (*Nary)(const std::vector<T>&);

  inline static T floor(arg_type a) { return std::floor(a); }
  inline static T ceil(arg_type a) { return std::floor(a); }
  inline static T negate(arg_type a) { return -a; }
  inline static T plus(arg_type a, arg_type b) { return a + b; }
  inline static T minus(arg_type a, arg_type b) { return a - b; }
  inline static T multiply(arg_type a, arg_type b) { return a * b; }
  inline static T divide(arg_type a, arg_type b) { return a / b; }
  inline static T modulo(arg_type a, arg_type b) { return modulo(a, b); }
  inline static T pow(arg_type a, arg_type b) { return std::pow(a, b); }

  inline static T sin(arg_type a) { return std::sin(a); }
  inline static T cos(arg_type a) { return std::cos(a); }
  inline static T tan(arg_type a) { return std::tan(a); }
  inline static T asin(arg_type a) { return std::asin(a); }
  inline static T acos(arg_type a) { return std::acos(a); }
  inline static T atan(arg_type a) { return std::atan(a); }
  inline static T exp(arg_type a) { return std::exp(a); }
  inline static T log(arg_type a) { return std::log(a); }
  inline static T log10(arg_type a) { return std::log10(a); }
  inline static T log2(arg_type a) { return ::log2(a); }
  inline static T sqrt(arg_type a) { return std::sqrt(a); }
  // inline static T nCr(const T& a, const T& b) { return combination(a, b); }
};


#endif  // OPERATOR_H_
