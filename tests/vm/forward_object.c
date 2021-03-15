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

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex);

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex);

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

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex) {
  check_expected(mutex);

  return mock_type(int);
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex) {
  check_expected(mutex);

  return mock_type(int);
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

  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0;

  for (size_t i = MAX_OOP_TABLE_SIZE; i; i >>= 1) {
    expect_value(__wrap__gst_heap_create, address, NULL);
    expect_value(__wrap__gst_heap_create, size, (sizeof(struct oop_s) * i));
  }

  will_return_always(__wrap__gst_heap_create, NULL);

  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x1234);

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

  expect_value(__wrap_xcalloc, nb, 524289);
  expect_value(__wrap_xcalloc, size, 4);

  _gst_init_oop_table(NULL, 0x1234);

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x1234);
  assert_true(_gst_mem.ot_size == 0x1234);

  assert_true(_gst_mem.ot_arena_size == 524289);
  for (size_t i = 0; i < 524289; i++) {
    assert_true(_gst_mem.ot_arena[i].thread_id == 0);
    assert_true(_gst_mem.ot_arena[i].free_oops == 32768);
  }

  free(_gst_mem.ot_arena);
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

  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, MAX_OOP_TABLE_SIZE);

  assert_true(_gst_mem.num_free_oops == 0);
  assert_true(_gst_mem.ot_size == 0);
}

static void init_oop_table_with_heap_but_null_sbrk(void **state) {
  (void) state;

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
  will_return(__wrap__gst_heap_sbrk, NULL);

  expect_value(__wrap_xcalloc, nb, 524289);
  expect_value(__wrap_xcalloc, size, 4);

  expect_function_calls(__wrap_nomemory, 1);

  _gst_init_oop_table(NULL, 0x1234);

  assert_true(_gst_mem.num_free_oops == 0);
  assert_true(_gst_mem.ot_size == 0x1234);

  free(_gst_mem.ot_arena);
}

static void realloc_oop_table(void **state) {
  (void) state;

  oop_heap = (char *)0xABAB;
  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap__gst_heap_sbrk, hd, 0xABAB);
  expect_value(__wrap__gst_heap_sbrk, size, 0x100 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, 0x2000);

  assert_true(_gst_realloc_oop_table(0x1100));

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x100);
  assert_true(_gst_mem.ot_size == 0x1100);
}

static void realloc_oop_table_with_smaller_new_number_of_oop(void **state) {
  (void) state;

  oop_heap = (char *)0xABAB;
  nomemory_called = 0;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  assert_true(_gst_realloc_oop_table(0x100));

  assert_true(nomemory_called == 0);
  assert_true(_gst_mem.num_free_oops == 0x0);
  assert_true(_gst_mem.ot_size == 0x1000);
}

static void realloc_oop_table_sbrk_fail(void **state) {
  (void) state;

  oop_heap = (char *)0xABAB;
  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 0x1000;

  expect_value(__wrap__gst_heap_sbrk, hd, 0xABAB);
  expect_value(__wrap__gst_heap_sbrk, size, 0x100 * sizeof(struct oop_s));
  will_return(__wrap__gst_heap_sbrk, NULL);

  expect_function_calls(__wrap_nomemory, 1);

  assert_false(_gst_realloc_oop_table(0x1100));

  assert_true(_gst_mem.num_free_oops == 0x0);
  assert_true(_gst_mem.ot_size == 0x1000);
}

static void test_alloc_oop(void **state) {
  OOP result;

  (void) state;

  _gst_mem.ot = calloc(1000, sizeof(struct oop_s));
  if (!_gst_mem.ot) { abort(); }

  _gst_mem.num_free_oops = 1000;
  _gst_mem.ot_size = 1000;
  _gst_mem.next_oop_to_sweep = _gst_mem.last_allocated_oop = _gst_mem.last_swept_oop = _gst_mem.ot - 1;
  _gst_mem.live_flags = 0x2;

  expect_value(__wrap_pthread_mutex_lock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_lock, 0);

  expect_value(__wrap_pthread_mutex_unlock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_unlock, 0);

  result = alloc_oop((PTR) 0xBABA, 0x1234);

  assert_true(result == _gst_mem.ot);
  assert_true(OOP_GET_FLAGS(result) == 0x1234);
  assert_true(OOP_TO_OBJ(result) == (gst_object) 0xBABA);
  assert_true(_gst_mem.last_swept_oop == result);
  assert_true(_gst_mem.num_free_oops == 999);
  assert_true(_gst_mem.last_allocated_oop == result);

  /* Skip first allocated and test if the others are still free */
  for (size_t i = 1; i < 1000; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == 0);
  }

  free(_gst_mem.ot);
}

static void test_alloc_oop_with_allocated_objects(void **state) {
  OOP result;

  (void) state;

  _gst_mem.live_flags = 0x2;

  _gst_mem.ot = calloc(1000, sizeof(struct oop_s));
  if (!_gst_mem.ot) { abort(); }

  /* Skip first allocated and test if the others are still free */
  for (size_t i = 0; i < 500; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], _gst_mem.live_flags);
  }

  _gst_mem.num_free_oops = 500;
  _gst_mem.ot_size = 1000;
  _gst_mem.last_allocated_oop = &_gst_mem.ot[499];
  _gst_mem.next_oop_to_sweep = _gst_mem.last_swept_oop = _gst_mem.ot - 1;

  expect_value(__wrap_pthread_mutex_lock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_lock, 0);

  expect_value(__wrap_pthread_mutex_unlock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_unlock, 0);

  result = alloc_oop((PTR) 0xBABA, 0x1234);

  assert_true(result == &_gst_mem.ot[500]);
  assert_true(OOP_GET_FLAGS(result) == 0x1234);
  assert_true(OOP_TO_OBJ(result) == (gst_object) 0xBABA);
  assert_true(_gst_mem.last_swept_oop == result);
  assert_true(_gst_mem.num_free_oops == 499);
  assert_true(_gst_mem.last_allocated_oop == result);

  /* Skip firsts allocated and test if the others are still free */
  for (size_t i = 0; i < 500; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == _gst_mem.live_flags);
  }

  assert_true(OOP_GET_FLAGS(&_gst_mem.ot[500]) == 0x1234);

  for (size_t i = 501; i < 1000; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == 0);
  }

  free(_gst_mem.ot);
}

static void test_alloc_oop_with_no_more_slots_available(void **state) {
  OOP result;

  (void) state;

  _gst_mem.live_flags = 0x2;

  _gst_mem.ot = calloc(1001, sizeof(struct oop_s));
  if (!_gst_mem.ot) { abort(); }

  for (size_t i = 0; i < 1000; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], _gst_mem.live_flags);
  }

  _gst_mem.num_free_oops = 0;
  _gst_mem.ot_size = 1000;
  _gst_mem.last_allocated_oop = &_gst_mem.ot[999];
  _gst_mem.next_oop_to_sweep = _gst_mem.last_swept_oop = _gst_mem.ot - 1;

  expect_value(__wrap_pthread_mutex_lock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_lock, 0);

  expect_value(__wrap_pthread_mutex_unlock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_unlock, 0);

  result = alloc_oop((PTR) 0xBABA, 0x1234);

  assert_true(result == NULL);
  assert_true(_gst_mem.last_swept_oop == (_gst_mem.ot - 1));
  assert_true(_gst_mem.num_free_oops == 0);
  assert_true(_gst_mem.last_allocated_oop == &_gst_mem.ot[999]);

  /* Skip firsts allocated and test if the others are still free */
  for (size_t i = 0; i < 1000; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == _gst_mem.live_flags);
  }

  assert_true(OOP_GET_FLAGS(&_gst_mem.ot[1000]) == 0);

  free(_gst_mem.ot);
}

static void test_alloc_oop_with_lazy_sweep(void **state) {
  OOP result;

  (void) state;

  _gst_mem.live_flags = 0x2;

  _gst_mem.ot = calloc(1000, sizeof(struct oop_s));
  if (!_gst_mem.ot) { abort(); }

  /* Skip first allocated and test if the others are still free */
  for (size_t i = 0; i < 500; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], _gst_mem.live_flags | F_REACHABLE);
  }

  _gst_mem.num_free_oops = 500;
  _gst_mem.ot_size = 1000;
  _gst_mem.last_allocated_oop = &_gst_mem.ot[499];
  _gst_mem.last_swept_oop = &_gst_mem.ot[300];
  _gst_mem.next_oop_to_sweep = &_gst_mem.ot[600];

  expect_value(__wrap_pthread_mutex_lock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_lock, 0);

  expect_value(__wrap_pthread_mutex_unlock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_unlock, 0);

  expect_value(__wrap__gst_sweep_oop, oop, &_gst_mem.ot[500]);

  result = alloc_oop((PTR) 0xBABA, 0x1234);

  assert_true(result == &_gst_mem.ot[500]);
  assert_true(OOP_GET_FLAGS(result) == 0x1234);
  assert_true(OOP_TO_OBJ(result) == (gst_object) 0xBABA);
  assert_true(_gst_mem.last_swept_oop == result);
  assert_true(_gst_mem.num_free_oops == 500);
  assert_true(_gst_mem.last_allocated_oop == result);

  /* Skip firsts allocated and test if the others are still free */
  for (size_t i = 0; i <= 300; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == (_gst_mem.live_flags | F_REACHABLE));
  }

  for (size_t i = 301; i < 500; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == _gst_mem.live_flags);
  }

  assert_true(OOP_GET_FLAGS(&_gst_mem.ot[500]) == 0x1234);

  for (size_t i = 501; i < 1000; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == 0);
  }

  free(_gst_mem.ot);
}

static void test_alloc_oop_with_lazy_sweep_and_overflow_last_oop(void **state) {
  OOP result;

  (void) state;

  sweep_called = 0;

  _gst_mem.live_flags = 0x2;

  _gst_mem.ot = calloc(1000, sizeof(struct oop_s));
  if (!_gst_mem.ot) { abort(); }

  /* Skip first allocated and test if the others are still free */
  for (size_t i = 0; i < 500; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], _gst_mem.live_flags | F_REACHABLE);
  }

  _gst_mem.num_free_oops = 500;
  _gst_mem.ot_size = 1000;
  _gst_mem.last_allocated_oop = &_gst_mem.ot[499];
  _gst_mem.last_swept_oop = &_gst_mem.ot[300];
  _gst_mem.next_oop_to_sweep = &_gst_mem.ot[400];

  expect_value(__wrap_pthread_mutex_lock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_lock, 0);

  expect_value(__wrap_pthread_mutex_unlock, mutex, &alloc_oop_mutex);
  will_return(__wrap_pthread_mutex_unlock, 0);

  expect_function_call(__wrap__gst_finished_incremental_gc);

  result = alloc_oop((PTR) 0xBABA, 0x1234);

  assert_true(result == &_gst_mem.ot[500]);
  assert_true(OOP_GET_FLAGS(result) == 0x1234);
  assert_true(OOP_TO_OBJ(result) == (gst_object) 0xBABA);
  assert_true(_gst_mem.last_swept_oop == result);
  assert_true(_gst_mem.num_free_oops == 499);
  assert_true(_gst_mem.last_allocated_oop == result);

  /* Skip firsts allocated and test if the others are still free */
  for (size_t i = 0; i <= 300; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == (_gst_mem.live_flags | F_REACHABLE));
  }

  for (size_t i = 301; i <= 400; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == _gst_mem.live_flags);
  }

  for (size_t i = 401; i < 500; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == (_gst_mem.live_flags | F_REACHABLE));
  }

  assert_true(OOP_GET_FLAGS(&_gst_mem.ot[500]) == 0x1234);

  for (size_t i = 501; i < 1000; i++) {
    assert_true(OOP_GET_FLAGS(&_gst_mem.ot[i]) == 0);
  }

  assert_true(sweep_called == 0);

  free(_gst_mem.ot);
}

static void test_alloc_oop_arena_entry(void **state) {
  (void) state;

  nomemory_called = 0;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, 4);

  _gst_alloc_oop_arena(32768 * 14);

  for(size_t i = 0; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
  }

  for(size_t i = 5; i < 10; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  assert_true(_gst_alloc_oop_arena_entry(2) == 10);
  assert_true(nomemory_called == 0);

  free(_gst_mem.ot_arena);
}

static void test_alloc_oop_arena_entry_no_memory(void **state) {
  (void) state;

  expect_value(__wrap_xcalloc, nb, 15);
  expect_value(__wrap_xcalloc, size, 4);

  _gst_alloc_oop_arena(32768 * 14);

  for(size_t i = 0; i < 5; i++) {
    _gst_mem.ot_arena[i].thread_id = 1;
  }

  for(size_t i = 5; i < _gst_mem.ot_arena_size; i++) {
    _gst_mem.ot_arena[i].free_oops = 0;
  }

  expect_function_calls(__wrap_nomemory, 1);

  assert_true(_gst_alloc_oop_arena_entry(2) == 0);

  free(_gst_mem.ot_arena);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(null_test_success),
     cmocka_unit_test(init_oop_table_with_heap),
     cmocka_unit_test(init_oop_table_with_null_heap),
     cmocka_unit_test(init_oop_table_with_heap_smaller_than_requested_size),
     cmocka_unit_test(init_oop_table_with_heap_but_null_sbrk),
     cmocka_unit_test(realloc_oop_table),
     cmocka_unit_test(realloc_oop_table_with_smaller_new_number_of_oop),
     cmocka_unit_test(realloc_oop_table_sbrk_fail),
     cmocka_unit_test(test_alloc_oop),
     cmocka_unit_test(test_alloc_oop_with_allocated_objects),
     cmocka_unit_test(test_alloc_oop_with_no_more_slots_available),
     cmocka_unit_test(test_alloc_oop_with_lazy_sweep),
     cmocka_unit_test(test_alloc_oop_with_lazy_sweep_and_overflow_last_oop),
     cmocka_unit_test(test_alloc_oop_arena_entry),
     cmocka_unit_test(test_alloc_oop_arena_entry_no_memory),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
