#ifndef MAP_UTILS_H_
#define MAP_UTILS_H_

#include "string-utils.h"
#include <iostream>

struct StreamCompare {
  // template<class T, class U>
  // bool operator()(const T& left, const U& right) const {
  //   return strcmp(string::cstr(left), string::cstr(right)) > 0;
  // }
  static bool Equal(const char* left, const char* right) {
    return string::StreamCompare(left, right);
  }
  static bool Equal(const std::string& left, const char* right) {
    return string::StreamCompare(left.c_str(), right);
  }
  static bool Equal(const std::string& left, const std::string& right) {
    return string::StreamCompare(left.c_str(), right.c_str());
  }
  bool operator()(const char* left, const char* right) const {
    return strcmp(left, right) > 0;
    // return strcmp(left, right) < 0; // Doesn't work...
  }
  bool operator()(const std::string& left, const std::string& right) const {
    return strcmp(left.c_str(), right.c_str()) > 0;
    // return strcmp(left, right) < 0; // Doesn't work...
  }
};

struct StreamCaseCompare {
  // template<class T, class U>
  // bool operator()(const T& left, const U& right) const {
  //   return stricmp(string::cstr(left), string::cstr(right)) > 0;
  // }
  static bool Equal(const char* left, const char* right) {
    return string::StreamCaseCompare(left, right);
  }
  static bool Equal(const std::string& left, const char* right) {
    return string::StreamCaseCompare(left.c_str(), right);
  }
  static bool Equal(const std::string& left, const std::string& right) {
    return string::StreamCaseCompare(left.c_str(), right.c_str());
  }
  bool operator()(const char* left, const char* right) const {
    return stricmp(left, right) > 0;
    // return strcmp(left, right) < 0; // Doesn't work...
  }
  bool operator()(const std::string& left, const std::string& right) const {
    return stricmp(left.c_str(), right.c_str()) > 0;
    // return strcmp(left, right) < 0; // Doesn't work...
  }
};

template<class Key, class T, class Compare = StreamCompare,
         class Allocator = std::allocator<std::pair<const Key,T> > >
class StreamMap : public std::map<Key, T, Compare, Allocator> {
 public:
  typedef std::map<Key, T, Compare, Allocator> map_type;
//  typedef map_type::iterator iterator;
//  typedef map_type::const_iterator const_iterator;
  // T* Find(const Key& stream) {
  //   typedef typename std::map<Key, T, Compare, Allocator>::iterator Iterator;
  //   std::pair<Iterator, Iterator> pair = this->equal_range(stream);
  //   if (pair.first == this->end() || pair.first != pair.second)
  //     return NULL;
  //   return &pair.first->second;
  // }
//  StreamMap(const StreamMap&) = default;
//  StreamMap& operator=(const StreamMap&) = default;

  typename std::map<Key, T, Compare, Allocator>::iterator Find(const Key& stream) {
    typedef typename std::map<Key, T, Compare, Allocator>::iterator Iterator;
    std::pair<Iterator, Iterator> pair = this->equal_range(stream);
    // if (pair.first != this->end() && pair.first == pair.second)
    //   if (Compare::Equal(pair.first->first, stream))
    //     return pair.first;
    // for (Iterator idx = pair.first, end = pair.second; idx != end; ++idx)
    //   if (Compare::Equal(idx->first, stream))
    //     return idx;
    Iterator& idx = pair.first, &end = pair.second;
    if (idx == this->end())
      return idx;
    do {
      // printf("dicks: %s\n", idx->first);
      if (Compare::Equal(idx->first, stream))
        return idx;
      // if (Compare::Equal(idx->first, stream)) {
      //   printf("boobs: %s\n", idx->first);
      //   return idx;
      // }

      // Will probably result in infinite loop if `idx` == `end`.
      // } while (idx != end || ++idx != idx);
    } while (idx++ != end);
    return this->end();
  }
  typename std::map<Key, T, Compare, Allocator>::const_iterator Find(const Key& stream) const {
    typedef typename std::map<Key, T, Compare, Allocator>::const_iterator Iterator;
    std::pair<Iterator, Iterator> pair = this->equal_range(stream);
    // if (pair.first != this->end() && pair.first == pair.second)
    //   if (Compare::Equal(pair.first->first, stream))
    //     return pair.first;
    // for (Iterator idx = pair.first, end = pair.second; idx != end; ++idx)
    //   if (Compare::Equal(idx->first, stream))
    //     return idx;

    Iterator& idx = pair.first, &end = pair.second;
    if (idx == this->end())
      return idx;
    do {
      puts("StreamCompare: Find Loop");
      if (Compare::Equal(idx->first, stream))
        return idx;
      // Will probably result in infinite loop if `idx` == `end`.
      // } while (idx != end || ++idx != idx);
    } while (idx++ != end);
    return this->end();
  }
};

// template<class T, class Compare = StreamCompare,
//          class Allocator = std::allocator<std::pair<const char* const,T> > >
// class StreamMap : public std::map<const char*, T, Compare, Allocator> {
//  public:
//   T* Find(const char* stream) {
//     typedef typename std::map<const char*, T, Compare, Allocator>::iterator Iterator;
//     std::pair<Iterator, Iterator> pair = this->equal_range(stream);
//     if (pair.first == this->end() || pair.first != pair.second)
//       return NULL;
//     return &pair.first->second;
//   }
// };

// template<class Key, class T, class Compare = std::less<Key>,
//          class Allocator = std::allocator<std::pair<const Key,T> > >
// struct MapWrap {
//   typedef std::map<Key, T, Compare, Allocator> type;
//   MapWrap(type& m) : map(m) {}
//   MapWrap<Key, T, Compare, Allocator>& operator() (const Key& key, const T& value) {
//     map[key] = value;
//     return *this;
//   }

//   type& map;
// };

template<class Map>
struct MapWrap {
  // typedef std::map<Key, T, Compare, Allocator> type;
  typedef Map type;
  typedef typename Map::mapped_type mapped_type;
  
  MapWrap(type& m) : map(m) {}
  
  template<class Key>
  MapWrap<Map>& operator() (const Key& key, const mapped_type& value) {
//    map[key] = value;
    map.insert(typename Map::value_type(key, value));
    return *this;
  }

  type& map;
};

template<class Map>
MapWrap<Map> WrapMap(Map& map) {
  return MapWrap<Map>(map);
}


template<class Map, class Compare>
typename Map::iterator UnGreedyFind(Map& map, const char* str, Compare compare) {
  typedef typename Map::iterator Iterator;
  size_t longest_size = 0;
  // typename Map::iterator longest = map.end();
  // for (typename Map::iterator idx = map.begin(); idx != map.end(); ++idx)
  std::pair<Iterator, Iterator> range = map.equal_range(str);
  if (range.first == range.second && range.first != map.end()) {
    return range.first;
  }
  // if (range.first == map.end())
  //   return map.end();
  //   throw "MONKEY NUTS";
  Iterator longest = map.end(), &idx = range.first, &finish = range.second;
  for ( ; idx != finish; ++idx) {
    printf("Matched Key: %s\n", idx->first);
    if (compare(string::cstr(idx->first), str)) {
      size_t size = string::ssize(idx->first);
      if (size > longest_size) {
        longest = idx;
        longest_size = size;
      }
    }
  }
  return longest;
}

template<class Map, class Compare>
typename Map::iterator GreedyFind(Map& map, const char* str, Compare compare) {
  typedef typename Map::iterator Iterator;
  size_t shortest_size = -1;
  // typename Map::iterator shortest = map.end();
  // for (typename Map::iterator idx = map.begin(); idx != map.end(); ++idx)
  std::pair<Iterator, Iterator> range = map.equal_range(str);
  Iterator shortest = map.end(), &idx = range.first, &finish = range.second;
  for ( ; idx != finish; ++idx) {
    printf("Matched Key: %s\n", idx->first);
    if (compare(string::cstr(idx->first), str)) {
      size_t size = string::ssize(idx->first);
      if (size < shortest_size) {
        shortest = idx;
        shortest_size = size;
      }
    }
  }
  return shortest;
}

template<class Map, class Compare>
typename Map::iterator FindInMap(Map& map, const char* str, Compare compare) {
  for (typename Map::iterator idx = map.begin(); idx != map.end(); ++idx)
    if (compare(string::cstr(idx->first), str))
      return idx;
  return map.end();
}

struct DefaultCompare {
  template<class T, class U>
  inline bool operator()(const T& t, const U& u) {
    return t == u;
  }
};

template<class Map>
typename Map::iterator GreedyFind(Map& map, const char* str) {
  return GreedyFind(map, str, DefaultCompare());
}

template<class Map>
typename Map::iterator UnGreedyFind(Map& map, const char* str) {
  return UnGreedyFind(map, str, DefaultCompare());
  // return UnGreedyFind(map, str, Compare());
}

#endif  // MAP_UTILS_H_
