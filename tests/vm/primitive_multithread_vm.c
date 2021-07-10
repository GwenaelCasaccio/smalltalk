#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#define PRIM_SUCCEEDED return (false)
#define PRIM_FAILED return (true)

OOP highest_priority_process(void);
void change_process_context(OOP newProcess);

OOP switch_to_process[100];
bool async_queue_enabled[100] = { true };
async_queue_entry queued_async_signals_tail[100];
async_queue_entry *queued_async_signals[100] = { &queued_async_signals_tail[0] };
async_queue_entry *queued_async_signals_sig[100] = { &queued_async_signals_tail[0] };
_Atomic(void *const *) dispatch_vec_per_thread[100] = { NULL };
thread_local void *const *global_normal_bytecodes;
bool _gst_interp_need_to_wait[100] = { false };

OOP *sp[100];

void __wrap__gst_init_context(void);
void __wrap_empty_context_stack(void);

#include "libgst/primitive_process.inl"

OOP highest_priority_process(void) {
  abort();
}

void change_process_context(OOP newProcess) {
  abort();
}


OOP _gst_processor_oop[100];
OOP _gst_this_context_oop[100];
OOP _gst_nil_oop;
thread_local unsigned long _gst_sample_counter;
thread_local unsigned long _gst_primitives_executed;

void __wrap_empty_context_stack(void) {
  function_called();
}

void __wrap_nomemory(int fatal) {
  check_expected(fatal);

  function_called();

  return ;
}
void __wrap__gst_init_context(void) {
  abort();
}

void __wrap__gst_invalidate_method_cache(void) {
  abort();
}

void __wrap__gst_check_process_state(void) {
  abort();
}

OOP __wrap__gst_interpret(OOP processOOP) {
  abort();
}

void __wrap_set_except_flag_for_thread(bool reset, size_t thread_id) {
  check_expected(reset);
  check_expected(thread_id);

  function_called();
}

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {
  assert_true(thread != NULL);
  check_expected(attr);
  check_expected(start_routine);
  check_expected(arg);

  function_called();

  return (int) mock();
}

int __wrap_pthread_cond_signal(pthread_cond_t *cond) {
  check_expected(cond);

  function_called();

  return (int) mock();
}

void __wrap__gst_vm_global_barrier_wait(void) {

  function_called();

  return ;
}

void __wrap__gst_vm_end_barrier_wait(void) {

  function_called();

  return ;
}

void __wrap_perror(const char *msg) {
  assert_true(msg != NULL);

  function_called();

  return ;
}

static void should_not_create_vm_thread_if_cannot_create_a_thread(void **state) {

  (void) state;

  OOP processor = malloc(sizeof(*processor));

  // When
  current_thread_id = 0;
  _gst_count_threaded_vm = 1;

  sp[current_thread_id] = &processor;

  // Then
  expect_function_calls(__wrap__gst_vm_global_barrier_wait, 1);

  expect_value(__wrap_pthread_create, attr, NULL);
  expect_value(__wrap_pthread_create, start_routine, &start_vm_thread);
  expect_value(__wrap_pthread_create, arg, processor);
  will_return(__wrap_pthread_create, 1);
  expect_function_calls(__wrap_pthread_create, 1);

  expect_function_calls(__wrap_perror, 1);

  expect_function_calls(__wrap__gst_vm_end_barrier_wait, 1);

  intptr_t result = VMpr_Processor_newThread(123, 0);

  assert_true(_gst_count_threaded_vm == 1);
  assert_true(result == true);
}

static void should_create_vm_thread(void **state) {

  (void) state;

  OOP processor = malloc(sizeof(*processor));

  // When
  current_thread_id = 0;
  _gst_count_threaded_vm = 1;

  sp[current_thread_id] = &processor;

  // Then
  expect_function_calls(__wrap__gst_vm_global_barrier_wait, 1);

  expect_value(__wrap_pthread_create, attr, NULL);
  expect_value(__wrap_pthread_create, start_routine, &start_vm_thread);
  expect_value(__wrap_pthread_create, arg, processor);
  will_return(__wrap_pthread_create, 0);
  expect_function_calls(__wrap_pthread_create, 1);

  expect_function_calls(__wrap__gst_vm_end_barrier_wait, 1);

  intptr_t result = VMpr_Processor_newThread(123, 0);

  assert_true(_gst_count_threaded_vm == 2);
  assert_true(result == false);
}

static void should_abort_if_cannot_signal_conditional_variable(void **state) {

  (void) state;

  // When
  current_thread_id = 0;

  // Then
  expect_function_calls(__wrap__gst_vm_global_barrier_wait, 1);

  expect_value(__wrap_set_except_flag_for_thread, reset, false);
  expect_value(__wrap_set_except_flag_for_thread, thread_id, current_thread_id);
  expect_function_calls(__wrap_set_except_flag_for_thread, 1);

  expect_function_calls(__wrap_empty_context_stack, 1);

  expect_value(__wrap_pthread_cond_signal, cond, &_gst_vm_end_barrier_cond);
  will_return(__wrap_pthread_cond_signal, 1);
  expect_function_calls(__wrap_pthread_cond_signal, 1);

  expect_function_calls(__wrap_perror, 1);

  intptr_t result = VMpr_Processor_killThread(123, 0);

  assert_true(result == true);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_not_create_vm_thread_if_cannot_create_a_thread),
      cmocka_unit_test(should_create_vm_thread),
      cmocka_unit_test(should_abort_if_cannot_signal_conditional_variable),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
