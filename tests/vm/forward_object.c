#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/forward_object.c"

static int nomemory_called = 0;

static int sweep_called = 0;

void *__wrap_xcalloc(size_t nb, size_t size);

heap __wrap__gst_heap_create(PTR address, size_t size);

void __wrap_nomemory(int fatal);

PTR __wrap__gst_heap_sbrk(heap hd, size_t size);

void __wrap__gst_display_oop(OOP oop);

void __wrap__gst_display_oop_short(OOP oop);

void __wrap__gst_finished_incremental_gc(void);

void __wrap__gst_sweep_oop(OOP oop);

static void null_test_success(void **state) {
  (void) state;
}

void *__wrap_xcalloc(size_t nb, size_t size) {

  check_expected(nb);
  check_expected(size);

  return calloc(nb, size);
}

heap __wrap__gst_heap_create(PTR address, size_t size) {

  check_expected(address);
  check_expected(size);

  return mock_ptr_type(void *);
}

void __wrap_nomemory(int fatal) {
  nomemory_called++;

  check_expected(fatal);

  function_called();

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

void __wrap__gst_finished_incremental_gc(void) {
  function_called();
}

void __wrap__gst_sweep_oop(OOP oop) {
  check_expected(oop);

  sweep_called++;
}

static void init_oop_table_with_null_heap(void **state) {
  (void) state;

  _gst_mem.oop_heap = NULL;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  for (size_t i = MAX_OOP_TABLE_SIZE; i; i >>= 1) {
    expect_value(__wrap__gst_heap_create, address, NULL);
    expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * i));
  }

  will_return_always(__wrap__gst_heap_create, NULL);

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x8000);
}

static void init_oop_table_already_initialized(void **state) {
  (void) state;

  _gst_mem.oop_heap = (void *)0x1234;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x8000);
}

static void init_oop_table_not_aligned(void **state) {
  (void) state;

  _gst_mem.oop_heap = NULL;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x123);
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
  will_return(__wrap__gst_heap_create, 0xA000);

  expect_value(__wrap__gst_heap_sbrk, hd, 0xA000);
  expect_value(__wrap__gst_heap_sbrk, size, 0x8000 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, 0x2000);

  expect_value(__wrap_xcalloc, nb, 0x80001);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_init_oop_table(NULL, 0x8000);

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x8000);
  assert_true(_gst_mem.ot_size == 0x8000);

  assert_true(_gst_mem.ot_arena_size == 0x80001);

  assert_true(_gst_mem.ot_arena[0].thread_id == 0);
  assert_true(_gst_mem.ot_arena[0].free_oops == 32768);

  for (size_t i = 1; i < 0x80001; i++) {
    assert_true(_gst_mem.ot_arena[i].thread_id == UINT16_MAX);
    assert_true(_gst_mem.ot_arena[i].free_oops == 32768);
    assert_true(_gst_mem.ot_arena[i].first_free_oop == &_gst_mem.ot[i * 32768]);
  }

  assert_true(&_gst_mem.ot_arena[0] == _gst_mem.current_arena[0]);

  free(_gst_mem.ot_arena);
  _gst_mem.current_arena[0] = NULL;
}

static void init_oop_table_with_heap_smaller_than_requested_size(void **state) {
  (void) state;

  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * MAX_OOP_TABLE_SIZE));
  will_return(__wrap__gst_heap_create, NULL);

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * (MAX_OOP_TABLE_SIZE >> 1)));
  will_return(__wrap__gst_heap_create, 0x1000);

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, MAX_OOP_TABLE_SIZE);
}

static void init_oop_table_with_heap_but_null_sbrk(void **state) {
  (void) state;

  _gst_mem.oop_heap = NULL;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;
  _gst_mem.ot_arena = NULL;

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * MAX_OOP_TABLE_SIZE));
  will_return(__wrap__gst_heap_create, NULL);

  expect_value(__wrap__gst_heap_create, address, NULL);
  expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * (MAX_OOP_TABLE_SIZE >> 1)));
  will_return(__wrap__gst_heap_create, 0x1000);

  expect_value(__wrap__gst_heap_sbrk, hd, 0x1000);
  expect_value(__wrap__gst_heap_sbrk, size, 0x8000 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, NULL);

  expect_value(__wrap_xcalloc, nb, 0x80001);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x8000);

  free(_gst_mem.ot_arena);
}

static void realloc_oop_table(void **state) {
  (void) state;

  _gst_mem.oop_heap = (char *)0xABAB;
  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap__gst_heap_sbrk, hd, 0xABAB);
  expect_value(__wrap__gst_heap_sbrk, size, 0x7000 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, 0x2000);

  assert_true(_gst_realloc_oop_table(0x8000));

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x7000);
  assert_true(_gst_mem.ot_size == 0x8000);
}

static void realloc_oop_table_with_smaller_new_number_of_oop(void **state) {
  (void) state;

  _gst_mem.oop_heap = (char *)0xABAB;
  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x10000;

  assert_true(_gst_realloc_oop_table(0x8000));

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x0);
  assert_true(_gst_mem.ot_size == 0x10000);
}

static void realloc_oop_table_sbrk_fail(void **state) {
  (void) state;

  _gst_mem.oop_heap = (char *)0xABAB;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap__gst_heap_sbrk, hd, 0xABAB);
  expect_value(__wrap__gst_heap_sbrk, size, 0x7000 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, NULL);

  expect_value(__wrap_nomemory, fatal, 0);
  expect_function_calls(__wrap_nomemory, 1);

  assert_false(_gst_realloc_oop_table(0x8000));

  assert_true(_gst_mem.num_free_oops == 0x0);
  assert_true(_gst_mem.ot_size == 0x1000);
}

static void realloc_oop_table_not_initialized(void **state) {
  (void) state;

  _gst_mem.oop_heap = NULL;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  assert_false(_gst_realloc_oop_table(0x8000));
}

static void realloc_oop_table_not_aligned(void **state) {
  (void) state;

  _gst_mem.oop_heap = (char *)0xABAB;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  assert_false(_gst_realloc_oop_table(0x123));
}

static void test_alloc_oop_arena_entry(void **state) {
  (void) state;

  nomemory_called = 0;

  _gst_mem.ot_size = 32768 * 14;
  _gst_mem.current_arena[0] = NULL;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_alloc_oop_arena(32768 * 14);

  _gst_alloc_oop_arena_entry_init(0);

  _gst_mem.ot_arena[0].free_oops = 0;

  for(size_t i = 1; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
  }

  for(size_t i = 5; i < 10; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  assert_true(_gst_alloc_oop_arena_entry(0) == 10);
  assert_true(nomemory_called == 0);

  free(_gst_mem.ot_arena);
  _gst_mem.current_arena[0] = NULL;
}

static void test_alloc_oop_arena_entry_no_memory(void **state) {
  (void) state;

  _gst_mem.ot_size = 32768 * 14;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_alloc_oop_arena(32768 * 14);

  _gst_alloc_oop_arena_entry_init(0);

  _gst_mem.ot_arena[0].free_oops = 0;

  for(size_t i = 1; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  for(size_t i = 5; i < _gst_mem.ot_arena_size; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  assert_true(_gst_alloc_oop_arena_entry(0) == 0);

  free(_gst_mem.ot_arena);
  _gst_mem.current_arena[0] = NULL;
}

static void test_detach_oop_arena_entry(void **state) {
  (void) state;

  nomemory_called = 0;

  _gst_mem.ot_size = 32768 * 14;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_alloc_oop_arena(32768 * 14);

  _gst_alloc_oop_arena_entry_init(0);

  for(size_t i = 0; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
  }

  for(size_t i = 5; i < 10; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  _gst_alloc_oop_arena_entry(0);

  _gst_detach_oop_arena_entry(10);

  free(_gst_mem.ot_arena);
  _gst_mem.current_arena[0] = NULL;
}

static void test_detach_oop_arena_entry_overflow(void **state) {
  (void) state;

  nomemory_called = 0;

  _gst_mem.ot_size = 32768 * 14;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_alloc_oop_arena(32768 * 14);

  _gst_alloc_oop_arena_entry_init(0);

  for(size_t i = 0; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
  }

  for(size_t i = 5; i < 10; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  _gst_alloc_oop_arena_entry(0);

  _gst_detach_oop_arena_entry(100);

  for(size_t i = 0; i < 5; i++) {
    assert_int_equal(_gst_mem.ot_arena[i].thread_id, 1);
  }

  for(size_t i = 5; i < 10; i++) {
    assert_int_equal(_gst_mem.ot_arena[i].free_oops, 0);
  }

  free(_gst_mem.ot_arena);
  _gst_mem.current_arena[0] = NULL;
}

static void test_detach_oop_arena_entry_already_initialized(void **state) {
  _gst_forward_object_allocator_t arena_entry =
    {
     .free_oops = 32768,
     .thread_id = 1
    };

  (void) state;

  _gst_mem.current_arena[0] = &arena_entry;

  assert_int_equal(_gst_alloc_oop_arena_entry(UINT16_MAX), 0);

  _gst_mem.current_arena[0] = NULL;
}

static void test_detach_oop_arena_entry_invalid_thread_id(void **state) {
  (void) state;

  assert_int_equal(_gst_alloc_oop_arena_entry(UINT16_MAX), 0);
}

static void test_alloc_oop_arena_entry_init_reach_thread_limit(void **state) {
  (void) state;

  assert_int_equal(_gst_alloc_oop_arena_entry_init(UINT16_MAX), 0);
}

static void test_alloc_oop_arena_entry_init_thread_already_initialized(void **state) {
  (void) state;

  expect_value(__wrap_xcalloc, nb, 151);
  expect_value(__wrap_xcalloc, size, sizeof(*_gst_mem.ot_arena));

  _gst_alloc_oop_arena(150 * 32768);

  _gst_mem.current_arena[0] = &_gst_mem.ot_arena[100];

  assert_int_equal(_gst_alloc_oop_arena_entry_init(0), 100);
  assert_ptr_equal(_gst_mem.current_arena[0], &_gst_mem.ot_arena[100]);

  _gst_mem.current_arena[0] = NULL;
  free(_gst_mem.ot_arena);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(null_test_success),
     cmocka_unit_test(init_oop_table_with_heap),
     cmocka_unit_test(init_oop_table_with_null_heap),
     cmocka_unit_test(init_oop_table_already_initialized),
     cmocka_unit_test(init_oop_table_not_aligned),
     cmocka_unit_test(init_oop_table_with_heap_smaller_than_requested_size),
     cmocka_unit_test(init_oop_table_with_heap_but_null_sbrk),
     cmocka_unit_test(realloc_oop_table),
     cmocka_unit_test(realloc_oop_table_with_smaller_new_number_of_oop),
     cmocka_unit_test(realloc_oop_table_sbrk_fail),
     cmocka_unit_test(realloc_oop_table_not_initialized),
     cmocka_unit_test(realloc_oop_table_not_aligned),
     cmocka_unit_test(test_alloc_oop_arena_entry),
     cmocka_unit_test(test_alloc_oop_arena_entry_no_memory),
     cmocka_unit_test(test_detach_oop_arena_entry),
     cmocka_unit_test(test_detach_oop_arena_entry_overflow),
     cmocka_unit_test(test_detach_oop_arena_entry_already_initialized),
     cmocka_unit_test(test_detach_oop_arena_entry_invalid_thread_id),
     cmocka_unit_test(test_alloc_oop_arena_entry_init_reach_thread_limit),
     cmocka_unit_test(test_alloc_oop_arena_entry_init_thread_already_initialized),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
