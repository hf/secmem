// Copyright (c) 2015 Stojan Dimitrovski
//
// This file is distributed under The MIT License. See the included file
// LICENSE.txt for the full text.

#define CATCH_CONFIG_MAIN

#include <catch.hpp>

#include <secmem/memory.hpp>

TEST_CASE ("Protection should define values correctly.", "[secmem::Protection]") {
  REQUIRE (secmem::Protection::RW == (secmem::Protection::READ | secmem::Protection::WRITE));
  REQUIRE (secmem::Protection::RWX == (secmem::Protection::RW | secmem::Protection::EXECUTE));
}

TEST_CASE ("Should reserve a chunk of secure memory.", "[secmem::Memory]") {
  secmem::Memory memory(1024, secmem::Protection::RW);

  REQUIRE (memory.HasError() == false);
  REQUIRE (memory.Size() == 1024);
  REQUIRE (memory.Get() == memory());
  REQUIRE (memory.Get() != NULL);
}

TEST_CASE ("Should fill memory with values.", "[secmem::Memory]") {
  secmem::Memory memory(1024, secmem::Protection::RW);

  REQUIRE (memory.HasError() == false);

  memory.Clear();

  for (size_t i = 0; i < memory.Size(); i++) {
    REQUIRE (*(((char*) memory()) + i) == 0);
  }

  memory.Fill(1);

  for (size_t i = 0; i < memory.Size(); i++) {
    REQUIRE (*(((char*) memory()) + i) == 1);
  }
}

static void test_pass_by_reference(const secmem::Memory& mem, unsigned int refs) {
  REQUIRE (mem.References() == refs);
}

static void test_pass_by_value(secmem::Memory mem, unsigned int refs) {
  REQUIRE (mem.References() == refs + 1);
}

TEST_CASE ("Should properly refcount.", "[secmem::Memory]") {
  secmem::Memory memory(1024, secmem::Protection::RW);

  REQUIRE (memory.HasError() == false);

  REQUIRE (memory.References() == 1);

  secmem::Memory memory1 = memory;

  REQUIRE (memory.References() == 2);
  REQUIRE (memory1.References() == 2);

  test_pass_by_reference(memory, 2);
  test_pass_by_reference(memory1, 2);

  REQUIRE (memory.References() == 2);
  REQUIRE (memory1.References() == 2);

  test_pass_by_value(memory, 2);
  test_pass_by_value(memory1, 2);

  REQUIRE (memory.References() == 2);
  REQUIRE (memory1.References() == 2);

  secmem::Memory memory2(1024, secmem::Protection::RW);

  REQUIRE (memory2.References() == 1);

  memory1 = memory2;

  REQUIRE (memory.References() == 1);
  REQUIRE (memory1.References() == 2);
  REQUIRE (memory2.References() == 2);
}
