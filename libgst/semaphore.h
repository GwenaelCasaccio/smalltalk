#ifndef GST_SEMAPHORE_H_
#define GST_SEMAPHORE_H_

#include <immintrin.h>

static inline void wait_for_semaphore(OOP semaphoreOOP, size_t thread_id);
static inline void signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id);

static inline void wait_for_semaphore(OOP semaphoreOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP nilOOP = _gst_nil_oop;

  while (!atomic_compare_exchange_strong((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)),
                                         &nilOOP,
                                         threadIdOOP)) {
    _mm_pause();
  }
}

static inline void signal_and_broadcast_for_semaphore(OOP semaphoreOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP currentThreadIdOOP = atomic_load((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)));

  if (UNCOMMON(currentThreadIdOOP != threadIdOOP)) {
    char *str;
    asprintf(&str, "signal_and_broadcast_for_semaphore Thread ID are not the same %O != %O", threadIdOOP, currentThreadIdOOP);
    perror(str);
    nomemory(true);
    return ;
  }

  atomic_store((_Atomic OOP*) &OBJ_SEMAPHORE_GET_LOCK_THREAD_ID(OOP_TO_OBJ(semaphoreOOP)), _gst_nil_oop);
}

#endif /* GST_SEMAPHORE_H_ */

