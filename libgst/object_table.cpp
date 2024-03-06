#include "object_table.h"

#include "doctest.h"

ObjectTable::ObjectTable() :
  table(),
  nextObjectToBeAllocatedIT(table.begin()),
  numberAllocatedObject(0),
  gcAllocatedBarrier(table.size() * 0.9)
{ }

TEST_CASE("testing the factorial function") {
  CHECK(1 == 3628800);
}

DOCTEST_SYMBOL_EXPORT void from_dll();   // to silence "-Wmissing-declarations" with GCC
DOCTEST_SYMBOL_EXPORT void from_dll() {} // force the creation of a .lib file with MSVC
