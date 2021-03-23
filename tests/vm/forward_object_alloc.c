#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

void __wrap_nomemory(int fatal);

void __wrap__gst_detach_oop_arena_entry(size_t arena_index);

size_t __wrap__gst_alloc_oop_arena_entry(uint16_t thread_id);

void __wrap_nomemory(int fatal) {
  check_expected(fatal);
}

void __wrap__gst_detach_oop_arena_entry(size_t arena_index) {
  check_expected(arena_index);
}

size_t __wrap__gst_alloc_oop_arena_entry(uint16_t thread_id) {
  check_expected(thread_id);

  _gst_mem.current_arena[current_thread_id] = &_gst_mem.ot_arena[1];

  return mock_type(int);
}

static void test_alloc_oop(void **state) {
  OOP oop;

  (void) state;
  _gst_mem.live_flags = 1;

  _gst_mem.ot = calloc(100, sizeof(*_gst_mem.ot));
  _gst_mem.last_allocated_oop = NULL;
  _gst_mem.num_free_oops = 1000;

  current_thread_id = 0;

  _gst_alloc_oop_arena(32768);

  _gst_mem.ot_arena[0].free_oops = 100;
  _gst_mem.ot_arena[0].thread_id = 0;
  _gst_mem.ot_arena[0].first_free_oop = &_gst_mem.ot[0];

  _gst_mem.current_arena[current_thread_id] = &_gst_mem.ot_arena[0];

  for (size_t i = 0; i < 20; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], 1);
  }

  oop = alloc_oop((void *) 0xBABA, 0x1234);

  assert_true(_gst_mem.current_arena[current_thread_id]->free_oops == 99);
  assert_true(_gst_mem.current_arena[current_thread_id]->first_free_oop == &_gst_mem.ot[20]);
  assert_true(_gst_mem.last_allocated_oop == &_gst_mem.ot[20]);
  assert_true(_gst_mem.num_free_oops == 999);
  assert_true(OOP_TO_OBJ(oop) == (void *) 0xBABA);
  assert_true(OOP_GET_FLAGS(oop) == 0x1234);

  free(_gst_mem.ot);
  free(_gst_mem.ot_arena);
}

static void test_alloc_oop_and_allocate_new_arena_entry(void **state) {
  OOP oop;

  (void) state;
  _gst_mem.live_flags = 1;

  _gst_mem.ot = calloc(32768 * 2, sizeof(*_gst_mem.ot));
  _gst_mem.last_allocated_oop = NULL;
  _gst_mem.num_free_oops = 1000;

  current_thread_id = 0;

  _gst_alloc_oop_arena(32768 * 2);

  _gst_mem.ot_arena[0].free_oops = 0;
  _gst_mem.ot_arena[0].thread_id = 0;
  _gst_mem.ot_arena[0].first_free_oop = &_gst_mem.ot[0];

  _gst_mem.ot_arena[1].free_oops = 32768;
  _gst_mem.ot_arena[1].thread_id = 0;
  _gst_mem.ot_arena[1].first_free_oop = &_gst_mem.ot[32768];

  _gst_mem.current_arena[current_thread_id] = &_gst_mem.ot_arena[0];

  for (size_t i = 0; i < 20; i++) {
    OOP_SET_FLAGS(&_gst_mem.ot[i], 1);
  }

  expect_value(__wrap__gst_detach_oop_arena_entry, arena_index, 0);

  expect_value(__wrap__gst_alloc_oop_arena_entry, thread_id, 0);
  will_return(__wrap__gst_alloc_oop_arena_entry, 1);

  oop = alloc_oop((void *) 0xBABA, 0x1234);

  assert_true(_gst_mem.current_arena[current_thread_id]->free_oops == 32767);
  assert_true(_gst_mem.current_arena[current_thread_id]->first_free_oop == &_gst_mem.ot[32768]);
  assert_true(_gst_mem.last_allocated_oop == &_gst_mem.ot[32768]);
  assert_true(_gst_mem.num_free_oops == 999);
  assert_true(OOP_TO_OBJ(oop) == (void *) 0xBABA);
  assert_true(OOP_GET_FLAGS(oop) == 0x1234);

  free(_gst_mem.ot);
  free(_gst_mem.ot_arena);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(test_alloc_oop),
     cmocka_unit_test(test_alloc_oop_and_allocate_new_arena_entry),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
