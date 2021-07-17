/*
 * Primitives Atomic Operations
 */

static intptr_t VMpr_Atomic_set(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP valueOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  atomic_inst_var_at_put(selfOOP, index, valueOOP);
  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_get(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP))) {
    UNPOP(1);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(1);
    PRIM_FAILED;
  }

  SET_STACKTOP(atomic_inst_var_at(selfOOP, index));

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_add(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP firstTermOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP) || !IS_INT(firstTermOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const OOP secondTermOOP = atomic_inst_var_at(selfOOP, index);

  if (!IS_INT(secondTermOOP)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t firstTerm = TO_INT(firstTermOOP);
  const intptr_t secondTerm = TO_INT(secondTermOOP);
  const OOP resultOOP = FROM_INT(firstTerm + secondTerm);

  atomic_inst_var_at_put(selfOOP, index, resultOOP);
  SET_STACKTOP(resultOOP);

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_sub(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP firstTermOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP) || !IS_INT(firstTermOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const OOP secondTermOOP = atomic_inst_var_at(selfOOP, index);

  if (!IS_INT(secondTermOOP)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t firstTerm = TO_INT(firstTermOOP);
  const intptr_t secondTerm = TO_INT(secondTermOOP);
  const OOP resultOOP = FROM_INT(secondTerm - firstTerm);

  atomic_inst_var_at_put(selfOOP, index, resultOOP);
  SET_STACKTOP(resultOOP);

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_or(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP firstTermOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP) || !IS_INT(firstTermOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const OOP secondTermOOP = atomic_inst_var_at(selfOOP, index);

  if (!IS_INT(secondTermOOP)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t firstTerm = TO_INT(firstTermOOP);
  const intptr_t secondTerm = TO_INT(secondTermOOP);
  const OOP resultOOP = FROM_INT(secondTerm | firstTerm);

  atomic_inst_var_at_put(selfOOP, index, resultOOP);
  SET_STACKTOP(resultOOP);

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_and(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP firstTermOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP) || !IS_INT(firstTermOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const OOP secondTermOOP = atomic_inst_var_at(selfOOP, index);

  if (!IS_INT(secondTermOOP)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t firstTerm = TO_INT(firstTermOOP);
  const intptr_t secondTerm = TO_INT(secondTermOOP);
  const OOP resultOOP = FROM_INT(secondTerm & firstTerm);

  atomic_inst_var_at_put(selfOOP, index, resultOOP);
  SET_STACKTOP(resultOOP);

  PRIM_SUCCEEDED;
}

static intptr_t VMpr_Atomic_xor(int id, volatile int numArgs) {
  _gst_primitives_executed++;

  const OOP firstTermOOP = POP_OOP();
  const OOP indexOOP = POP_OOP();
  const OOP selfOOP  = STACKTOP();

  if (UNCOMMON (!IS_INT(indexOOP) || !IS_INT(firstTermOOP))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t index = TO_INT(indexOOP);

  if (UNCOMMON (!CHECK_BOUNDS_OF(selfOOP, index))) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const OOP secondTermOOP = atomic_inst_var_at(selfOOP, index);

  if (!IS_INT(secondTermOOP)) {
    UNPOP(2);
    PRIM_FAILED;
  }

  const intptr_t firstTerm = TO_INT(firstTermOOP);
  const intptr_t secondTerm = TO_INT(secondTermOOP);
  const OOP resultOOP = FROM_INT(secondTerm ^ firstTerm);

  atomic_inst_var_at_put(selfOOP, index, resultOOP);
  SET_STACKTOP(resultOOP);

  PRIM_SUCCEEDED;
}

