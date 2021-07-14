/******************************** -*- C -*- ****************************
 *
 *	Primitive Process include file
 *
 ***********************************************************************/

/* Process suspend */
static intptr_t VMpr_Process_suspend(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();
  const OOP processorSchedulerOOP = OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(oop1));

  if (!IS_NIL(processorSchedulerOOP) && processorSchedulerOOP != _gst_processor_oop[current_thread_id]) {
    PRIM_FAILED;
  }

  suspend_process(oop1);
  PRIM_SUCCEEDED;
}

/* Process resume */
static intptr_t VMpr_Process_resume(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();
  const OOP processorSchedulerOOP = OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(oop1));

  if (IS_NIL(processorSchedulerOOP)) {
    OBJ_PROCESS_SET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(oop1), _gst_processor_oop[current_thread_id]);
  } else if (processorSchedulerOOP != _gst_processor_oop[current_thread_id]) {
    PRIM_FAILED;
  }

  if (resume_process(oop1, false)) {
    PRIM_SUCCEEDED;
  } else {
    PRIM_FAILED;
  }
}

/* Process yield */
static intptr_t VMpr_Process_yield(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();
  const OOP processorSchedulerOOP = OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(oop1));

  if (!IS_NIL(processorSchedulerOOP) && processorSchedulerOOP != _gst_processor_oop[current_thread_id]) {
    PRIM_FAILED;
  }

  if (oop1 == get_active_process()) {
    SET_STACKTOP(_gst_nil_oop); /* this is our return value */
    active_process_yield();
  }
  PRIM_SUCCEEDED;
}

/* Process singleStepWaitingOn: */
static intptr_t VMpr_Process_singleStepWaitingOn(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop2 = POP_OOP();
  const OOP oop1 = POP_OOP();
  const OOP processorSchedulerOOP = OBJ_PROCESS_GET_PROCESSOR_SCHEDULER(OOP_TO_OBJ(oop1));

  if (!IS_NIL(processorSchedulerOOP) && processorSchedulerOOP != _gst_processor_oop[current_thread_id]) {
    UNPOP(2);
    PRIM_FAILED;
  }

  if (is_process_ready(oop1) || is_process_terminating(oop1)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  /* Put the current process to sleep, switch execution to the
     new one, and set up the VM to signal the semaphore as soon
     as possible.  */
  _gst_sync_wait(oop2);
  resume_process(oop1, true);
  single_step_semaphore = oop2;
  PRIM_SUCCEEDED;
}

