#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include <secmem/memory.hpp>
#include <secmem/array.hpp>

TEST_CASE ("Array should initialize properly.", "[secmem::Array]") {
  secmem::Array<char> a(secmem::Memory(1024, secmem::Protection::RW));

  REQUIRE (a.Size() == 1024 / sizeof(char));
  REQUIRE (a.Width() == sizeof(char));
  REQUIRE (a.Get(0) == a(0));
  REQUIRE (a[0] == *(a(0)));

  secmem::Array<char> b = a;

  REQUIRE (b.Size() == 1024 / sizeof(char));
  REQUIRE (b.Width() == sizeof(char));
  REQUIRE (b.Get(0) == b(0));
  REQUIRE (b[0] == *(b(0)));

  secmem::Array<char> c(secmem::Memory(1000, secmem::Protection::RW));

  b = c;

  REQUIRE (b.Size() == 1000 / sizeof(char));
  REQUIRE (b.Width() == sizeof(char));
  REQUIRE (b.Get(0) == b(0));
  REQUIRE (b[0] == *(b(0)));
}
