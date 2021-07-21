#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"
#include "libgst/object_pointer.h"

static void should_get_and_set_vm_thread_id(void **state) {

  (void) state;

  /* When */
  gst_object processor = calloc(20, sizeof(OOP));
  OBJ_PROCESSOR_SCHEDULER_SET_VM_THREAD_ID(processor, FROM_INT(123));

  /* Then */
  assert_true(TO_INT(OBJ_PROCESSOR_SCHEDULER_GET_VM_THREAD_ID(processor)) == 123);
}

static void should_get_and_set_processor_scheduler_lock_thread_id(void **state) {

  (void) state;

  /* When */
  gst_object processor = calloc(20, sizeof(OOP));
  OBJ_PROCESSOR_SCHEDULER_SET_LOCK_THREAD_ID(processor, FROM_INT(123));

  /* Then */
  assert_true(TO_INT(OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(processor)) == 123);
}

static void should_get_and_set_semaphore_signals(void **state) {

  (void) state;

  /* When */
  gst_object semaphore = calloc(20, sizeof(OOP));
  OBJ_SEMAPHORE_SET_SIGNALS(semaphore, FROM_INT(123));

  /* Then */
  assert_true(TO_INT(OBJ_SEMAPHORE_GET_SIGNALS(semaphore)) == 123);
}

static void should_get_and_set_semaphore_lock_thread_id(void **state) {

  (void) state;

  /* When */
  gst_object semaphore = calloc(20, sizeof(OOP));
  OBJ_SEMAPHORE_SET_LOCK_THREAD_ID(semaphore, FROM_INT(123));

  /* Then */
  assert_true(TO_INT(OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(semaphore)) == 123);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_get_and_set_vm_thread_id),
      cmocka_unit_test(should_get_and_set_processor_scheduler_lock_thread_id),
      cmocka_unit_test(should_get_and_set_semaphore_signals),
      cmocka_unit_test(should_get_and_set_semaphore_lock_thread_id),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
