#include "libgst/gstpriv.h"

int test_initial_state() {
  if (_gst_count_total_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_locked_vm != 0) {
    return 1;
  }

  if (_gst_count_end_locked_vm != 0) {
    return 1;
  }

  return 0;
}

int test_barrier_wait_without_thread() {
  bool res;

  res = _gst_vm_barrier_wait();
  if (!res) {
    return 1;
  }

  // Check state
  if (_gst_count_total_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_locked_vm != 0) {
    return 1;
  }

  if (_gst_count_end_locked_vm != 0) {
    return 1;
  }

  res = _gst_vm_end_barrier_wait();

  if (!res) {
    return 1;
  }

  // Check state
  if (_gst_count_total_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_threaded_vm != 1) {
    return 1;
  }

  if (_gst_count_locked_vm != 0) {
    return 1;
  }

  if (_gst_count_end_locked_vm != 0) {
    return 1;
  }

  return 0;
}

static pthread_barrier_t thread_barrier_wait;
static volatile _Atomic(size_t) count_barrier_access = 0;
static volatile _Atomic(size_t) count_nb_of_first_barrier_item = 0;

void *thread_barrier_test(void *argument) {
  int error;

  error = pthread_barrier_wait(&thread_barrier_wait);

  if ((error != PTHREAD_BARRIER_SERIAL_THREAD) && (error != 0)) {
    fprintf(stderr, "failed to wait for barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  if (_gst_vm_barrier_wait()) {
    atomic_fetch_add(&count_nb_of_first_barrier_item, 1);
  }

  // Check barrier state
  atomic_fetch_add(&count_barrier_access, 1);

  _gst_vm_end_barrier_wait();

  return NULL;
}

int test_barrier_wait_with_threads() {
  int error;
  pthread_t thread_id[10];

  _gst_count_total_threaded_vm = 10;
  _gst_count_threaded_vm = 10;

  if ((error = pthread_barrier_init(&thread_barrier_wait, NULL, 10))) {
    fprintf(stderr, "failed to init barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  for (int i = 0; i < 10; i++) {
    if ((error = pthread_create(&thread_id[i], NULL, &thread_barrier_test, NULL))) {
      fprintf(stderr, "failed to create new thread: %s\n", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  for (int i = 0; i < 10; i++) {
    if ((error = pthread_join(thread_id[i], NULL))) {
      fprintf(stderr, "failed to join thread: %s\n", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  if ((error = pthread_barrier_destroy(&thread_barrier_wait))) {
    fprintf(stderr, "failed to destroy barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  if (atomic_load(&count_nb_of_first_barrier_item) != 1) {
    return 1;
  }

  if (atomic_load(&_gst_count_locked_vm) != 0) {
    return 1;
  }

  return atomic_load(&count_barrier_access) != 10;
}

void *thread_barrier_end_with_dynamic_thread_test(void *argument) {

  // Check barrier state
  atomic_fetch_add(&count_barrier_access, 1);

  _gst_vm_end_barrier_wait();

  return NULL;
}

void *thread_barrier_with_dynamic_thread_test(void *argument) {
  int error;

  error = pthread_barrier_wait(&thread_barrier_wait);

  if ((error != PTHREAD_BARRIER_SERIAL_THREAD) && (error != 0)) {
    fprintf(stderr, "failed to wait for barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  if (_gst_vm_barrier_wait()) {
    pthread_t thread_id;

    atomic_fetch_add(&count_nb_of_first_barrier_item, 1);
    atomic_fetch_add(&_gst_count_threaded_vm, 1);

    if ((error = pthread_create(&thread_id, NULL, &thread_barrier_end_with_dynamic_thread_test, NULL))) {
      fprintf(stderr, "failed to create new thread: %s\n", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  // Check barrier state
  atomic_fetch_add(&count_barrier_access, 1);

  _gst_vm_end_barrier_wait();

  return NULL;
}

int test_barrier_wait_with_dynamic_threads() {
  int error;
  pthread_t thread_id[10];

  _gst_count_total_threaded_vm = 10;
  _gst_count_threaded_vm = 10;
  count_nb_of_first_barrier_item = 0;
  count_barrier_access = 0;

  if ((error = pthread_barrier_init(&thread_barrier_wait, NULL, 10))) {
    fprintf(stderr, "failed to init barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  for (int i = 0; i < 10; i++) {
    if ((error = pthread_create(&thread_id[i], NULL, &thread_barrier_with_dynamic_thread_test, NULL))) {
      fprintf(stderr, "failed to create new thread: %s\n", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  for (int i = 0; i < 10; i++) {
    if ((error = pthread_join(thread_id[i], NULL))) {
      fprintf(stderr, "failed to join thread: %s\n", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  if ((error = pthread_barrier_destroy(&thread_barrier_wait))) {
    fprintf(stderr, "failed to destroy barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }

  if (atomic_load(&count_nb_of_first_barrier_item) != 1) {
    return 1;
  }

  if (atomic_load(&_gst_count_locked_vm) != 0) {
    return 1;
  }
  return atomic_load(&count_barrier_access) != 11;
}

int main() {
  int res = 0;

  printf ("TEST BARRIER: ");

  if ((res = test_initial_state())) {
    return 1;
  }

  if ((res = test_barrier_wait_without_thread())) {
    return 2;
  }

  if ((res = test_barrier_wait_with_threads())) {
    return 3;
  }

  if ((res = test_barrier_wait_with_dynamic_threads())) {
    return 4;
  }

  return 0;
}
