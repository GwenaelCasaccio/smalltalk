#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

thread_local unsigned long _gst_bytecode_counter = 0;
unsigned long _gst_saved_bytecode_counter = 0;
const char *_gst_kernel_file_path;
const char *_gst_image_file_path;
const char *_gst_binary_image_name;
int _gst_verbosity;
bool _gst_regression_testing;

OOP _gst_raw_profile;
OOP _gst_nil_oop;
OOP _gst_true_oop;
OOP _gst_false_oop;
OOP _gst_smalltalk_namespace_symbol;
OOP _gst_symbol_table;
OOP _gst_current_namespace;
OOP _gst_vm_primitives_symbol;
OOP _gst_array_class;
OOP _gst_method_dictionary_class;
signed char _gst_log2_sizes[32] = {0};

#include "libgst/method_dictionary.c"

void __wrap_nomemory(int fatal){check_expected(fatal);

  function_called();

  return ;
}

void __wrap__gst_alloc_obj(size_t size, OOP *p_oop) {
  abort();
}

void __wrap__gst_errorf(const char *str, ...) {
  abort();
}

gst_object __wrap__gst_alloc_words(size_t size) {
  check_expected(size);

  function_called();

  return calloc(size, sizeof(OOP));
}

void __wrap__gst_init_symbols_pass1(void) {
  abort();
}

void __wrap__gst_init_symbols_pass2(void) {
  abort();
}

void __wrap__gst_init_builtin_objects_classes(void) {
  abort();
}

OOP __wrap__gst_intern_string(const char *str) {
  abort();
}

void __wrap__gst_tenure_all_survivors(void) {
  abort();
}

OOP __wrap__gst_make_instance_variable_array(OOP superclassOOP,
                                             const char *variableString) {
  abort();
}

OOP __wrap__gst_make_class_variable_dictionary(const char *variableNames,
                                               OOP classOOP) {
  abort();
}

OOP __wrap__gst_make_pool_array(const char *poolNames) {
  abort();
}

void *__wrap_xmalloc(size_t size) {
  abort();
}

void __wrap_xfree(void *ptr) {
  abort();
}

void __wrap__gst_init_process_system(void) {
  abort();
}

char *__wrap__gst_relocate_path(const char *path) {
  abort();
}

void __wrap__gst_swap_objects(OOP oop1, OOP oop2) {
  abort();
}

void __wrap__gst_set_primitive_attributes(int primitive, prim_table_entry *pte) {
  abort();
}

OOP __wrap__gst_register_oop(OOP oop) {
  abort();
}

prim_table_entry *__wrap__gst_get_primitive_attributes(int primitive) {
  abort();
}

bool __wrap__gst_is_pipe(int fd) {
  abort();
}

void __wrap__gst_restore_symbols(void) {
  abort();
}

void __wrap__gst_inc_grow_registry(void) {
  abort();
}

static OOP *oop_to_instantiate = NULL;
static OOP *next_oop_to_allocate = NULL;

gst_object __wrap_instantiate(OOP class_oop, OOP *p_oop) {
  check_expected(class_oop);
  check_expected(p_oop);
  assert_non_null(p_oop);

  assert_non_null(next_oop_to_allocate);

  *p_oop = *next_oop_to_allocate;

  gst_object result = (gst_object)mock();
  OOP_SET_OBJECT(*p_oop, result);

  next_oop_to_allocate++;

  function_called();

  return OOP_TO_OBJ(*p_oop);
}

static OOP *oop_to_instantiate_with = NULL;
static OOP *next_oop_to_allocate_with = NULL;

gst_object __wrap_instantiate_with(OOP class_oop, size_t numIndexedFields, OOP *p_oop) {
  check_expected(class_oop);
  check_expected(numIndexedFields);
  check_expected(p_oop);
  assert_non_null(p_oop);

  assert_non_null(next_oop_to_allocate_with);

  *p_oop = *next_oop_to_allocate_with;

  gst_object result = (gst_object)mock();
  OOP_SET_OBJECT(*p_oop, result);

  next_oop_to_allocate_with++;

  function_called();

  return OOP_TO_OBJ(*p_oop);
}

uintptr_t __wrap_scramble(uintptr_t x) {
  check_expected(x);

  function_called();

  return (uintptr_t) mock();
}

size_t __wrap_new_num_fields(size_t value) {
  check_expected(value);

  function_called();

  return (size_t) mock();
}

static void should_instantiate_method_dictionary(void **state) {
  (void) state;

  _gst_method_dictionary_class = (OOP)0x1234;
  _gst_array_class = (OOP)0x2345;

  oop_to_instantiate = calloc(100, sizeof(*oop_to_instantiate));
  oop_to_instantiate[0] = malloc(sizeof(*oop_to_instantiate[0]));
  next_oop_to_allocate = &oop_to_instantiate[0];

  oop_to_instantiate_with = calloc(100, sizeof(*oop_to_instantiate_with));
  oop_to_instantiate_with[0] = malloc(sizeof(*oop_to_instantiate_with[0]));
  oop_to_instantiate_with[1] = malloc(sizeof(*oop_to_instantiate_with[1]));
  next_oop_to_allocate_with = &oop_to_instantiate_with[0];

  expect_value(__wrap_new_num_fields, value, 100);
  will_return(__wrap_new_num_fields, 32);

  expect_function_calls(__wrap_new_num_fields, 1);

  gst_object method_dictionary_result_obj = malloc(sizeof(OOP) * 100);

  expect_value(__wrap_instantiate, class_oop, _gst_method_dictionary_class);
  expect_any(__wrap_instantiate, p_oop);
  will_return(__wrap_instantiate, method_dictionary_result_obj);
  expect_function_call(__wrap_instantiate);

  gst_object keys_object = malloc(100 * sizeof(OOP));
  expect_value(__wrap_instantiate_with, class_oop, _gst_array_class);
  expect_value(__wrap_instantiate_with, numIndexedFields, 32);
  expect_any(__wrap_instantiate_with, p_oop);
  will_return(__wrap_instantiate_with, keys_object);

  gst_object values_object = malloc(100 * sizeof(OOP));
  expect_value(__wrap_instantiate_with, class_oop, _gst_array_class);
  expect_value(__wrap_instantiate_with, numIndexedFields, 32);
  expect_any(__wrap_instantiate_with, p_oop);
  will_return(__wrap_instantiate_with, values_object);

  expect_function_calls(__wrap_instantiate_with, 2);

  OOP method_dictionary_oop = _gst_method_dictionary_new(100);

  assert_true(method_dictionary_oop == oop_to_instantiate[0]);
  assert_true(TO_INT(OBJ_METHOD_DICTIONARY_GET_TALLY(OOP_TO_OBJ(method_dictionary_oop))) == 0);
  assert_true(OBJ_METHOD_DICTIONARY_GET_KEYS(OOP_TO_OBJ(method_dictionary_oop)) == oop_to_instantiate_with[0]);
  assert_true(OBJ_METHOD_DICTIONARY_GET_VALUES(OOP_TO_OBJ(method_dictionary_oop)) == oop_to_instantiate_with[1]);
}

static void when_method_dictionary_find_key(void **state) {
  (void) state;

  OOP method_dictionary_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object method_dictionary_obj = calloc(100, sizeof(OOP));

  OOP_SET_OBJECT(method_dictionary_oop, method_dictionary_obj);

  OOP keys_array_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object keys_array_obj = calloc(100, sizeof(OOP));

  OBJ_SET_SIZE(keys_array_obj, FROM_INT(25));
  OOP_SET_OBJECT(keys_array_oop, keys_array_obj);

  OBJ_METHOD_DICTIONARY_SET_KEYS(method_dictionary_obj, keys_array_oop);

  OOP key_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object key_obj = calloc(100, sizeof(OOP));

  OOP_SET_OBJECT(key_oop, key_obj);

  ARRAY_AT_PUT(keys_array_oop, 15, key_oop);

  ssize_t value_idx =
      _gst_method_dictionary_find_key(method_dictionary_oop, key_oop);

  assert_true(value_idx == 15);
}

static void when_method_dictionary_find_key_not_found(void **state) {
  (void)state;

  OOP method_dictionary_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object method_dictionary_obj = calloc(100, sizeof(OOP));

  OOP_SET_OBJECT(method_dictionary_oop, method_dictionary_obj);

  OOP keys_array_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object keys_array_obj = calloc(100, sizeof(OOP));

  OBJ_SET_SIZE(keys_array_obj, FROM_INT(25));
  OOP_SET_OBJECT(keys_array_oop, keys_array_obj);

  OBJ_METHOD_DICTIONARY_SET_KEYS(method_dictionary_obj, keys_array_oop);

  OOP key_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object key_obj = calloc(100, sizeof(OOP));

  OOP_SET_OBJECT(key_oop, key_obj);

  ssize_t value_idx =
      _gst_method_dictionary_find_key(method_dictionary_oop, key_oop);

  assert_true(value_idx == -1);
}

static void when_method_dictionary_add_method(void **state) {
  (void)state;

  OOP method_dictionary_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object method_dictionary_obj = calloc(100, sizeof(OOP));

  OBJ_METHOD_DICTIONARY_SET_TALLY(method_dictionary_obj, FROM_INT(0));

  OOP_SET_OBJECT(method_dictionary_oop, method_dictionary_obj);

  OOP keys_array_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object keys_array_obj = calloc(100, sizeof(OOP));

  OBJ_SET_SIZE(keys_array_obj, FROM_INT(25));
  OOP_SET_OBJECT(keys_array_oop, keys_array_obj);

  OOP values_array_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object values_array_obj = calloc(100, sizeof(OOP));

  OBJ_SET_SIZE(values_array_obj, FROM_INT(25));
  OOP_SET_OBJECT(values_array_oop, values_array_obj);

  OBJ_METHOD_DICTIONARY_SET_KEYS(method_dictionary_obj, keys_array_oop);
  OBJ_METHOD_DICTIONARY_SET_VALUES(method_dictionary_obj, values_array_oop);

  OOP key_oop = malloc(sizeof(*method_dictionary_oop));
  gst_object key_obj = calloc(100, sizeof(OOP));

  OOP_SET_OBJECT(key_oop, key_obj);

  OOP value_oop = malloc(sizeof(*method_dictionary_oop));

  _gst_object_identity = 0;

  expect_value(__wrap_scramble, x, 1);
  will_return(__wrap_scramble, 1);
  expect_function_calls(__wrap_scramble, 1);

  _gst_method_dictionary_at_put(method_dictionary_oop, key_oop, value_oop);

  ssize_t value_idx = _gst_method_dictionary_find_key(method_dictionary_oop, key_oop);
  assert_true(value_idx != -1);
  assert_true(TO_INT(OBJ_METHOD_DICTIONARY_GET_TALLY(method_dictionary_obj)) == 1);
  assert_true(ARRAY_AT(keys_array_oop, value_idx) == key_oop);
  assert_true(ARRAY_AT(values_array_oop, value_idx) == value_oop);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(should_instantiate_method_dictionary),
      cmocka_unit_test(when_method_dictionary_find_key),
      cmocka_unit_test(when_method_dictionary_find_key_not_found),
      cmocka_unit_test(when_method_dictionary_add_method),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
