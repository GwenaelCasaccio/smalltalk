#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/heap/tlab.c"

void __wrap_nomemory(int fatal);
void __wrap__gst_scavenge(void);
void *__wrap_xcalloc(size_t nb, size_t size);
void __wrap_set_except_flag_for_thread(bool value, size_t thread_id);
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

void __wrap_set_except_flag_for_thread(bool value, size_t thread_id) {
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

static void should_reset_local_heap_state(void **state) {
  gst_heap_t heap;
  gst_tlab_t *tlab;
  size_t nb_of_tlab;

  (void) state;

  heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  nb_of_tlab = get_total_of_tlab(&heap);
  heap.meta_inf.free_space = nb_of_tlab * SIZEOF_OOP * TLAB_ENTRY_SIZE;

  heap.meta_inf.reserved_for_allocator = calloc(nb_of_tlab, sizeof(gst_tlab_t));

  gst_tlab_reset_for_local_heap(&heap);

  tlab = heap.meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab; i++) {
    assert_true(tlab[i].thread_id == UINT16_MAX);
    assert_true(tlab[i].position == &heap.oop[i * TLAB_ENTRY_SIZE]);
    assert_true(tlab[i].end_of_buffer == &heap.oop[(i + 1) * TLAB_ENTRY_SIZE]);
  }

  free(heap.meta_inf.reserved_for_allocator);
}

static void should_fail_to_reset_heap_when_heap_is_null_for_local_heap_initialization(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_tlab_init_for_local_heap(NULL);
}

static void should_fail_to_reset_heap_when_allocator_is_initialized_for_local_heap_initialization(void **state) {
  gst_heap_t heap;

  (void) state;

  heap.meta_inf.reserved_for_allocator = (void *) 0xBABA;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_tlab_init_for_local_heap(&heap);
}

static void should_allocate_and_init_when_local_heap_is_initialized(void **state) {
  gst_heap_t heap;
  size_t nb_of_tlab;
  gst_tlab_t *tlab;

  (void) state;

  heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  nb_of_tlab = get_total_of_tlab(&heap);
  heap.meta_inf.reserved_for_allocator = NULL;
  heap.meta_inf.next_heap_area = NULL;

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_function_calls(__wrap_xcalloc, 1);

  gst_tlab_init_for_local_heap(&heap);

  assert_true(heap.meta_inf.reserved_for_allocator != NULL);

  tlab = heap.meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab; i++) {
    assert_true(tlab[i].thread_id == UINT16_MAX);
    assert_true(tlab[i].position == &heap.oop[i * TLAB_ENTRY_SIZE]);
    assert_true(tlab[i].end_of_buffer == &heap.oop[(i + 1) * TLAB_ENTRY_SIZE]);
  }
}

static void should_fail_to_allocate_and_init_when_heap_is_null(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_tlab_init_for_heap(NULL);
}

static void should_allocate_and_init_when_heap_is_initialized(void **state) {
  gst_heap_t heap;
  gst_heap_t next_heap;
  size_t nb_of_tlab;
  gst_tlab_t *tlab;

  (void) state;

  heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  nb_of_tlab = get_total_of_tlab(&heap);
  heap.meta_inf.reserved_for_allocator = NULL;
  heap.meta_inf.next_heap_area = &next_heap;

  next_heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  next_heap.meta_inf.reserved_for_allocator = NULL;
  next_heap.meta_inf.next_heap_area = NULL;

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_function_calls(__wrap_xcalloc, 2);

  gst_tlab_init_for_heap(&heap);

  assert_true(heap.meta_inf.reserved_for_allocator != NULL);

  tlab = heap.meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab; i++) {
    assert_true(tlab[i].thread_id == UINT16_MAX);
    assert_true(tlab[i].position == &heap.oop[i * TLAB_ENTRY_SIZE]);
    assert_true(tlab[i].end_of_buffer == &heap.oop[(i + 1) * TLAB_ENTRY_SIZE]);
  }

  assert_true(next_heap.meta_inf.reserved_for_allocator != NULL);

  tlab = next_heap.meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab; i++) {
    assert_true(tlab[i].thread_id == UINT16_MAX);
    assert_true(tlab[i].position == &next_heap.oop[i * TLAB_ENTRY_SIZE]);
    assert_true(tlab[i].end_of_buffer == &next_heap.oop[(i + 1) * TLAB_ENTRY_SIZE]);
  }
}

static void should_fail_to_allocate_tlab_entry_when_heap_is_null(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_heap(NULL, 0);
}

static void should_fail_to_allocate_tlab_entry_when_thread_id_is_invalid(void **state) {
  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_heap(NULL, UINT16_MAX);
}

static void should_allocate_tlab_entry(void **state) {
  gst_heap_t heap;
  gst_heap_t next_heap;
  size_t nb_of_tlab;
  gst_tlab_t *tlab;

  (void) state;

  /* When  */
  heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  nb_of_tlab = get_total_of_tlab(&heap);
  heap.meta_inf.reserved_for_allocator = NULL;
  heap.meta_inf.next_heap_area = &next_heap;

  next_heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  next_heap.meta_inf.reserved_for_allocator = NULL;
  next_heap.meta_inf.next_heap_area = NULL;

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_function_calls(__wrap_xcalloc, 2);

  gst_tlab_init_for_heap(&heap);

  /* Then  */
  for (size_t i = 0; i < nb_of_tlab; i++) {
    const gst_tlab_t *reserved_tlab = gst_allocate_in_heap(&heap, 0);
    assert_true(reserved_tlab != NULL);
    assert_true(reserved_tlab->thread_id == 0);
    assert_true(reserved_tlab->position == &heap.oop[i * TLAB_ENTRY_SIZE]);
  }

  for (size_t i = 0; i < nb_of_tlab; i++) {
    const gst_tlab_t *reserved_tlab = gst_allocate_in_heap(&heap, 0);
    assert_true(reserved_tlab != NULL);
    assert_true(reserved_tlab->thread_id == 0);
    assert_true(reserved_tlab->position == &next_heap.oop[i * TLAB_ENTRY_SIZE]);
  }

  assert_true(gst_allocate_in_heap(&heap, 0) == NULL);
}

static void should_fail_to_allocate_oop_when_heap_is_null(void **state) {
  gst_tlab_t *tlab = (gst_tlab_t *)0xBABA;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_lab(NULL, &tlab, 0, 5);
}

static void should_fail_to_allocate_oop_when_thread_id_is_invalid(void **state) {
  gst_heap_t heap;
  gst_tlab_t *tlab = (gst_tlab_t *)0xBABA;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_lab(&heap, &tlab, UINT16_MAX, 5);
}

static void should_fail_to_allocate_oop_when_tlab_is_null(void **state) {
  gst_heap_t heap;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_lab(&heap, NULL, 0, 5);
}

static void should_fail_to_allocate_oop_when_tlab_content_is_null(void **state) {
  gst_heap_t heap;
  gst_tlab_t *tlab = NULL;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_lab(&heap, &tlab, 0, 5);
}

static void should_fail_to_allocate_oop_when_allocate_nothing(void **state) {
  gst_heap_t heap;
  gst_tlab_t *tlab = NULL;

  (void) state;

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  gst_allocate_in_lab(&heap, &tlab, 0, 0);
}

static void should_allocate_oop(void **state) {
  gst_heap_t heap;
  gst_tlab_t *tlab = NULL;
  size_t nb_of_tlab;
  OOP *object;

  (void) state;

  /* When */
  heap.meta_inf.free_space = sizeof(heap) - sizeof(heap.meta_inf);
  nb_of_tlab = get_total_of_tlab(&heap);
  heap.meta_inf.reserved_for_allocator = NULL;
  heap.meta_inf.next_heap_area = NULL;

  expect_value(__wrap_xcalloc, nb, nb_of_tlab + 1);
  expect_value(__wrap_xcalloc, size, sizeof(*tlab));

  expect_function_calls(__wrap_xcalloc, 1);

  gst_tlab_init_for_heap(&heap);

  tlab = gst_allocate_in_heap(&heap, 0);

  /* Then */

  for (size_t i = 0; i < nb_of_tlab; i++ ) {
    do {
      object = gst_allocate_in_lab(&heap, &tlab, 0, 1);
      assert_true(tlab->position == (object + 1));
    } while (tlab->position < tlab->end_of_buffer - 1);
  }

  expect_function_calls(__wrap__gst_vm_global_barrier_wait, 1);
  expect_function_calls(__wrap_set_except_flag_for_thread, 1);
  expect_function_calls(__wrap__gst_scavenge, 1);
  expect_function_calls(__wrap__gst_vm_end_barrier_wait, 1);

  expect_value(__wrap_nomemory, fatal, 1);
  expect_function_calls(__wrap_nomemory, 1);

  object = gst_allocate_in_lab(&heap, &tlab, 0, 1);
  assert_true(object == NULL);
  assert_true(tlab == NULL);
}

int main(void) {
  const struct CMUnitTest tests[] =
    {
     cmocka_unit_test(should_fail_to_reset_heap_when_heap_is_null),
     cmocka_unit_test(should_fail_to_reset_heap_when_is_not_initialized),
     cmocka_unit_test(should_reset_local_heap_state),
     cmocka_unit_test(should_fail_to_reset_heap_when_heap_is_null_for_local_heap_initialization),
     cmocka_unit_test(should_fail_to_reset_heap_when_allocator_is_initialized_for_local_heap_initialization),
     cmocka_unit_test(should_allocate_and_init_when_local_heap_is_initialized),
     cmocka_unit_test(should_fail_to_allocate_and_init_when_heap_is_null),
     cmocka_unit_test(should_allocate_and_init_when_heap_is_initialized),
     cmocka_unit_test(should_fail_to_allocate_tlab_entry_when_heap_is_null),
     cmocka_unit_test(should_fail_to_allocate_tlab_entry_when_thread_id_is_invalid),
     cmocka_unit_test(should_allocate_tlab_entry),
     cmocka_unit_test(should_fail_to_allocate_oop_when_heap_is_null),
     cmocka_unit_test(should_fail_to_allocate_oop_when_thread_id_is_invalid),
     cmocka_unit_test(should_fail_to_allocate_oop_when_tlab_is_null),
     cmocka_unit_test(should_fail_to_allocate_oop_when_tlab_content_is_null),
     cmocka_unit_test(should_fail_to_allocate_oop_when_allocate_nothing),
     cmocka_unit_test(should_allocate_oop),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
