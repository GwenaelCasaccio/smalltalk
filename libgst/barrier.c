#include "gstpriv.h"

pthread_mutex_t alloc_oop_mutex = PTHREAD_MUTEX_INITIALIZER;

_Atomic(size_t) _gst_count_total_threaded_vm = 1;

_Atomic(size_t) _gst_count_threaded_vm = 1;

_Atomic(size_t) _gst_count_locked_vm = 0;

static _Atomic(size_t) _gst_count_released_locked_vm = 0;

pthread_cond_t _gst_vm_barrier_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t _gst_vm_barrier_mutex = PTHREAD_MUTEX_INITIALIZER;

_Atomic(size_t) _gst_count_end_locked_vm = 0;

static _Atomic(size_t) _gst_count_end_release_locked_vm = 0;

pthread_cond_t _gst_vm_end_barrier_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t _gst_vm_end_barrier_mutex = PTHREAD_MUTEX_INITIALIZER;

mst_Boolean _gst_vm_barrier_wait(void) {
  int error;
  size_t count;

  if ((error = pthread_mutex_lock(&_gst_vm_barrier_mutex))) {
    fprintf(stderr, "error while locking: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  count = atomic_fetch_add(&_gst_count_locked_vm, 1);
  if ((error = pthread_cond_signal(&_gst_vm_barrier_cond))) {
    fprintf(stderr, "error while signaling: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  while (atomic_load(&_gst_count_locked_vm) < atomic_load(&_gst_count_threaded_vm)) {
    if ((error = pthread_cond_wait(&_gst_vm_barrier_cond, &_gst_vm_barrier_mutex))) {
      fprintf(stderr, "error while cond wait: %s", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  _gst_count_released_locked_vm++;

  if (_gst_count_released_locked_vm == _gst_count_threaded_vm) {
    _gst_count_released_locked_vm = 0;
    atomic_store(&_gst_count_locked_vm, 0);
  }

  if ((error = pthread_cond_signal(&_gst_vm_barrier_cond))) {
    fprintf(stderr, "error while signaling: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  if ((error = pthread_mutex_unlock(&_gst_vm_barrier_mutex))) {
    fprintf(stderr, "error while unlocking: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  return count == 0;
}

mst_Boolean _gst_vm_end_barrier_wait(void) {
  int error;
  size_t count;

  if ((error = pthread_mutex_lock(&_gst_vm_end_barrier_mutex))) {
    fprintf(stderr, "error while locking: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  count = atomic_fetch_add(&_gst_count_end_locked_vm, 1);
  if ((error = pthread_cond_signal(&_gst_vm_end_barrier_cond))) {
    fprintf(stderr, "error while signaling: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  while (atomic_load(&_gst_count_end_locked_vm) < atomic_load(&_gst_count_threaded_vm)) {
    if ((pthread_cond_wait(&_gst_vm_end_barrier_cond, &_gst_vm_end_barrier_mutex))) {
      fprintf(stderr, "error while cond wait: %s", strerror(error));
      fflush(stderr);

      abort();
    }
  }

  _gst_count_end_release_locked_vm++;

  if (_gst_count_end_release_locked_vm == _gst_count_threaded_vm) {
    _gst_count_end_release_locked_vm = 0;
    atomic_store(&_gst_count_end_locked_vm, 0);
  }

  if ((error = pthread_cond_signal(&_gst_vm_end_barrier_cond))) {
    fprintf(stderr, "error while signaling: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  if ((error = pthread_mutex_unlock(&_gst_vm_end_barrier_mutex))) {
    fprintf(stderr, "error while unlocking: %s", strerror(error));
    fflush(stderr);

    abort();
  }

  return count == 0;
}
