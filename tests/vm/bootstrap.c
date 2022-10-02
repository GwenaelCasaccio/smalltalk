#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/bootstrap.c"

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
/* Primary class variables.  These variables hold the class objects for
   most of the builtin classes in the system */
OOP _gst_abstract_namespace_class = NULL;
OOP _gst_array_class = NULL;
OOP _gst_arrayed_collection_class = NULL;
OOP _gst_association_class = NULL;
OOP _gst_behavior_class = NULL;
OOP _gst_binding_dictionary_class = NULL;
OOP _gst_block_closure_class = NULL;
OOP _gst_block_context_class = NULL;
OOP _gst_boolean_class = NULL;
OOP _gst_byte_array_class = NULL;
OOP _gst_c_callable_class = NULL;
OOP _gst_c_callback_descriptor_class = NULL;
OOP _gst_c_func_descriptor_class = NULL;
OOP _gst_c_object_class = NULL;
OOP _gst_c_type_class = NULL;
OOP _gst_callin_process_class = NULL;
OOP _gst_char_class = NULL;
OOP _gst_character_array_class = NULL;
OOP _gst_class_class = NULL;
OOP _gst_class_description_class = NULL;
OOP _gst_collection_class = NULL;
OOP _gst_compiled_block_class = NULL;
OOP _gst_compiled_code_class = NULL;
OOP _gst_compiled_method_class = NULL;
OOP _gst_context_part_class = NULL;
OOP _gst_continuation_class = NULL;
OOP _gst_date_class = NULL;
OOP _gst_deferred_variable_binding_class = NULL;
OOP _gst_dictionary_class = NULL;
OOP _gst_directed_message_class = NULL;
OOP _gst_message_lookup_class = NULL;
OOP _gst_false_class = NULL;
OOP _gst_file_descriptor_class = NULL;
OOP _gst_file_segment_class = NULL;
OOP _gst_file_stream_class = NULL;
OOP _gst_float_class = NULL;
OOP _gst_floatd_class = NULL;
OOP _gst_floate_class = NULL;
OOP _gst_floatq_class = NULL;
OOP _gst_fraction_class = NULL;
OOP _gst_hashed_collection_class = NULL;
OOP _gst_homed_association_class = NULL;
OOP _gst_identity_dictionary_class = NULL;
OOP _gst_identity_set_class = NULL;
OOP _gst_integer_class = NULL;
OOP _gst_interval_class = NULL;
OOP _gst_iterable_class = NULL;
OOP _gst_large_integer_class = NULL;
OOP _gst_large_negative_integer_class = NULL;
OOP _gst_large_positive_integer_class = NULL;
OOP _gst_large_zero_integer_class = NULL;
OOP _gst_link_class = NULL;
OOP _gst_linked_list_class = NULL;
OOP _gst_lookup_key_class = NULL;
OOP _gst_lookup_table_class = NULL;
OOP _gst_magnitude_class = NULL;
OOP _gst_memory_class = NULL;
OOP _gst_message_class = NULL;
OOP _gst_metaclass_class = NULL;
OOP _gst_method_context_class = NULL;
OOP _gst_method_dictionary_class = NULL;
OOP _gst_method_info_class = NULL;
OOP _gst_namespace_class = NULL;
OOP _gst_number_class = NULL;
OOP _gst_object_class = NULL;
OOP _gst_object_memory_class = NULL;
OOP _gst_ordered_collection_class = NULL;
OOP _gst_permission_class = NULL;
OOP _gst_positionable_stream_class = NULL;
OOP _gst_process_class = NULL;
OOP _gst_processor_scheduler_class = NULL;
OOP _gst_read_stream_class = NULL;
OOP _gst_read_write_stream_class = NULL;
OOP _gst_root_namespace_class = NULL;
OOP _gst_security_policy_class = NULL;
OOP _gst_semaphore_class = NULL;
OOP _gst_sequenceable_collection_class = NULL;
OOP _gst_set_class = NULL;
OOP gst_small_integer_class = NULL;
OOP _gst_smalltalk_dictionary = NULL;
OOP _gst_sorted_collection_class = NULL;
OOP _gst_stream_class = NULL;
OOP _gst_string_class = NULL;
OOP _gst_sym_link_class = NULL;
OOP _gst_symbol_class = NULL;
OOP _gst_system_dictionary_class = NULL;
OOP _gst_time_class = NULL;
OOP _gst_true_class = NULL;
OOP _gst_undefined_object_class = NULL;
OOP _gst_unicode_character_class = NULL;
OOP _gst_unicode_string_class = NULL;
OOP _gst_variable_binding_class = NULL;
OOP _gst_weak_array_class = NULL;
OOP _gst_weak_set_class = NULL;
OOP _gst_weak_key_dictionary_class = NULL;
OOP _gst_weak_value_lookup_table_class = NULL;
OOP _gst_weak_identity_set_class = NULL;
OOP _gst_weak_key_identity_dictionary_class = NULL;
OOP _gst_weak_value_identity_dictionary_class = NULL;
OOP _gst_write_stream_class = NULL;
OOP _gst_processor_oop[100] = {NULL};
OOP _gst_key_hash_oop = NULL;
OOP _gst_debug_information_class = NULL;
OOP _gst_key_hash_class = NULL;

void __wrap_nil_fill(OOP *oop, size_t numWords) {
  check_expected(numWords);

  function_called();

  return;
}

OOP __wrap__gst_dictionary_add(OOP dictionaryOOP, OOP associationOOP) {
  return NULL;
}

OOP __wrap__gst_binding_dictionary_new(OOP dictionaryOOP, OOP associationOOP) {
  return NULL;
}

OOP __wrap__gst_string_new(OOP dictionaryOOP, OOP associationOOP) {
  return NULL;
}

OOP __wrap__gst_process_file(OOP dictionaryOOP, OOP associationOOP) {
  return NULL;
}

OOP __wrap__gst_msg_sendf(OOP foo) { return NULL; }

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
  expect_function_calls(__wrap__gst_alloc_words, 1);
  expect_value(__wrap_nil_fill, numWords, 512);
  expect_function_calls(__wrap_nil_fill, 1);

  expect_value(__wrap__gst_alloc_words, size, 520);
  expect_function_calls(__wrap__gst_alloc_words, 1);
  expect_value(__wrap_nil_fill, numWords, 512);
  expect_function_calls(__wrap_nil_fill, 1);

  expect_value(__wrap__gst_alloc_words, size, 12);
  expect_function_calls(__wrap__gst_alloc_words, 1);
  expect_value(__wrap_nil_fill, numWords, 9);
  expect_function_calls(__wrap_nil_fill, 1);

  init_proto_oops();

  assert_true(OOP_TO_OBJ(_gst_symbol_table) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_symbol_table)) == _gst_array_class);

  assert_true(OOP_TO_OBJ(_gst_smalltalk_dictionary) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == _gst_system_dictionary_class);
  assert_true(OBJ_NAMESPACE_GET_TALLY(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == FROM_INT(0));
  assert_true(OBJ_NAMESPACE_GET_NAME(OOP_TO_OBJ(_gst_smalltalk_dictionary)) == _gst_smalltalk_namespace_symbol);

  assert_true(OOP_TO_OBJ(_gst_processor_oop[0]) != NULL);
  assert_true(OBJ_CLASS(OOP_TO_OBJ(_gst_processor_oop[0])) == _gst_processor_scheduler_class);
  assert_true(OBJ_PROCESSOR_SCHEDULER_GET_VM_THREAD_ID(OOP_TO_OBJ(_gst_processor_oop[0])) == FROM_INT(0));
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_initialize_builtins_objects),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
