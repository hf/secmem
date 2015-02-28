// Copyright (c) 2015 Stojan Dimitrovski
//
// This file is distributed under The MIT License. See the included file
// LICENSE.txt for the full text.

#pragma once

#include <cstddef>
#include <cassert>

using namespace std;

#include "memory.hpp"

namespace secmem {

/*
 * A simple, managed and typed wrapper around Memory.
 */
template <typename T>
class Array {
private:
  Memory memory;

public:
  Array(const Memory& m) :
    memory(m)
  {
    // No-op.
  }

  Array(const Array<T>& o) :
    memory(o.memory)
  {
    // No-op.
  }

  inline Array<T>& operator=(const Array<T>& a) {
    memory = a.memory;

    return *this;
  }

  ~Array() {
    // No-op.
  }

  inline Memory UnderlyingMemory() const {
    return memory;
  }

  inline size_t Width() const {
    return sizeof(T);
  }

  inline size_t Size() const {
    return memory.Size() / Width();
  }

  inline T* Get(size_t i) const {
    assert (i < Size());

    return ((T*) memory.Get()) + i;
  }

  inline T* operator()(size_t i) const {
    return Get(i);
  }

  inline T& operator[](size_t i) const {
    return *Get(i);
  }

  inline bool Protect(int protection) {
    return memory.Protect(protection);
  }

  inline void Clear() const {
    memory.Clear();
  }
};

} // secmem
