// Copyright (c) 2009 Ashkan Kiani
// A header file that defines scoped_ptr and scoped_array
#ifndef SCOPED_H_
#define SCOPED_H_

//#include <crtdefs.h>
#include "basictypes.h"

template<class T>
class scoped_ptr {
 public:
  typedef T element_type;

  explicit scoped_ptr(T* ptr = NULL) : px_(ptr) {}

  ~scoped_ptr() {
    delete px_;
  }

  template<class R>
  R* reinterpret() const {
    return reinterpret_cast<R*>(px_);
  }

  T* get() const {
    return px_;
  }

  void reset(T* p = 0) {
    if (p != px_) {
#ifdef _DEBUG
      RUNTIME_ASSERT(sizeof(T), scoped_ptr_type_isnt_complete);
#endif  // _DEBUG
      scoped_ptr<T>(p).swap(*this);
    }
  }

  T* release() {
    T* tmp = px_;
    px_ = 0;
    return tmp;
  }

  void swap(scoped_ptr & b) {
    T* tmp = b.px_;
    b.px_ = px_;
    px_ = tmp;
  }

  operator bool() const {
    return px_ != 0;
  }

  T& operator*() const {
    return *px_;
  }

  T* operator->() const {
    return px_;
  }

 private:
  T* px_;
  DISALLOW_COPY_AND_ASSIGN(scoped_ptr);
};

template<class T>
class scoped_array {
 public:
  typedef T element_type;

  explicit scoped_array(T* ptr = NULL) : px_(ptr) {}

  ~scoped_array() {
    delete[] px_;
  }

  template<class R>
  R* reinterpret() const {
    return reinterpret_cast<R*>(px_);
  }

  T* get() const {
    return px_;
  }

  void reset(T* p = NULL) {
    if (p != px_) {
#ifdef _DEBUG
      RUNTIME_ASSERT(sizeof(T), scoped_ptr_type_isnt_complete);
#endif  // _DEBUG
      scoped_array<T>(p).swap(*this);
    }
  }

  T* release() {
    T* tmp = px_;
    px_ = 0;
    return tmp;
  }

  void swap(scoped_array & b) {
    T* tmp = b.px_;
    b.px_ = px_;
    px_ = tmp;
  }

  operator bool() const {
    return px_ != 0;
  }

  T& operator[] (ptrdiff_t pos) const {
    return px_[pos];
  }

 private:
  T* px_;
  DISALLOW_COPY_AND_ASSIGN(scoped_array);
};

template<class T>
inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) {
  a.swap(b);
}

template<class T>
inline void swap(scoped_array<T> & a, scoped_array<T> & b) {
  a.swap(b);
}
#endif  // SCOPED_H_
