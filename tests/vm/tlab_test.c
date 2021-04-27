#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/heap/tlab.c"

void __wrap_nomemory(int fatal);
void __wrap__gst_scavenge(void);
void *__wrap_xcalloc(size_t nb, size_t size);
void __wrap_set_except_flag_for_thread(mst_Boolean value, size_t thread_id);
void __wrap__gst_vm_global_barrier_wait(void);
void __wrap__gst_vm_end_barrier_wait(void);

void __wrap_nomemory(int fatal) {
  check_expected(fatal);

  function_called();

  return ;
}

void __wrap__gst_scavenge(void) {
  function_called();
}

void *__wrap_xcalloc(size_t nb, size_t size) {

  check_expected(nb);
  check_expected(size);

  function_called();

  return calloc(nb, size);
}

void __wrap_set_except_flag_for_thread(mst_Boolean value, size_t thread_id) {
  function_called();
}

void __wrap__gst_vm_global_barrier_wait(void) {
  function_called();
}

void __wrap__gst_vm_end_barrier_wait(void) {
  function_called();
}

static void should_fail_to_reset_heap_when_heap_is_null(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_tlab_reset_for_local_heap(NULL);
}

static void should_fail_to_reset_heap_when_is_not_initialized(void **state) {
  gst_heap_t heap;

  (void) state;

  heap.meta_inf.reserved_for_allocator = NULL;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_tlab_reset_for_local_heap(&heap);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(should_fail_to_reset_heap_when_heap_is_null),
     cmocka_unit_test(should_fail_to_reset_heap_when_is_not_initialized),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
