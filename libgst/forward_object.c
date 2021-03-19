#include "gstpriv.h"

void _gst_init_oop_table(PTR address, size_t number_of_forwarding_objects) {
  size_t i;

  if (_gst_mem.oop_heap) {
    nomemory(true);
    return ;
  }

  if ((number_of_forwarding_objects & 0x7FFF) != 0) {
    nomemory(true);
    return ;
  }

  for (i = MAX_OOP_TABLE_SIZE; i && !_gst_mem.oop_heap; i >>= 1) {
    _gst_mem.oop_heap = _gst_heap_create(address, i * sizeof(struct oop_s));
  }

  if (!_gst_mem.oop_heap || i < number_of_forwarding_objects) {
    nomemory(true);
    return ;
  }

  _gst_alloc_oop_table(number_of_forwarding_objects);
  _gst_alloc_oop_arena(i);
  _gst_alloc_oop_arena_entry_init(0);
}

void _gst_alloc_oop_table(size_t number_of_forwarding_objects) {
  size_t bytes;

  _gst_mem.ot_size = number_of_forwarding_objects;
  bytes = number_of_forwarding_objects * sizeof(struct oop_s);
  _gst_mem.ot = (struct oop_s *)_gst_heap_sbrk(_gst_mem.oop_heap, bytes);
  if (!_gst_mem.ot) {
    nomemory(true);
    return ;
  }

  _gst_mem.num_free_oops = number_of_forwarding_objects;
  _gst_mem.last_allocated_oop = _gst_mem.last_swept_oop = _gst_mem.ot - 1;
  _gst_mem.next_oop_to_sweep = _gst_mem.ot - 1;
}

void _gst_alloc_oop_arena(size_t size) {
  size_t arena_size = (size / 32768) + 1;

  _gst_mem.ot_arena = xcalloc(arena_size, sizeof(*_gst_mem.ot_arena));
  _gst_mem.ot_arena_size = arena_size;

  for (size_t i = 0; i < arena_size; i++) {
    _gst_mem.ot_arena[i].thread_id = UINT16_MAX;
    _gst_mem.ot_arena[i].free_oops = 32768;
  }
}

size_t _gst_alloc_oop_arena_entry(uint16_t thread_id) {
  if (thread_id == UINT16_MAX) {
    return 0;
  }

  if (UNCOMMON (_gst_mem.current_arena[thread_id]->free_oops > 0)) {
    return _gst_mem.current_arena[thread_id] - &_gst_mem.ot_arena[0];
  }

  return _gst_alloc_oop_arena_entry_unchecked(thread_id);
}

size_t _gst_alloc_oop_arena_entry_init(uint16_t thread_id) {
  if (thread_id == UINT16_MAX) {
    return 0;
  }

  if (UNCOMMON (_gst_mem.current_arena[thread_id])) {
    return _gst_mem.current_arena[thread_id] - &_gst_mem.ot_arena[0];
  }

  return _gst_alloc_oop_arena_entry_unchecked(thread_id);
}

size_t _gst_alloc_oop_arena_entry_unchecked(uint16_t thread_id) {
  for (size_t i = 0; i < _gst_mem.ot_arena_size; i++) {
    uint16_t expected_thread_id = UINT16_MAX;
    if (atomic_compare_exchange_strong(&_gst_mem.ot_arena[i].thread_id, &expected_thread_id, thread_id)) {
      if (atomic_load(&_gst_mem.ot_arena[i].free_oops) > 0) {
        _gst_mem.current_arena[thread_id] = &_gst_mem.ot_arena[i];
        return i;
      }

      atomic_store(&_gst_mem.ot_arena[i].thread_id, UINT16_MAX);
    }
  }

  nomemory(true);
  return 0;
}

void _gst_detach_oop_arena_entry(size_t area_index) {
  if (area_index >= _gst_mem.ot_arena_size) {
    return ;
  }

  atomic_store(&_gst_mem.ot_arena[area_index].thread_id, UINT16_MAX);
}

mst_Boolean _gst_realloc_oop_table(size_t number_of_forwarding_objects) {
  size_t bytes;

  if (!_gst_mem.oop_heap) {
    return false;
  }

  if ((number_of_forwarding_objects & 0x7FFF) != 0) {
    nomemory(true);
    return false;
  }

  if (number_of_forwarding_objects <= _gst_mem.ot_size) {
    return true;
  }

  bytes = (number_of_forwarding_objects - _gst_mem.ot_size) * sizeof(struct oop_s);

  if (!_gst_heap_sbrk(_gst_mem.oop_heap, bytes)) {
    /* try to recover. Note that we cannot move
       the OOP table like we do with the object data.  */

    nomemory(false);
    return false;
  }

  _gst_mem.num_free_oops += number_of_forwarding_objects - _gst_mem.ot_size;
  _gst_mem.ot_size = number_of_forwarding_objects;
  return true;
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
