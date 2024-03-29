/******************************** -*- C -*- ****************************
 *
 *	Object table module Inlines.
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2006, 2009 Free Software Foundation, Inc.
 * Written by Steve Byrne.
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

static inline OOP alloc_oop(PTR obj, intptr_t flags);

/* Copy the OOP object because it is part of the root set.  Integers
   and already-copied OOPs are not processed silently.  */
#define MAYBE_COPY_OOP(oop)                                                    \
  do {                                                                         \
    if (!IS_OOP_COPIED(oop)) {                                                 \
      _gst_copy_an_oop(oop);                                                   \
    }                                                                          \
  } while (0)

/* Mark the OOP object because it is part of the root set.  Integers
   and already-marked OOPs are not processed silently.  */
#define MAYBE_MARK_OOP(oop)                                                    \
  do {                                                                         \
    if (IS_OOP(oop) && !IS_OOP_MARKED(oop)) {                                  \
      _gst_mark_an_oop_internal((oop));                                        \
    }                                                                          \
  } while (0)

#define IS_OOP_COPIED(oop) (IS_INT(oop) || IS_OOP_VALID_GC(oop))

#define IS_OOP_NEW(oop) ((OOP_GET_FLAGS((oop)) & F_SPACES) != 0)

/* This can only be used at the start or the end of an incremental
   GC cycle.  */
#define IS_OOP_VALID_GC(oop) ((OOP_GET_FLAGS((oop)) & _gst_mem.live_flags) != 0)

/* After a global GC, the live_flags say that an object is live
   if it is marked reachable.  Old objects that have already survived
   the incremental sweep pass, however, are not marked as reachable.  */
#define IS_OOP_VALID(oop)                                                      \
  (OOP_GET_FLAGS((oop)) & _gst_mem.live_flags ||                               \
   ((OOP_GET_FLAGS((oop)) & F_OLD) &&                                          \
    ((oop) <= _gst_mem.last_swept_oop || (oop) > _gst_mem.next_oop_to_sweep)))

#define IS_OOP_MARKED(oop) ((OOP_GET_FLAGS((oop)) & F_REACHABLE) != 0)

#define IS_OOP_FREE(oop) (OOP_GET_FLAGS((oop)) == 0)

/* Checks to see if INDEX (a long index into the OOP table, 1 based
   due to being called from Smalltalk via a primitive) represents a
   valid OOP.  Returns true if so.  */
#define OOP_INDEX_VALID(index)                                                 \
  ((index) < _gst_mem.ot_size)

/* Answer the INDEX-th OOP in the table.  */
#define OOP_AT(index) (&_gst_mem.ot[index])

/* Answer the index of OOP in the table.  */
#define OOP_INDEX(oop) ((OOP)(oop)-_gst_mem.ot)

/* Answer whether ADDR is part of the OOP table.  */
#define IS_OOP_ADDR(addr)                                                      \
  ((OOP)(addr) >= _gst_mem.ot &&                                          \
   (OOP)(addr) <= _gst_mem.last_allocated_oop &&                               \
   (((intptr_t)addr & (sizeof(struct oop_s) - 1)) == 0))

/* Answer whether ADDR is part of newspace.  */
#define IS_EDEN_ADDR(addr)                                                     \
  ((OOP *)(addr) >= _gst_mem.eden.minPtr &&                                    \
   (OOP *)(addr) < _gst_mem.eden.maxPtr)

/* Answer whether ADDR is part of survivor space N.  */
#define IS_SURVIVOR_ADDR(addr, n)                                              \
  ((OOP *)(addr) >= _gst_mem.surv[(n)].minPtr &&                               \
   (OOP *)(addr) < _gst_mem.surv[(n)].maxPtr)

#define INC_ADD_OOP(oop)                                                       \
  ((_gst_mem.inc_ptr >= _gst_mem.inc_end ? _gst_inc_grow_registry()            \
                                         : (void)0),                           \
   *_gst_mem.inc_ptr++ = (oop))

#define INC_SAVE_POINTER()                                                     \
  (_gst_mem.inc_depth++, _gst_mem.inc_ptr - _gst_mem.inc_base)

#define INC_RESTORE_POINTER(ptr)                                               \
  (_gst_mem.inc_depth--, _gst_mem.inc_ptr = (ptr) + _gst_mem.inc_base)

static inline intptr_t inc_current_depth(void) { return _gst_mem.inc_depth; }

/* Given an object OBJ, allocate an OOP table slot for it and returns
   it.  It marks the OOP so that it indicates the object is in new
   space, and that the oop has been referenced on this pass (to keep
   the OOP table reaper from reclaiming this OOP).  */
static inline OOP alloc_oop(PTR objData, intptr_t flags) {
  REGISTER(1, OOP oop);
  REGISTER(2, OOP lastOOP);

  /* Slow path find a new arena */
  if (UNCOMMON(_gst_mem.current_arena[current_thread_id]->free_oops == 0)) {
    /* Release current arena */
    _gst_detach_oop_arena_entry(_gst_mem.current_arena[current_thread_id] - _gst_mem.ot_arena);
    _gst_alloc_oop_arena_entry(current_thread_id);
  }

  oop = _gst_mem.current_arena[current_thread_id]->first_free_oop;
  lastOOP = &_gst_mem.ot[((_gst_mem.current_arena[current_thread_id] - _gst_mem.ot_arena) * 32768) + 32768];

  while (IS_OOP_VALID_GC(oop) && oop < lastOOP) {
    OOP_NEXT(oop);
  }

  if (UNCOMMON (oop == lastOOP)) {
    nomemory(true);
    return NULL;
  }

  _gst_mem.current_arena[current_thread_id]->first_free_oop = oop;
  _gst_mem.current_arena[current_thread_id]->free_oops--;

  /* there are no free OOP.  */
  if (UNCOMMON (!atomic_load(&_gst_mem.num_free_oops))) {
    nomemory(true);
    return NULL;
  }

  _gst_mem.last_swept_oop = oop;

  atomic_fetch_sub(&_gst_mem.num_free_oops, 1);

  /* Force a GC as soon as possible if we're low on OOPs.  */
  if (UNCOMMON (atomic_load(&_gst_mem.num_free_oops) < LOW_WATER_OOP_THRESHOLD * atomic_load(&_gst_interpret_thread_counter) * 4)) {
    atomic_store(&_gst_mem.eden.maxPtr, _gst_mem.eden.allocPtr);
  }

  while (oop > atomic_load(&_gst_mem.last_allocated_oop)) {
    atomic_store(&_gst_mem.last_allocated_oop, oop);
  }

  OOP_SET_OBJECT(oop, (gst_object)objData);
  OOP_SET_FLAGS(oop, flags);

  return (oop);
}
