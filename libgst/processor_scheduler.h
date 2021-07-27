#ifndef GST_PROCESSOR_SCHEDULER_H_
#define GST_PROCESSOR_SCHEDULER_H_

#include <immintrin.h>

static inline void wait_for_processor_scheduler(OOP processorOOP, size_t thread_id);
static inline void signal_and_broadcast_for_processor_scheduler(OOP processorOOP, size_t thread_id);

static inline void wait_for_processor_scheduler(OOP processorSchedulerOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP nilOOP = _gst_nil_oop;

  while (!atomic_compare_exchange_strong((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)),
                                         &nilOOP,
                                         threadIdOOP)) {
    _mm_pause();
  }
}

static inline void signal_and_broadcast_for_processor_scheduler(OOP processorSchedulerOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP currentThreadIdOOP = atomic_load((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)));

  if (UNCOMMON(currentThreadIdOOP != threadIdOOP)) {
    perror("Thread ID are not the same");
    nomemory(true);
    return ;
  }

  atomic_store((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)), _gst_nil_oop);
}

#endif /* GST_PROCESSOR_SCHEDULER_H_ */
