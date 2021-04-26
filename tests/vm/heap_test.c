#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/heap/heap.c"

void __wrap_nomemory(int fatal);
int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size);
void __wrap_perror(const char *msg);

void __wrap_nomemory(int fatal) {
  check_expected(fatal);

  function_called();

  return ;
}

int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size) {
  check_expected(alignment);
  check_expected(size);

  function_called();

  assert_true(NULL != memptr);

  return mock_type(int);
}

void __wrap_perror(const char *msg) {
  check_expected(msg);

  function_called();
}

static void should_failed_when_heap_is_null(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_heap_new_area(NULL, 0x2000);
}

static void should_failed_when_heap_address_is_not_null(void **state) {
  gst_heap_t heap;
  gst_heap_t *heap_ptr = &heap;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_heap_new_area(&heap_ptr, 0x2000);
}

static void should_failed_when_posix_memalign_failed(void **state) {
  gst_heap_t *heap_ptr = NULL;

  (void) state;

  expect_value(__wrap_posix_memalign, alignment, 0x200000);
  expect_value(__wrap_posix_memalign, size, sizeof(gst_heap_t));
  will_return(__wrap_posix_memalign, 1);
  expect_function_calls(__wrap_posix_memalign, 1);

  expect_value(__wrap_perror, msg, "error while allocating heap");
  expect_function_calls(__wrap_perror, 1);

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_heap_new_area(&heap_ptr, 0x2000);

  assert_true(NULL == heap_ptr);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(should_failed_when_heap_is_null),
     cmocka_unit_test(should_failed_when_heap_address_is_not_null),
     cmocka_unit_test(should_failed_when_posix_memalign_failed),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
