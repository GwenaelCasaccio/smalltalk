#include "gstpriv.h"

/* This is the memory area which holds the object table.  */
static heap oop_heap = NULL;

void _gst_init_oop_table(PTR address, size_t size) {
  size_t i;

  oop_heap = NULL;
  for (i = MAX_OOP_TABLE_SIZE; i && !oop_heap; i >>= 1) {
    oop_heap = _gst_heap_create(address, i * sizeof(struct oop_s));
  }

  if (!oop_heap || i < size) {
    nomemory(true);
    return ;
  }

  _gst_alloc_oop_table(size);
}

void _gst_alloc_oop_table(size_t size) {
  size_t bytes;

  _gst_mem.ot_size = size;
  bytes = size * sizeof(struct oop_s);
  _gst_mem.ot = (struct oop_s *)_gst_heap_sbrk(oop_heap, bytes);
  if (!_gst_mem.ot) {
    nomemory(true);
    return ;
  }

  _gst_mem.num_free_oops = size;
  _gst_mem.last_allocated_oop = _gst_mem.last_swept_oop = _gst_mem.ot - 1;
  _gst_mem.next_oop_to_sweep = _gst_mem.ot - 1;
}

mst_Boolean _gst_realloc_oop_table(size_t newSize) {
  size_t bytes;

  bytes = (newSize - _gst_mem.ot_size) * sizeof(struct oop_s);
  if (bytes < 0)
    return (true);

  if (!_gst_heap_sbrk(oop_heap, bytes)) {
    /* try to recover. Note that we cannot move
       the OOP table like we do with the object data.  */

    nomemory(false);
    return (false);
  }

  _gst_mem.num_free_oops += newSize - _gst_mem.ot_size;
  _gst_mem.ot_size = newSize;
  return (true);
}

void _gst_dump_oop_table() {
  OOP oop;

  for (oop = _gst_mem.ot; oop <= _gst_mem.last_allocated_oop; OOP_NEXT(oop))
    if (!IS_OOP_FREE(oop)) {
      if (IS_OOP_VALID(oop))
        _gst_display_oop(oop);
      else
        _gst_display_oop_short(oop);
    }
}

void _gst_dump_owners(OOP oop) {
  OOP oop2, lastOOP;

  for (oop2 = _gst_mem.ot, lastOOP = &_gst_mem.ot[_gst_mem.ot_size];
       oop2 < lastOOP; OOP_NEXT(oop2))
    if UNCOMMON (IS_OOP_VALID(oop2) && is_owner(oop2, oop))
      _gst_display_oop(oop2);
}

void _gst_check_oop_table() {
  OOP oop, lastOOP;

  for (oop = _gst_mem.ot, lastOOP = &_gst_mem.ot[_gst_mem.ot_size];
       oop < lastOOP; OOP_NEXT(oop)) {
    gst_object object;
    OOP *scanPtr;
    int n;

    if (!IS_OOP_VALID_GC(oop))
      continue;

    object = OOP_TO_OBJ(oop);
    scanPtr = &OBJ_CLASS(object);
    if (OOP_GET_FLAGS(oop) & F_CONTEXT) {
      const intptr_t methodSP = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(object));
      n = OBJ_METHOD_CONTEXT_CONTEXT_STACK(object) + methodSP + 1 -
          object->data;
    } else
      n = NUM_OOPS(object) + 1;

    while (n--) {
      OOP pointedOOP = *scanPtr++;
      if (IS_OOP(pointedOOP) &&
          (!IS_OOP_ADDR(pointedOOP) || !IS_OOP_VALID_GC(pointedOOP)))
        abort();
    }
  }
}
