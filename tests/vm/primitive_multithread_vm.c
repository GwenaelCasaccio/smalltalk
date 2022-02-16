#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#define PRIM_SUCCEEDED return (false)
#define PRIM_FAILED return (true)

void change_process_context(OOP newProcess);
void empty_context_stack(void);
OOP highest_priority_process(void);
OOP GET_PROCESS_LISTS();
void add_first_link(OOP listOOP, OOP processOOP);

OOP switch_to_process[100];
bool async_queue_enabled[100] = { true };
async_queue_entry queued_async_signals_tail[100];
async_queue_entry *queued_async_signals[100] = { &queued_async_signals_tail[0] };
async_queue_entry *queued_async_signals_sig[100] = { &queued_async_signals_tail[0] };
_Atomic(void *const *) dispatch_vec_per_thread[100] = { NULL };
thread_local void *const *global_normal_bytecodes;
bool _gst_interp_need_to_wait[100] = { false };

OOP *sp[100];

#include "libgst/primitive_process.inl"

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
}

void __wrap__gst_init_context(void) {
  function_called();
}

void __wrap__gst_invalidate_method_cache(void) {
  function_called();
}

void __wrap__gst_check_process_state(void) {
  function_called();
}

OOP __wrap__gst_interpret(OOP processOOP) {
  check_expected(processOOP);

  function_called();

  return (OOP) mock();
}

size_t __wrap__gst_alloc_oop_arena_entry_init(uint16_t thread_id) {
  check_expected(thread_id);

  function_called();

  return (size_t) mock();
}

gst_tlab_t *__wrap_gst_allocate_in_heap(gst_heap_t *heap, uint16_t thread_id) {
  check_expected(heap);
  check_expected(thread_id);

  function_called();

  return (gst_tlab_t *) mock();
}

OOP __wrap_highest_priority_process(void) {
  function_called();

  return (OOP) mock();
}

void __wrap_change_process_context(OOP processOOP) {
  check_expected(processOOP);

  function_called();
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

void __wrap_pthread_exit(void *retval) {
  check_expected(retval);

  function_called();
}

void __wrap__gst_vm_global_barrier_wait(void) {

  function_called();
}

void __wrap__gst_vm_end_barrier_wait(void) {

  function_called();
}

void __wrap_perror(const char *msg) {
  assert_true(msg != NULL);

  function_called();
}


OOP __wrap_GET_PROCESS_LISTS() {
  function_called();

  return (OOP) mock();
}

void __wrap_add_first_link(OOP listOOP, OOP processOOP) {
  check_expected(listOOP);
  check_expected(processOOP);

  function_called();
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
  _gst_count_threaded_vm = 1;

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

static void should_exit_thread(void **state) {

  (void) state;

  // When
  current_thread_id = 0;
  _gst_count_threaded_vm = 1;

  // Then
  expect_function_calls(__wrap__gst_vm_global_barrier_wait, 1);

  expect_value(__wrap_set_except_flag_for_thread, reset, false);
  expect_value(__wrap_set_except_flag_for_thread, thread_id, current_thread_id);
  expect_function_calls(__wrap_set_except_flag_for_thread, 1);

  expect_function_calls(__wrap_empty_context_stack, 1);

  expect_value(__wrap_pthread_cond_signal, cond, &_gst_vm_end_barrier_cond);
  will_return(__wrap_pthread_cond_signal, 0);
  expect_function_calls(__wrap_pthread_cond_signal, 1);

  expect_value(__wrap_pthread_exit, retval, NULL);
  expect_function_calls(__wrap_pthread_exit, 1);

  // Since pthread_kill is called the compiler does not generate much more
  // code. No need to test the result
  VMpr_Processor_killThread(123, 0);

  assert_true(_gst_count_threaded_vm == 0);
}

static void should_initialize_new_vm_thread(void **state) {
  (void) state;

  // When
  _gst_mem.gen0 = (gst_heap_t *) 0x1234;
  _gst_count_total_threaded_vm = 1;
  _gst_interpret_thread_counter= 1;
  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = NULL;
  _gst_mem.tlab_per_thread[current_thread_id] = NULL;
  _gst_nil_oop = malloc(sizeof(*_gst_nil_oop));

  OOP processor_oop = malloc(sizeof(*processor_oop));
  gst_object processor = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processor_oop, processor);
  OBJ_PROCESSOR_SCHEDULER_SET_VM_THREAD_ID(processor, FROM_INT(123));

  // Then
  expect_value(__wrap__gst_alloc_oop_arena_entry_init, thread_id, 1);
  will_return(__wrap__gst_alloc_oop_arena_entry_init, 0x1000);
  expect_function_calls(__wrap__gst_alloc_oop_arena_entry_init, 1);

  expect_value(__wrap_gst_allocate_in_heap, heap, _gst_mem.gen0);
  expect_value(__wrap_gst_allocate_in_heap, thread_id, 1);
  will_return(__wrap_gst_allocate_in_heap, 0x1000);
  expect_function_calls(__wrap_gst_allocate_in_heap, 1);

  expect_function_calls(__wrap__gst_init_context, 1);

  expect_function_calls(__wrap__gst_invalidate_method_cache, 1);

  expect_function_calls(__wrap__gst_check_process_state, 1);

  const OOP processOOP = malloc(sizeof(*processOOP));
  const gst_object process = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  will_return(__wrap_highest_priority_process, processOOP);
  expect_function_calls(__wrap_highest_priority_process, 1);

  const OOP processListsOOP = malloc(sizeof(*processListsOOP));
  const gst_object processLists = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processListsOOP, processLists);

  const OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  const gst_object semaphore = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(semaphoreOOP, semaphore);

  processLists->data[8] = semaphoreOOP;

  will_return(__wrap_GET_PROCESS_LISTS, processListsOOP);
  expect_function_calls(__wrap_GET_PROCESS_LISTS, 1);

  expect_value(__wrap_add_first_link, listOOP, semaphoreOOP);
  expect_value(__wrap_add_first_link, processOOP, processOOP);
  expect_function_calls(__wrap_add_first_link, 1);

  expect_value(__wrap_change_process_context, processOOP, processOOP);
  expect_function_calls(__wrap_change_process_context, 1);

  expect_function_calls(__wrap__gst_check_process_state, 1);

  expect_function_calls(__wrap__gst_vm_end_barrier_wait, 1);

  expect_value(__wrap__gst_interpret, processOOP, processOOP);
  will_return(__wrap__gst_interpret, NULL);
  expect_function_calls(__wrap__gst_interpret, 1);

  start_vm_thread(processor_oop);

  //
  assert_true(current_thread_id == 1);
  assert_true(_gst_count_total_threaded_vm == 2);
  assert_true(_gst_mem.tlab_per_thread[current_thread_id] == (gst_tlab_t *) 0x1000);
  assert_true(_gst_sample_counter == 1);
  assert_true(_gst_interpret_thread_counter == 2);
  assert_true(switch_to_process[current_thread_id] == _gst_nil_oop);
  assert_true(_gst_this_context_oop[current_thread_id] == _gst_nil_oop);
  assert_true(async_queue_enabled[current_thread_id] == true);
  assert_true(queued_async_signals[current_thread_id] == &queued_async_signals_tail[current_thread_id]);
  assert_true(queued_async_signals_sig[current_thread_id] == &queued_async_signals_tail[current_thread_id]);
  assert_true(_gst_interp_need_to_wait[current_thread_id] == false);
  assert_true(_gst_processor_oop[current_thread_id] == processor_oop);
  assert_true(OBJ_PROCESSOR_SCHEDULER_GET_VM_THREAD_ID(OOP_TO_OBJ(processor_oop)) == FROM_INT(current_thread_id));
  assert_true(dispatch_vec_per_thread[current_thread_id] == global_normal_bytecodes);
}

static void should_set_current_thread_id(void **state) {
  (void) state;

  // When
  OOP stack[1] = { (OOP) 0x1234 };

  current_thread_id = 0;

  sp[current_thread_id] = &stack[0];

  // Then
  VMpr_Processor_currentThreadId(0, 0);

  //
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(*sp[current_thread_id] == FROM_INT(current_thread_id));
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_not_create_vm_thread_if_cannot_create_a_thread),
      cmocka_unit_test(should_create_vm_thread),
      cmocka_unit_test(should_abort_if_cannot_signal_conditional_variable),
      cmocka_unit_test(should_exit_thread),
      cmocka_unit_test(should_initialize_new_vm_thread),
      cmocka_unit_test(should_set_current_thread_id),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
