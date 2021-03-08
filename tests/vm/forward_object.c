#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/forward_object.c"

static int nomemory_called = 0;

heap __wrap__gst_heap_create(PTR address, size_t size);

void __wrap_nomemory(int fatal);

PTR __wrap__gst_heap_sbrk(heap hd, size_t size);

void __wrap__gst_display_oop(OOP oop);

void __wrap__gst_display_oop_short(OOP oop);

static void null_test_success(void **state) {
  (void) state;
}

heap __wrap__gst_heap_create(PTR address, size_t size) {

  check_expected(address);
  check_expected(size);

  return mock_ptr_type(void *);
}

void __wrap_nomemory(int fatal) {
  nomemory_called++;

  return ;
}

PTR __wrap__gst_heap_sbrk(heap hd, size_t size) {

  check_expected(hd);
  check_expected(size);

  return mock_ptr_type(void *);
}

void __wrap__gst_display_oop(OOP oop) {
  abort();
  return ;
}

void __wrap__gst_display_oop_short(OOP oop) {
  abort();
  return ;
}

static void init_oop_table_with_null_heap(void **state) {
  (void) state;

  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  for (size_t i = MAX_OOP_TABLE_SIZE; i; i >>= 1) {
    expect_value(__wrap__gst_heap_create, address, NULL);
    expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * i));
  }

  will_return_always(__wrap__gst_heap_create, NULL);

  _gst_init_oop_table(NULL, 0x1234);

  assert_true(nomemory_called == 1);
  assert_true(_gst_mem.num_free_oops == 0);
  assert_true(_gst_mem.ot_size == 0);
}

static void init_oop_table_with_heap(void **state) {
  (void) state;

  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * MAX_OOP_TABLE_SIZE));
  will_return(__wrap__gst_heap_create, NULL);

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * (MAX_OOP_TABLE_SIZE >> 1)));
  will_return(__wrap__gst_heap_create, 0x1000);

  expect_value(__wrap__gst_heap_sbrk, hd, 0x1000);
  expect_value(__wrap__gst_heap_sbrk, size, 0x1234 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, 0x2000);

  _gst_init_oop_table(NULL, 0x1234);

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x1234);
  assert_true(_gst_mem.ot_size == 0x1234);
}

static void init_oop_table_with_heap_smaller_than_requested_size(void **state) {
  (void) state;

  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * MAX_OOP_TABLE_SIZE));
  will_return(__wrap__gst_heap_create, NULL);

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * (MAX_OOP_TABLE_SIZE >> 1)));
  will_return(__wrap__gst_heap_create, 0x1000);

  _gst_init_oop_table(NULL, MAX_OOP_TABLE_SIZE);

  assert_true(nomemory_called == 1);
  assert_true(_gst_mem.num_free_oops == 0);
  assert_true(_gst_mem.ot_size == 0);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(null_test_success),
     cmocka_unit_test(init_oop_table_with_heap),
     cmocka_unit_test(init_oop_table_with_null_heap),
     cmocka_unit_test(init_oop_table_with_heap_smaller_than_requested_size),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
