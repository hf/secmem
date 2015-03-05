// Copyright (c) 2015 Stojan Dimitrovski
//
// This file is distributed under The MIT License. See the included file
// LICENSE.txt for the full text.

#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>

using namespace std;

#include <boost/atomic/atomic.hpp>

using namespace boost;

#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

namespace secmem {

namespace Protection {
  static const int NONE = PROT_NONE;
  static const int READ = PROT_READ;
  static const int WRITE = PROT_WRITE;
  static const int EXECUTE = PROT_EXEC;

  static const int RW = READ | WRITE;
  static const int RWX = RW | EXECUTE;
} // Protection

// Tries to securely clear memory.
// See: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1381.pdf
static void* SecureMemset(void* const memory, unsigned char v, size_t n) {
  #pragma optimize("", off)

  volatile unsigned char* p = (unsigned char*) memory;
  while (n--) {
    *p = v;
    p++;
  }

  #pragma optimize("", on)

  return memory;
}

/*
 * Holds a chunk of secure memory.
 *
 * This memory is reference counted, so you can and should pass values of this.
 *
 * Upon destruction the memory is write-only, cleared (filled with 0s) and then
 * made unnacessible. After that, it is unmapped.
 */
class Memory {
private:
  atomic<unsigned int>* refcount;
  size_t size;

  void* memory;

  int error;

  static void* Map(size_t size, int protection, int& error) {
    void* const memory = mmap(NULL,
      size,
      protection,
      MAP_PRIVATE | MAP_ANONYMOUS | MAP_LOCKED | MAP_NORESERVE,
      0,
      PageSize());

    if (MAP_FAILED == memory) {
      error = errno;

      return NULL;
    }

    return memory;
  }

  inline void Unref() {
    assert (refcount != NULL);

    if (1 == refcount->fetch_sub(1, memory_order_release)) {
      atomic_thread_fence(memory_order_acquire);

      Protect(Protection::WRITE);
      Clear();
      Protect(Protection::NONE);

      // there are no more objects that can modify refcount
      // so we delete it here
      delete refcount;
      refcount = NULL;

      if (-1 == munmap(Get(), Size())) {
        int error = errno;
        throw error;
      }
    }
  }

  inline void Ref() {
    assert (refcount != NULL);

    refcount->fetch_add(1, memory_order_relaxed);
  }

public:
  inline static size_t PageSize() {
    return sysconf(_SC_PAGE_SIZE);
  }

  Memory(size_t s, int protection) :
    error(0),
    refcount(new atomic<unsigned int>(1)),
    size(s),
    memory(Map(size, protection, error))
  {
    assert (size > 0);
    assert (refcount != NULL);
  }

  Memory(const Memory& o) :
    error(o.error),
    refcount(o.refcount),
    size(o.size),
    memory(o.memory)
  {
    Ref();
  }

  ~Memory() {
    Unref();
  }

  /*
   * Checks whether an error occured on a previous error-producing operation.
   */
  inline bool HasError() const {
    return Error() != 0;
  }

  /*
   * Returns the platform-specific error code if an error has occured.
   */
  inline int Error() const {
    return error;
  }

  #ifdef SECMEM_TEST
  inline unsigned int References() const {
    return refcount->load(memory_order_relaxed);
  }
  #endif

  inline size_t Size() const {
    return size;
  }

  /*
   * Changes the protection of this memory. Use values from Protection.
   *
   * May produce an error, always check HasError().
   */
  inline bool Protect(int protection) {
    error = 0;

    if (-1 == mprotect(Get(), size, protection)) {
      error = errno;

      return false;
    }

    return true;
  }

  /*
   * Fills the memory block with value.
   */
  inline void Fill(unsigned char value) const {
    SecureMemset(Get(), value, size);
  }

  /*
   * Fills the memory block with 0s.
   */
  inline void Clear() const {
    Fill(0);
  }

  /*
   * Returns the first address of the memory block. Don't be too smart about
   * this.
   */
  inline void* Get() const {
    return memory;
  }

  /*
   * Returns the first address of the memory block shifted byte_offset bytes
   * to the right. Also, don't try to be too smart about this.
   */
  inline void* Get(size_t byte_offset) const {
    assert (byte_offset < Size());

    return (void*) (((char*) memory) + byte_offset);
  }

  /*
   * Same as Get().
   */
  inline void* operator()() const {
    return Get();
  }

  /*
   * Same as Get(size_t).
   */
  inline void* operator()(size_t byte_offset) const {
    return Get(byte_offset);
  }

  inline Memory& operator=(const Memory& m) {
    Unref();

    refcount = m.refcount;
    memory = m.memory;
    size = m.size;
    error = m.error;

    Ref();
  }
};

} // secmem
