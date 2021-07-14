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

bool __wrap_is_process_ready(OOP processOOP) {
  check_expected(processOOP);

  function_called();

  return (bool) mock();
}

bool __wrap_is_process_terminating(OOP processOOP) {
  check_expected(processOOP);

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

void __wrap__gst_sync_wait(OOP semaphoreOOP) {
  check_expected(semaphoreOOP);

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

static void should_not_yield_process_when_scheduler_is_not_the_thread_one(void **state) {

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
  intptr_t result = VMpr_Process_yield(123, 0);

  assert_true(result == true);
}

static void should_not_yield_process_when_process_is_not_active(void **state) {

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
  will_return(__wrap_get_active_process, NULL);
  expect_function_calls(__wrap_get_active_process, 1);

  intptr_t result = VMpr_Process_yield(123, 0);

  assert_true(result == false);
}

static void should_not_yield_process(void **state) {

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
  will_return(__wrap_get_active_process, processOOP);
  expect_function_calls(__wrap_get_active_process, 1);

  expect_function_calls(__wrap_active_process_yield, 1);

  intptr_t result = VMpr_Process_yield(123, 0);

  assert_true(result == false);
}

static void should_not_step_process_when_scheduler_is_not_the_thread_one(void **state) {

  (void) state;

  // When
  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  OOP processorOOP = malloc(sizeof(*processorOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, processorOOP);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  OOP stack[2] = { processOOP, semaphoreOOP };
  sp[current_thread_id] = &stack[1];

  // Then
  intptr_t result = VMpr_Process_singleStepWaitingOn(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[1]);
}

static void should_not_step_process_when_process_is_not_ready(void **state) {

  (void) state;

  // When
  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, (OOP) 0x1234);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  OOP stack[2] = { processOOP, semaphoreOOP };
  sp[current_thread_id] = &stack[1];

  // Then
  expect_value(__wrap_is_process_ready, processOOP, processOOP);
  will_return(__wrap_is_process_ready, true);
  expect_function_calls(__wrap_is_process_ready, 1);

  intptr_t result = VMpr_Process_singleStepWaitingOn(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[1]);
}

static void should_not_step_process_when_process_is_terminating(void **state) {

  (void) state;

  // When
  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, (OOP) 0x1234);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  OOP stack[2] = { processOOP, semaphoreOOP };
  sp[current_thread_id] = &stack[1];

  // Then
  expect_value(__wrap_is_process_ready, processOOP, processOOP);
  will_return(__wrap_is_process_ready, false);
  expect_function_calls(__wrap_is_process_ready, 1);

  expect_value(__wrap_is_process_terminating, processOOP, processOOP);
  will_return(__wrap_is_process_terminating, true);
  expect_function_calls(__wrap_is_process_terminating, 1);

  intptr_t result = VMpr_Process_singleStepWaitingOn(123, 0);

  assert_true(result == true);
  assert_true(sp[current_thread_id] == &stack[1]);
}

static void should_step_process(void **state) {

  (void) state;

  // When
  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  OOP processOOP = malloc(sizeof(*processOOP));
  gst_object process = calloc(100, sizeof(OOP));
  OOP_SET_OBJECT(processOOP, process);
  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(process, (OOP) 0x1234);

  current_thread_id = 0;
  _gst_processor_oop[current_thread_id] = (OOP) 0x1234;
  OOP stack[3] = { NULL, processOOP, semaphoreOOP };
  sp[current_thread_id] = &stack[2];

  // Then
  expect_value(__wrap_is_process_ready, processOOP, processOOP);
  will_return(__wrap_is_process_ready, false);
  expect_function_calls(__wrap_is_process_ready, 1);

  expect_value(__wrap_is_process_terminating, processOOP, processOOP);
  will_return(__wrap_is_process_terminating, false);
  expect_function_calls(__wrap_is_process_terminating, 1);

  expect_value(__wrap__gst_sync_wait, semaphoreOOP, semaphoreOOP);
  expect_function_calls(__wrap__gst_sync_wait, 1);

  expect_value(__wrap_resume_process, processOOP, processOOP);
  expect_value(__wrap_resume_process, resume, true);
  will_return(__wrap_resume_process, true);
  expect_function_calls(__wrap_resume_process, 1);

  intptr_t result = VMpr_Process_singleStepWaitingOn(123, 0);

  assert_true(result == false);
  assert_true(sp[current_thread_id] == &stack[0]);
  assert_true(single_step_semaphore == semaphoreOOP);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_not_suspend_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_suspend_process),
      cmocka_unit_test(should_not_resume_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_resume_process),
      cmocka_unit_test(should_resume_process_and_set_process_scheduler),
      cmocka_unit_test(should_not_yield_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_not_yield_process_when_process_is_not_active),
      cmocka_unit_test(should_not_yield_process),
      cmocka_unit_test(should_not_step_process_when_scheduler_is_not_the_thread_one),
      cmocka_unit_test(should_not_step_process_when_process_is_not_ready),
      cmocka_unit_test(should_not_step_process_when_process_is_terminating),
      cmocka_unit_test(should_step_process),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
