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
  abort();
  return ;
}

void __wrap__gst_scavenge(void) {
  abort();
}

void *__wrap_xcalloc(size_t nb, size_t size) {
  return calloc(nb, size);
}

void __wrap_set_except_flag_for_thread(bool value, size_t thread_id) {
}

void __wrap__gst_vm_global_barrier_wait(void) {
}

void __wrap__gst_vm_end_barrier_wait(void) {
}


#define number_of_threads 10

static pthread_barrier_t barrier;
static pthread_barrier_t tlab_allocate_barrier;

static gst_heap_t *test_heap;
static gst_tlab_t *tlab[number_of_threads];

static void *allocation_thread(void *argument) {

  const uint8_t thread_id = (uintptr_t) argument;
  const size_t nb_of_tlab = get_total_of_tlab(test_heap);

  pthread_barrier_wait(&barrier);

  tlab[thread_id] = gst_allocate_in_heap(test_heap, thread_id);

	for (size_t i = 0; i < nb_of_tlab / 10; i++) {
    pthread_barrier_wait(&tlab_allocate_barrier);

    if (NULL == tlab[thread_id]) {
      abort();
    }

    if (thread_id != tlab[thread_id]->thread_id) {
      abort();
    }

    do {
      OOP *object = gst_allocate_in_lab(test_heap, &tlab[thread_id], thread_id, 1);

      if (!object) {
        abort();
      }

      if (object != tlab[thread_id]->position - 1) {
        abort();
      }

    } while (tlab[thread_id]->position < tlab[thread_id]->end_of_buffer - 1);
  }

  return NULL;
}

static void should_allocate_oop() {
  pthread_t thread_id[number_of_threads];
  size_t nb_of_tlab;
  gst_tlab_t *check_tlab;

  /* When */
  _gst_heap_new_area(&test_heap, 12 * 1024 * 1024);

  gst_tlab_init_for_heap(test_heap);

  if (pthread_barrier_init(&barrier, NULL, number_of_threads)) {
    perror("error in barrier creation");
    abort();
  }

  if (pthread_barrier_init(&tlab_allocate_barrier, NULL, number_of_threads)) {
    perror("error in barrier creation");
    abort();
  }

  for (uint8_t i = 0; i < 10; i++) {
    if (pthread_create(&thread_id[i], NULL, &allocation_thread, (void *)(uintptr_t)i)) {
      perror("error in thread creation");
      abort();
    }
  }

  for (uint8_t i = 0; i < 10; i++) {
    pthread_join(thread_id[i], NULL);
  }

  nb_of_tlab = get_total_of_tlab(test_heap);

  check_tlab = test_heap->meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab / 10 * 10; i++) {
    if (check_tlab[i].thread_id == UINT16_MAX) {
      abort();
    }

    if (check_tlab[i].position == check_tlab[i].end_of_buffer - 2) {
      abort();
    }

  }

  _gst_heap_free_area(test_heap);

}

int main(void) {

  should_allocate_oop();

  return 0;
}
