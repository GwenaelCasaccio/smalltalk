#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"
#include "libgst/semaphore.h"

OOP _gst_nil_oop = (OOP) 0x2222;

void __wrap_nomemory(bool fatal) {
  function_called();
}

void __wrap_perror(const char *str) {
  function_called();
}

static void should_take_the_semaphore_lock(void **state) {

  (void) state;

  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  gst_object semaphore = calloc(10, sizeof(OOP));
  OOP_SET_OBJECT(semaphoreOOP, semaphore);
  OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(semaphore, _gst_nil_oop);

  wait_for_semaphore(semaphoreOOP, 0);

  assert_true(OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(semaphore) == FROM_INT(0));
}

static void should_release_the_semaphore_lock(void **state) {

  (void) state;

  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  gst_object semaphore = calloc(10, sizeof(OOP));
  OOP_SET_OBJECT(semaphoreOOP, semaphore);
  OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(semaphore, FROM_INT(0));

  signal_and_broadcast_for_semaphore(semaphoreOOP, 0);

  assert_true(OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(semaphore) == _gst_nil_oop);
}

static void should_not_release_the_semaphore_lock(void **state) {

  (void) state;

  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  gst_object semaphore = calloc(10, sizeof(OOP));
  OOP_SET_OBJECT(semaphoreOOP, semaphore);
  OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(semaphore, FROM_INT(1));

  expect_function_calls(__wrap_perror, 1);
  expect_function_calls(__wrap_nomemory, 1);

  signal_and_broadcast_for_semaphore(semaphoreOOP, 0);
}

static _Atomic int counter = 0;

static void *owner_of_semaphore(void *arg) {

  (void) arg;

  OOP semaphoreOOP = (OOP) arg;

  wait_for_semaphore(semaphoreOOP, 0);

  atomic_fetch_add(&counter, 1);

  signal_and_broadcast_for_semaphore(semaphoreOOP, 0);

  return NULL;
}

static void should_take_and_release_the_semaphore(void **state) {

  (void) state;

  pthread_t owner_thread;

  OOP semaphoreOOP = malloc(sizeof(*semaphoreOOP));
  gst_object semaphore = calloc(10, sizeof(OOP));
  OOP_SET_OBJECT(semaphoreOOP, semaphore);
  OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(semaphore, _gst_nil_oop);

  wait_for_semaphore(semaphoreOOP, 1);

  pthread_create(&owner_thread, NULL, owner_of_semaphore, semaphoreOOP);

  assert_true(counter == 0);

  signal_and_broadcast_for_semaphore(semaphoreOOP, 1);

  pthread_join(owner_thread, NULL);

  assert_true(counter == 1);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_take_the_semaphore_lock),
      cmocka_unit_test(should_release_the_semaphore_lock),
      cmocka_unit_test(should_not_release_the_semaphore_lock),
      cmocka_unit_test(should_take_and_release_the_semaphore)
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

