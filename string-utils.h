#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <cstring>

#if defined(OS_WIN)
#define stricmp _stricmp
#define strnicmp _strnicmp
#else
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif  // OS_WIN

namespace string {
inline bool InRange(int value, int minimum, int maximum) {
  return static_cast<unsigned long>(value - minimum) <= (maximum - minimum);
}

inline bool IsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r'
      || c == '\f' || c == '\v';
}

inline bool IsAlpha(char c) {
  return InRange(c | 0x20, 'a', 'z');
}

inline bool IsDigit(char c) {
  return InRange(c, '0', '9');
}

inline bool IsAlnum(char c) {
  return IsAlpha(c) || IsDigit(c);
}

inline char BitLower(const char& c) { return c | 0x20; }
inline int BitLower(const int& c) { return c | 0x20; }

inline bool StreamCompare(const char* key, const char* stream) {
  while (*key && *stream)
    if (*key++ != *stream++)
      return false;
  return true;
}

inline bool StreamCaseCompare(const char* key, const char* stream) {
  while (*key && *stream)
    if (BitLower(*key++) != BitLower(*stream++))
      return false;
  return true;
}

inline std::pair<std::string, std::string> SplitInTwo(const std::string& s, const std::string& d) {
  std::pair<std::string, std::string> result;
  size_t pos = s.find(d);
  result.first = s.substr(0, pos);
  if (pos == std::string::npos)
    return result;
  result.second = s.substr(pos + d.length());
  return result;
}

template<class charT, class Traits>
inline size_t ssize(const std::basic_string<charT, Traits>& str)
{ return str.size(); }

template<class charT>
inline size_t ssize(const charT* str) { return strlen(str); }

template<class charT, class Traits>
inline const charT* cstr(const std::basic_string<charT, Traits>& str) {
  return &str[0];
}

template<class charT>
inline const charT* cstr(const charT* str) { return str; }

// struct CaseCompare {
//   template<class T, class U>
//   bool operator() (const T& left, const U& right) {
//     return stricmp(string::cstr(left), string::cstr(right)) == 0;
//   }
// };

// struct Compare {
//   template<class T, class U>
//   bool operator() (const T& left, const U& right) {
//     return strcmp(string::cstr(left), string::cstr(right)) == 0;
//   }
// };

struct traits_nocase : std::char_traits<char> {
  static bool eq(const char& c1, const char& c2)
  { return BitLower(c1) == BitLower(c2); }
  static bool lt(const char& c1, const char& c2)
  { return BitLower(c1) < BitLower(c2); }
  static int compare(const char* s1, const char* s2, size_t N)
  { return strnicmp(s1, s2, N); }
  static const char* find(const char* s, size_t N, const char& a) {
    char c = BitLower(a);
    for (size_t i = 0; i < N; ++i)
      if (BitLower(*s) == c) return s + i;
    return NULL;
  }
  static bool eq_int_type(const int_type& c1, const int_type& c2)
  { return BitLower(c1) == BitLower(c2); }
};

typedef std::basic_string<char, traits_nocase> case_string;

}

#endif  // STRING_UTILS_H_
