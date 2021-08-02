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

OOP __wrap_FROM_INT(intptr_t number) {
  function_called();

  return (OOP) mock();
}

intptr_t __wrap_TO_INT(OOP oop) {
  function_called();

  return (intptr_t) mock();
}

void __wrap_SET_STACKTOP_BOOLEAN(bool value) {
  function_called();
}

OOP __wrap_POP_OOP() {
  function_called();

  return (OOP) mock();
}

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

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_semaphore_notify_all),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

