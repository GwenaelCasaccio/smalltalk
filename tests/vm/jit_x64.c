#include <unicorn/unicorn.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/jit-x64.c"

OOP _gst_nil_oop;
OOP _gst_true_oop;
OOP _gst_false_oop;
OOP _gst_compiled_method_class;
OOP _gst_compiled_block_class;

int main(int argc, const char **argv) {
  return 0;
}
