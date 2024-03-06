#include "object_table.h"

#include "doctest.h"

#include <iostream>
#include <ostream>

template <std::size_t N> ObjectTable<N>::ObjectTable() :
  table(),
  nextObjectToBeAllocatedIT(table.begin()),
  numberAllocatedObject(0),
  gcAllocatedBarrier(table.size() * 0.9),
  allocatedFlag(true)
{ }

template <std::size_t N> void ObjectTable<N>::displaySomeStats() {
  std::cerr << "numberAllocatedObject "
            << numberAllocatedObject
            << "gcAllocatedBarrier "
            << gcAllocatedBarrier
            << std::endl;
}

TEST_CASE("initialization of the object table") {
  ObjectTable<100> ot;

  for (auto i = ot.begin(); i != ot.end(); i++) {
    REQUIRE_FALSE((*i).getAllocatedFlag());
  }

  REQUIRE_FALSE(ot.shouldLaunchGC());
}

TEST_CASE("allocate different objects on the object table") {
  ObjectTable<10> ot;

  for (auto i = ot.begin(); i != ot.end(); i++) {
    REQUIRE_FALSE((*i).getAllocatedFlag());
  }

  for (uint8_t i = 0; i < 10; i++) {
    std::optional<std::reference_wrapper<object_s>>object = ot.alloc();
    REQUIRE(object.has_value());
  }

  std::optional<std::reference_wrapper<object_s>>object = ot.alloc();
  REQUIRE_FALSE(object.has_value());

  for (auto i = ot.begin(); i != ot.end(); i++) {
    REQUIRE((*i).getAllocatedFlag());
  }
}

TEST_CASE("should launch a GC when low on objects on the object table") {
  ObjectTable<10> ot;

  for (auto i = ot.begin(); i != ot.end(); i++) {
    REQUIRE_FALSE((*i).getAllocatedFlag());
  }

  for (uint8_t i = 0; i < 8; i++) {
    std::optional<std::reference_wrapper<object_s>>object = ot.alloc();
    REQUIRE(object.has_value());
    REQUIRE_FALSE(ot.shouldLaunchGC());
  }

  std::optional<std::reference_wrapper<object_s>>object = ot.alloc();
  REQUIRE(object.has_value());
  REQUIRE(ot.shouldLaunchGC());
}

