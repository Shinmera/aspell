// This file is part of The New Aspell
// Copyright (C) 2001-2003 by Kevin Atkinson under the GNU LGPL license
// version 2.0 or 2.1.  You should have received a copy of the LGPL
// license along with this library if you did not you can find
// it at http://www.gnu.org/.

#ifndef ASPELL_VECTOR__HPP
#define ASPELL_VECTOR__HPP

#include <vector>

namespace acommon
{
  template <typename T>
  class Vector : public std::vector<T>
  {
  public:

#if defined _MSC_VER && _MSC_VER < 1300
    // Bring type definitions from the std::vector class to acommon::Vector
    typedef T value_type;
    typedef std::vector<T>::size_type size_type;
    typedef std::vector<T>::difference_type difference_type;
    typedef std::vector<T>::reference reference;
    typedef std::vector<T>::const_reference const_reference;
    typedef std::vector<T>::iterator iterator;
    typedef std::vector<T>::const_iterator const_iterator;
#endif //_MSC_VER

    Vector() {}
    Vector(unsigned int s) : std::vector<T>(s) {}
    Vector(unsigned int s, const T & val) : std::vector<T>(s, val) {}

    void append(T t) {
      this->push_back(t);
    }
    void append(const T * begin, unsigned int size) {
      insert(this->end(), begin, begin+size);
    }
    void append(const T * begin, const T * end) {
      insert(this->end(), begin, end);
    }
    int alloc(int s) {
      int pos = this->size();
      this->resize(pos + s);
      return pos;
    }
    T * data() {return &*this->begin();}
    T * data(int pos) {return &*this->begin() + pos;}
    T * data_end() {return &*this->end();}

    T * pbegin() {return &*this->begin();}
    T * pend()   {return &*this->end();}

    const T * pbegin() const {return &*this->begin();}
    const T * pend()   const {return &*this->end();}

    template <typename U>
    U * datap() { 
      return reinterpret_cast<U * >(&this->front());
    }
    template <typename U>
    U * datap(int pos) {
      return reinterpret_cast<U * >(&this->front() + pos);
    }

    void pop_front() {this->erase(this->begin());}
    void push_front(const T & v) {this->insert(this->begin(), v);}
  };
}

#endif
