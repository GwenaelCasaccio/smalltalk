#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdatomic.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gst.h"

#define PRIM_SUCCEEDED return (false)
#define PRIM_FAILED return (true)

OOP _gst_nil_oop = (OOP) 0x2222;
OOP _gst_true_oop = (OOP) 0x4444;
OOP _gst_false_oop = (OOP) 0x4440;
unsigned long _gst_primitives_executed = 0;
size_t current_thread_id = 0;

bool _gst_sync_signal(OOP semaphoreOOP, bool async);
void _gst_sync_wait(OOP semaphoreOOP);
void wait_for_semaphore(OOP semaphoreOOP, size_t thread_id);
void signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id);
OOP STACKTOP();
OOP FROM_INT(intptr_t number);
intptr_t TO_INT(OOP oop);
void SET_STACKTOP_BOOLEAN(bool value);
OOP POP_OOP();

void __wrap_nomemory(bool fatal) {
  function_called();
}

bool __wrap__gst_sync_signal(OOP semaphoreOOP, bool async) {

  check_expected(semaphoreOOP);
  check_expected(async);

  function_called();

  return (bool) mock();
}

void __wrap__gst_sync_wait(OOP semaphoreOOP) {

  check_expected(semaphoreOOP);

  function_called();
}

void __wrap_wait_for_semaphore(OOP semaphoreOOP, size_t thread_id) {

  check_expected(semaphoreOOP);
  check_expected(thread_id);

  function_called();
}

void __wrap_signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id) {

  check_expected(semaphoreOOP);
  check_expected(thread_id);

  function_called();
}

OOP __wrap_STACKTOP() {
  function_called();

  return (OOP) mock();
}

OOP __wrap_FROM_INT(intptr_t value) {

  check_expected(value);

  function_called();

  return (OOP) mock();
}

intptr_t __wrap_TO_INT(OOP oop) {

  check_expected(oop);

  function_called();

  return (intptr_t) mock();
}

void __wrap_SET_STACKTOP_BOOLEAN(bool value) {

  check_expected(value);

  function_called();
}

OOP __wrap_POP_OOP() {
  function_called();

  return (OOP) mock();
}

#define UNUSED(x) (void)(x)

#include "libgst/primitive_semaphore.inl"

static void should_semaphore_notify_all(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));
  gst_object object = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(oop, object);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_wait_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_wait_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_wait_for_semaphore, 1);

  expect_value(__wrap__gst_sync_signal, semaphoreOOP, oop);
  expect_value(__wrap__gst_sync_signal, async, false);
  will_return(__wrap__gst_sync_signal, true);

  expect_value(__wrap__gst_sync_signal, semaphoreOOP, oop);
  expect_value(__wrap__gst_sync_signal, async, false);
  will_return(__wrap__gst_sync_signal, false);
  expect_function_calls(__wrap__gst_sync_signal, 2);

  expect_value(__wrap_signal_and_broadcast_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_signal_and_broadcast_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_signal_and_broadcast_for_semaphore, 1);

  intptr_t result = VMpr_Semaphore_notifyAll(0, 123);

  assert_true(result == false);
}

static void should_semaphore_signal(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));
  gst_object object = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(oop, object);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_wait_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_wait_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_wait_for_semaphore, 1);

  expect_value(__wrap__gst_sync_signal, semaphoreOOP, oop);
  expect_value(__wrap__gst_sync_signal, async, true);
  will_return(__wrap__gst_sync_signal, false);
  expect_function_calls(__wrap__gst_sync_signal, 1);

  expect_value(__wrap_signal_and_broadcast_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_signal_and_broadcast_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_signal_and_broadcast_for_semaphore, 1);

  intptr_t result = VMpr_Semaphore_signalNotify(0, 0);

  assert_true(result == false);
}

static void should_semaphore_signal_notify(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));
  gst_object object = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(oop, object);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_wait_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_wait_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_wait_for_semaphore, 1);

  expect_value(__wrap__gst_sync_signal, semaphoreOOP, oop);
  expect_value(__wrap__gst_sync_signal, async, false);
  will_return(__wrap__gst_sync_signal, false);
  expect_function_calls(__wrap__gst_sync_signal, 1);

  expect_value(__wrap_signal_and_broadcast_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_signal_and_broadcast_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_signal_and_broadcast_for_semaphore, 1);

  intptr_t result = VMpr_Semaphore_signalNotify(1, 0);

  assert_true(result == false);
}

static void should_semaphore_lock(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));
  gst_object object = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(oop, object);
  OBJ_SEMAPHORE_SET_SIGNALS(object, (OOP) 0x1234);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_TO_INT, oop, 0x1234);
  will_return(__wrap_TO_INT, 10);
  expect_function_calls(__wrap_TO_INT, 1);

  expect_value(__wrap_SET_STACKTOP_BOOLEAN, value, true);
  expect_function_calls(__wrap_SET_STACKTOP_BOOLEAN, 1);

  OOP numberOOP = malloc(sizeof(*oop));
  expect_value(__wrap_FROM_INT, value, 0);
  will_return(__wrap_FROM_INT, numberOOP);
  expect_function_calls(__wrap_FROM_INT, 1);

  intptr_t result = VMpr_Semaphore_lock(0, 0);

  assert_true(result == false);
  assert_true(OBJ_SEMAPHORE_GET_SIGNALS(object) == numberOOP);
}

static void should_semaphore_wait(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));
  gst_object object = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(oop, object);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_wait_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_wait_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_wait_for_semaphore, 1);

  expect_value(__wrap__gst_sync_wait, semaphoreOOP, oop);
  expect_function_calls(__wrap__gst_sync_wait, 1);

  expect_value(__wrap_signal_and_broadcast_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_signal_and_broadcast_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_signal_and_broadcast_for_semaphore, 1);

  intptr_t result = VMpr_Semaphore_wait(0, 0);

  assert_true(result == false);
}

static void should_semaphore_wait_after_signalling(void **state) {

  (void) state;

  OOP oop = malloc(sizeof(*oop));

  OOP oop1 = malloc(sizeof(*oop1));

  will_return(__wrap_POP_OOP, oop1);
  expect_function_calls(__wrap_POP_OOP, 1);

  will_return(__wrap_STACKTOP, oop);
  expect_function_calls(__wrap_STACKTOP, 1);

  expect_value(__wrap_wait_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_wait_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_wait_for_semaphore, 1);

  expect_value(__wrap__gst_sync_signal, semaphoreOOP, oop1);
  expect_value(__wrap__gst_sync_signal, async, true);
  will_return(__wrap__gst_sync_signal, false);
  expect_function_calls(__wrap__gst_sync_signal, 1);

  expect_value(__wrap__gst_sync_wait, semaphoreOOP, oop);
  expect_function_calls(__wrap__gst_sync_wait, 1);

  expect_value(__wrap_signal_and_broadcast_for_semaphore, semaphoreOOP, oop);
  expect_value(__wrap_signal_and_broadcast_for_semaphore, thread_id, current_thread_id);
  expect_function_calls(__wrap_signal_and_broadcast_for_semaphore, 1);

  intptr_t result = VMpr_Semaphore_waitAfterSignalling(0, 0);

  assert_true(result == false);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_semaphore_notify_all),
      cmocka_unit_test(should_semaphore_signal),
      cmocka_unit_test(should_semaphore_signal_notify),
      cmocka_unit_test(should_semaphore_lock),
      cmocka_unit_test(should_semaphore_wait),
      cmocka_unit_test(should_semaphore_wait_after_signalling),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

