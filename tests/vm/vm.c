#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#undef NEXT_BC

OOP _gst_self[100];
OOP _gst_true_oop;
OOP _gst_false_oop;
OOP _gst_this_context_oop[100];
OOP *_gst_literals[100];
OOP _gst_bad_return_error_symbol;

struct builtin_selector _gst_builtin_selectors[256] = { };

bool unwind_method() {
  return false;
}

OOP _gst_make_block_closure(OOP oop) {
  return oop;
}

#include "libgst/vm.c"

static void should_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0xFF };
  tip = &bytecode[0];
  
  bc();
  
  assert_true(tip == &bytecode[1]);
}

static void should_load_self_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x00, 0x01, 0xFF };
  tip = &bytecode[0];
  context = malloc(sizeof(*context) * 100);
  _gst_self[0] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x01] == _gst_self[0]);

  free(context);
}

static void should_load_self_to_outer_scope(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x01, 0x02, 0x01, 0xFF };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  _gst_self[0] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(outer_context_2->data[0x01] == _gst_self[0]);

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_load_self_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x02, 0x05, 0xFF };
  tip = &bytecode[0];
  context = malloc(sizeof(*context) * 100);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  
  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(INSTANCE_VARIABLE(_gst_self[0], 0x05) == _gst_self[0]);

  free(context);
  free(_gst_self[0]);
  free(obj);
}

static void should_load_literal_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x03, 0x3, 0x01, 0xFF };
  tip = &bytecode[0];
  OOP literals[] = { NULL, NULL, NULL, FROM_INT(123) }; 
  _gst_literals[0] = &literals[0];
  context = malloc(sizeof(*context) * 100);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x01] == FROM_INT(123));

  free(context);
}

static void should_load_literal_to_outer_scope(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x04, 0x02, 0x02, 0x01, 0xFF };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, FROM_INT(123) };
  _gst_literals[0] = &literals[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  _gst_self[0] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[5]);
  assert_true(outer_context_2->data[0x01] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_load_literal_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x05, 0x02, 0x05, 0xFF };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, FROM_INT(123) };
  _gst_literals[0] = &literals[0];
  context = malloc(sizeof(*context) * 100);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(INSTANCE_VARIABLE(_gst_self[0], 0x05) == FROM_INT(123));
  free(_gst_self[0]);
  free(obj);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_return),
      cmocka_unit_test(should_load_self_to_register),
      cmocka_unit_test(should_load_self_to_outer_scope),
      cmocka_unit_test(should_load_self_to_ivar),
      cmocka_unit_test(should_load_literal_to_register),
      cmocka_unit_test(should_load_literal_to_outer_scope),
      cmocka_unit_test(should_load_literal_to_ivar),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}


