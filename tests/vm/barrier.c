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
  mst_Boolean res;

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

void *thread_barrier_test(void *argument) {
  int error;

  error = pthread_barrier_wait(&thread_barrier_wait);

  if ((error != PTHREAD_BARRIER_SERIAL_THREAD) && (error != 0)) {
    fprintf(stderr, "failed to wait for barrier: %s\n", strerror(error));
    fflush(stderr);

    abort();
  }


  _gst_vm_barrier_wait();

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

  return atomic_load(&count_barrier_access) != 10;
}

int main() {
  int res = 0;

  printf ("TEST BARRIER: ");

  if ((res = test_initial_state())) {
    return res;
  }

  if ((res = test_barrier_wait_without_thread())) {
    return res;
  }

  if ((res = test_barrier_wait_with_threads())) {
    return res;
  }

  printf ("YES ");
  return 0;
}
