#ifndef PRINT_H_
#define PRINT_H_

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "print-tuple-utils.h"

template<class T>
std::ostream& operator<<(std::ostream&, const std::vector<T>&);

template<class T>
std::ostream& operator<<(std::ostream&, const std::set<T>&);

template<class T, size_t n>
std::ostream& operator<<(std::ostream&, const std::array<T, n>&);

template<class T>
std::ostream& operator<<(std::ostream&, const std::unordered_set<T>&);

template<class K, class V>
std::ostream& operator<<(std::ostream&, const std::unordered_map<K, V>&);

template<class K, class V, class C>
std::ostream& operator<<(std::ostream&, const std::map<K, V, C>&);


template<class T>
std::ostream& operator<<(std::ostream& stream, const std::set<T>& set) {
  stream << '[';
  auto n = set.size();
  // TODO: Use iterators to make this nice.
  for (const auto& e: set) {
    stream << e;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << ']';
}

template<class T, size_t n>
std::ostream& operator<<(std::ostream& stream, const std::array<T, n>& set) {
  stream << '{' << set[0];
  for (int i = 1; i < n; ++i) {
    const auto& e = set[i];
    stream << ", " << e;
  }
  return stream << '}';
}


template<class T>
std::ostream& operator<<(std::ostream& stream, const std::unordered_set<T>& set) {
  stream << '{';
  auto n = set.size();
  for (const auto& e: set) {
    stream << e;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << '}';
}

template<class T>
using unary = T (*)(T);

#define DOUT(x) std::cout << #x ": " << x << std::endl

template<class K, class V>
std::ostream& operator<<(std::ostream& stream, std::unordered_map<K, V>&& map) {
  int n = map.size();
  stream << "{ ";
  for (const auto& pair : map) {
    stream << '[' << pair.first << "] = " << pair.second;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << " }";
}

template<class K, class V>
std::ostream& operator<<(std::ostream& stream, const std::unordered_map<K, V>& map) {
  int n = map.size();
  stream << '{';
  for (const auto& pair : map) {
    stream << pair.first << ": " << pair.second;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << '}';
}

template<class K, class V, class C>
std::ostream& operator<<(std::ostream& stream, const std::map<K, V, C>& map) {
  long n = map.size();
  stream << '{';
  for (const auto& pair : map) {
    stream << pair.first << ": " << pair.second;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << '}';
}

template<class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v) {
  // stream << '{';
  stream << '[';
  auto n = v.size();
  for (const auto& e: v) {
    stream << e;
    if (--n == 0)
      break;
    stream << ", ";
  }
  return stream << ']';
}



#endif  // PRINT_H_
