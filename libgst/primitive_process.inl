/******************************** -*- C -*- ****************************
 *
 *	Primitive ProcessScheduler include file
 *
 ***********************************************************************/

void *start_vm_thread(void *argument) {
  OOP activeProcess;

  /* The other VM threads are stopped so we are safe! */
  current_thread_id = atomic_fetch_add(&_gst_count_total_threaded_vm, 1);

  _gst_alloc_oop_arena_entry_init(current_thread_id);

  _gst_processor_oop[current_thread_id] = (OOP) argument;

  OBJ_PROCESSOR_SCHEDULER_SET_VM_THREAD_ID(OOP_TO_OBJ(_gst_processor_oop[current_thread_id]),
                                           FROM_INT(current_thread_id));

  _gst_mem.tlab_per_thread[current_thread_id] = gst_allocate_in_heap(_gst_mem.gen0, current_thread_id);

  if (NULL == _gst_mem.tlab_per_thread[current_thread_id]) {
    nomemory(true);
    return NULL;
  }

  switch_to_process[current_thread_id] = _gst_nil_oop;
  _gst_this_context_oop[current_thread_id] = _gst_nil_oop;
  async_queue_enabled[current_thread_id] = true;
  queued_async_signals[current_thread_id] = &queued_async_signals_tail[current_thread_id];
  queued_async_signals_sig[current_thread_id] = &queued_async_signals_tail[current_thread_id];

  _gst_init_context();

  /* Force cache cleanup and *init* */
  _gst_sample_counter = 1;
  _gst_invalidate_method_cache();

  _gst_check_process_state();

  activeProcess = highest_priority_process();

  if (IS_NIL(activeProcess)) {
    nomemory(true);
    return NULL;
  }

  OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(activeProcess), _gst_processor_oop[current_thread_id]);

  const OOP processLists = GET_PROCESS_LISTS();
  const OOP semaphoreOOP = ARRAY_AT(processLists, 9);
  add_first_link(semaphoreOOP, activeProcess);

  change_process_context(activeProcess);

  _gst_check_process_state();

  atomic_store(&dispatch_vec_per_thread[current_thread_id], global_normal_bytecodes);
  _gst_interp_need_to_wait[current_thread_id] = false;

  /* the current thread can be locked with the global barrier */
  atomic_fetch_add(&_gst_interpret_thread_counter, 1);

  _gst_vm_end_barrier_wait();

  _gst_interpret(activeProcess);

  return NULL;
}

static intptr_t VMpr_Processor_newThread(int id, volatile int numArgs) {
  pthread_t thread_id;
  OOP oop1;

  oop1 = STACKTOP();

  _gst_vm_global_barrier_wait();

  _gst_interp_need_to_wait[current_thread_id] = false;

  atomic_fetch_add(&_gst_count_threaded_vm, 1);

  if (pthread_create(&thread_id, NULL, &start_vm_thread, oop1)) {
    perror("failed to create new thread");

    atomic_fetch_add(&_gst_count_threaded_vm, -1);

    _gst_vm_end_barrier_wait();

    PRIM_FAILED;
  }

  _gst_vm_end_barrier_wait();

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Processor_killThread(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  _gst_vm_global_barrier_wait();

  set_except_flag_for_thread(false, current_thread_id);

  empty_context_stack();

  // FIXME since its an array we cannot decrease we've hole on it
  // atomic_fetch_add(&_gst_interpret_thread_counter, -1);

  atomic_fetch_add(&_gst_count_threaded_vm, -1);

  if (pthread_cond_signal(&_gst_vm_end_barrier_cond)) {
    perror("failed signal conditional variable");

    PRIM_FAILED;
  }

  pthread_exit(NULL);

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Processor_currentThreadId(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  SET_STACKTOP(FROM_INT(current_thread_id));

  PRIM_SUCCEEDED;
}
