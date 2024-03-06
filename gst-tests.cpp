#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

DOCTEST_SYMBOL_IMPORT void from_dll();

int main(int argc, char** argv) {
  // force the use of a symbol from the dll so tests from it get registered
  from_dll();

  doctest::Context context(argc, argv);
  return context.run();
}
