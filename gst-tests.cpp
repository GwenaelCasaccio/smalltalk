#define DOCTEST_CONFIG_IMPLEMENTATION_IN_DLL
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

DOCTEST_SYMBOL_IMPORT void gst_shared_lib();

int main(int argc, char** argv) {
  gst_shared_lib();

  doctest::Context context(argc, argv);
  return context.run();
}
