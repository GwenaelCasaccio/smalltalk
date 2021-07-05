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

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_get_and_set_vm_thread_id),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
