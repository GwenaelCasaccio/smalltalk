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
  function_called();
  return true;
}

OOP _gst_make_block_closure(OOP oop) {
  return oop;
}

void _new_gst_send_message_internal(OOP receiverOOP, OOP classOOP, OOP selectorOOP, uint32_t numArgs) {
  check_expected(receiverOOP);
  check_expected(classOOP);
  check_expected(selectorOOP);
  check_expected(numArgs);

  function_called();
}


#include "libgst/vm.c"

static void should_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  
  bc();
  
  assert_true(tip == &bytecode[1]);
}

static void should_load_self_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x00, 0x01, END_OF_INTERPRETER_BC };
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

  uint32_t bytecode[] = { 0x01, 0x02, 0x01, END_OF_INTERPRETER_BC };
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

  uint32_t bytecode[] = { 0x02, 0x05, END_OF_INTERPRETER_BC };
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

  uint32_t bytecode[] = { 0x03, 0x3, 0x01, END_OF_INTERPRETER_BC };
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

  uint32_t bytecode[] = { 0x04, 0x02, 0x02, 0x01, END_OF_INTERPRETER_BC };
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

  uint32_t bytecode[] = { 0x05, 0x02, 0x05, END_OF_INTERPRETER_BC };
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

static void should_load_integer_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x06, (intptr_t) FROM_INT(123), 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[] = { NULL, NULL, NULL, NULL }; 
  _gst_literals[0] = &literals[0];
  context = malloc(sizeof(*context) * 100);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x01] == FROM_INT(123));

  free(context);
}

static void should_load_integer_to_outer_scope(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x07, (intptr_t) FROM_INT(123), 0x02, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
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

static void should_load_integer_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x08, (intptr_t) FROM_INT(-123), 0x05, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
  _gst_literals[0] = &literals[0];
  context = malloc(sizeof(*context) * 100);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(INSTANCE_VARIABLE(_gst_self[0], 0x05) == FROM_INT(-123));
  free(_gst_self[0]);
  free(obj);
}

static void should_move_register_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x09, 0x01, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(123);
  context->data[0x02] = FROM_INT(0);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x02] == FROM_INT(123));

  free(context);
}

static void should_move_outer_scope_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0A, 0x2, 0x1, 0x2, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
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
  outer_context_2->data[0x1] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[5]);
  assert_true(context->data[0x02] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_move_ivar_to_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0B, 0x02, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  obj->data[0x02] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x01] == FROM_INT(123));

  free(context);
}

static void should_move_register_to_outer_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0C, 0x2, 0x2, 0x1, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
  _gst_literals[0] = &literals[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x2] = FROM_INT(123);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  
  bc();
  
  assert_true(tip == &bytecode[5]);
  assert_true(outer_context_2->data[0x1] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_move_outer_register_to_outer_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0D, 0x1, 0x2, 0x2, 0x3, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
  _gst_literals[0] = &literals[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  outer_context_1->data[0x2] = FROM_INT(123);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  
  bc();
  
  assert_true(tip == &bytecode[6]);
  assert_true(outer_context_2->data[0x3] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_move_ivar_to_outer_register(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0E, 0x2, 0x2, 0x1, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
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
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  obj->data[0x02] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[5]);
  assert_true(outer_context_2->data[0x1] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_move_register_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x0F, 0x01, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(123);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(obj->data[0x02] == FROM_INT(123));

  free(context);
}

static void should_move_outer_register_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x10, 0x2, 0x1, 0x2, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  OOP literals[]  = { NULL, NULL, NULL };
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
  outer_context_2->data[0x1] = FROM_INT(123);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  
  bc();
  
  assert_true(tip == &bytecode[5]);
  assert_true(obj->data[0x02] == FROM_INT(123));

  free(context);
  free(outer_context_1_oop);
  free(outer_context_1);
  free(outer_context_2_oop);
  free(outer_context_2);
}

static void should_move_ivar_to_ivar(void **state) {

  (void) state;

  uint32_t bytecode[] = { 0x11, 0x02, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object obj = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_self[0], obj);
  obj->data[0x02] = FROM_INT(123);
  
  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(obj->data[0x01] == FROM_INT(123));

  free(context);
}

static void should_literal_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { LITERAL_SEND_BC, 0x01, 0x02, 0x05, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_literals[0][0x2] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, numArgs, 0x05);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_literals[0]);
}

static void should_self_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { SELF_SEND_BC, 0x01, 0x05, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, numArgs, 0x05);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);

  free(_gst_literals[0]);
}

static void should_super_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { SUPER_SEND_BC, 0x04, 0x01, 0x05, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_literals[0][0x4] = FROM_INT(789);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(789));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, numArgs, 0x05);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_literals[0]);
}

static void should_register_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { REGISTER_SEND_BC, 0x01, 0x05, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x5] = FROM_INT(123);
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, numArgs, 0x06);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_literals[0]);
}

static void should_outer_register_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { OUTER_REGISTER_SEND_BC, 0x02, 0x01, 0x05, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x5] = FROM_INT(123);
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
  outer_context_2->data[0x1] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, numArgs, 0x06);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[6]);

  free(_gst_literals[0]);
}

static void should_ivar_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { IVAR_SEND_BC, 0x06, 0x01, 0x05, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(999));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, numArgs, 0x05);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_literals[0]);
}

static void should_literal_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { LITERAL_IMMEDIATE_SEND_BC, 0x01, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_literals[0][0x2] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(123));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);

  free(_gst_literals[0]);
}

static void should_self_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { SELF_IMMEDIATE_SEND_BC, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);

  free(_gst_literals[0]);
}

static void should_super_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { SUPER_IMMEDIATE_SEND_BC, 0x04, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_literals[0][0x4] = FROM_INT(789);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(789));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);

  free(_gst_literals[0]);
}

static void should_register_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { REGISTER_IMMEDIATE_SEND_BC, 0x01, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x5] = FROM_INT(123);
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);

  free(_gst_literals[0]);
}

static void should_outer_register_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { OUTER_REGISTER_IMMEDIATE_SEND_BC, 0x02, 0x01, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = FROM_INT(234);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x5] = FROM_INT(123);
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
  outer_context_2->data[0x1] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(234));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_literals[0]);
}

static void should_ivar_immediate_send(void **state) {

  (void) state;

  uint32_t bytecode[] = { IVAR_IMMEDIATE_SEND_BC, 0x06, 0x02, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x1] = FROM_INT(123);
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  gst_small_integer_class = FROM_INT(456);
  
  expect_value(_new_gst_send_message_internal, receiverOOP, FROM_INT(999));
  expect_value(_new_gst_send_message_internal, classOOP, FROM_INT(456));
  expect_value(_new_gst_send_message_internal, selectorOOP, _gst_builtin_selectors[0x02].symbol);
  expect_value(_new_gst_send_message_internal, numArgs, _gst_builtin_selectors[0x02].numArgs);

  expect_function_calls(_new_gst_send_message_internal, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);

  free(_gst_literals[0]);
}

static void should_jump(void **state) {

  (void) state;

  uint32_t bytecode[] = { JUMP_BC, 0x1, END_OF_INTERPRETER_BC, JUMP_BC, -0x03 };
  tip = &bytecode[0];

  bc();
  
  assert_true(tip == &bytecode[3]);
}

static void should_register_jump_if_true(void **state) {

  (void) state;

  uint32_t bytecode[] = { REGISTER_JUMP_IF_TRUE_BC, 0x01, 0x01, END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  _gst_true_oop = FROM_INT(234);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_this_context_oop[0]);
}

static void should_outer_register_jump_if_true(void **state) {

  (void) state;

  uint32_t bytecode[] = { OUTER_REGISTER_JUMP_IF_TRUE_BC, 0x02, 0x01, 0x01, END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC };
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
  outer_context_2->data[0x1] = FROM_INT(234);
  _gst_true_oop = FROM_INT(234);
  
  bc();
  
  assert_true(tip == &bytecode[6]);

  free(_gst_this_context_oop[0]);
}

static void should_ivar_jump_if_true(void **state) {

  (void) state;

  uint32_t bytecode[] = { IVAR_JUMP_IF_TRUE_BC, 0x06, 0x01 , END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC};
  tip = &bytecode[0];
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
  _gst_true_oop = FROM_INT(999);
  
  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_self[0]);
}

static void should_register_jump_if_false(void **state) {

  (void) state;

  uint32_t bytecode[] = { REGISTER_JUMP_IF_FALSE_BC, 0x01, 0x01, END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  _gst_false_oop = FROM_INT(234);

  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_this_context_oop[0]);
}

static void should_outer_register_jump_if_false(void **state) {

  (void) state;

  uint32_t bytecode[] = { OUTER_REGISTER_JUMP_IF_FALSE_BC, 0x02, 0x01, 0x01, END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC };
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
  outer_context_2->data[0x1] = FROM_INT(234);
  _gst_false_oop = FROM_INT(234);
  
  bc();
  
  assert_true(tip == &bytecode[6]);

  free(_gst_this_context_oop[0]);
}

static void should_ivar_jump_if_false(void **state) {

  (void) state;

  uint32_t bytecode[] = { IVAR_JUMP_IF_FALSE_BC, 0x06, 0x01 , END_OF_INTERPRETER_BC, END_OF_INTERPRETER_BC};
  tip = &bytecode[0];
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
  _gst_false_oop = FROM_INT(999);
  
  bc();
  
  assert_true(tip == &bytecode[5]);

  free(_gst_self[0]);
}

static void should_register_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { RETURN_REGISTER_BC, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  context->data[0x07] = FROM_INT(2);

  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(234));

  free(_gst_this_context_oop[0]);
}

static void should_outer_register_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { RETURN_OUTER_REGISTER_BC, 0x02, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  outer_context_2->data[0x1] = FROM_INT(234);
  
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x02] == FROM_INT(234));

  free(_gst_this_context_oop[0]);
}

static void should_ivar_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { RETURN_IVAR_BC, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
 
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(999));

  free(_gst_this_context_oop[0]);
}

static void should_self_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { RETURN_SELF_BC, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
 
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[2]);
  assert_true(context->data[0x02] == _gst_self[0]);

  free(_gst_this_context_oop[0]);
}

static void should_literal_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { RETURN_LITERAL_BC, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x06] = FROM_INT(999);
  
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(999));

  free(_gst_this_context_oop[0]);
}






































static void should_register_non_local_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { NON_LOCAL_RETURN_REGISTER_BC, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  context->data[0x01] = FROM_INT(234);
  context->data[0x07] = FROM_INT(2);

  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(234));

  free(_gst_this_context_oop[0]);
}

static void should_outer_register_non_local_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { NON_LOCAL_RETURN_OUTER_REGISTER_BC, 0x02, 0x01, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  OOP outer_context_1_oop = malloc(sizeof(*outer_context_1_oop));
  gst_object outer_context_1 = malloc(sizeof(*outer_context_1) * 100);
  OOP_SET_OBJECT(outer_context_1_oop, outer_context_1);
  OOP outer_context_2_oop = malloc(sizeof(*outer_context_2_oop));
  gst_object outer_context_2 = malloc(sizeof(*outer_context_2) * 100);
  OOP_SET_OBJECT(outer_context_2_oop, outer_context_2);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(context, outer_context_1_oop);
  OBJ_BLOCK_CONTEXT_SET_OUTER_CONTEXT(outer_context_1, outer_context_2_oop);
  outer_context_2->data[0x1] = FROM_INT(234);
  
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[4]);
  assert_true(context->data[0x02] == FROM_INT(234));

  free(_gst_this_context_oop[0]);
}

static void should_ivar_non_local_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { NON_LOCAL_RETURN_IVAR_BC, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
 
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(999));

  free(_gst_this_context_oop[0]);
}

static void should_self_non_local_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { NON_LOCAL_RETURN_SELF_BC, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_self[0] = malloc(sizeof(*_gst_self[0]));
  gst_object self_obj = malloc(sizeof(*self_obj) * 100);
  self_obj->data[0x06] = FROM_INT(999);
  OOP_SET_OBJECT(_gst_self[0], self_obj);
 
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[2]);
  assert_true(context->data[0x02] == _gst_self[0]);

  free(_gst_this_context_oop[0]);
}

static void should_literal_non_local_return(void **state) {

  (void) state;

  uint32_t bytecode[] = { NON_LOCAL_RETURN_LITERAL_BC, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
  _gst_this_context_oop[0] = malloc(sizeof(*_gst_this_context_oop[0]));
  context = malloc(sizeof(*context) * 100);
  context->data[0x07] = FROM_INT(2);
  OOP_SET_OBJECT(_gst_this_context_oop[0], context);
  _gst_literals[0] = malloc(sizeof(*_gst_literals[0]) * 100);
  _gst_literals[0][0x06] = FROM_INT(999);
  
  expect_function_calls(unwind_method, 1);

  bc();
  
  assert_true(tip == &bytecode[3]);
  assert_true(context->data[0x02] == FROM_INT(999));

  free(_gst_this_context_oop[0]);
}

static void should_line_number(void **state) {

  (void) state;

  uint32_t bytecode[] = { LINE_NUMBER_BC, 0x06, END_OF_INTERPRETER_BC };
  tip = &bytecode[0];
 
  bc();
  
  assert_true(tip == &bytecode[3]);
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
      cmocka_unit_test(should_load_integer_to_register),
      cmocka_unit_test(should_load_integer_to_outer_scope),
      cmocka_unit_test(should_load_integer_to_ivar),
      cmocka_unit_test(should_move_register_to_register),
      cmocka_unit_test(should_move_outer_scope_to_register),
      cmocka_unit_test(should_move_ivar_to_register),
      cmocka_unit_test(should_move_register_to_outer_register),
      cmocka_unit_test(should_move_outer_register_to_outer_register),
      cmocka_unit_test(should_move_ivar_to_outer_register),
      cmocka_unit_test(should_move_register_to_ivar),
      cmocka_unit_test(should_move_outer_register_to_ivar),
      cmocka_unit_test(should_move_ivar_to_ivar),
      cmocka_unit_test(should_literal_send),
      cmocka_unit_test(should_self_send),
      cmocka_unit_test(should_super_send),
      cmocka_unit_test(should_register_send),
      cmocka_unit_test(should_outer_register_send),
      cmocka_unit_test(should_ivar_send),
      cmocka_unit_test(should_literal_immediate_send),
      cmocka_unit_test(should_self_immediate_send),
      cmocka_unit_test(should_super_immediate_send),
      cmocka_unit_test(should_register_immediate_send),
      cmocka_unit_test(should_outer_register_immediate_send),
      cmocka_unit_test(should_ivar_immediate_send),
      cmocka_unit_test(should_jump),
      cmocka_unit_test(should_register_jump_if_true),
      cmocka_unit_test(should_outer_register_jump_if_true),
      cmocka_unit_test(should_ivar_jump_if_true),
      cmocka_unit_test(should_register_jump_if_false),
      cmocka_unit_test(should_outer_register_jump_if_false),
      cmocka_unit_test(should_ivar_jump_if_false),
      cmocka_unit_test(should_register_return),
      cmocka_unit_test(should_outer_register_return),
      cmocka_unit_test(should_ivar_return),
      cmocka_unit_test(should_self_return),
      cmocka_unit_test(should_literal_return),
      cmocka_unit_test(should_register_non_local_return),
      cmocka_unit_test(should_outer_register_non_local_return),
      cmocka_unit_test(should_ivar_non_local_return),
      cmocka_unit_test(should_self_non_local_return),
      cmocka_unit_test(should_literal_non_local_return),
      cmocka_unit_test(should_line_number),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

