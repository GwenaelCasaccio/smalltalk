#include <unicorn/unicorn.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/jit-x64.c"

#define ADDRESS 0x1000000

OOP _gst_nil_oop = NULL;
OOP _gst_true_oop = NULL;
OOP _gst_false_oop = NULL;
OOP _gst_compiled_method_class = (OOP) 0x1234;
OOP _gst_compiled_block_class = NULL;

OOP array(size_t items)
{
  OOP array_oop = malloc(sizeof(*array_oop));
  gst_object foo = malloc(1024);

  OBJ_SET_SIZE(foo, FROM_INT(0));
  OOP_SET_FLAGS(array_oop, 0);
  OOP_SET_OBJECT(array_oop, foo);

  return array_oop;
}

OOP compiled_code_builder(OOP literals)
{
  OOP compiled_code_oop = malloc(sizeof(*compiled_code_oop));
  gst_object foo = malloc(1024);
  gst_compiled_method method = (gst_compiled_method) foo;
  method_header header;

  OBJ_SET_CLASS(foo, _gst_compiled_method_class);
  OOP_SET_OBJECT(compiled_code_oop, foo);

  memset (&header, 0, sizeof (header));

  method->header = header;
  method->literals = literals;

  return compiled_code_oop;
}

int main(int argc, const char **argv) {
  uc_engine *uc;
  uc_err err;

  err = uc_open(UC_ARCH_X86, UC_MODE_64, &uc);
  if (err != UC_ERR_OK)
    {
      printf("Failed on uc_open() with error returned: %u\n", err);
      return -1;
    }

  OOP compiled_code = compiled_code_builder(array(0));
  dasm_State *result = compile(compiled_code);

  size_t instructionSize;
  void* buf;
  dasm_link(&result, &instructionSize);
  buf = malloc(instructionSize);
  dasm_encode(&result, buf);

  err = uc_mem_map(uc, ADDRESS, 4096, UC_PROT_ALL);
  if (err != UC_ERR_OK)
    {
      printf("Failed to map memory, quit!\n");
      return -1;
    }

  err = uc_mem_write(uc, ADDRESS, buf, instructionSize);
  if (err != UC_ERR_OK)
    {
      printf("Failed to write emulation code to memory, quit!\n");
      return -1;
    }

  free(buf);

  return 0;
}
