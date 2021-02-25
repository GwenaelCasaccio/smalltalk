#ifndef GST_BARRIER_H
#define GST_BARRIER_H

/* only used for the oop allocation */
extern pthread_mutex_t alloc_oop_mutex;

/* The total number of interpreter theads */
extern volatile _Atomic(size_t) _gst_count_total_threaded_vm;

/* The number of interpreter threads used for the global lock */
extern volatile _Atomic(size_t) _gst_count_threaded_vm;

/* Count the interpreter threads that reached the barrier */
extern volatile _Atomic(size_t) _gst_count_locked_vm;

extern pthread_cond_t _gst_vm_barrier_cond;

extern pthread_mutex_t _gst_vm_barrier_mutex;

/* return true if I am the first you acquired the lock */
extern mst_Boolean _gst_vm_barrier_wait(void);

/* Count the interpreter threads that reached the barrier */
extern volatile _Atomic(size_t) _gst_count_end_locked_vm;

extern pthread_cond_t _gst_vm_end_barrier_cond;

extern pthread_mutex_t _gst_vm_end_barrier_mutex;

/* return true if I am the first you acquired the lock */
extern mst_Boolean _gst_vm_end_barrier_wait(void);

#endif /* GST_BARRIER_H */
