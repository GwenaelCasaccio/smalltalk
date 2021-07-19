#ifndef GST_SEMAPHORE_H_
#define GST_SEMAPHORE_H_

static pthread_mutex_t semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t semaphore_cond = PTHREAD_COND_INITIALIZER;

static inline void wait_for_semaphore(OOP semaphoreOOP, size_t thread_id);
static inline void signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id);

static inline void wait_for_semaphore(OOP semaphoreOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP nilOOP = _gst_nil_oop;

  while (!atomic_compare_exchange_strong((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)),
                                         &nilOOP,
                                         threadIdOOP)) {
    pthread_cond_wait(&semaphore_cond, &semaphore_mutex);
  }
}

static inline void signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP currentThreadIdOOP = atomic_load((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)));

  if (UNCOMMON(currentThreadIdOOP != threadIdOOP)) {
    nomemory(false);
  }

  atomic_store((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)), _gst_nil_oop);
  pthread_cond_broadcast(&semaphore_cond);
}

#endif /* GST_SEMAPHORE_H_ */

