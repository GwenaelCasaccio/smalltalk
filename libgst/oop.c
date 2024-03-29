/******************************** -*- C -*- ****************************
 *
 *	Object Table maintenance module.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 1988,89,90,91,92,94,95,99,2000,2001,2002,2006,2007,2008,2009
 * Free Software Foundation, Inc.
 * Written by Steve Byrne and Paolo Bonzini.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * Linking GNU Smalltalk statically or dynamically with other modules is
 * making a combined work based on GNU Smalltalk.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the Free Software Foundation
 * give you permission to combine GNU Smalltalk with free software
 * programs or libraries that are released under the GNU LGPL and with
 * independent programs running under the GNU Smalltalk virtual machine.
 *
 * You may copy and distribute such a system following the terms of the
 * GNU GPL for GNU Smalltalk and the licenses of the other code
 * concerned, provided that you include the source code of that other
 * code when and as the GNU GPL requires distribution of source code.
 *
 * Note that people who make modified versions of GNU Smalltalk are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so.  The GNU General
 * Public License gives permission to release a modified version without
 * this exception; this exception also makes it possible to release a
 * modified version which carries forward this exception.
 *
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/

#include "gstpriv.h"

#define K 1024
#define INIT_NUM_INCUBATOR_OOPS 50
#define INCUBATOR_CHUNK_SIZE 20

/* The number of OOPs that are swept on each incremental GC step.  */
#define INCREMENTAL_SWEEP_STEP 100

/* Define this flag to turn on debugging dumps for garbage collection */
/* #define GC_DEBUG_OUTPUT */

/* Define this flag to turn on debugging code for OOP table management */
/* #define GC_DEBUGGING */

/* Define this flag to turn on debugging code for oldspace management */
/* #define MMAN_DEBUG_OUTPUT */

#if defined(GC_DEBUG_OUTPUT)
#define GC_DEBUGGING
#endif

#if !defined(OPTIMIZE)
#define GC_DEBUGGING
#endif

/* These are the real OOPS for nil, true, and false */
OOP _gst_nil_oop = NULL;
OOP _gst_true_oop = NULL;
OOP _gst_false_oop = NULL;

/* This is true to show a message whenever a GC happens.  */
int _gst_gc_message = true;

/* This is != 0 in the middle of a GC.  */
int _gst_gc_running = 0;

/* This variable represents information about the memory space.
   _gst_mem holds the required information: basically the
   pointer to the base and top of the space, and the pointers into it
   for allocation and copying.  */
struct memory_space _gst_mem;

/* Data to compute the statistics in _gst_mem.  */
struct statistical_data {
  int reclaimedOldSpaceBytesSinceLastGlobalGC;
  unsigned long timeOfLastScavenge, timeOfLastGlobalGC, timeOfLastGrowth,
      timeOfLastCompaction;
} stats;

/* Free N slots from the beginning of the queue Q and return a pointer
   to their base.  */
static OOP *queue_get(surv_space *q, int n);

/* Allocate N slots at the end of the queue Q and return a pointer
   to their base.  */
static OOP *queue_put(surv_space *q, OOP *src, int n);

/* Move an object from survivor space to oldspace.  */
static void tenure_one_object();

/* Initialize an allocation heap with the oldspace hooks set.  */
static heap_data *init_old_space(size_t size);

/* Initialize a surv_space structure.  */
static void init_survivor_space(struct surv_space *space, size_t size);

/* Raise the oldspace size limit to SIZE bytes without compacting it.  */
static void grow_memory_no_compact(size_t size);

/* Reset a surv_space structure (same as init, but without allocating
   memory.  */
static void reset_survivor_space(struct surv_space *space);

/* Return whether the incremental collector is running.  */
static inline bool incremental_gc_running(void);

/* Restart the incremental collector.  Objects before FIRSTOOP
   are assumed to be alive (currently the base of the OOP table is
   always passed, but you never know).  */
static void reset_incremental_gc(OOP firstOOP);

/* Compact the old objects.  Grow oldspace to NEWSIZE bytes.  */
static void compact(size_t new_heap_limit);

/* Allocate and return space for a fixedspace object of SIZE bytes.
   The pointer to the object data is returned, the OOP is
   stored in P_OOP.  */
static gst_object alloc_fixed_obj(size_t size, OOP *p_oop);

/* Gather statistics.  */
static void update_stats(unsigned long *last, double *between,
                         double *duration);

/* The copying collector.  */
static inline void copy_oops(void);

/* Grey ranges are generated in two cases.  The standard one is when we
   write to oldspace; another one is when we copy objects to the destination
   semispace faster than the scanner can go past them.  When this happens,
   tenure_one_object puts the object onto a special list of old objects
   that are to be scanned.  What this function does is to consume this last
   list.  (It also completes the special treatment of ephemeron objects).  */
static void scan_grey_objects();

/* The treatment of grey pages is different from grey objects.  Since some
   new objects might not be tenured, grey pages might still hold some
   pointers to new objects.  For this reason, and to avoid the cost of
   delivering two signals, a grey page is *not* removed from the tree
   until no new object is found in it.  */
static void scan_grey_pages();

/* Greys the object OOP.  */
static void add_grey_object(OOP oop);

/* Do the breadth-first scanning of copied objects.  */
static void cheney_scan(void);

/* Hook that allows pages to be created grey.  */
static void oldspace_after_allocating(heap_data *h, heap_block *blk, size_t sz);

/* Hook that discards freed pages from the remembered table.  */
static void oldspace_before_freeing(heap_data *h, heap_block *blk, size_t sz);

#ifndef NO_SIGSEGV_HANDLING
/* The a global SIGSEGV handler.  */
static int oldspace_sigsegv_handler(void *fault_address, int serious);
#endif

/* Hook that triggers garbage collection.  */
static heap_data *oldspace_nomemory(heap_data *h, size_t sz);

/* Answer the number of fields to be scanned in the object starting
   at OBJ, with the given FLAGS on its OOP.  */
static int scanned_fields_in(gst_object obj, int flags) ATTRIBUTE_PURE;

/* The mark phase of oldspace GC.  */
static inline void mark_oops(void);

/* Communicate to the finalization thread which objects have to be sent
   the #mourn message.

   When one of the objects pointed to by a weak object have no other
   references, the slot of the weak object is replaced by a zero and
   the #mourn message is sent to it.  Ephemerons' keys are checked for
   reachability after non-ephemerons are marked, and if no objects outside
   the ephemeron refer to it, the ephemeron is sent #mourn as well.  */
static inline void mourn_objects(void);

/* Mark the ephemeron objects.  This is done after other objects
   are marked.  */
static inline void mark_ephemeron_oops(void);

/* Walks the instance variables of weak objects and zeroes out those that are
   not surviving the garbage collection.  Called by preare_for_sweep.  */
static inline void check_weak_refs();

void init_survivor_space(struct surv_space *space, size_t size) {
  space->totalSize = size;
  space->minPtr = (OOP *)xmalloc(size);
  space->maxPtr = (OOP *)((char *)space->minPtr + size);

  reset_survivor_space(space);
}

heap_data *init_old_space(size_t size) {
  heap_data *h = _gst_mem_new_heap(0, size);
  h->after_prim_allocating = oldspace_after_allocating;
  h->before_prim_freeing = oldspace_before_freeing;
  h->nomemory = oldspace_nomemory;

  return h;
}

void _gst_init_mem_default() { _gst_init_mem(0, 0, 0, 0, 0, 0); }

void _gst_init_mem(size_t eden, size_t survivor, size_t old,
                   size_t big_object_threshold, int grow_threshold_percent,
                   int space_grow_rate) {
  if (!_gst_mem.old) {
    _gst_mem.weak_areas = NULL;

#ifndef NO_SIGSEGV_HANDLING
    sigsegv_install_handler(oldspace_sigsegv_handler);
#endif
    if (!eden) {
      eden = 3 * K * K;
    }
    if (!survivor) {
      survivor = 512 * K;
    }
    if (!old) {
      old = 8 * K * K;
    }
    if (!big_object_threshold) {
      big_object_threshold = 4 * K;
    }
    if (!grow_threshold_percent) {
      grow_threshold_percent = 80;
    }
    if (!space_grow_rate) {
      space_grow_rate = 30;
    }
  } else {
    if (eden || survivor) {
      _gst_scavenge();
    }

    if (survivor) {
      _gst_tenure_all_survivors();
    }

    if (old && old != _gst_mem.old->heap_total) {
      _gst_grow_memory_to(old);
    }
  }

  _gst_heap_new_area(&_gst_mem.gen0, 12 * K * K);
  gst_tlab_init_for_heap(_gst_mem.gen0);
  _gst_mem.tlab_per_thread[0] = gst_allocate_in_heap(_gst_mem.gen0, 0);

  if (NULL == _gst_mem.tlab_per_thread[0]) {
    nomemory(true);
    return;
  }

  if (eden) {
    _gst_mem.eden.totalSize = eden;
    _gst_mem.eden.minPtr = (OOP *)xmalloc(eden);
    _gst_mem.eden.allocPtr = _gst_mem.eden.minPtr;
    _gst_mem.eden.maxPtr = (OOP *)((char *)_gst_mem.eden.minPtr + eden);
  }

  if (survivor) {
    init_survivor_space(&_gst_mem.surv[0], survivor);
    init_survivor_space(&_gst_mem.surv[1], survivor);
    init_survivor_space(&_gst_mem.tenuring_queue,
                        survivor / OBJ_HEADER_SIZE_WORDS);
  }

  if (big_object_threshold) {
    _gst_mem.big_object_threshold = big_object_threshold;
  }

  if (_gst_mem.eden.totalSize < _gst_mem.big_object_threshold) {
    _gst_mem.big_object_threshold = _gst_mem.eden.totalSize;
  }

  if (grow_threshold_percent) {
    _gst_mem.grow_threshold_percent = grow_threshold_percent;
  }

  if (space_grow_rate) {
    _gst_mem.space_grow_rate = space_grow_rate;
  }

  if (!_gst_mem.old) {
    if (old) {
      _gst_mem.old = init_old_space(old);
      _gst_mem.fixed = init_old_space(old);
    }

    _gst_mem.active_half = &_gst_mem.surv[0];
    _gst_mem.active_flag = F_EVEN;
    _gst_mem.live_flags = F_EVEN | F_OLD;

    stats.timeOfLastScavenge = stats.timeOfLastGlobalGC =
        stats.timeOfLastGrowth = stats.timeOfLastCompaction =
            _gst_get_milli_time();

    _gst_mem.factor = 0.4;

    _gst_inc_init_registry();
  }

  _gst_mem.markQueue =
      (struct mark_queue *)xcalloc(8 * K, sizeof(struct mark_queue));
  _gst_mem.lastMarkQueue = &_gst_mem.markQueue[8 * K];
}

void _gst_update_object_memory_oop(OOP oop) {
  gst_object_memory data;

  /* Ensure the statistics are coherent.  */
  for (;;) {
    OOP floatOOP;

    data = (gst_object_memory)OOP_TO_OBJ(oop);
    data->bytesPerOOP = FROM_INT(sizeof(PTR));
    data->bytesPerOTE =
        FROM_INT(sizeof(struct oop_s) + sizeof(gst_object_header));

    data->edenSize = FROM_INT(_gst_mem.eden.totalSize);
    data->survSpaceSize = FROM_INT(_gst_mem.active_half->totalSize);
    data->oldSpaceSize = FROM_INT(_gst_mem.old->heap_limit);
    data->fixedSpaceSize = FROM_INT(_gst_mem.fixed->heap_limit);
    data->edenUsedBytes =
        FROM_INT((char *)_gst_mem.eden.allocPtr - (char *)_gst_mem.eden.minPtr);
    data->survSpaceUsedBytes = FROM_INT(_gst_mem.active_half->filled);
    data->oldSpaceUsedBytes = FROM_INT(_gst_mem.old->heap_total);
    data->fixedSpaceUsedBytes = FROM_INT(_gst_mem.fixed->heap_total);
    data->rememberedTableEntries = FROM_INT(_gst_mem.rememberedTableEntries);
    data->numScavenges = FROM_INT(_gst_mem.numScavenges);
    data->numGlobalGCs = FROM_INT(_gst_mem.numGlobalGCs);
    data->numCompactions = FROM_INT(_gst_mem.numCompactions);
    data->numGrowths = FROM_INT(_gst_mem.numGrowths);
    data->numOldOOPs = FROM_INT(_gst_mem.numOldOOPs);
    data->numFixedOOPs = FROM_INT(_gst_mem.numFixedOOPs);
    data->numWeakOOPs = FROM_INT(_gst_mem.numWeakOOPs);
    data->numOTEs = FROM_INT(_gst_mem.ot_size);
    data->numFreeOTEs = FROM_INT(_gst_mem.num_free_oops);
    data->allocFailures =
        FROM_INT(_gst_mem.old->failures + _gst_mem.fixed->failures);
    data->allocMatches =
        FROM_INT(_gst_mem.old->matches + _gst_mem.fixed->matches);
    data->allocSplits = FROM_INT(_gst_mem.old->splits + _gst_mem.fixed->splits);
    data->allocProbes = FROM_INT(_gst_mem.old->probes + _gst_mem.fixed->probes);

    /* Every allocation of a FloatD might cause a garbage
       collection! */
#define SET_FIELD(x)                                                           \
  floatOOP = floatd_new(_gst_mem.x);                                           \
  if (data != (gst_object_memory)OOP_TO_OBJ(oop))                              \
    continue;                                                                  \
  data->x = floatOOP;

    SET_FIELD(timeBetweenScavenges);
    SET_FIELD(timeBetweenGlobalGCs);
    SET_FIELD(timeBetweenGrowths);
    SET_FIELD(timeToScavenge);
    SET_FIELD(timeToCollect);
    SET_FIELD(timeToCompact);
    SET_FIELD(reclaimedBytesPerScavenge);
    SET_FIELD(tenuredBytesPerScavenge);
    SET_FIELD(reclaimedBytesPerGlobalGC);
    SET_FIELD(reclaimedPercentPerScavenge);

#undef SET_FIELD

    break;
  }
}

void _gst_init_basic_objects() {
  gst_object _gst_char_object_table[NUM_CHAR_OBJECTS];
  gst_object _gst_boolean_objects[2];

  for (unsigned int i = 0; i < NUM_CHAR_OBJECTS; i++) {
    OOP oop;

    _gst_char_object_table[i] =
        alloc_fixed_obj((OBJ_HEADER_SIZE_WORDS + 1) * SIZEOF_OOP, &oop);
    _gst_char_object_table[i]->data[0] = FROM_INT(i);

    _gst_register_oop(oop);
  }

  alloc_fixed_obj(OBJ_HEADER_SIZE_WORDS * SIZEOF_OOP, &_gst_nil_oop);
  _gst_register_oop(_gst_nil_oop);

  _gst_boolean_objects[0] =
      alloc_fixed_obj((OBJ_HEADER_SIZE_WORDS + 1) * SIZEOF_OOP, &_gst_true_oop);
  _gst_boolean_objects[0]->data[0] = _gst_true_oop;
  _gst_register_oop(_gst_true_oop);

  _gst_boolean_objects[1] = alloc_fixed_obj(
      (OBJ_HEADER_SIZE_WORDS + 1) * SIZEOF_OOP, &_gst_false_oop);
  _gst_boolean_objects[1]->data[0] = _gst_false_oop;
  _gst_register_oop(_gst_false_oop);
}

void _gst_init_builtin_objects_classes(void) {

  _gst_nil_oop = OOP_AT(NIL_OOP_INDEX);
  _gst_true_oop = OOP_AT(TRUE_OOP_INDEX);
  _gst_false_oop = OOP_AT(FALSE_OOP_INDEX);

  OBJ_SET_CLASS(OOP_TO_OBJ(_gst_nil_oop), _gst_undefined_object_class);
  OBJ_SET_CLASS(OOP_TO_OBJ(_gst_true_oop), _gst_true_class);
  OBJ_SET_CLASS(OOP_TO_OBJ(_gst_false_oop), _gst_false_class);

  for (unsigned int i = 0; i < NUM_CHAR_OBJECTS; i++) {
    OBJ_SET_CLASS(OOP_TO_OBJ(&_gst_mem.ot[i]), _gst_char_class);
  }
}

OOP _gst_find_an_instance(OOP class_oop) {
  OOP oop;

  PREFETCH_START(_gst_mem.ot, PREF_READ | PREF_NTA);
  for (oop = _gst_mem.ot; oop <= _gst_mem.last_allocated_oop; OOP_NEXT(oop)) {
    PREFETCH_LOOP(oop, PREF_READ | PREF_NTA);
    if (IS_OOP_VALID(oop) && (OOP_CLASS(oop) == class_oop)) {
      return (oop);
    }
  }

  return (_gst_nil_oop);
}

void _gst_make_oop_non_weak(OOP oop) {
  weak_area_tree *entry = _gst_mem.weak_areas;

  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~F_WEAK);
  _gst_mem.numWeakOOPs--;
  while (entry) {
    if (entry->oop == oop) {
      rb_erase(&entry->rb, (rb_node_t **)&_gst_mem.weak_areas);
      xfree(entry);
      break;
    }

    entry = (weak_area_tree *)(oop < entry->oop ? entry->rb.rb_left
                                                : entry->rb.rb_right);
  }
}

void _gst_make_oop_weak(OOP oop) {
  weak_area_tree *entry;
  weak_area_tree *node = NULL;
  rb_node_t **p = (rb_node_t **)&_gst_mem.weak_areas;

  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_WEAK);
  _gst_mem.numWeakOOPs++;

  while (*p) {
    node = (weak_area_tree *)*p;

    if (oop < node->oop) {
      p = &(*p)->rb_left;
    } else if (oop > node->oop) {
      p = &(*p)->rb_right;
    } else {
      return;
    }
  }

  entry = (weak_area_tree *)xmalloc(sizeof(weak_area_tree));
  entry->oop = oop;

  if (node) {
    entry->rb.rb_parent = &node->rb;
  } else {
    entry->rb.rb_parent = NULL;
  }

  entry->rb.rb_left = entry->rb.rb_right = NULL;
  *p = &(entry->rb);

  rb_rebalance(&entry->rb, (rb_node_t **)&_gst_mem.weak_areas);
}

void _gst_swap_objects(OOP oop1, OOP oop2) {
  struct oop_s tempOOP;
  inc_ptr incPtr;
  OOP tempId;

  _gst_vm_global_barrier_wait();

  set_except_flag_for_thread(false, current_thread_id);

  incPtr = INC_SAVE_POINTER();
  INC_ADD_OOP(oop1);
  INC_ADD_OOP(oop2);

  if (OOP_GET_FLAGS(oop2) & F_WEAK) {
    _gst_make_oop_non_weak(oop2);
  }

  if (OOP_GET_FLAGS(oop1) & F_WEAK) {
    _gst_make_oop_non_weak(oop1);
  }

  /* Put the two objects in the same generation.  FIXME: this can be
     a cause of early tenuring, especially since one of them is often
     garbage!  */
  if ((OOP_GET_FLAGS(oop1) & F_OLD) ^ (OOP_GET_FLAGS(oop2) & F_OLD)) {
    _gst_tenure_oop((OOP_GET_FLAGS(oop1) & F_OLD) ? oop2 : oop1);
  }

  tempOOP = *oop2; /* note structure assignment going on here */
  *oop2 = *oop1;
  *oop1 = tempOOP;

  tempId = OBJ_IDENTITY(OOP_TO_OBJ(oop1));
  OBJ_SET_IDENTITY(OOP_TO_OBJ(oop1), OBJ_IDENTITY(OOP_TO_OBJ(oop2)));
  OBJ_SET_IDENTITY(OOP_TO_OBJ(oop2), tempId);

  /* If the incremental GC has reached oop1 but not oop2 (or vice versa),
     this flag will end up in the wrong OOP, i.e. in the one that has already
     been scanned by the incremental GC.  Restore things.  */
  if ((OOP_GET_FLAGS(oop1) & F_REACHABLE) ^
      (OOP_GET_FLAGS(oop2) & F_REACHABLE)) {
    OOP_SET_FLAGS(oop1, OOP_GET_FLAGS(oop1) ^ F_REACHABLE);
    OOP_SET_FLAGS(oop2, OOP_GET_FLAGS(oop2) ^ F_REACHABLE);
  }

  if (OOP_GET_FLAGS(oop2) & F_WEAK) {
    _gst_make_oop_weak(oop2);
  }

  if (OOP_GET_FLAGS(oop1) & F_WEAK) {
    _gst_make_oop_weak(oop1);
  }

  INC_RESTORE_POINTER(incPtr);

  _gst_vm_end_barrier_wait();
}

void _gst_make_oop_fixed(OOP oop) {
  gst_object newObj;
  int size;

  _gst_vm_global_barrier_wait();

  set_except_flag_for_thread(false, current_thread_id);

  if (OOP_GET_FLAGS(oop) & F_FIXED) {
    _gst_vm_end_barrier_wait();

    return;
  }

  if ((OOP_GET_FLAGS(oop) & F_LOADED) == 0) {
    size = SIZE_TO_BYTES(TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
    newObj = (gst_object)_gst_mem_alloc(_gst_mem.fixed, size);
    if (!newObj) {
      abort();
    }

    memcpy(newObj, OOP_TO_OBJ(oop), size);
    if ((OOP_GET_FLAGS(oop) & F_OLD) == 0) {
      _gst_mem.numOldOOPs++;
    } else {
      _gst_mem_free(_gst_mem.old, OOP_TO_OBJ(oop));
    }

    OOP_SET_OBJECT(oop, newObj);
  }

  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~(F_SPACES | F_POOLED));
  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_OLD | F_FIXED);

  _gst_vm_end_barrier_wait();
}

void _gst_tenure_oop(OOP oop) {
  gst_object newObj;
  if (OOP_GET_FLAGS(oop) & F_OLD) {
    return;
  }

  if (!(OOP_GET_FLAGS(oop) & F_FIXED)) {
    int size = SIZE_TO_BYTES(TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
    newObj = (gst_object)_gst_mem_alloc(_gst_mem.old, size);
    if (!newObj) {
      abort();
    }

    memcpy(newObj, OOP_TO_OBJ(oop), size);
    _gst_mem.numOldOOPs++;

    OOP_SET_OBJECT(oop, newObj);
  }

  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~(F_SPACES | F_POOLED));
  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_OLD);
}

gst_object _gst_alloc_obj(size_t size, OOP *p_oop) {
  gst_object p_instance;

  /* Force a GC as soon as possible if we're low on OOPs or memory.  */
  if UNCOMMON (_gst_mem.num_free_oops <
                   LOW_WATER_OOP_THRESHOLD *
                       atomic_load(&_gst_interpret_thread_counter) * 4 ||
               _gst_mem.old->heap_total * 100.0 / _gst_mem.old->heap_limit >
                   _gst_mem.grow_threshold_percent ||
               _gst_mem.fixed->heap_total * 100.0 / _gst_mem.fixed->heap_limit >
                   _gst_mem.grow_threshold_percent) {
    _gst_vm_global_barrier_wait();

    set_except_flag_for_thread(false, current_thread_id);

    _gst_global_gc(0);
    _gst_incremental_gc_step();

    _gst_vm_end_barrier_wait();
  }

  size = ROUNDED_BYTES(size);

  if (UNCOMMON(size >= _gst_mem.big_object_threshold)) {
    _gst_vm_global_barrier_wait();

    set_except_flag_for_thread(false, current_thread_id);

    p_instance = alloc_fixed_obj(size, p_oop);

    _gst_vm_end_barrier_wait();

    return p_instance;
  }

  p_instance = (gst_object)gst_allocate_in_lab(
      _gst_mem.gen0, &_gst_mem.tlab_per_thread[current_thread_id],
      current_thread_id, BYTES_TO_SIZE(size));
  if (UNCOMMON(NULL == p_instance)) {
    // Check Too Big
    // Reallocate new TLAB Entry
    nomemory(true);
    return NULL;
  }

  *p_oop = alloc_oop(p_instance, _gst_mem.active_flag);
  OBJ_SET_SIZE(p_instance, FROM_INT(BYTES_TO_SIZE(size)));
  OBJ_SET_IDENTITY(p_instance, FROM_INT(0));

  return p_instance;
}

gst_object alloc_fixed_obj(size_t size, OOP *p_oop) {
  gst_object p_instance;

  size = ROUNDED_BYTES(size);

  /* If the object is big enough, we put it directly in oldspace.  */
  p_instance = (gst_object)_gst_mem_alloc(_gst_mem.fixed, size);
  if COMMON (p_instance)
    goto ok;

  _gst_global_gc(size);
  p_instance = (gst_object)_gst_mem_alloc(_gst_mem.fixed, size);
  if COMMON (p_instance)
    goto ok;

  compact(0);
  p_instance = (gst_object)_gst_mem_alloc(_gst_mem.fixed, size);
  if UNCOMMON (!p_instance) {
    /* !!! do something more reasonable in the future */
    _gst_errorf("Cannot recover, exiting...");
    exit(1);
  }

ok:
  *p_oop = alloc_oop(p_instance, F_OLD | F_FIXED);
  OBJ_SET_SIZE(p_instance, FROM_INT(BYTES_TO_SIZE(size)));
  OBJ_SET_IDENTITY(p_instance, FROM_INT(0));

  return p_instance;
}

gst_object _gst_alloc_words(size_t size) {
  gst_object p_instance;

  if (UNCOMMON(size >= _gst_mem.big_object_threshold)) {
    abort();
  }

  p_instance = (gst_object)gst_allocate_in_lab(
      _gst_mem.gen0, &_gst_mem.tlab_per_thread[current_thread_id],
      current_thread_id, size);
  if (UNCOMMON(NULL == p_instance)) {
    // Check Too Big
    // Reallocate new TLAB Entry
    nomemory(true);
    return NULL;
  }

  OBJ_SET_SIZE(p_instance, FROM_INT(size));
  OBJ_SET_IDENTITY(p_instance, FROM_INT(0));
  return p_instance;
}

void reset_survivor_space(surv_space *space) {
  space->allocated = space->filled = 0;
  space->tenurePtr = space->allocPtr = space->topPtr = space->minPtr;
}

void oldspace_after_allocating(heap_data *h, heap_block *blk, size_t sz) {
  UNUSED(h);

#ifdef MMAN_DEBUG_OUTPUT
  printf("Allocating oldspace page at %p (%d)\n", blk, sz);
#endif

  add_to_grey_list((OOP *)blk, sz / sizeof(PTR));
  _gst_mem.rememberedTableEntries++;
}

void oldspace_before_freeing(heap_data *h, heap_block *blk, size_t sz) {
  UNUSED(h);

  grey_area_node *node, *last, **next;

#ifdef MMAN_DEBUG_OUTPUT
  printf("Freeing oldspace page at %p (%d)\n", blk, sz);
#endif

  /* Remove related entries from the remembered table.  */
  for (last = NULL, next = &_gst_mem.grey_pages.head; (node = *next);) {
    if (node->base >= (OOP *)blk &&
        node->base + node->n <= (OOP *)(((char *)blk) + sz)) {
#ifdef MMAN_DEBUG_OUTPUT
      printf("  Remembered table entry removed %p..%p\n", node->base,
             node->base + node->n);
#endif

      _gst_mem.rememberedTableEntries--;
      *next = node->next;
      xfree(node);
    } else {
      last = node;
      next = &(node->next);
    }
  }

  _gst_mem.grey_pages.tail = last;
  _gst_mem_protect((PTR)blk, sz, PROT_READ | PROT_WRITE);
}

heap_data *oldspace_nomemory(heap_data *h, size_t sz) {
  heap_data **p_heap;

  assert(h == _gst_mem.old || h == _gst_mem.fixed);
  p_heap = (h == _gst_mem.old ? &_gst_mem.old : &_gst_mem.fixed);

  if (!_gst_gc_running) {
    _gst_global_gc(sz);
  } else {
    /* Already garbage collecting, emergency growth just to satisfy
       tenuring necessities.  */
    int grow_amount_to_satisfy_rate =
        h->heap_limit * (100.0 + _gst_mem.space_grow_rate) / 100;
    int grow_amount_to_satisfy_threshold =
        (sz + h->heap_total) * 100.0 / _gst_mem.grow_threshold_percent;

    h->heap_limit =
        MAX(grow_amount_to_satisfy_rate, grow_amount_to_satisfy_threshold);
  }

  return *p_heap;
}

#ifndef NO_SIGSEGV_HANDLING
int oldspace_sigsegv_handler(void *fault_address, int serious) {
  static int reentering, reentered;
  void *page;
  if UNCOMMON (reentering) {
    reentered = 1;
    abort();
  } else {
    reentered = 0;
    reentering = 1;
  }

  page =
      (char *)fault_address - ((intptr_t)fault_address & (getpagesize() - 1));
  errno = 0;
  if (_gst_mem_protect(page, getpagesize(), PROT_READ | PROT_WRITE) == -1 &&
      (errno == ENOMEM || errno == EFAULT || errno == EACCES ||
       errno == EINVAL)) {
#if defined(MMAN_DEBUG_OUTPUT)
    printf("Plain old segmentation violation -- address = %p\n", page);
#endif
    reentering = 0;
    abort();
  }

  /* Try accessing the page */
  (void)*(volatile char *)fault_address;
  reentering = 0;

#if defined(MMAN_DEBUG_OUTPUT)
  printf("Unprotected %p (SIGSEGV at %p)\n", page, fault_address);
#endif

  _gst_mem.rememberedTableEntries++;
  add_to_grey_list((PTR)page, getpagesize() / sizeof(PTR));
  return !reentered;
}
#endif

void update_stats(unsigned long *last, double *between, double *duration) {
  unsigned long now = _gst_get_milli_time();
  unsigned long since = now - *last;
  if (between) {
    *between = _gst_mem.factor * *between + (1 - _gst_mem.factor) * since;
  }

  if (duration) {
    *duration = _gst_mem.factor * *duration + (1 - _gst_mem.factor) * since;
  } else {
    *last = now;
  }
}

void _gst_grow_memory_to(size_t spaceSize) { compact(spaceSize); }

void grow_memory_no_compact(size_t new_heap_limit) {
  _gst_mem.old->heap_limit = new_heap_limit;
  _gst_mem.fixed->heap_limit = new_heap_limit;
  _gst_mem.numGrowths++;
  update_stats(&stats.timeOfLastGrowth, &_gst_mem.timeBetweenGrowths, NULL);
}

void compact(size_t new_heap_limit) {
  OOP oop;
  heap_data *new_heap = init_old_space(
      new_heap_limit ? new_heap_limit : _gst_mem.old->heap_limit);

  if (new_heap_limit) {
    _gst_mem.fixed->heap_limit = new_heap_limit;
    _gst_mem.numGrowths++;
    update_stats(&stats.timeOfLastGrowth, &_gst_mem.timeBetweenGrowths, NULL);
    stats.timeOfLastCompaction = stats.timeOfLastGrowth;
  } else {
    /* Do not copy garbage.  */
    _gst_finish_incremental_gc();
    _gst_mem.numCompactions++;
    update_stats(&stats.timeOfLastCompaction, NULL, NULL);
  }

  _gst_fixup_object_pointers();

  /* Now do the copying loop which will compact oldspace.  */
  PREFETCH_START(_gst_mem.ot, PREF_READ | PREF_NTA);
  for (oop = _gst_mem.ot; oop < &_gst_mem.ot[_gst_mem.ot_size]; OOP_NEXT(oop)) {
    PREFETCH_LOOP(oop, PREF_READ | PREF_NTA);
    if ((OOP_GET_FLAGS(oop) & (F_OLD | F_FIXED | F_LOADED)) == F_OLD) {
      gst_object new;
      size_t size = SIZE_TO_BYTES(TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
      new = _gst_mem_alloc(new_heap, size);
      memcpy(new, OOP_TO_OBJ(oop), size);
      _gst_mem_free(_gst_mem.old, OOP_TO_OBJ(oop));
      OOP_SET_OBJECT(oop, new);
    }
  }

  xfree(_gst_mem.old);
  _gst_mem.old = new_heap;
  new_heap->nomemory = oldspace_nomemory;

  _gst_restore_object_pointers();

  update_stats(&stats.timeOfLastCompaction, NULL, &_gst_mem.timeToCompact);
}

void _gst_global_compact() {
  _gst_global_gc(0);
  compact(0);
}

void _gst_global_gc(int next_allocation) {
  const char *s;
  size_t old_limit;

  _gst_mem.numGlobalGCs++;

  old_limit = _gst_mem.old->heap_limit;
  _gst_mem.old->heap_limit = 0;

  if (!_gst_gc_running++ && _gst_gc_message && _gst_verbosity >= 2 &&
      !_gst_regression_testing) {
    /* print the first part of this message before we finish
       scanning oop table for live ones, so that the delay caused by
       this scanning is apparent.  Note the use of stderr for the
       printed message.  The idea here was that generated output
       could be treated as Smalltalk code, HTML or whatever else you
       want without harm.  */
    fflush(stdout);
    fprintf(stderr, "\"Global garbage collection... ");
    fflush(stderr);
  }

  update_stats(&stats.timeOfLastGlobalGC, &_gst_mem.timeBetweenGlobalGCs, NULL);

  _gst_finish_incremental_gc();
  _gst_fixup_object_pointers();
  copy_oops();
  _gst_tenure_all_survivors();
  mark_oops();
  _gst_mem.live_flags &= ~F_OLD;
  _gst_mem.live_flags |= F_REACHABLE;
  check_weak_refs();
  _gst_restore_object_pointers();
#if defined(GC_DEBUGGING)
  _gst_check_oop_table();
#endif
  reset_incremental_gc(_gst_mem.ot);

  gst_tlab_reset_for_local_heap(_gst_mem.gen0);

  update_stats(&stats.timeOfLastGlobalGC, NULL, &_gst_mem.timeToCollect);

  s = "done";

  /* Compaction and growth tests are only done during the outermost GC (well
     I am not sure that GC's can nest...)  */
  if (old_limit) {
    old_limit = MAX(old_limit, _gst_mem.old->heap_total);

    /* if memory is still low, go all the way on sweeping */
    if UNCOMMON ((next_allocation + _gst_mem.old->heap_total) * 100.0 /
                     old_limit >
                 _gst_mem.grow_threshold_percent) {
      size_t target_limit;
      _gst_finish_incremental_gc();

      /* Check if it's time to compact the heap. Compaction make the most
         sense if there were lots of garbage. And the heap limit is shrunk
         to avoid excessive garbage accumulation in the next round */
      target_limit = MAX(_gst_mem.eden.totalSize,
                         ((next_allocation + _gst_mem.old->heap_total) *
                          (100.0 + _gst_mem.space_grow_rate) /
                          _gst_mem.grow_threshold_percent));
      if (target_limit < old_limit) {
        s = "done, heap compacted";
        compact(0);
        grow_memory_no_compact(target_limit);
      }
    }

    /* Check if it's time to grow the heap.  */
    if UNCOMMON ((next_allocation + _gst_mem.old->heap_total) * 100.0 /
                         old_limit >
                     _gst_mem.grow_threshold_percent ||
                 (next_allocation + _gst_mem.fixed->heap_total) * 100.0 /
                         _gst_mem.fixed->heap_limit >
                     _gst_mem.grow_threshold_percent) {
      int grow_amount_to_satisfy_rate =
          old_limit * (100.0 + _gst_mem.space_grow_rate) / 100;
      int grow_amount_to_satisfy_threshold =
          (next_allocation + _gst_mem.old->heap_total) * 100.0 /
          _gst_mem.grow_threshold_percent;

      s = "done, heap grown";
      grow_memory_no_compact(
          MAX(grow_amount_to_satisfy_rate, grow_amount_to_satisfy_threshold));
    }
  }

  if (!--_gst_gc_running && _gst_gc_message && _gst_verbosity >= 2 &&
      !_gst_regression_testing) {
    fprintf(stderr, "%s\"\n", s);
    fflush(stderr);
  }

  /* If the heap was grown, don't reset the old limit! */
  if (!_gst_mem.old->heap_limit) {
    _gst_mem.old->heap_limit = old_limit;
  }

  _gst_invalidate_croutine_cache();
  mourn_objects();
}

void _gst_scavenge(void) {
  int oldBytes, reclaimedBytes, tenuredBytes, reclaimedPercent;

  /* Check if oldspace had to be grown in emergency.  */
  size_t prev_heap_limit = _gst_mem.old->heap_limit;

  /* Force a GC as soon as possible if we're low on OOPs or memory.  */
  if UNCOMMON (_gst_mem.num_free_oops < LOW_WATER_OOP_THRESHOLD ||
               _gst_mem.old->heap_total * 100.0 / _gst_mem.old->heap_limit >
                   _gst_mem.grow_threshold_percent ||
               _gst_mem.fixed->heap_total * 100.0 / _gst_mem.fixed->heap_limit >
                   _gst_mem.grow_threshold_percent) {
    _gst_global_gc(0);
    _gst_incremental_gc_step();
    return;
  }

  if (!_gst_gc_running++ && _gst_gc_message && _gst_verbosity > 2 &&
      !_gst_regression_testing) {
    /* print the first part of this message before we finish
       scanning oop table for live ones, so that the delay caused by
       this scanning is apparent.  Note the use of stderr for the
       printed message.  The idea here was that generated output
       could be treated as Smalltalk code, HTML or whatever else you
       want without harm.  */
    fflush(stdout);
    fprintf(stderr, "\"Scavenging... ");
    fflush(stderr);
  }

  oldBytes = (char *)_gst_mem.eden.allocPtr - (char *)_gst_mem.eden.minPtr +
             _gst_mem.active_half->filled;

  _gst_mem.numScavenges++;
  update_stats(&stats.timeOfLastScavenge, &_gst_mem.timeBetweenScavenges, NULL);

  _gst_finish_incremental_gc();
  _gst_fixup_object_pointers();

  copy_oops();

  check_weak_refs();

  _gst_restore_object_pointers();

  gst_tlab_reset_for_local_heap(_gst_mem.gen0);

  reset_incremental_gc(_gst_mem.ot);

  update_stats(&stats.timeOfLastScavenge, NULL, &_gst_mem.timeToScavenge);

  reclaimedBytes = oldBytes - _gst_mem.active_half->allocated;
  if (reclaimedBytes < 0) {
    reclaimedBytes = 0;
  }

  tenuredBytes = _gst_mem.active_half->allocated - _gst_mem.active_half->filled;
  reclaimedPercent = 100.0 * reclaimedBytes / oldBytes;

  if (!--_gst_gc_running && _gst_gc_message && _gst_verbosity > 2 &&
      !_gst_regression_testing) {
    fprintf(stderr, "%d%% reclaimed, done\"\n", reclaimedPercent);
    fflush(stderr);
  }

  _gst_mem.reclaimedBytesPerScavenge =
      _gst_mem.factor * reclaimedBytes +
      (1 - _gst_mem.factor) * _gst_mem.reclaimedBytesPerScavenge;

  _gst_mem.reclaimedPercentPerScavenge =
      _gst_mem.factor * reclaimedPercent +
      (1 - _gst_mem.factor) * _gst_mem.reclaimedPercentPerScavenge;

  _gst_mem.tenuredBytesPerScavenge =
      _gst_mem.factor * tenuredBytes +
      (1 - _gst_mem.factor) * _gst_mem.tenuredBytesPerScavenge;

  _gst_invalidate_croutine_cache();
  mourn_objects();

  /* If tenuring had to grow oldspace, do a global garbage collection
     now.  */
  if (_gst_mem.old->heap_limit > prev_heap_limit) {
    _gst_global_gc(0);
    _gst_incremental_gc_step();
    return;
  }
}

bool incremental_gc_running() {
  return (_gst_mem.next_oop_to_sweep > _gst_mem.last_swept_oop);
}

void _gst_finish_incremental_gc() {
  OOP oop, firstOOP;

#if defined(GC_DEBUG_OUTPUT)
  printf("Completing sweep (%p...%p), validity flags %x\n",
         _gst_mem.last_swept_oop, _gst_mem.next_oop_to_sweep,
         _gst_mem.live_flags);
#endif

  PREFETCH_START(_gst_mem.next_oop_to_sweep,
                 PREF_BACKWARDS | PREF_READ | PREF_NTA);
  for (oop = _gst_mem.next_oop_to_sweep, firstOOP = _gst_mem.last_swept_oop;
       oop > firstOOP; OOP_PREV(oop)) {
    PREFETCH_LOOP(oop, PREF_BACKWARDS | PREF_READ | PREF_NTA);
    if (IS_OOP_VALID_GC(oop)) {
      OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~F_REACHABLE);
    } else {
      _gst_sweep_oop(oop);
      _gst_mem.num_free_oops++;
      if (oop == _gst_mem.last_allocated_oop) {
        OOP_PREV(_gst_mem.last_allocated_oop);
      }
    }
  }

  _gst_mem.next_oop_to_sweep = oop;
  _gst_finished_incremental_gc();
}

void _gst_finished_incremental_gc(void) {
  if (_gst_mem.live_flags & F_OLD) {
    return;
  }

  _gst_mem.live_flags &= ~F_REACHABLE;
  _gst_mem.live_flags |= F_OLD;

  if (stats.reclaimedOldSpaceBytesSinceLastGlobalGC) {
    _gst_mem.reclaimedBytesPerGlobalGC =
        _gst_mem.factor * stats.reclaimedOldSpaceBytesSinceLastGlobalGC +
        (1 - _gst_mem.factor) * _gst_mem.reclaimedBytesPerGlobalGC;
    stats.reclaimedOldSpaceBytesSinceLastGlobalGC = 0;
  }
#ifdef ENABLE_JIT_TRANSLATION
  /* Go and really free the blocks associated to garbage collected
     native code.  */
  _gst_free_released_native_code();
#endif
}

bool _gst_incremental_gc_step() {
  OOP oop, firstOOP;
  int i;

  if (!incremental_gc_running()) {
    return true;
  }

  i = 0;
  firstOOP = _gst_mem.last_swept_oop;
  for (oop = _gst_mem.next_oop_to_sweep; oop > firstOOP; OOP_PREV(oop)) {
    if (IS_OOP_VALID_GC(oop)) {
      OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~F_REACHABLE);
    } else {
      _gst_sweep_oop(oop);
      _gst_mem.num_free_oops++;
      if (oop == _gst_mem.last_allocated_oop) {
        OOP_PREV(_gst_mem.last_allocated_oop);
      }
      if (++i == INCREMENTAL_SWEEP_STEP) {
        _gst_mem.next_oop_to_sweep = OOP_PREV(oop);
        return false;
      }
    }
  }

  _gst_mem.next_oop_to_sweep = oop;
  _gst_finished_incremental_gc();
  return true;
}

void reset_incremental_gc(OOP firstOOP) {
  OOP oop;

  /* This loop is the same as that in alloc_oop.  Skip low OOPs
     that are allocated */
  for (oop = firstOOP; IS_OOP_VALID_GC(oop);) {
    OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~F_REACHABLE);
    OOP_NEXT(oop);
  }

  /* Initialize these here so that IS_OOP_VALID works correctly.  */
  _gst_mem.next_oop_to_sweep = _gst_mem.last_allocated_oop;
  _gst_mem.last_swept_oop = oop - 1;

  _gst_finish_incremental_gc();

  _gst_mem.num_free_oops =
      _gst_mem.ot_size - (_gst_mem.last_allocated_oop - _gst_mem.ot);

  /* Check if it's time to grow the OOP table.  */
  if (_gst_mem.num_free_oops * 100.0 / _gst_mem.ot_size <
      100 - _gst_mem.grow_threshold_percent) {
    _gst_realloc_oop_table(
        ((_gst_mem.ot_size * (100 + _gst_mem.space_grow_rate) / 100) + 0x8000) &
        ~0x7FFF);
  }

#if defined(GC_DEBUG_OUTPUT)
  printf("Last allocated OOP %p\n"
         "Next OOP swept top to bottom %p, highest swept bottom to top %p\n",
         _gst_mem.last_allocated_oop, _gst_mem.next_oop_to_sweep,
         _gst_mem.last_swept_oop);
#endif
}

void _gst_sweep_oop(OOP oop) {
  if (IS_OOP_FREE(oop)) {
    return;
  }

  if UNCOMMON (OOP_GET_FLAGS(oop) & F_WEAK)
    _gst_make_oop_non_weak(oop);

  /* Free unreachable oldspace objects.  */
  if UNCOMMON (OOP_GET_FLAGS(oop) & F_FIXED) {
    _gst_mem.numOldOOPs--;
    stats.reclaimedOldSpaceBytesSinceLastGlobalGC +=
        SIZE_TO_BYTES(TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
    if ((OOP_GET_FLAGS(oop) & F_LOADED) == 0) {
      _gst_mem_free(_gst_mem.fixed, OOP_TO_OBJ(oop));
    }
  } else if UNCOMMON (OOP_GET_FLAGS(oop) & F_OLD) {
    _gst_mem.numOldOOPs--;
    stats.reclaimedOldSpaceBytesSinceLastGlobalGC +=
        SIZE_TO_BYTES(TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
    if ((OOP_GET_FLAGS(oop) & F_LOADED) == 0) {
      _gst_mem_free(_gst_mem.old, OOP_TO_OBJ(oop));
    }
  }

  OOP_SET_FLAGS(oop, 0);

  _gst_mem.ot_arena[(oop - _gst_mem.ot) / 32768].free_oops++;

  if (oop < _gst_mem.ot_arena[(oop - _gst_mem.ot) / 32768].first_free_oop) {
    _gst_mem.ot_arena[(oop - _gst_mem.ot) / 32768].first_free_oop = oop;
  }
}

gst_object unsafe_new_instance_with(OOP class_oop, size_t numIndexFields,
                                    OOP *p_oop) {
  size_t numBytes, alignedBytes;
  intptr_t instanceSpec;
  gst_object p_instance;

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
  numBytes = sizeof(gst_object_header) +
             SIZE_TO_BYTES(instanceSpec >> ISP_NUMFIXEDFIELDS) +
             (numIndexFields << _gst_log2_sizes[instanceSpec & ISP_SHAPE]);

  alignedBytes = ROUNDED_BYTES(numBytes);
  p_instance = _gst_alloc_obj(alignedBytes, p_oop);
  INIT_UNALIGNED_OBJECT(*p_oop, alignedBytes - numBytes);

  OBJ_SET_CLASS(p_instance, class_oop);

  return p_instance;
}

void mourn_objects(void) {
  gst_object array;
  long size;
  gst_object processor;

  size = _gst_buffer_size() / sizeof(OOP);
  if (!size) {
    return;
  }

  processor = OOP_TO_OBJ(_gst_processor_oop[0]);
  if (!IS_NIL(OBJ_PROCESSOR_SCHEDULER_GET_GC_ARRAY(processor))) {
    _gst_errorf("Too many garbage collections, finalizers missed!");
    _gst_errorf("This is a bug, please report.");
  } else {
    /* Copy the buffer into an Array */
    array = unsafe_new_instance_with(
        _gst_array_class, size,
        &OBJ_PROCESSOR_SCHEDULER_GET_GC_ARRAY(processor));
    _gst_copy_buffer(array->data);
    if (!IS_NIL(OBJ_PROCESSOR_SCHEDULER_GET_GC_SEMAPHORE(processor))) {
      static async_queue_entry e;
      e.func = _gst_do_async_signal;
      e.data = OBJ_PROCESSOR_SCHEDULER_GET_GC_SEMAPHORE(processor);
      _gst_async_call_internal(&e);
    } else {
      _gst_errorf("Running finalizers before initialization.");
      abort();
    }
  }
}

#define IS_QUEUE_SPLIT(q) ((q)->topPtr != (q)->allocPtr)

OOP *queue_get(surv_space *q, int n) {
  OOP *result = q->tenurePtr;
  q->filled -= n * sizeof(PTR);
  q->tenurePtr += n;

  /* Check if the read pointer has to wrap.  */
  if (q->tenurePtr == q->topPtr) {
    q->tenurePtr = q->minPtr;
    q->topPtr = q->allocPtr;
  }

  return result;
}

OOP *queue_put(surv_space *q, OOP *src, int n) {
  OOP *result, *newAlloc;
  for (;;) {
    result = q->allocPtr;
    newAlloc = q->allocPtr + n;

#if defined(GC_DEBUG_OUTPUT)
    printf("Top %p alloc %p tenure %p\n", q->topPtr, q->allocPtr, q->tenurePtr);
#endif

    if (IS_QUEUE_SPLIT(q) && UNCOMMON(newAlloc > q->tenurePtr))
    /* We tenure old objects as we copy more objects into
       the circular survivor space.  */
    {
#if defined(GC_DEBUG_OUTPUT)
      printf("Tenure: current max %p, needed %p\n", q->tenurePtr, newAlloc);
#endif
      tenure_one_object();
      continue;
    }

    if UNCOMMON (newAlloc > q->maxPtr) {
#if defined(GC_DEBUG_OUTPUT)
      printf("Wrap: survivor space ends at %p, needed %p\n", q->maxPtr,
             newAlloc);
#endif
      q->topPtr = q->allocPtr;
      q->allocPtr = q->minPtr;
      continue;
    }

    break;
  }

  if (!IS_QUEUE_SPLIT(q)) {
    /* We are still extending towards the top.  Push further the
       valid area (which is space...topPtr and minPtr...allocPtr
       if topPtr != allocPtr (not circular yet), space...allocPtr
       if topPtr == allocPtr (circular).  */
    q->topPtr = newAlloc;
  }

  q->filled += n * sizeof(PTR);
  q->allocated += n * sizeof(PTR);
  q->allocPtr = newAlloc;
  memcpy(result, src, n * sizeof(PTR));
  return result;
}

void tenure_one_object() {
  OOP oop;

  oop = *_gst_mem.tenuring_queue.tenurePtr;
#if defined(GC_DEBUG_OUTPUT)
  printf("      ");
  _gst_display_oop(oop);
#endif

  if (_gst_mem.scan.current == oop) {
#if defined(GC_DEBUG_OUTPUT)
    printf("Tenured OOP %p was being scanned\n", oop);
#endif

    _gst_tenure_oop(oop);
    _gst_mem.scan.at = (OOP *)OOP_TO_OBJ(oop);
  }

  else if (_gst_mem.scan.queue_at == _gst_mem.tenuring_queue.tenurePtr) {
#if defined(GC_DEBUG_OUTPUT)
    printf("Tenured OOP %p had not been scanned yet\n", oop);
#endif

    /* Since tenurePtr is going to advance by a place, we must
       keep the Cheney scan pointer up to date.  Check if it has
       to wrap!  */
    _gst_mem.scan.queue_at++;
    if (_gst_mem.scan.queue_at >= _gst_mem.tenuring_queue.topPtr &&
        IS_QUEUE_SPLIT(&_gst_mem.tenuring_queue)) {
      _gst_mem.scan.queue_at = _gst_mem.tenuring_queue.minPtr;
    }

    _gst_tenure_oop(oop);
    add_grey_object(oop);
  } else {
    _gst_tenure_oop(oop);
  }

  queue_get(&_gst_mem.tenuring_queue, 1);
  queue_get(_gst_mem.active_half, TO_INT(OBJ_SIZE(OOP_TO_OBJ(oop))));
}

void _gst_grey_oop_range(PTR from, size_t size) {
#ifndef NO_SIGSEGV_HANDLING
  volatile char *last, *page;

  for (last = ((char *)from) + size,
      page = ((char *)from) - ((intptr_t)from & (getpagesize() - 1));
       page < last; page += getpagesize())
    *page = *page;
#else
  UNUSED(from);
  UNUSED(size);
#endif
}

void add_grey_object(OOP oop) {
  grey_area_node *entry;
  gst_object obj = OOP_TO_OBJ(oop);
  size_t numFields = scanned_fields_in(obj, OOP_GET_FLAGS(oop));
  OOP *base = (OOP *)obj;

  if (!numFields) {
    return;
  }

  entry = (grey_area_node *)xmalloc(sizeof(grey_area_node));
  entry->base = base;
  entry->n = numFields;
  entry->oop = oop;
  entry->next = NULL;
  if (_gst_mem.grey_areas.tail) {
    _gst_mem.grey_areas.tail->next = entry;
  } else {
    _gst_mem.grey_areas.head = entry;
  }

  _gst_mem.grey_areas.tail = entry;
}

void add_to_grey_list(OOP *base, size_t n) {
  grey_area_node *entry = (grey_area_node *)xmalloc(sizeof(grey_area_node));
  entry->base = base;
  entry->n = n;
  entry->oop = NULL;
  entry->next = NULL;
  if (_gst_mem.grey_pages.tail) {
    _gst_mem.grey_pages.tail->next = entry;
  } else {
    _gst_mem.grey_pages.head = entry;
  }

  _gst_mem.grey_pages.tail = entry;
}

void _gst_tenure_all_survivors() {
  OOP oop;
  while (_gst_mem.tenuring_queue.filled) {
    oop = *queue_get(&_gst_mem.tenuring_queue, 1);
    _gst_tenure_oop(oop);
  }
}

void check_weak_refs() {
  rb_node_t *node;
  rb_traverse_t t;

  if (!_gst_mem.weak_areas) {
    return;
  }

  for (node = rb_first(&(_gst_mem.weak_areas->rb), &t); node;
       node = rb_next(&t)) {
    weak_area_tree *area = (weak_area_tree *)node;
    bool mourn = false;
    OOP *field, oop;
    int n;

    oop = area->oop;
    if (!IS_OOP_VALID_GC(oop)) {
      continue;
    }

    for (field = (OOP *)OOP_TO_OBJ(oop) + OBJ_HEADER_SIZE_WORDS,
        n = NUM_OOPS(OOP_TO_OBJ(oop));
         n--; field++) {
      OOP oop = *field;
      if (IS_INT(oop)) {
        continue;
      }

      if (!IS_OOP_VALID_GC(oop)) {
        mourn = true;
        *field = _gst_nil_oop;
      }
    }

    if (mourn) {
      _gst_add_buf_pointer(area->oop);
    }
  }
}

void copy_oops(void) {
  _gst_reset_buffer();

  /* Do the flip! */
  _gst_mem.live_flags ^= F_SPACES;
  _gst_mem.active_flag ^= F_SPACES;
  _gst_mem.active_half = &_gst_mem.surv[_gst_mem.active_flag == F_ODD];

  reset_survivor_space(_gst_mem.active_half);
  reset_survivor_space(&_gst_mem.tenuring_queue);

  /* And the pointer for Cheney scanning.  */
  _gst_mem.scan.queue_at = _gst_mem.tenuring_queue.tenurePtr;

  /* Do these first, they are more likely to stay around for long,
     so it makes sense to make their tenuring more likely (the first
     copied objects are also tenured first).  */
  scan_grey_pages();

  _gst_copy_registered_oops();
  cheney_scan();

  /* Do these last since they are often alive only till the next
     scavenge.  */
  _gst_copy_processor_registers();
  cheney_scan();

  scan_grey_objects();

  /* Reset the new-space pointers */
  _gst_empty_context_pool();
  _gst_mem.eden.allocPtr = _gst_mem.eden.minPtr;
}

void _gst_print_grey_list(bool check_pointers) {
  grey_area_node *node;
  OOP *pOOP, oop;
  size_t i, n;

  for (n = 0, node = _gst_mem.grey_pages.head; node; node = node->next, n++) {
    int new_pointers = 0;
    if (check_pointers) {
      for (new_pointers = 0, pOOP = node->base, i = node->n; i--; pOOP++) {
        PREFETCH_LOOP(pOOP, PREF_READ | PREF_NTA);
        oop = *pOOP;

        /* Not all addresses are known to contain valid OOPs! */
        if (!IS_OOP_ADDR(oop)) {
          continue;
        }

        if (!IS_OOP_NEW(oop)) {
          continue;
        }

        new_pointers++;
      }
    }

    printf("%11p%c ", node->base, new_pointers == 0 ? ' ' : '*');
    if ((n & 3) == 3) {
      putchar('\n');
    }
  }

  if (_gst_mem.grey_pages.tail) {
    printf("  (tail = %12p)", _gst_mem.grey_pages.tail->base);
  }

  printf("\n");
}

void scan_grey_pages() {
  grey_area_node *node, **next, *last;
  OOP *pOOP, oop;
  size_t i, n;

#if defined(MMAN_DEBUG_OUTPUT)
  printf("Pages on the grey list:\n");
  _gst_print_grey_list(true);
#endif

  for (last = NULL, next = &_gst_mem.grey_pages.head; (node = *next);) {
#if defined(GC_DEBUG_OUTPUT) || defined(MMAN_DEBUG_OUTPUT)
    printf("Scanning grey page %p...%p ", node->base, node->base + node->n);
#if defined(GC_DEBUG_OUTPUT)
    putchar('\n');
#else
    fflush(stdout);
#endif
#endif

    PREFETCH_START(node->base, PREF_READ | PREF_NTA);
    for (n = 0, pOOP = node->base, i = node->n; i--; pOOP++) {
      PREFETCH_LOOP(pOOP, PREF_READ | PREF_NTA);
      oop = *pOOP;

      /* Not all addresses are known to contain valid OOPs! */
      if (!IS_OOP_ADDR(oop)) {
        continue;
      }

      if (!IS_OOP_NEW(oop)) {
        continue;
      }

      n++;
      if (!IS_OOP_COPIED(oop)) {
        _gst_copy_an_oop(oop);
      }
    }

#if !defined(NO_SIGSEGV_HANDLING)
    if (!n) {
      /* The entry was temporary, or we found no new-space
         pointers in it.  Delete it and make the page read-only.  */
#if defined(MMAN_DEBUG_OUTPUT)
      printf("Protecting %p\n", node->base);
#endif
      _gst_mem.rememberedTableEntries--;
      _gst_mem_protect((PTR)node->base, node->n * sizeof(OOP), PROT_READ);
      *next = node->next;
      xfree(node);
    } else
#endif
    {
#if defined(MMAN_DEBUG_OUTPUT)
      printf("Found %d pointers\n", n);
#endif
      last = node;
      next = &(node->next);
    }

    cheney_scan();
  }

  _gst_mem.grey_pages.tail = last;

#if defined(MMAN_DEBUG_OUTPUT)
  printf("Pages left on the grey list:\n");
  _gst_print_grey_list(false);
#endif
}

void scan_grey_objects() {
  grey_area_node *node, *next;
  OOP oop;
  gst_object obj;

  for (next = _gst_mem.grey_areas.head; (node = next);) {
    oop = node->oop;
    obj = OOP_TO_OBJ(oop);

#if defined(GC_DEBUG_OUTPUT)
    printf("Scanning grey range %p...%p (%p)\n", node->base,
           node->base + node->n, oop);
#endif

    if (OOP_GET_FLAGS(oop) & F_EPHEMERON) {
      OOP key = obj->data[0];
      _gst_copy_oop_range((OOP *)obj, ((OOP *)obj) + OBJ_HEADER_SIZE_WORDS);
      _gst_copy_oop_range(&obj->data[1], ((OOP *)obj) + node->n);

      /* Copy the key, mourn the object if it was not reachable.  */
      if (!IS_OOP_COPIED(key)) {
        _gst_copy_an_oop(key);
        _gst_add_buf_pointer(oop);
      }
    } else {
      _gst_copy_oop_range(node->base, node->base + node->n);
    }

    _gst_mem.grey_areas.head = next = node->next;
    xfree(node);
    if (!next) {
      _gst_mem.grey_areas.tail = NULL;
    }

    cheney_scan();

    /* The scan might have greyed more areas.  */
    if (!next) {
      next = _gst_mem.grey_areas.head;
    }
  }
}

int scanned_fields_in(gst_object object, int flags) {
  if COMMON (!(flags & (F_WEAK | F_CONTEXT))) {
    return NUM_OOPS(object) + OBJ_HEADER_SIZE_WORDS;
  }

  if COMMON (flags & F_CONTEXT) {
    const intptr_t methodSP = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(object));
    return OBJ_METHOD_CONTEXT_CONTEXT_STACK(object) + methodSP + 1 -
           (OOP *)object;
  }

  return OBJ_HEADER_SIZE_WORDS;
}

void cheney_scan(void) {
#if defined(GC_DEBUG_OUTPUT)
  printf("Starting Cheney scan\n");
#endif

  while (_gst_mem.scan.queue_at != _gst_mem.tenuring_queue.allocPtr) {
    OOP oop;
    int i, numFields;

    if (_gst_mem.scan.queue_at >= _gst_mem.tenuring_queue.topPtr) {
      _gst_mem.scan.queue_at = _gst_mem.tenuring_queue.minPtr;
    }

    if (_gst_mem.scan.queue_at == _gst_mem.tenuring_queue.allocPtr) {
      break;
    }

    oop = *_gst_mem.scan.queue_at;

#if defined(GC_DEBUGGING)
    if (!IS_OOP_ADDR(oop))
      abort();
#endif

#if defined(GC_DEBUG_OUTPUT)
    printf(">Scan ");
    _gst_display_oop(oop);
#endif

    _gst_mem.scan.current = oop;
    _gst_mem.scan.queue_at++;

    if (OOP_GET_FLAGS(oop) & F_EPHEMERON) {
      continue;
    }

    _gst_mem.scan.at = (OOP *)OOP_TO_OBJ(oop);
    numFields = scanned_fields_in(OOP_TO_OBJ(oop), OOP_GET_FLAGS(oop));

    for (i = 0; i < numFields; i++) {
      MAYBE_COPY_OOP(_gst_mem.scan.at[i]);
    }
  }

#if defined(GC_DEBUG_OUTPUT)
  printf("Ending Cheney scan\n");
#endif
}

void _gst_copy_oop_range(OOP *curOOP, OOP *atEndOOP) {
  OOP *pOOP;
  for (pOOP = curOOP; pOOP < atEndOOP; pOOP++) {
    MAYBE_COPY_OOP(*pOOP);
  }
}

void _gst_copy_an_oop(OOP oop) {
  int i, n;
  do {
    gst_object obj;
    OOP *pData;

    obj = OOP_TO_OBJ(oop);
    pData = (OOP *)obj;

#if defined(GC_DEBUG_OUTPUT)
    printf(">Copy ");
    _gst_display_oop(oop);
#endif

#if defined(GC_DEBUGGING)
    if UNCOMMON (!IS_INT(OBJ_SIZE(obj))) {
      printf("Size not an integer in OOP %p (%p)\n", oop, obj);
      abort();
    }

    if UNCOMMON (TO_INT(OBJ_SIZE(obj)) < 2) {
      printf("Invalid size for OOP %p (%p)\n", oop, obj);
      abort();
    }

    if UNCOMMON (OOP_GET_FLAGS(oop) == 0) {
      printf("Free OOP %p was referenced\n", oop);
      abort();
    }

    if UNCOMMON ((OOP_GET_FLAGS(oop) & F_OLD) ||
                 IS_SURVIVOR_ADDR(obj,
                                  _gst_mem.active_half == &_gst_mem.surv[1])) {
      printf("Copying an already copied object\n");
      abort();
      return;
    }
#endif

    queue_put(&_gst_mem.tenuring_queue, &oop, 1);
    OOP_SET_OBJECT(oop, obj = (gst_object)queue_put(_gst_mem.active_half, pData,
                                                    TO_INT(OBJ_SIZE(obj))));

    OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~(F_SPACES | F_POOLED));
    OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | _gst_mem.active_flag);

    /* Look for a child that has not been copied and move it
       near the object.  This improves the locality of reference.
       We do not copy the class (that's the reason for the -1
       here).  */
    n = scanned_fields_in(obj, OOP_GET_FLAGS(oop)) - OBJ_HEADER_SIZE_WORDS;
    if (OOP_GET_FLAGS(oop) & F_EPHEMERON) {
      /* For ephemerons, do the work later.  */
      add_grey_object(oop);
      return;
    }

    for (i = 0; i < n; i++) {
      OOP newOOP = obj->data[i];
      if (!IS_OOP_COPIED(newOOP)) {
        oop = newOOP;
        break;
      }
    }
  } while (i < n);
}

void mark_oops(void) {
  _gst_reset_buffer();
  _gst_mark_registered_oops();
  _gst_mark_processor_registers();
  mark_ephemeron_oops();
}

void mark_ephemeron_oops(void) {
  OOP *pOOP, *pDeadOOP, *base;
  int i, size;

  /* Make a local copy of the buffer */
  size = _gst_buffer_size();
  base = alloca(size);
  _gst_copy_buffer(base);
  _gst_reset_buffer();
  size /= sizeof(PTR);

  /* First pass: distinguish objects whose key was reachable from
     the outside by clearing their F_EPHEMERON bit.  */
  for (pOOP = base, i = size; i--; pOOP++) {
    OOP oop = *pOOP;
    gst_object obj = OOP_TO_OBJ(oop);
    OOP key = obj->data[0];

    if (OOP_GET_FLAGS(key) & F_REACHABLE) {
      OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) & ~F_EPHEMERON);
    }

    OOP_SET_FLAGS(key, OOP_GET_FLAGS(key) | F_REACHABLE);
  }

  for (pOOP = pDeadOOP = base, i = size; i--;) {
    OOP oop = *pOOP++;
    gst_object obj = OOP_TO_OBJ(oop);
    OOP key = obj->data[0];
    int num = NUM_OOPS(obj);
    int j;

    /* Find if the key is reachable from the objects (so that
       we can mourn the ephemeron if this is not so).  */
    OOP_SET_FLAGS(key, OOP_GET_FLAGS(key) & ~F_REACHABLE);

    for (j = 1; j < num; j++) {
      MAYBE_MARK_OOP(obj->data[j]);
    }

    /* Remember that above we cleared F_EPHEMERON if the key
       is alive.  */
    if (!IS_OOP_MARKED(key) && (OOP_GET_FLAGS(oop) & F_EPHEMERON)) {
      *pDeadOOP++ = oop;
    }

    /* Ok, now mark the key.  */
    MAYBE_MARK_OOP(key);

    /* Restore the flag in case it was cleared.  */
    OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_EPHEMERON);
  }

  /* If more ephemerons were reachable from the object, go on...  */
  if (_gst_buffer_size()) {
    mark_ephemeron_oops();
  }

  _gst_add_buf_data(base, (char *)pDeadOOP - (char *)base);
}

#define TAIL_MARK_OOP(newOOP)                                                  \
  do {                                                                         \
    PREFETCH_ADDR(OOP_TO_OBJ((newOOP)), PREF_READ | PREF_NTA);                 \
    oop = (newOOP);                                                            \
    goto markOne; /* tail recurse!!! */                                        \
  } while (0)

#define TAIL_MARK_OOPRANGE(firstOOP, oopAtEnd)                                 \
  do {                                                                         \
    PREFETCH_START(firstOOP, PREF_READ | PREF_NTA);                            \
    curOOP = (OOP *)(firstOOP);                                                \
    atEndOOP = (OOP *)(oopAtEnd);                                              \
    oop = NULL;                                                                \
    goto markRange;                                                            \
  } while (0)

void _gst_mark_an_oop_internal(OOP oop) {
  OOP *curOOP, *atEndOOP;
  struct mark_queue *markQueue = _gst_mem.markQueue;
  struct mark_queue *lastMarkQueue = _gst_mem.lastMarkQueue;
  struct mark_queue *currentMarkQueue = markQueue;
  goto markOne;

markRange : {
  OOP firstOOP = NULL;

  /* The first unmarked OOP is used for tail recursion.  */
  while (curOOP < atEndOOP) {
    oop = *curOOP++;
    if (IS_OOP(oop) && !IS_OOP_MARKED(oop)) {
      OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_REACHABLE);
      firstOOP = oop;
      break;
    }
  }

  /* The second unmarked OOP is the first that is placed on the mark
     queue.  TODO: split REACHABLE and VISITED flags.  An object is
     marked REACHABLE here, and REACHABLE|VISITED in the markOne label.
     At the end of GC, all REACHABLE objects are also VISITED.
     The above loop should seek an object that is not VISITED so
     that it can be marked.  For the loop below, however, REACHABLE
     objects are known to be somewhere else on the mark stack, and can
     be skipped.

     Skipping objects after the first unmarked OOP is still useful,
     because it keeps the stack size a bit lower in the relatively common
     case of many integer or nil instance variables.  */
  while (curOOP < atEndOOP) {
    oop = *curOOP;

    if (IS_OOP(oop) && !IS_OOP_MARKED(oop)) {
      if (currentMarkQueue == lastMarkQueue) {
        const size_t size = lastMarkQueue - markQueue;

        _gst_mem.markQueue = (struct mark_queue *)xrealloc(
            _gst_mem.markQueue, 2 * size * sizeof(struct mark_queue));
        _gst_mem.lastMarkQueue = &_gst_mem.markQueue[2 * size];

        markQueue = _gst_mem.markQueue;
        lastMarkQueue = _gst_mem.lastMarkQueue;
        currentMarkQueue = &_gst_mem.markQueue[size];
      }
      currentMarkQueue->firstOOP = curOOP;
      currentMarkQueue->endOOP = atEndOOP;
      currentMarkQueue++;
      break;
    }

    curOOP++;
  }

  if (!firstOOP) {
    goto pop;
  }

  TAIL_MARK_OOP(firstOOP);
}

markOne : {
  OOP objClass;
  gst_object object;
  uintptr_t size;

#if defined(GC_DEBUGGING)
  if UNCOMMON (IS_OOP_FREE(oop)) {
    printf("Error! Free OOP %p is being marked!\n", oop);
    abort();
    return;
  }
#endif

#if defined(GC_DEBUG_OUTPUT)
  printf(">Mark ");
  _gst_display_oop(oop);
#endif

  /* see if the object has pointers, set up to copy them if so.
   */
  OOP_SET_FLAGS(oop, OOP_GET_FLAGS(oop) | F_REACHABLE);
  object = OOP_TO_OBJ(oop);
  objClass = OBJ_CLASS(object);
  if UNCOMMON (OOP_GET_FLAGS(oop) & F_CONTEXT) {
    const intptr_t methodSP = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(object));
    /* printf("setting up for loop on context %x, sp = %d\n",
       ctx, methodSP); */
    TAIL_MARK_OOPRANGE(&OBJ_CLASS(object),
                       OBJ_METHOD_CONTEXT_CONTEXT_STACK(object) + methodSP + 1);

  } else if UNCOMMON (OOP_GET_FLAGS(oop) & (F_EPHEMERON | F_WEAK)) {
    if (OOP_GET_FLAGS(oop) & F_EPHEMERON) {
      _gst_add_buf_pointer(oop);
    }

    /* In general, there will be many instances of a class,
       but only the first time will it be unmarked.  So I'm
       marking this as uncommon.  */
    if UNCOMMON (!IS_OOP_MARKED(objClass))
      TAIL_MARK_OOP(objClass);
  } else {
    size = NUM_OOPS(object);
    if COMMON (size)
      TAIL_MARK_OOPRANGE(&OBJ_CLASS(object), object->data + size);

    else if UNCOMMON (!IS_OOP_MARKED(objClass))
      TAIL_MARK_OOP(objClass);
  }
}

pop : {
  if (currentMarkQueue > markQueue) {
    currentMarkQueue--;
    TAIL_MARK_OOPRANGE(currentMarkQueue->firstOOP, currentMarkQueue->endOOP);
  }
}
}

void _gst_mark_oop_range(OOP *curOOP, OOP *atEndOOP) {
  OOP *pOOP;
  for (pOOP = curOOP; pOOP < atEndOOP; pOOP++) {
    MAYBE_MARK_OOP(*pOOP);
  }
}

void _gst_inc_init_registry(void) {
  _gst_mem.inc_base = (OOP *)xmalloc(INIT_NUM_INCUBATOR_OOPS * sizeof(OOP *));
  _gst_mem.inc_ptr = _gst_mem.inc_base;
  _gst_mem.inc_end = _gst_mem.inc_base + INIT_NUM_INCUBATOR_OOPS;

  /* Make the incubated objects part of the root set */
  _gst_register_oop_array(&_gst_mem.inc_base, &_gst_mem.inc_ptr);
}

void _gst_inc_grow_registry(void) {
  unsigned oldPtrOffset;
  unsigned oldRegistrySize, newRegistrySize;

  oldPtrOffset = _gst_mem.inc_ptr - _gst_mem.inc_base;
  oldRegistrySize = _gst_mem.inc_end - _gst_mem.inc_base;
  newRegistrySize = oldRegistrySize + INCUBATOR_CHUNK_SIZE;

  _gst_mem.inc_base =
      (OOP *)xrealloc(_gst_mem.inc_base, newRegistrySize * sizeof(OOP *));
  _gst_mem.inc_ptr = _gst_mem.inc_base + oldPtrOffset;
  _gst_mem.inc_end = _gst_mem.inc_base + newRegistrySize;
}
