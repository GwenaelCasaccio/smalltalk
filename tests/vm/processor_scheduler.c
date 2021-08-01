#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"
#include "libgst/processor_scheduler.h"

OOP _gst_nil_oop = (OOP) 0x2222;

void __wrap_nomemory(bool fatal) {
  function_called();
}

void __wrap_perror(const char *str) {
  function_called();
}

static void should_take_the_processor_scheduler_lock(void **state) {

  (void) state;

  OOP processorSchedulerOOP = malloc(sizeof(*processorSchedulerOOP));
  gst_object processorScheduler = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processorSchedulerOOP, processorScheduler);
  OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(processorScheduler, _gst_nil_oop);

  wait_for_processor_scheduler(processorSchedulerOOP, 0);

  assert_true(OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(processorScheduler) == FROM_INT(0));
}

static void should_release_the_processor_scheduler_lock(void **state) {

  (void) state;

  OOP processorSchedulerOOP = malloc(sizeof(*processorSchedulerOOP));
  gst_object processorScheduler = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processorSchedulerOOP, processorScheduler);
  OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(processorScheduler, FROM_INT(0));

  signal_and_broadcast_for_processor_scheduler(processorSchedulerOOP, 0);

  assert_true(OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(processorScheduler) == _gst_nil_oop);
}

static void should_not_release_the_processor_scheduler_lock(void **state) {

  (void) state;

  OOP processorSchedulerOOP = malloc(sizeof(*processorSchedulerOOP));
  gst_object processorScheduler = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processorSchedulerOOP, processorScheduler);
  OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(processorScheduler, FROM_INT(1));

  expect_function_calls(__wrap_perror, 1);
  expect_function_calls(__wrap_nomemory, 1);

  signal_and_broadcast_for_processor_scheduler(processorSchedulerOOP, 0);
}

static _Atomic int counter = 0;

static void *owner_of_processor_scheduler(void *arg) {

  (void) arg;

  OOP processorSchedulerOOP = (OOP) arg;

  wait_for_processor_scheduler(processorSchedulerOOP, 0);

  atomic_fetch_add(&counter, 1);

  signal_and_broadcast_for_processor_scheduler(processorSchedulerOOP, 0);

  return NULL;
}

static void should_take_and_release_the_processor_scheduler(void **state) {

  (void) state;

  pthread_t owner_thread;

  OOP processorSchedulerOOP = malloc(sizeof(*processorSchedulerOOP));
  gst_object processorScheduler = calloc(20, sizeof(OOP));
  OOP_SET_OBJECT(processorSchedulerOOP, processorScheduler);
  OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(processorScheduler, _gst_nil_oop);

  wait_for_processor_scheduler(processorSchedulerOOP, 1);

  pthread_create(&owner_thread, NULL, owner_of_processor_scheduler, processorSchedulerOOP);

  assert_true(counter == 0);

  signal_and_broadcast_for_processor_scheduler(processorSchedulerOOP, 1);

  pthread_join(owner_thread, NULL);

  assert_true(counter == 1);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_take_the_processor_scheduler_lock),
      cmocka_unit_test(should_release_the_processor_scheduler_lock),
      cmocka_unit_test(should_not_release_the_processor_scheduler_lock),
      cmocka_unit_test(should_take_and_release_the_processor_scheduler)
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

