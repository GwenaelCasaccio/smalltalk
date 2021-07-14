#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#define PRIM_SUCCEEDED return (false)
#define PRIM_FAILED return (true)

void suspend_process(OOP process);
bool resume_process(OOP process, bool resume);
bool is_process_ready(OOP process);
bool is_process_terminating(OOP process);
OOP get_active_process();
void active_process_yield();

OOP *sp[100];
OOP single_step_semaphore;

#include "libgst/primitive_process_scheduling.inl"

OOP _gst_processor_oop[100];
OOP _gst_nil_oop = (OOP) 0x2222;
thread_local unsigned long _gst_primitives_executed;

void __wrap_suspend_process(OOP processOOP) {
  check_expected(processOOP);

  function_called();
}

bool __wrap_resume_process(OOP processOOP, bool resume) {
  check_expected(processOOP);
  check_expected(resume);

  function_called();

  return (bool) mock();
}

bool __wrap_is_process_ready(OOP process) {
  function_called();

  return (bool) mock();
}

bool __wrap_is_process_terminating(OOP process) {
  function_called();

  return (bool) mock();
}

OOP __wrap_get_active_process() {
  function_called();

  return (OOP) mock();
}

void __wrap_active_process_yield() {
  function_called();
}

void _gst_sync_wait(OOP semaphore) {
  function_called();
}

static void should_not_suspend_process_when_scheduler_is_not_the_thread_one(void **state) {

  (void) state;

  // When
  OOP processorOOP = malloc(sizeof(*processorOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, processorOOP);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  sp[current_thread_id] = &processOOP;

  // Then
  intptr_t result = VMpr_Process_suspend(123, 0);

  assert_true(result == true);
}

static void should_suspend_process(void **state) {

  (void) state;

  // When
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, (OOP) 0x1234);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  sp[current_thread_id] = &processOOP;

  // Then
  expect_value(__wrap_suspend_process, processOOP, processOOP);
  expect_function_calls(__wrap_suspend_process, 1);

  intptr_t result = VMpr_Process_suspend(123, 0);

  assert_true(result == false);
}

static void should_not_resume_process_when_scheduler_is_not_the_thread_one(void **state) {

  (void) state;

  // When
  OOP processorOOP = malloc(sizeof(*processorOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, processorOOP);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  sp[current_thread_id] = &processOOP;

  // Then
  intptr_t result = VMpr_Process_resume(123, 0);

  assert_true(result == true);
}

static void should_resume_process(void **state) {

  (void) state;

  // When
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, (OOP) 0x1234);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  sp[current_thread_id] = &processOOP;

  // Then
  expect_value(__wrap_resume_process, processOOP, processOOP);
  expect_value(__wrap_resume_process, resume, false);
  will_return(__wrap_resume_process, true);
  expect_function_calls(__wrap_resume_process, 1);

  intptr_t result = VMpr_Process_resume(123, 0);

  assert_true(result == false);
}

static void should_resume_process_and_set_process_scheduler(void **state) {

  (void) state;

  // When
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, _gst_nil_oop);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  sp[current_thread_id] = &processOOP;

  // Then
  expect_value(__wrap_resume_process, processOOP, processOOP);
  expect_value(__wrap_resume_process, resume, false);
  will_return(__wrap_resume_process, true);
  expect_function_calls(__wrap_resume_process, 1);

  intptr_t result = VMpr_Process_resume(123, 0);

  assert_true(result == false);
  assert_true(OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(process) == _gst_processor_oop[current_thread_id]);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_not_suspend_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_suspend_process),
      cmocka_unit_test(should_not_resume_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_resume_process),
      cmocka_unit_test(should_resume_process_and_set_process_scheduler),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
