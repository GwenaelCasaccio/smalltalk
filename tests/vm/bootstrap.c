#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/dict.c"

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
OOP _gst_sip_hash_key_symbol;

void __wrap_nomemory(int fatal) {
  check_expected(fatal);

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

static void should_initialize_builtins_objects(void **state) {

  (void) state;

  _gst_array_class = (OOP) 0x1234;
  _gst_system_dictionary_class = (OOP) 0x2345;
  _gst_smalltalk_namespace_symbol = (OOP) 0x3456;
  _gst_processor_scheduler_class = (OOP) 0x6789;
  _gst_nil_oop = (OOP) 0x5678;

  _gst_symbol_table = malloc(sizeof(*_gst_symbol_table));
  OOP_SET_OBJECT(_gst_symbol_table, NULL);

  _gst_smalltalk_dictionary = malloc(sizeof(*_gst_smalltalk_dictionary));
  OOP_SET_OBJECT(_gst_smalltalk_dictionary, NULL);

  _gst_processor_oop[0] = malloc(sizeof(*_gst_processor_oop[0]));
  OOP_SET_OBJECT(_gst_processor_oop[0], NULL);

  expect_value(__wrap__gst_alloc_words, size, 515);
  expect_value(__wrap__gst_alloc_words, size, 520);
  expect_value(__wrap__gst_alloc_words, size, 12);

  expect_function_calls(__wrap__gst_alloc_words, 3);

  init_proto_oops();

  assert_true(OOP_TO_OBJ(_gst_symbol_table) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_symbol_table)) == _gst_array_class);
  for (size_t i = 0; i < 515 - OBJ_HEADER_SIZE_WORDS; i++) {
    assert_true(OOP_TO_OBJ(_gst_symbol_table)->data[i] == _gst_nil_oop);
  }

  assert_true(OOP_TO_OBJ(_gst_smalltalk_dictionary) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == _gst_system_dictionary_class);
  assert_true(OBJ_NAMESPACE_GET_TALLY(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == FROM_INT(0));
  for (size_t i = 1; i < 520 - OBJ_HEADER_SIZE_WORDS; i++) {
    if (i != 2) {
      assert_true(OOP_TO_OBJ(_gst_smalltalk_dictionary)->data[i] == _gst_nil_oop);
    } else {
      assert_true(OBJ_NAMESPACE_GET_NAME(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == _gst_smalltalk_namespace_symbol);
    }
  }

  assert_true(OOP_TO_OBJ(_gst_processor_oop[0]) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_processor_oop[0])) == _gst_processor_scheduler_class);
  for (size_t i = 0; i < 11 - OBJ_HEADER_SIZE_WORDS; i++) {
    if (i != 7) {
      assert_true(OOP_TO_OBJ(_gst_processor_oop[0])->data[i] == _gst_nil_oop);
    } else {
      assert_true(OBJ_PROCESSOR_SCHEDULER_GET_VM_THREAD_ID(OOP_TO_OBJ(_gst_processor_oop[0])) == FROM_INT(0));
    }
  }
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_initialize_builtins_objects),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
