#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#define PRIM_SUCCEEDED return (false)
#define PRIM_FAILED return (true)

OOP *sp[100];
OOP single_step_semaphore;

#include "libgst/primitive_atomic.inl"

OOP _gst_processor_oop[100];
OOP _gst_nil_oop = (OOP) 0x2222;
thread_local unsigned long _gst_primitives_executed;

static void should_not_atomic_set_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_set(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_set_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_set(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_set(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_set(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(1234));
}

static void should_not_atomic_get_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[2] = { NULL, classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[1];

  // Then
  intptr_t result = VMpr_Atomic_get(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[1]);
  assert_true(stack[1] == classOOP);
}

static void should_not_atomic_get_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[2] = { NULL, FROM_INT(-123) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[1];

  // Then
  intptr_t result = VMpr_Atomic_get(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[1]);
  assert_true(stack[1] == FROM_INT(-123));
}

static void should_atomic_get(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(123);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[2] = { NULL, FROM_INT(1) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[1];

  // Then
  intptr_t result = VMpr_Atomic_get(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(stack[0] == FROM_INT(123));
}

static void should_not_atomic_add_when_term_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1234), classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_add(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_add_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_add(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_add_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_add(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_add(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(1234);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_add(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(2468));
}

static void should_not_atomic_sub_when_term_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1234), classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_sub(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_sub_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_sub(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_sub_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_sub(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_sub(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(1234);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_sub(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_or_when_term_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1234), classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_or(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_or_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_or(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_or_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_or(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_or(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(1);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(2) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_or(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(3));
}

static void should_not_atomic_and_when_term_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1234), classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_and(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_and_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_and(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_and_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_and(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_and(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(1);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(3) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_and(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(1));
}

static void should_not_atomic_xor_when_term_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1234), classOOP };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_xor(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_xor_when_index_is_not_int(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, classOOP, FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_xor(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_not_atomic_xor_when_index_is_underflow(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(0);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(-123), FROM_INT(1234) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_xor(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[2]);
  assert_true(object->data[0] == FROM_INT(0));
}

static void should_atomic_xor(void **state) {

  (void) state;

  // When
  OOP classOOP = malloc(sizeof(*classOOP));
  gst_object class = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(classOOP, class);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class, FROM_INT(6 << ISP_NUMFIXEDFIELDS));

  OOP objectOOP = malloc(sizeof(*objectOOP));
  gst_object object = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(objectOOP, object);
  object->data[0] = FROM_INT(1);
  OBJ_SET_CLASS(object, classOOP);

  OOP stack[3] = { NULL, FROM_INT(1), FROM_INT(3) };
  stack[0] = objectOOP;
  sp[current_thread_id] = &stack[2];

  // Then
  intptr_t result = VMpr_Atomic_xor(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(object->data[0] == FROM_INT(2));
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_not_atomic_set_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_set_when_index_is_underflow),
      cmocka_unit_test(should_atomic_set),
      cmocka_unit_test(should_not_atomic_get_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_get_when_index_is_underflow),
      cmocka_unit_test(should_atomic_get),
      cmocka_unit_test(should_not_atomic_add_when_term_is_not_int),
      cmocka_unit_test(should_not_atomic_add_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_add_when_index_is_underflow),
      cmocka_unit_test(should_atomic_add),
      cmocka_unit_test(should_not_atomic_sub_when_term_is_not_int),
      cmocka_unit_test(should_not_atomic_sub_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_sub_when_index_is_underflow),
      cmocka_unit_test(should_atomic_sub),
      cmocka_unit_test(should_not_atomic_or_when_term_is_not_int),
      cmocka_unit_test(should_not_atomic_or_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_or_when_index_is_underflow),
      cmocka_unit_test(should_atomic_or),
      cmocka_unit_test(should_not_atomic_and_when_term_is_not_int),
      cmocka_unit_test(should_not_atomic_and_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_and_when_index_is_underflow),
      cmocka_unit_test(should_atomic_and),
      cmocka_unit_test(should_not_atomic_xor_when_term_is_not_int),
      cmocka_unit_test(should_not_atomic_xor_when_index_is_not_int),
      cmocka_unit_test(should_not_atomic_xor_when_index_is_underflow),
      cmocka_unit_test(should_atomic_xor),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
