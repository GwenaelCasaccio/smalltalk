/*
 * Primitive Semaphore
 */

/* Semaphore notifyAll */
static intptr_t VMpr_Semaphore_notifyAll(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();

  wait_for_semaphore(oop1, current_thread_id);
  while (_gst_sync_signal(oop1, false))
    ;
  signal_and_broadcast_for_semaphore(oop1, current_thread_id);

  /* The process and the stack has changed so don't changed it! */

  PRIM_SUCCEEDED;
}

/* Semaphore signal (id = 0) or Semaphore notify (id = 1) */
static intptr_t VMpr_Semaphore_signalNotify(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();

  wait_for_semaphore(oop1, current_thread_id);
  _gst_sync_signal(oop1, id == 0);
  signal_and_broadcast_for_semaphore(oop1, current_thread_id);

  /* The process and the stack has changed so don't changed it! */

  PRIM_SUCCEEDED;
}

/* Semaphore wait lock */
static intptr_t VMpr_Semaphore_lock(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();
  const gst_object sem = OOP_TO_OBJ(oop1);

  SET_STACKTOP_BOOLEAN(TO_INT(OBJ_SEMAPHORE_GET_SIGNALS(sem)) > 0);
  OBJ_SEMAPHORE_SET_SIGNALS(sem, FROM_INT(0));

  /* The process and the stack has changed so don't changed it! */

  PRIM_SUCCEEDED;
}

/* Semaphore wait */
static intptr_t VMpr_Semaphore_wait(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop1 = STACKTOP();

  wait_for_semaphore(oop1, current_thread_id);
  _gst_sync_wait(oop1);
  signal_and_broadcast_for_semaphore(oop1, current_thread_id);

  /* The process and the stack has changed so don't changed it! */

  PRIM_SUCCEEDED;
}

/* Semaphore waitAfterSignalling: aSemaphore */
static intptr_t VMpr_Semaphore_waitAfterSignalling(int id,
                                                   volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP oop2 = POP_OOP();
  const OOP oop1 = STACKTOP();

  wait_for_semaphore(oop1, current_thread_id);
  _gst_sync_signal(oop2, true);
  _gst_sync_wait(oop1);
  signal_and_broadcast_for_semaphore(oop1, current_thread_id);

  /* The process and the stack has changed so don't changed it! */

  PRIM_SUCCEEDED;
}

