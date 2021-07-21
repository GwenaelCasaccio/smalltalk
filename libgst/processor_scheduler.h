#ifndef GST_PROCESSOR_SCHEDULER_H_
#define GST_PROCESSOR_SCHEDULER_H_

static pthread_mutex_t processor_scheduler_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t processor_scheduler_cond = PTHREAD_COND_INITIALIZER;

static inline void wait_for_processor_scheduler(OOP processorOOP, size_t thread_id);
static inline void signal_and_broadcast_for_processor_scheduler(OOP processorOOP, size_t thread_id);

static inline void wait_for_processor_scheduler(OOP processorSchedulerOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP nilOOP = _gst_nil_oop;

  while (!atomic_compare_exchange_strong((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)),
                                         &nilOOP,
                                         threadIdOOP)) {
    pthread_cond_wait(&processor_scheduler_cond, &processor_scheduler_mutex);
  }
}

static inline void signal_and_broadcast_for_processor_scheduler(OOP processorSchedulerOOP, size_t thread_id) {
  const OOP threadIdOOP = FROM_INT(thread_id);
  const OOP currentThreadIdOOP = atomic_load((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)));

  if (UNCOMMON(currentThreadIdOOP != threadIdOOP)) {
    nomemory(true);
    return ;
  }

  atomic_store((_Atomic OOP*) &OBJ_PROCESSOR_SCHEDULER_GET_LOCK_THREAD_ID(OOP_TO_OBJ(processorSchedulerOOP)), _gst_nil_oop);
  pthread_cond_broadcast(&processor_scheduler_cond);
}

#endif /* GST_PROCESSOR_SCHEDULER_H_ */
