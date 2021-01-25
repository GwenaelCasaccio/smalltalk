/******************************** -*- C -*- ****************************
 *
 *	The Smalltalk Virtual Machine itself.
 *
 *	This, together with oop.c, is the `bridge' between Smalltalk and
 *	the underlying machine
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 1988,89,90,91,92,94,95,99,2000,2001,2002,2006,2007,2008,2009
 * Free Software Foundation, Inc.
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
 * GNU Smalltalk; see the file COPYING.	 If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/

#include "gstpriv.h"
#include "lock.h"

/* The local regs concept hopes, by caching the values of IP and SP in
   local register variables, to increase performance.  You only need
   to export the variables when calling out to routines that might
   change them and that create objects.  This is because creating
   objects may trigger the GC, which can change the values of IP and
   SP (since they point into the object space).  It's easy to deal
   with that, however, it's just a matter of importing and exporting
   the registers at the correct places: for example stack operations
   are innocuous, while message sends can result in a GC (because
   stack chunks are exhausted or because primitive #new is invoked),
   so they export the registers and import them (possibly with their
   value changed by the GC) after the send.  I'm leaving the code to
   deal with them as local registers conditionally compiled in so that
   you can disable it easily if necessary; however this seems quite
   improbable except for debugging purposes.  */
#define LOCAL_REGS

/* By "hard wiring" the definitions of the special math operators
   (bytecodes 176-191), we get a performance boost of more than 50%.
   Yes, it means that we cannot redefine + et al for SmallInteger and
   Float, but I think the trade is worth it.  Besides, the Blue Book
   does it.  */
#define OPEN_CODE_MATH

/* Pipelining uses separate fetch-decode-execute stages, which is a
   nice choice for VLIW machines.  It also enables more aggressive
   caching of global variables.  It is currently enabled for the IA-64
   only, because it is a win only where we would have had lots of
   unused instruction scheduling slots and an awful lot of registers. */
#if REG_AVAILABILITY == 3
#define PIPELINING
#endif

/* The structure of execution context objects.  */
typedef struct gst_context_part {
  OBJ_HEADER;
  OOP parentContext;
  OOP native_ip; /* used by JIT */
  OOP ipOffset;  /* an integer byte index into method */
  OOP spOffset;  /* an integer index into cur context
                    stack */
  OOP receiver;  /* the receiver OOP */
  OOP method;    /* the method that we're executing */
  OOP x;         /* depends on the subclass */
  OOP contextStack[1];
} * gst_context_part;

/* These macros are used to quickly compute the number of words needed
   for a context with a maximum allowable stack depth of DEPTH.  */
#define FIXED_CTX_SIZE (sizeof(struct gst_context_part) / sizeof(PTR) - 1)
#define CTX_SIZE(depth) (((depth) << DEPTH_SCALE) + FIXED_CTX_SIZE)

/* Answer the quantum assigned to each Smalltalk process (in
   milliseconds) before it is preempted.  Setting this to zero
   disables preemption until gst_processor_scheduler>>#timeSlice: is
   invoked.  */
#define DEFAULT_PREEMPTION_TIMESLICE 40

/* This symbol does not control execution speed.  Instead, it causes
   SEND_MESSAGE to print every message that is ever sent in the
   SmallInteger(Object)>>#printString form.  Can be useful to find out
   the last method sent before an error, if the context stack is
   trashed when the debugger gets control and printing a backtrace is
   impossible.  */
/* #define DEBUG_CODE_FLOW */

/* The method cache is a hash table used to cache the most commonly
   used methods.  Its size is determined by this preprocessor
   constant.  It is currently 2048, a mostly random choice; you can
   modify it, but be sure it is a power of two.  Additionally,
   separately from this, the interpreter caches the last primitive
   numbers used for sends of #at:, #at:put: and #size, in an attempt
   to speed up these messages for Arrays, Strings, and ByteArrays.  */
#define METHOD_CACHE_SIZE (1 << 11)

typedef struct interp_jmp_buf {
  jmp_buf jmpBuf;
  struct interp_jmp_buf *next;
  unsigned short suspended;
  unsigned char interpreter;
  unsigned char interrupted;
  OOP processOOP;
} interp_jmp_buf;

/* If this is true, for each byte code that is executed, we print on
   stdout the byte index within the current gst_compiled_method and a
   decoded interpretation of the byte code.  */
thread_local int _gst_execution_tracing = 0;

/* When this is true, and an interrupt occurs (such as SIGABRT),
   Smalltalk will terminate itself by making a core dump (normally it
   produces a backtrace).  */
mst_Boolean _gst_make_core_file = false;

/* When true, this indicates that there is no top level loop for
   control to return to, so it causes the system to exit.  */
mst_Boolean _gst_non_interactive = true;

/* The table of functions that implement the primitives.  */
prim_table_entry _gst_primitive_table[NUM_PRIMITIVES];
prim_table_entry _gst_default_primitive_table[NUM_PRIMITIVES];

/* Some performance counters from the interpreter: these
   count the number of special returns.  */
thread_local unsigned long _gst_literal_returns = 0;
thread_local unsigned long _gst_inst_var_returns = 0;
thread_local unsigned long _gst_self_returns = 0;

/* The number of primitives executed.  */
thread_local unsigned long _gst_primitives_executed = 0;

/* The number of bytecodes executed.  */
thread_local unsigned long _gst_bytecode_counter = 0;

/* The number of method cache misses */
thread_local unsigned long _gst_cache_misses = 0;

/* The number of cache lookups - either hits or misses */
thread_local unsigned long _gst_sample_counter = 0;

/* The OOP for an IdentityDictionary that stores the raw profile.  */
OOP _gst_raw_profile = NULL;

/* A bytecode counter value used while profiling. */
unsigned long _gst_saved_bytecode_counter = 0;

static thread_local ip_type method_base;

/* Global state
   The following variables constitute the interpreter's state:

   ip -- the real memory address of the next byte code to be executed.

   sp -- the real memory address of the stack that's stored in the
   currently executing block or method context.

   _gst_this_method -- a gst_compiled_method or gst_compiled_block
   that is the currently executing method.

   _gst_this_context_oop -- a gst_block_context or gst_method_context
   that indicates the context that the interpreter is currently
   running in.

   _gst_temporaries -- physical address of the base of the method
   temporary variables.  Typically a small number of bytes (multiple
   of 4 since it points to OOPs) lower than sp.

   _gst_literals -- physical address of the base of the method
   literals.

   _gst_self -- an OOP that is the current receiver of the current
   message.  */

/* The virtual machine's stack and instruction pointers.  */
thread_local OOP *sp = NULL;
ip_type ip[100];

OOP *_gst_temporaries[100] = { NULL };
OOP *_gst_literals[100] = { NULL };
OOP _gst_self[100] = { NULL };
OOP _gst_this_context_oop[100] = { NULL };
OOP _gst_this_method[100] = { NULL };

/* Signal this semaphore at the following instruction.  */
static OOP single_step_semaphore = NULL;

/* CompiledMethod cache which memoizes the methods and some more
   information for each class->selector pairs.  */
static thread_local method_cache_entry method_cache[METHOD_CACHE_SIZE] CACHELINE_ALIGNED;

/* The number of the last primitive called.  */
static thread_local int last_primitive;

/* A special cache that tries to skip method lookup when #at:, #at:put
   and #size are implemented by a class through a primitive, and is
   repeatedly sent to instances of the same class.  Since this is a
   mini-inline cache it makes no sense when JIT translation is
   enabled.  */
#ifndef ENABLE_JIT_TRANSLATION
static thread_local OOP at_cache_class;
static thread_local intptr_t at_cache_spec;

static thread_local OOP at_put_cache_class;
static thread_local intptr_t at_put_cache_spec;

static thread_local OOP size_cache_class;
static thread_local int size_cache_prim;

static thread_local OOP class_cache_class;
static thread_local int class_cache_prim;
#endif

/* Queue for async (outside the interpreter) semaphore signals */
static thread_local mst_Boolean async_queue_enabled = true;
static async_queue_entry queued_async_signals_tail;
static async_queue_entry *queued_async_signals = &queued_async_signals_tail;
static async_queue_entry *queued_async_signals_sig = &queued_async_signals_tail;

/* When not NULL, this causes the byte code interpreter to immediately
   send the message whose selector is here to the current stack
   top.  */
const char *_gst_abort_execution = NULL;

/* Set to non-nil if a process must preempt the current one.  */
static thread_local OOP switch_to_process;

/* Set to true if it is time to switch process in a round-robin
   time-sharing fashion.  */
static mst_Boolean time_to_preempt;

/* Used to bail out of a C callout and back to the interpreter.  */
static thread_local interp_jmp_buf *reentrancy_jmp_buf = NULL;

/* when this flag is on and execution tracing is in effect, the top of
   the stack is printed as well as the byte code */
static int verbose_exec_tracing = false;

/* This is the bridge to the primitive operations in the GNU Smalltalk
   system.  This function invokes the proper primitive_func with the
   correct id and the same NUMARGS and METHODOOP with which it was
   invoked.  */
static inline intptr_t execute_primitive_operation(int primitive,
                                                   volatile int numArgs);

/* Execute a #at: primitive, with arguments REC and IDX, knowing that
   the receiver's class has an instance specification SPEC.  */
static inline mst_Boolean cached_index_oop_primitive(OOP rec, OOP idx,
                                                     intptr_t spec);

/* Execute a #at:put: primitive, with arguments REC/IDX/VAL, knowing that
   the receiver's class has an instance specification SPEC.  */
static inline mst_Boolean
cached_index_oop_put_primitive(OOP rec, OOP idx, OOP val, intptr_t spec);

/* This functions accepts an OOP for a Semaphore object and puts the
   PROCESSOOP to sleep, unless the semaphore has excess signals
   on it.  */
static void sync_wait_process(OOP semaphoreOOP, OOP processOOP);

/* Empty the queue of asynchronous calls.  */
static void empty_async_queue(void);

/* Try to find another process with higher or same priority as the
   active one.  Return whether there is one.  */
static mst_Boolean would_reschedule_process(void);

/* Locates in the ProcessorScheduler's process lists and returns the
   highest priority process different from the current process.  */
static OOP highest_priority_process(void);

/* Remove the head of the given list (a Semaphore is a subclass of
   LinkedList) and answer it.  */
static OOP remove_first_link(OOP semaphoreOOP);

/* Add PROCESSOOP as the head of the given list (a Semaphore is a
   subclass of LinkedList) and answer it.  */
static void add_first_link(OOP semaphoreOOP, OOP processOOP);

/* Add PROCESSOOP as the tail of the given list (a Semaphore is a
   subclass of LinkedList) and answer it.  */
static void add_last_link(OOP semaphoreOOP, OOP processOOP);

/* Answer the highest priority process different from the current one.
   Answer nil if there is no other process than the current one.
   Create a new process that terminates execution if there is no
   runnable process (which should never be because there is always the
   idle process).  */
static OOP next_scheduled_process(void);

/* Create a Process that is running at userSchedulingPriority on the
   CONTEXTOOP context, and answer it.  */
static OOP create_callin_process(OOP contextOOP);

/* Set a timer at the end of which we'll preempt the current process.  */
static void set_preemption_timer(void);

/* Same as _gst_parse_stream, but creating a reentrancy_jmpbuf.  Returns
   true if interrupted. */
static mst_Boolean parse_stream_with_protection(OOP currentNamespace);

/* Same as _gst_parse_method_from_stream, but creating a reentrancy_jmpbuf.
   Returns true if interrupted, pushes the last compiled method on the
   stack. */
static mst_Boolean
parse_method_from_stream_with_protection(OOP currentClass, OOP currentCategory);

/* Put the given process to sleep by rotating the list of processes for
   PROCESSOOP's priority (i.e. it was the head of the list and becomes
   the tail).  */
static void sleep_process(OOP processOOP);

/* Yield control from the active process.  */
static void active_process_yield(void);

/* Sets flags so that the interpreter switches to PROCESSOOP at the
   next sequence point.  Unless PROCESSOOP is already active, in which
   case nothing happens, the process is made the head of the list of
   processes for PROCESSOOP's priority.  Return PROCESSOOP.  */
static OOP activate_process(OOP processOOP);

/* Restore the virtual machine's state from the ContextPart OOP.  */
static void resume_suspended_context(OOP oop);

/* Save the virtual machine's state into the suspended Process and
   ContextPart objects, and load them from NEWPROCESS and from
   NEWPROCESS's suspendedContext.  The Processor (the only instance
   of ProcessorScheduler) is also updated accordingly.  */
static void change_process_context(OOP newProcess);

/* Mark the semaphores attached to the process system (asynchronous
   events, the signal queue, and if any the process which we'll
   switch to at the next sequence point).  */
static void mark_semaphore_oops(void);

/* Copy the semaphores attached to the process system (asynchronous
   events, the signal queue, and if any the process which we'll
   switch to at the next sequence point).  */
static void copy_semaphore_oops(void);

/* Suspend execution of PROCESSOOP.  */
static void suspend_process(OOP processOOP);

/* Resume execution of PROCESSOOP.  If it must preempt the currently
   running process, or if ALWAYSPREEMPT is true, put to sleep the
   active process and activate PROCESSOOP instead; if it must not,
   make it the head of the process list for its priority, so that
   it will be picked once higher priority processes all go to sleep.

   If PROCESSOOP is terminating, answer false.  If PROCESSOOP can
   be restarted or at least put back in the process list for its
   priority, answer true.  */
static mst_Boolean resume_process(OOP processOOP, mst_Boolean alwaysPreempt);

/* Answer whether PROCESSOOP is ready to execute (neither terminating,
   nor suspended, nor waiting on a semaphore).  */
static mst_Boolean is_process_ready(OOP processOOP) ATTRIBUTE_PURE;

/* Answer whether any processes are queued in the PROCESSLISTOOP
   (which can be a LinkedList or a Semaphore).  */
static inline mst_Boolean is_empty(OOP processListOOP) ATTRIBUTE_PURE;

/* Answer whether the processs is terminating, that is, it does not
   have an execution context to resume execution from.  */
static inline mst_Boolean is_process_terminating(OOP processOOP) ATTRIBUTE_PURE;

/* Answer the process that is scheduled to run (that is, the
   executing process or, if any, the process that is scheduled
   to start execution at the next sequence point.  */
static inline OOP get_scheduled_process(void) ATTRIBUTE_PURE;

/* Answer the active process (that is, the process that executed
   the last bytecode.  */
static inline OOP get_active_process(void) ATTRIBUTE_PURE;

/* Create a new Semaphore OOP with SIGNALS signals on it and return it.  */
static inline OOP semaphore_new(int signals);

/* Pop NUMARGS items from the stack and put them into a newly
   created Array object, which is them returned.  */
static inline OOP create_args_array(int numArgs);

/* This is the equivalent of SEND_MESSAGE, but is for blocks.  The
   block context that is to the the receiver of the "value" message
   should be the NUMARGS-th into the stack.  SP is set to the top of
   the arguments in the block context, which have been copied out of
   the caller's context.

   The block should accept between NUMARGS - CULL_UP_TO and
   NUMARGS arguments.  If this is not true (failure) return true;
   on success return false.  */
static mst_Boolean send_block_value(int numArgs, int cull_up_to);

/* This is a kind of simplified _gst_send_message_internal that,
   instead of setting up a context for a particular receiver, stores
   information on the lookup into METHODDATA.  Unlike
   _gst_send_message_internal, this function is generic and valid for
   both the interpreter and the JIT compiler.  */
static mst_Boolean lookup_method(OOP sendSelector,
                                 method_cache_entry *methodData, int sendArgs,
                                 OOP method_class);

/* This tenures context objects from the stack to the context pools
   (see below for a description).  */
static void empty_context_stack(void);

/* This allocates a new context pool, eventually triggering a GC once
   no more pools are available.  */
static void alloc_new_chunk();

/* This allocates a context object which is SIZE words big from
   a pool, allocating one if the current pool is full.  */
static inline gst_object alloc_stack_context(int size);

/* This frees the most recently allocated stack from the current
   context pool.  It is called when unwinding.  */
static inline void dealloc_stack_context(gst_context_part context);

/* This allocates a new context of SIZE, prepares an OOP for it
   (taking it from the LIFO_CONTEXTS arrays that is defined below),
   and pops SENDARGS arguments from the current context.  Only the
   parentContext field of the newly-allocated context is initialized,
   because the other fields can be desumed from the execution state:
   these other fields instead are filled in the parent context since
   the execution state will soon be overwritten.  */
static inline gst_object activate_new_context(int size, int sendArgs);

/* Push the ARGS topmost words below the stack pointer, and then TEMPS
   nil objects, onto the stack of CONTEXT.  */
static inline void prepare_context(gst_context_part context, int args,
                                   int temps);

/* Return from the current context and restore the virtual machine's
   status (ip, sp, _gst_this_method, _gst_self, ...).  */
static inline void unwind_context(void);

/* Check whether it is true that sending SENDSELECTOR to RECEIVER
   accepts NUMARGS arguments.  Note that the RECEIVER is only used to
   do a quick check in the method cache before examining the selector
   itself; in other words, true is returned even if a message is not
   understood by the receiver, provided that NUMARGS matches the
   number of arguments expected by the selector (1 if binary, else the
   number of colons).  If you don't know a receiver you can just pass
   _gst_nil_oop or directly call _gst_selector_num_args.  */
static inline mst_Boolean check_send_correctness(OOP receiver, OOP sendSelector,
                                                 int numArgs);

/* Unwind the contexts up until the caller of the method that
   created the block context, no matter how many levels of message
   sending are between where we currently are and the context that
   we are going to return from.

   Note that unwind_method is only called inside `dirty' (or `full')
   block closures, hence the context we return from can be found by
   following OUTERCONTEXT links starting from the currently executing
   context, and until we reach a MethodContext.  */
static mst_Boolean unwind_method(void);

/* Unwind up to context returnContextOOP, carefully examining the
   method call stack.  That is, we examine each context and we only
   deallocate those that, during their execution, did not create a
   block context; the others need to be marked as returned.  We
   continue up the call chain until we finally reach methodContextOOP
   or an unwind method.  In this case the non-unwind contexts between
   the unwind method and the returnContextOOP must be removed from the
   chain.  */
static mst_Boolean unwind_to(OOP returnContextOOP);

/* Arrange things so that all the non-unwinding contexts up to
   returnContextOOP aren't executed.  For block contexts this can
   be done simply by removing them from the chain, but method
   context must stay there so that we can do non-local returns
   from them!  For this reason, method contexts are flagged as
   disabled and unwind_context takes care of skipping them when
   doing a local return.  */
static mst_Boolean disable_non_unwind_contexts(OOP returnContextOOP);

/* Called to preempt the current process after a specified amount
   of time has been spent in the GNU Smalltalk interpreter.  */
#ifdef ENABLE_PREEMPTION
static RETSIGTYPE preempt_smalltalk_process(int sig);
#endif

/* Push an execution state for process PROCESSOOP.  The process is
   used for two reasons: 1) it is suspended if there is a call-in
   while the execution state is on the top of the stack; 2) it is
   sent #userInterrupt if the user presses Ctrl-C.  */
static void push_jmp_buf(interp_jmp_buf *jb, int for_interpreter,
                         OOP processOOP);

/* Pop an execution state.  Return true if the interruption has to
   be propagated up.  */
static mst_Boolean pop_jmp_buf(void);

/* Jump out of the top execution state.  This is used by C call-out
   primitives to jump out repeatedly until a Smalltalk process is
   encountered and terminated.  */
static void stop_execution(void);

/* Pick a process that is the highest-priority process different from
   the currently executing one, and schedule it for execution after
   the first sequence points.  */
#define ACTIVE_PROCESS_YIELD() activate_process(next_scheduled_process())

/* Answer an OOP for a Smalltalk object of class Array, holding the
   different process lists for each priority.  */
#define GET_PROCESS_LISTS()                                                    \
  ((OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_LISTS(OOP_TO_OBJ(_gst_processor_oop))))

/* Tell the interpreter that special actions are needed as soon as a
   sequence point is reached.  */
static thread_local void *const *global_sync_barrier_bytecodes;
static thread_local void *const *global_monitored_bytecodes;
static thread_local void *const *global_normal_bytecodes;
// static thread_local void *const *dispatch_vec;

static void *const *dispatch_vec_per_thread[100];
static thread_local size_t current_thread_id = 0;

pthread_barrier_t interp_sync_barrier;

volatile _Atomic(size_t) _gst_interpret_thread_counter = 1;

void global_lock_for_gc(void) {
  for (size_t i = 0; i < atomic_load(&_gst_interpret_thread_counter); i++) {
    if (i == current_thread_id)
      continue;
    dispatch_vec_per_thread[i] = global_sync_barrier_bytecodes;
    __sync_synchronize();
  }
}


#define SET_EXCEPT_FLAG(x)                                                     \
  do {                                                                         \
    dispatch_vec_per_thread[current_thread_id] = (x) ? global_monitored_bytecodes : global_normal_bytecodes; \
    __sync_synchronize();                                                      \
  } while (0)

/* Answer an hash value for a send of the SENDSELECTOR message, when
   the CompiledMethod is found in class METHODCLASS.  */
#define METHOD_CACHE_HASH(sendSelector, methodClass)                           \
  ((((intptr_t)(sendSelector)) ^                                               \
    ((intptr_t)(methodClass)) / (2 * sizeof(PTR))) &                           \
   (METHOD_CACHE_SIZE - 1))

/* Context management

   The contexts make up a linked list.  Their structure is:

      +-----------------------------------+
      | parentContext			  |
      +-----------------------------------+	THESE ARE CONTEXT'S
      | misc. information		  |	FIXED INSTANCE VARIABLES
      | ...				  |
      +-----------------------------------+-------------------------------
      | args				  |
      | ...				  |	THESE ARE THE CONTEXT'S
      +-----------------------------------+	INDEXED INSTANCE VARIABLES
      | temps				  |
      | ...				  |
      +-----------------------------------+
      | stack				  |
      | ...				  |
      +-----------------------------------+

   The space labeled "misc. information" is initialized when
   thisContext is pushed or when the method becomes the parent context
   of a newly activated context.  It contains, among other things, the
   pointer to the CompiledMethod or CompiledBlock for the context.
   That's comparable to leaf procedure optimization in RISC
   processors.

   Contexts are special in that they are not created immediately in
   the main heap.  Instead they have three life phases:

   a) their OOPs are allocated on a stack, and their object data is
   allocated outside of the main heap.  This state lasts until the
   context returns (in which case the OOP can be reused) or until a
   reference to the context is made (in which case we swiftly move all
   the OOPs to the OOP table, leaving the object data outside the
   heap).

   b) their OOPs are allocated in the main OOP table, their object
   data still resides outside of the main heap.  Unlike the main heap,
   this area grows more slowly, but like the main heap, a GC is
   triggered when it's full.  Upon GC, most context objects (which are
   generated by `full' or `dirty' blocks) that could not be discarded
   when they were returned from are reclaimed, and the others are
   tenured, moving them to the main heap.

   c) their OOPs are allocated in the main OOP table, their object
   data stays in the main heap.  And in this state they will remain
   until they become garbage and are reclaimed.  */

/* I made CHUNK_SIZE a nice power of two.  Allocate 64KB at a time,
   never use more than 3 MB; anyway these are here so behavior can be
   fine tuned.  MAX_LIFO_DEPTH is enough to have room for an entire
   stack chunk and avoid testing for overflows in lifo_contexts.  */
#define CHUNK_SIZE 16384
#define MAX_CHUNKS_IN_MEMORY 48
#define MAX_LIFO_DEPTH (CHUNK_SIZE / CTX_SIZE(0))

/* CHUNK points to an item of CHUNKS.  CUR_CHUNK_BEGIN is equal
   to *CHUNK (i.e. points to the base of the current chunk) and
   CUR_CHUNK_END is equal to CUR_CHUNK_BEGIN + CHUNK_SIZE.  */
static gst_context_part cur_chunk_begin[100] = { NULL }, cur_chunk_end[100] = { NULL };
static gst_context_part chunks[100][MAX_CHUNKS_IN_MEMORY] CACHELINE_ALIGNED;
static gst_context_part *chunk[100];

/* These are used for OOP's allocated in a LIFO manner.  A context is
   kept on this stack as long as it generates only clean blocks, as
   long as it resides in the same chunk as the newest object created,
   and as long as no context switches happen since the time the
   process was created.  FREE_LIFO_CONTEXT points to just after the
   top of the stack.  */
static struct oop_s lifo_contexts[100][MAX_LIFO_DEPTH] CACHELINE_ALIGNED;
static OOP free_lifo_context[100];

/* Include `plug-in' modules for the appropriate interpreter.

   A plug-in must define
   - _gst_send_message_internal
   - _gst_send_method
   - send_block_value
   - _gst_interpret
   - GET_CONTEXT_IP
   - SET_THIS_METHOD
   - _gst_validate_method_cache_entries
   - any others that are needed by the particular implementation (e.g.
     lookup_native_ip for the JIT plugin)

   They are included rather than linked to for speed (they need access
   to lots of inlines and macros).  */

#include "prims.inl"

#include "interp-bc.inl"

void _gst_empty_context_pool(void) {
  if (*chunks[current_thread_id]) {
    chunk[current_thread_id] = chunks[current_thread_id];
    cur_chunk_begin[current_thread_id] = *chunk[current_thread_id];
    cur_chunk_end[current_thread_id] = (gst_context_part)(((char *)cur_chunk_begin[current_thread_id]) +
                                       SIZE_TO_BYTES(CHUNK_SIZE));
  } else {
    chunk[current_thread_id] = chunks[current_thread_id] - 1;
    cur_chunk_begin[current_thread_id] = cur_chunk_end[current_thread_id] = NULL;
  }
}

void empty_context_stack(void) {
  OOP contextOOP, last, oop;
  gst_object context;

  /* printf("[[[[ Gosh, not lifo anymore! (free = %p, base = %p)\n",
     free_lifo_context, lifo_contexts); */
  if COMMON (free_lifo_context[current_thread_id] != lifo_contexts[current_thread_id])
    for (free_lifo_context[current_thread_id] = contextOOP = lifo_contexts[current_thread_id],
        last = _gst_this_context_oop[current_thread_id], context = OOP_TO_OBJ(contextOOP);
         ;) {
      oop =
          alloc_oop(context, OOP_GET_FLAGS(contextOOP) | _gst_mem.active_flag);

      /* Fill the object's uninitialized fields. */
      OBJ_SET_CLASS(context, (intptr_t)OBJ_METHOD_CONTEXT_FLAGS(context) &
                                     MCF_IS_METHOD_CONTEXT
                                 ? _gst_method_context_class
                                 : _gst_block_context_class);

      /* This field is unused without the JIT compiler, but it must
         be initialized when a context becomes a fully formed
         Smalltalk object.  We do that here.  Note that we need the
         field so that the same image is usable with or without the
         JIT compiler.  */
      OBJ_METHOD_CONTEXT_SET_NATIVE_IP(context, DUMMY_NATIVE_IP);

      /* The last context is not referenced anywhere, so we're done
         with it.  */
      if (contextOOP++ == last) {
        _gst_this_context_oop[current_thread_id] = oop;
        break;
      }

      /* Else we redirect its sender field to the main OOP table */
      context = OOP_TO_OBJ(contextOOP);
      OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(context, oop);
    }
  else {
    if (IS_NIL(_gst_this_context_oop[current_thread_id]))
      return;

    context = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
  }

  /* When a context gets out of the context stack it must be a fully
     formed Smalltalk object.  These fields were left uninitialized in
     _gst_send_message_internal and send_block_value -- set them here.  */
  OBJ_METHOD_CONTEXT_SET_METHOD(context, _gst_this_method[current_thread_id]);
  OBJ_METHOD_CONTEXT_SET_RECEIVER(context, _gst_self[current_thread_id]);
  OBJ_METHOD_CONTEXT_SET_SP_OFFSET(
      context, FROM_INT(sp - OBJ_METHOD_CONTEXT_CONTEXT_STACK(context)));
  OBJ_METHOD_CONTEXT_SET_IP_OFFSET(context, FROM_INT(ip[current_thread_id] - method_base));

  /* Even if the JIT is active, the current context might have no
     attached native_ip -- in fact it has one only if we are being
     called from activate_new_context -- so we have to `invent'
     one. We test for a valid native_ip first, though; this test must
     have no false positives, i.e. it won't ever overwrite a valid
     native_ip, and won't leave a bogus OOP for the native_ip.  */
  if (!IS_INT(OBJ_METHOD_CONTEXT_NATIVE_IP(context)))
    OBJ_METHOD_CONTEXT_SET_NATIVE_IP(context, DUMMY_NATIVE_IP);
}

void alloc_new_chunk(void) {
  if UNCOMMON (++chunk[current_thread_id] >= &chunks[current_thread_id][MAX_CHUNKS_IN_MEMORY]) {
    /* No more chunks available - GC */
    _gst_scavenge();
    return;
  }

  empty_context_stack();

  /* Allocate memory only the first time we're using the chunk.
     _gst_empty_context_pool resets the status but doesn't free
     the memory.  */
  if UNCOMMON (!*chunk[current_thread_id])
    *chunk[current_thread_id] = (gst_context_part)xcalloc(1, SIZE_TO_BYTES(CHUNK_SIZE));

  cur_chunk_begin[current_thread_id] = *chunk[current_thread_id];
  cur_chunk_end[current_thread_id] =
      (gst_context_part)(((char *)cur_chunk_begin[current_thread_id]) + SIZE_TO_BYTES(CHUNK_SIZE));
}

gst_object alloc_stack_context(int size) {
  gst_object newContext;

  size = CTX_SIZE(size);
  for (;;) {
    newContext = (gst_object)cur_chunk_begin[current_thread_id];
    cur_chunk_begin[current_thread_id] += size;
    if COMMON (cur_chunk_begin[current_thread_id] < cur_chunk_end[current_thread_id]) {
      OBJ_SET_SIZE(newContext, FROM_INT(size));
      OBJ_SET_IDENTITY (newContext, FROM_INT(0));
      return (newContext);
    }

    /* Not enough room in the current chunk */
    alloc_new_chunk();
  }
}

gst_object activate_new_context(int size, int sendArgs) {
  OOP oop;
  gst_object newContext;
  gst_object thisContext;

#ifndef OPTIMIZE
  if (IS_NIL(_gst_this_context_oop[current_thread_id])) {
    printf("Somebody forgot _gst_prepare_execution_environment!\n");
    abort();
  }
#endif

  /* We cannot overflow lifo_contexts, because it is designed to
     contain all of the contexts in a chunk, and we empty lifo_contexts
     when we exhaust a chunk.  So we can get the oop the easy way.  */
  newContext = alloc_stack_context(size);
  oop = free_lifo_context[current_thread_id]++;

  /* printf("[[[[ Context (size %d) allocated at %p (oop = %p)\n",
     size, newContext, oop); */
  OOP_SET_OBJECT(oop, newContext);

  OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(newContext, _gst_this_context_oop[current_thread_id]);

  /* save old context information */
  /* leave sp pointing to receiver, which is replaced on return with
     value */
  thisContext = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
  OBJ_METHOD_CONTEXT_SET_METHOD(thisContext, _gst_this_method[current_thread_id]);
  OBJ_METHOD_CONTEXT_SET_RECEIVER(thisContext, _gst_self[current_thread_id]);
  OBJ_METHOD_CONTEXT_SET_SP_OFFSET(
      thisContext,
      FROM_INT((sp - OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext)) -
               sendArgs));
  OBJ_METHOD_CONTEXT_SET_IP_OFFSET(thisContext, FROM_INT(ip[current_thread_id] - method_base));

  _gst_this_context_oop[current_thread_id] = oop;

  return (newContext);
}

void dealloc_stack_context(gst_context_part context) {
#ifndef OPTIMIZE
  if (free_lifo_context[current_thread_id] == lifo_contexts[current_thread_id] ||
      (OOP_TO_OBJ(free_lifo_context[current_thread_id] - 1) != (gst_object)context)) {
    _gst_errorf("Deallocating a non-LIFO context!!!");
    abort();
  }
#endif

  cur_chunk_begin[current_thread_id] = context;
  free_lifo_context[current_thread_id]--;
}

void prepare_context(gst_context_part context, int args, int temps) {
  REGISTER(1, OOP * stackBase);
  _gst_temporaries[current_thread_id] = stackBase =
      OBJ_METHOD_CONTEXT_CONTEXT_STACK((gst_object)context);
  if (args) {
    REGISTER(2, OOP * src);
    src = &sp[1 - args];
    stackBase[0] = src[0];
    if (args > 1) {
      stackBase[1] = src[1];
      if (args > 2) {
        stackBase[2] = src[2];
        if (args > 3)
          memcpy(&stackBase[3], &src[3], (args - 3) * sizeof(OOP));
      }
    }
    stackBase += args;
  }
  if (temps) {
    REGISTER(2, OOP src);
    src = _gst_nil_oop;
    stackBase[0] = src;
    if (temps > 1) {
      stackBase[1] = src;
      if (temps > 2) {
        int n = 2;
        do
          stackBase[n] = src;
        while UNCOMMON(n++ < temps);
      }
    }
    stackBase += temps;
  }
  sp = stackBase - 1;
}

mst_Boolean _gst_send_cannot_interpret_message(OOP sendSelector, method_cache_entry *methodData,
                          int sendArgs, OOP method_class) {
  inc_ptr inc;
  OOP argsArrayOOP;
  gst_object messageLookup;
  OOP messageLookupOOP;

  inc = INC_SAVE_POINTER();
  argsArrayOOP = create_args_array(sendArgs);
  INC_ADD_OOP(argsArrayOOP);

  messageLookup = new_instance(_gst_message_lookup_class,
                               &messageLookupOOP);

  OBJ_MESSAGE_LOOKUP_SET_SELECTOR(messageLookup, sendSelector);
  OBJ_MESSAGE_LOOKUP_SET_ARGS(messageLookup, argsArrayOOP);
  OBJ_MESSAGE_LOOKUP_SET_STARTING_CLASS(messageLookup, method_class);
  PUSH_OOP(messageLookupOOP);
  INC_RESTORE_POINTER(inc);

  return _gst_find_method(method_class, _gst_cannot_interpret_symbol, methodData);
}

mst_Boolean lookup_method(OOP sendSelector, method_cache_entry *methodData,
                          int sendArgs, OOP method_class) {
  inc_ptr inc;
  OOP argsArrayOOP;
  gst_object messageLookup;
  OOP messageLookupOOP;

  if (_gst_find_method(method_class, sendSelector, methodData))
    return (true);

  inc = INC_SAVE_POINTER();
  argsArrayOOP = create_args_array(sendArgs);
  INC_ADD_OOP(argsArrayOOP);

  messageLookup = new_instance(_gst_message_lookup_class,
                                                   &messageLookupOOP);

  OBJ_MESSAGE_LOOKUP_SET_SELECTOR(messageLookup, sendSelector);
  OBJ_MESSAGE_LOOKUP_SET_ARGS(messageLookup, argsArrayOOP);
  OBJ_MESSAGE_LOOKUP_SET_STARTING_CLASS(messageLookup, method_class);
  PUSH_OOP(messageLookupOOP);
  INC_RESTORE_POINTER(inc);

  return (false);
}

mst_Boolean _gst_find_method(OOP receiverClass, OOP sendSelector,
                             method_cache_entry *methodData) {
  OOP method_class = receiverClass;
  for (; !IS_NIL(method_class); method_class = SUPERCLASS(method_class)) {
    OOP methodOOP = _gst_find_class_method(method_class, sendSelector);
    if (!IS_NIL(methodOOP)) {
      methodData->startingClassOOP = receiverClass;
      methodData->selectorOOP = sendSelector;
      methodData->methodOOP = methodOOP;
      methodData->methodClassOOP = method_class;
      methodData->methodHeader = GET_METHOD_HEADER(methodOOP);
      _gst_cache_misses++;
      return (true);
    }
  }

  return (false);
}

OOP create_args_array(int numArgs) {
  gst_object argsArray;
  OOP argsArrayOOP;
  int i;

  argsArray = new_instance_with(_gst_array_class, numArgs, &argsArrayOOP);
  for (i = 0; i < numArgs; i++)
    argsArray->data[i] = STACK_AT(numArgs - i - 1);

  POP_N_OOPS(numArgs);
  return argsArrayOOP;
}

mst_Boolean check_send_correctness(OOP receiver, OOP sendSelector,
                                   int numArgs) {
  int hashIndex;
  method_cache_entry *methodData;
  OOP receiverClass;

  receiverClass = OOP_INT_CLASS(receiver);
  hashIndex = METHOD_CACHE_HASH(sendSelector, receiverClass);
  methodData = &method_cache[hashIndex];

  if (methodData->selectorOOP != sendSelector ||
      methodData->startingClassOOP != receiverClass) {
    /* If we do not find the method, don't worry and fire
       #doesNotUnderstand:  */
    if (!_gst_find_method(receiverClass, sendSelector, methodData))
      return (true);

    methodData = &method_cache[hashIndex];
  }

  return (methodData->methodHeader.numArgs == numArgs);
}

void unwind_context(void) {
  gst_object oldContext, newContext;
  OOP newContextOOP;

  newContextOOP = _gst_this_context_oop[current_thread_id];
  newContext = OOP_TO_OBJ(newContextOOP);

  do {
    oldContext = newContext;

    /* Descend in the chain...  */
    newContextOOP = OBJ_METHOD_CONTEXT_PARENT_CONTEXT(oldContext);

    if COMMON (free_lifo_context[current_thread_id] > lifo_contexts[current_thread_id])
      dealloc_stack_context((gst_context_part)oldContext);

    /* This context cannot be deallocated in a LIFO way.  We must
       keep it around so that the blocks it created can reference
       arguments and temporaries in it. Method contexts, however,
       need to be marked as non-returnable so that attempts to
       return from them to an undefined place will lose; doing
       that for block contexts too, we skip a test and are also
       able to garbage collect more context objects.  And doing
       that for _all_ method contexts is more icache-friendly.  */
    OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(oldContext, _gst_nil_oop);

    newContext = OOP_TO_OBJ(newContextOOP);
  } while UNCOMMON((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) ==
                   (MCF_IS_METHOD_CONTEXT | MCF_IS_DISABLED_CONTEXT));

  /* Clear the bit so that we return here just once.
     This makes this absurd snippet work:

        ^[ [ 12 ] ensure: [ ^34 ] ] ensure: [ 56 ]!

     If it were not for this statement, the inner #ensure:
     would resume after the ^34 block exited, and would answer
     12 (the result of the evaluation of the receiver of the
     inner #ensure:).

     HACK ALERT!!  This is actually valid only for method contexts
     but I carefully put the modified bits in the low bits so that
     they are already zero for block contexts.  */
  OBJ_METHOD_CONTEXT_SET_FLAGS(
      newContext, (OOP)((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) &
                        (~(MCF_IS_DISABLED_CONTEXT | MCF_IS_UNWIND_CONTEXT))));

  _gst_this_context_oop[current_thread_id] = newContextOOP;
  _gst_temporaries[current_thread_id] = OBJ_METHOD_CONTEXT_CONTEXT_STACK(newContext);
  sp = OBJ_METHOD_CONTEXT_CONTEXT_STACK(newContext) +
       TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(newContext));
  _gst_self[current_thread_id] = OBJ_METHOD_CONTEXT_RECEIVER(newContext);

  SET_THIS_METHOD(OBJ_METHOD_CONTEXT_METHOD(newContext),
                  GET_CONTEXT_IP(newContext));
}

mst_Boolean unwind_method(void) {
  OOP oldContextOOP, newContextOOP;
  gst_object newBlockContext;

  /* We're executing in a block context and an explicit return is
     encountered.  This means that we are to return from the caller of
     the method that created the block context, no matter how many
     levels of message sending are between where we currently are and
     our parent method context.  */

  newBlockContext = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
  do {
    newContextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(newBlockContext);
    newBlockContext = OOP_TO_OBJ(newContextOOP);
  } while UNCOMMON(!((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newBlockContext) &
                     MCF_IS_METHOD_CONTEXT));

  /* test for block return in a dead method */
  if UNCOMMON (IS_NIL(OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newBlockContext))) {
    /* We are to create a reference to thisContext, so empty the
       stack.  */
    empty_context_stack();
    oldContextOOP = _gst_this_context_oop[current_thread_id];

    /* Just unwind to the caller, and prepare to send a message to
       the context */
    unwind_context();
    SET_STACKTOP(oldContextOOP);

    return (false);
  }

  return unwind_to(OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newBlockContext));
}

mst_Boolean unwind_to(OOP returnContextOOP) {
  OOP oldContextOOP, newContextOOP;
  gst_object oldContext, newContext;

  empty_context_stack();

  newContextOOP = _gst_this_context_oop[current_thread_id];
  newContext = OOP_TO_OBJ(newContextOOP);

  while (newContextOOP != returnContextOOP) {
    oldContextOOP = newContextOOP;
    oldContext = newContext;

    /* Descend in the chain...  */
    newContextOOP = OBJ_METHOD_CONTEXT_PARENT_CONTEXT((gst_object)oldContext);
    newContext = OOP_TO_OBJ(newContextOOP);

    /* Check if we got to an unwinding context (#ensure:).  */
    if UNCOMMON ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) &
                 MCF_IS_UNWIND_CONTEXT) {
      mst_Boolean result;
      _gst_this_context_oop[current_thread_id] = oldContextOOP;

      /* _gst_this_context_oop is the context above the
         one we return to.   We only unwind up to the #ensure:
         context.  */
      result = disable_non_unwind_contexts(returnContextOOP);

      unwind_context();
      return result;
    }

    /* This context cannot be deallocated in a LIFO way.  We must
       keep it around so that the blocks it created can reference
       arguments and temporaries in it. Method contexts, however,
       need to be marked as non-returnable so that attempts to
       return from them to an undefined place will lose; doing
       that for block contexts too, we skip a test and are also
       able to garbage collect more context objects.  */
    OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(oldContext, _gst_nil_oop);
  }

  /* Clear the bit so that we return here just once.
     This makes this absurd snippet work:

        ^[ [ 12 ] ensure: [ ^34 ] ] ensure: [ 56 ]!

     If it were not for this statement, the inner #ensure:
     would resume after the ^34 block exited, and would answer
     12 (the result of the evaluation of the receiver of the
     inner #ensure:).

     HACK ALERT!!  This is actually valid only for method contexts
     but I carefully put the modified bits in the low bits so that
     they are already zero for block contexts.  */
  OBJ_METHOD_CONTEXT_SET_FLAGS(
      newContext, (OOP)(((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext)) &
                        (~(MCF_IS_DISABLED_CONTEXT | MCF_IS_UNWIND_CONTEXT))));

  _gst_this_context_oop[current_thread_id] = newContextOOP;
  _gst_temporaries[current_thread_id] = OBJ_METHOD_CONTEXT_CONTEXT_STACK(newContext);
  sp = OBJ_METHOD_CONTEXT_CONTEXT_STACK(newContext) +
       TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(newContext));
  _gst_self[current_thread_id] = OBJ_METHOD_CONTEXT_RECEIVER(newContext);

  SET_THIS_METHOD(OBJ_METHOD_CONTEXT_METHOD(newContext),
                  GET_CONTEXT_IP(newContext));
  return (true);
}

mst_Boolean disable_non_unwind_contexts(OOP returnContextOOP) {
  OOP newContextOOP, *chain;
  gst_object oldContext, newContext;

  newContextOOP = _gst_this_context_oop[current_thread_id];
  newContext = OOP_TO_OBJ(newContextOOP);
  chain = &OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newContext);

  for (;;) {
    oldContext = newContext;

    /* Descend in the chain...  */
    newContextOOP = OBJ_METHOD_CONTEXT_PARENT_CONTEXT(oldContext);
    newContext = OOP_TO_OBJ(newContextOOP);

    if (!((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(oldContext) &
          MCF_IS_METHOD_CONTEXT)) {
      /* This context cannot be deallocated in a LIFO way.  Setting
         its parent context field to nil makes us able to garbage
         collect more context objects.  */
      OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(oldContext, _gst_nil_oop);
    }

    if (IS_NIL(newContextOOP)) {
      *chain = newContextOOP;
      return (false);
    }

    if (newContextOOP == returnContextOOP) {
      *chain = newContextOOP;
      chain = &OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newContext);
      break;
    }

    if ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) &
        MCF_IS_METHOD_CONTEXT) {
      OBJ_METHOD_CONTEXT_SET_FLAGS(
          newContext, (OOP)(((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) |
                             MCF_IS_DISABLED_CONTEXT)));
      *chain = newContextOOP;
      chain = &OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newContext);
    }
  }

  /* Skip any disabled methods.  */
  while
    UNCOMMON((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(newContext) ==
             (MCF_IS_METHOD_CONTEXT | MCF_IS_DISABLED_CONTEXT)) {
      oldContext = newContext;

      /* Descend in the chain...  */
      newContextOOP = OBJ_METHOD_CONTEXT_PARENT_CONTEXT(oldContext);
      if (IS_NIL(newContextOOP)) {
        *chain = _gst_nil_oop;
        return false;
      }

      newContext = OOP_TO_OBJ(newContextOOP);

      /* This context cannot be deallocated in a LIFO way.  We must
         keep it around so that the blocks it created can reference
         arguments and temporaries in it. Method contexts, however,
         need to be marked as non-returnable so that attempts to
         return from them to an undefined place will lose; doing
         that for block contexts too, we skip a test and are also
         able to garbage collect more context objects.  */
      OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(oldContext, _gst_nil_oop);
    }

  *chain = OBJ_METHOD_CONTEXT_PARENT_CONTEXT(newContext);
  return (true);
}

OOP _gst_make_block_closure(OOP blockOOP) {
  gst_block_closure closure;
  gst_compiled_block block;
  OOP closureOOP;

  closure =
      (gst_block_closure)new_instance(_gst_block_closure_class, &closureOOP);

  /* Check how clean the block is: if it only accesses self,
     we can afford not moving the context chain to the heap
     and setting the outerContext to nil.  */
  block = (gst_compiled_block)OOP_TO_OBJ(blockOOP);

  if (block->header.clean > 1) {
    empty_context_stack();
    closure->outerContext = _gst_this_context_oop[current_thread_id];
  } else
    closure->outerContext = _gst_nil_oop;

  closure->block = blockOOP;
  closure->receiver = _gst_self[current_thread_id];
  return (closureOOP);
}

void change_process_context(OOP newProcess) {
  OOP processOOP;
  gst_object process;
  gst_object processor;
  mst_Boolean enable_async_queue;

  switch_to_process = _gst_nil_oop;

  /* save old context information */
  if (!IS_NIL(_gst_this_context_oop[current_thread_id])) {
    empty_context_stack();
  }

  /* printf("Switching to process %#O at priority %#O\n",
    ((gst_process) OOP_TO_OBJ (newProcess))->name,
    ((gst_process) OOP_TO_OBJ (newProcess))->priority); */

  processor = OOP_TO_OBJ(_gst_processor_oop);
  processOOP = OBJ_PROCESSOR_SCHEDULER_GET_ACTIVE_PROCESS(processor);
  if (processOOP != newProcess) {
    process = OOP_TO_OBJ(processOOP);

    if (!IS_NIL(processOOP) && !is_process_terminating(processOOP))
      OBJ_PROCESS_SET_SUSPENDED_CONTEXT(process, _gst_this_context_oop[current_thread_id]);

    OBJ_PROCESSOR_SCHEDULER_SET_ACTIVE_PROCESS(processor, newProcess);
    process = OOP_TO_OBJ(newProcess);
    enable_async_queue =
        IS_NIL(OBJ_PROCESS_GET_INTERRUPTS(process)) || TO_INT(OBJ_PROCESS_GET_INTERRUPTS(process)) >= 0;

    resume_suspended_context(OBJ_PROCESS_GET_SUSPENDED_CONTEXT(process));

    /* Interrupt-enabling cannot be controlled globally from Smalltalk,
       but only on a per-Process basis.  You might think that this leaves
       much to be desired, because you could actually reenter a Process
       with interrupts disabled, if it yields control to another which
       has interrupts enabled.  In principle, this is true, but consider
       that when interrupts are disabled you can yield in three ways only:
       - by doing Process>>#suspend -- and then your process will not
         be scheduled
       - by doing ProcessorScheduler>>#yield -- and then I'll tell you that
         I gave you enough rope to shoot yourself on your feet, and that's
         what you did
       - by doing Semaphore>>#wait -- and then most likely your blocking
         section has terminated (see RecursionLock>>#critical: for an
         example).  */

    async_queue_enabled = enable_async_queue;
  }
}

void resume_suspended_context(OOP oop) {
  gst_object thisContext;

  _gst_this_context_oop[current_thread_id] = oop;
  thisContext = OOP_TO_OBJ(oop);
  sp = OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext) +
       TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(thisContext));
  SET_THIS_METHOD(OBJ_METHOD_CONTEXT_METHOD(thisContext),
                  GET_CONTEXT_IP(thisContext));
  _gst_temporaries[current_thread_id] = OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext);
  _gst_self[current_thread_id] = OBJ_METHOD_CONTEXT_RECEIVER(thisContext);
  free_lifo_context[current_thread_id] = lifo_contexts[current_thread_id];
}

OOP get_active_process(void) {
  if (!IS_NIL(switch_to_process))
    return (switch_to_process);
  else
    return (get_scheduled_process());
}

OOP get_scheduled_process(void) {
  gst_object processor;

  processor = OOP_TO_OBJ(_gst_processor_oop);

  return (OBJ_PROCESSOR_SCHEDULER_GET_ACTIVE_PROCESS(processor));
}

static void remove_process_from_list(OOP processOOP) {
  gst_object sem;
  gst_object process, lastProcess;
  OOP lastProcessOOP;

  if (IS_NIL(processOOP))
    return;

  process = OOP_TO_OBJ(processOOP);
  if (!IS_NIL(OBJ_PROCESS_GET_MY_LIST(process))) {
    /* Disconnect the process from its list.  */
    sem = OOP_TO_OBJ(OBJ_PROCESS_GET_MY_LIST(process));
    if (OBJ_SEMAPHORE_GET_FIRST_LINK(sem) == processOOP) {
      OBJ_SEMAPHORE_SET_FIRST_LINK(sem, OBJ_PROCESS_GET_NEXT_LINK(process));
      if (OBJ_SEMAPHORE_GET_LAST_LINK(sem) == processOOP)
        /* It was the only process in the list */
        OBJ_SEMAPHORE_SET_LAST_LINK(sem, _gst_nil_oop);
    } else {
      /* Find the new prev node */
      lastProcessOOP = OBJ_SEMAPHORE_GET_FIRST_LINK(sem);
      lastProcess = OOP_TO_OBJ(lastProcessOOP);
      while (OBJ_PROCESS_GET_NEXT_LINK(lastProcess) != processOOP) {
        lastProcessOOP = OBJ_PROCESS_GET_NEXT_LINK(lastProcess);
        lastProcess = OOP_TO_OBJ(lastProcessOOP);
      }

      OBJ_PROCESS_SET_NEXT_LINK(lastProcess, OBJ_PROCESS_GET_NEXT_LINK(process));
      if (OBJ_SEMAPHORE_GET_LAST_LINK(sem) == processOOP)
        OBJ_SEMAPHORE_SET_LAST_LINK(sem, lastProcessOOP);
    }

    OBJ_PROCESS_SET_MY_LIST(process, _gst_nil_oop);
  }

  OBJ_PROCESS_SET_NEXT_LINK(process, _gst_nil_oop);
}

void add_first_link(OOP semaphoreOOP, OOP processOOP) {
  gst_object sem;
  gst_object process;

  process = OOP_TO_OBJ(processOOP);
  remove_process_from_list(processOOP);

  sem = OOP_TO_OBJ(semaphoreOOP);
  OBJ_PROCESS_SET_MY_LIST(process, semaphoreOOP);
  OBJ_PROCESS_SET_NEXT_LINK(process, OBJ_SEMAPHORE_GET_FIRST_LINK(sem));

  OBJ_SEMAPHORE_SET_FIRST_LINK(sem, processOOP);
  if (IS_NIL(OBJ_SEMAPHORE_GET_LAST_LINK(sem)))
    OBJ_SEMAPHORE_SET_LAST_LINK(sem, processOOP);
}

void suspend_process(OOP processOOP) {
  remove_process_from_list(processOOP);
  if (get_scheduled_process() == processOOP)
    ACTIVE_PROCESS_YIELD();
}

void _gst_terminate_process(OOP processOOP) {
  gst_object process;

  process = OOP_TO_OBJ(processOOP);
  OBJ_PROCESS_SET_SUSPENDED_CONTEXT(process, _gst_nil_oop);
  suspend_process(processOOP);
}

void add_last_link(OOP semaphoreOOP, OOP processOOP) {
  gst_object sem;
  gst_object process, lastProcess;
  OOP lastProcessOOP;

  process = OOP_TO_OBJ(processOOP);
  remove_process_from_list(processOOP);

  sem = OOP_TO_OBJ(semaphoreOOP);
  OBJ_PROCESS_SET_MY_LIST(process, semaphoreOOP);
  OBJ_PROCESS_SET_NEXT_LINK(process, _gst_nil_oop);

  if (IS_NIL(OBJ_SEMAPHORE_GET_LAST_LINK(sem))) {
    OBJ_SEMAPHORE_SET_FIRST_LINK(sem, processOOP);
    OBJ_SEMAPHORE_SET_LAST_LINK(sem, processOOP);
  } else {
    lastProcessOOP = OBJ_SEMAPHORE_GET_LAST_LINK(sem);
    lastProcess = OOP_TO_OBJ(lastProcessOOP);
    OBJ_PROCESS_SET_NEXT_LINK(lastProcess, processOOP);
    OBJ_SEMAPHORE_SET_LAST_LINK(sem, processOOP);
  }
}

mst_Boolean is_empty(OOP processListOOP) {
  gst_object processList;

  processList = OOP_TO_OBJ(processListOOP);
  return (IS_NIL(OBJ_SEMAPHORE_GET_FIRST_LINK(processList)));
}

/* TODO: this was taken from VMpr_Processor_yield.  Try to use
   the macro ACTIVE_PROCESS_YIELD instead?  */

void active_process_yield(void) {
  OOP activeProcess = get_active_process();
  OOP newProcess = highest_priority_process();

  if (is_process_ready(activeProcess))
    sleep_process(activeProcess); /* move to the end of the list */

  activate_process(IS_NIL(newProcess) ? activeProcess : newProcess);
}

mst_Boolean _gst_sync_signal(OOP semaphoreOOP, mst_Boolean incr_if_empty) {
  gst_object sem;
  gst_object process;
  gst_object suspendedContext;
  OOP processOOP;
  int spOffset;

  sem = OOP_TO_OBJ(semaphoreOOP);
  do {
    /* printf ("signal %O %O\n", semaphoreOOP, sem->firstLink); */
    if (is_empty(semaphoreOOP)) {
      if (incr_if_empty)
        OBJ_SEMAPHORE_SET_SIGNALS(sem,
                                  INCR_INT(OBJ_SEMAPHORE_GET_SIGNALS(sem)));
      return false;
    }

    processOOP = remove_first_link(semaphoreOOP);

    /* If they terminated this process, well, try another */
  } while (!resume_process(processOOP, false));

  /* Put the semaphore at the stack top as a marker that the
     wait was not interrupted.  This assumes that _gst_sync_wait
     is only called from primitives.  */
  process = OOP_TO_OBJ(processOOP);
  suspendedContext = OOP_TO_OBJ(OBJ_PROCESS_GET_SUSPENDED_CONTEXT(process));
  spOffset = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(suspendedContext));
  OBJ_METHOD_CONTEXT_CONTEXT_STACK_AT_PUT(suspendedContext, spOffset,
                                          semaphoreOOP);
  return true;
}

void _gst_do_async_signal(OOP semaphoreOOP) {
  _gst_sync_signal(semaphoreOOP, true);
}

void _gst_do_async_signal_and_unregister(OOP semaphoreOOP) {
  _gst_sync_signal(semaphoreOOP, true);
  _gst_unregister_oop(semaphoreOOP);
}

/* Async-signal-safe version, does no allocation.  Using an atomic operation
   is still the simplest choice, but on top of that we check that the entry
   is not already in the list.  Also, the datum and next field are NULLed
   automatically when the call is made.  */
void _gst_async_call_internal(async_queue_entry *e) {
  /* For async-signal safety, we need to check that the entry is not
     already in the list.  Checking that atomically with CAS is the
     simplest way.  */
  do
    if (__sync_val_compare_and_swap(&e->next, NULL, queued_async_signals_sig))
      return;
  while (!__sync_bool_compare_and_swap(&queued_async_signals_sig, e->next, e));
  SET_EXCEPT_FLAG(true);
}

void _gst_async_call(void (*func)(OOP), OOP arg) {
  /* Thread-safe version for the masses.  This lockless stack
     is reversed in the interpreter loop to get FIFO behavior.  */
  async_queue_entry *sig = xmalloc(sizeof(async_queue_entry));
  sig->func = func;
  sig->data = arg;

  do
    sig->next = queued_async_signals;
  while (!__sync_bool_compare_and_swap(&queued_async_signals, sig->next, sig));
  _gst_wakeup();
  SET_EXCEPT_FLAG(true);
}

mst_Boolean _gst_have_pending_async_calls() {
  return (queued_async_signals != &queued_async_signals_tail ||
          queued_async_signals_sig != &queued_async_signals_tail);
}

void empty_async_queue() {
  async_queue_entry *sig, *sig_reversed;

  /* Process a batch of asynchronous requests.  These are pushed
     in LIFO order by _gst_async_call.  By reversing the list
     in place before walking it, we get FIFO order.  */
  sig = __sync_swap(&queued_async_signals, &queued_async_signals_tail);
  sig_reversed = &queued_async_signals_tail;
  while (sig != &queued_async_signals_tail) {
    async_queue_entry *next = sig->next;
    sig->next = sig_reversed;
    sig_reversed = sig;
    sig = next;
  }

  sig = sig_reversed;
  while (sig != &queued_async_signals_tail) {
    async_queue_entry *next = sig->next;
    sig->func(sig->data);
    free(sig);
    sig = next;
  }

  /* For async-signal-safe processing, we need to avoid entering
     the same item twice into the list.  So we use NEXT to mark
     items that have been added...  */
  sig = __sync_swap(&queued_async_signals_sig, &queued_async_signals_tail);
  sig_reversed = &queued_async_signals_tail;
  while (sig != &queued_async_signals_tail) {
    async_queue_entry *next = sig->next;
    sig->next = sig_reversed;
    sig_reversed = sig;
    sig = next;
  }

  sig = sig_reversed;
  while (sig != &queued_async_signals_tail) {
    async_queue_entry *next = sig->next;
    void (*func)(OOP) = sig->func;
    OOP data = sig->data;
    barrier();

    sig->data = NULL;
    barrier();

    /* ... and we only NULL it after a signal handler can start
       writing to it.  */
    sig->next = NULL;
    barrier();
    func(data);
    sig = next;
  }
}

void _gst_async_signal(OOP semaphoreOOP) {
  _gst_async_call(_gst_do_async_signal, semaphoreOOP);
}

void _gst_async_signal_and_unregister(OOP semaphoreOOP) {
  _gst_async_call(_gst_do_async_signal_and_unregister, semaphoreOOP);
}

void sync_wait_process(OOP semaphoreOOP, OOP processOOP) {
  gst_object sem;
  mst_Boolean isActive;

  sem = OOP_TO_OBJ(semaphoreOOP);
  if (IS_NIL(processOOP)) {
    processOOP = get_active_process();
    isActive = true;
  } else
    isActive = (processOOP == get_active_process());

  if (TO_INT(OBJ_SEMAPHORE_GET_SIGNALS(sem)) <= 0) {
    /* Have to suspend.  Prepare return value for #wait and move
       this process to the end of the list.

       Tweaking the stack top means that this function should only
       be called from a primitive.  */
    SET_STACKTOP(_gst_nil_oop);
    remove_process_from_list(processOOP);
    add_last_link(semaphoreOOP, processOOP);
    if (isActive && IS_NIL(ACTIVE_PROCESS_YIELD())) {
      printf("No runnable process");
      activate_process(_gst_prepare_execution_environment());
    }
  } else
    OBJ_SEMAPHORE_SET_SIGNALS(sem, DECR_INT(OBJ_SEMAPHORE_GET_SIGNALS(sem)));

  /* printf ("wait %O %O\n", semaphoreOOP, sem->firstLink); */
}

void _gst_sync_wait(OOP semaphoreOOP) {
  sync_wait_process(semaphoreOOP, _gst_nil_oop);
}

OOP remove_first_link(OOP semaphoreOOP) {
  gst_object sem;
  gst_object process;
  OOP processOOP;

  sem = OOP_TO_OBJ(semaphoreOOP);
  processOOP = OBJ_SEMAPHORE_GET_FIRST_LINK(sem);
  process = OOP_TO_OBJ(processOOP);

  sem = OOP_TO_OBJ(semaphoreOOP);
  OBJ_SEMAPHORE_SET_FIRST_LINK(sem, OBJ_PROCESS_GET_NEXT_LINK(process));
  if (IS_NIL(OBJ_SEMAPHORE_GET_FIRST_LINK(sem)))
    OBJ_SEMAPHORE_SET_LAST_LINK(sem, _gst_nil_oop);

  /* Unlink the process from any list it was in! */
  OBJ_PROCESS_SET_MY_LIST(process, _gst_nil_oop);
  OBJ_PROCESS_SET_NEXT_LINK(process, _gst_nil_oop);
  return (processOOP);
}

mst_Boolean resume_process(OOP processOOP, mst_Boolean alwaysPreempt) {
  int priority;
  OOP activeOOP;
  OOP processLists;
  OOP processList;
  gst_object process, active;
  /* mst_Boolean ints_enabled; */

  /* 2002-19-12: tried get_active_process instead of get_scheduled_process.  */
  activeOOP = get_active_process();
  active = OOP_TO_OBJ(activeOOP);
  process = OOP_TO_OBJ(processOOP);
  priority = TO_INT(OBJ_PROCESS_GET_PRIORITY(process));

  /* As a special exception, don't preempt a process that has disabled
     interrupts. ### this behavior is currently disabled.
  ints_enabled = IS_NIL (active->interrupts)
                 || TO_INT(active->interrupts) <= 0; */

  /* resume_process is also used when changing the priority of a ready/active
     process.  In this case, first remove the process from its current list.  */
  if (processOOP == activeOOP) {
    assert(!alwaysPreempt);
    remove_process_from_list(processOOP);
  } else if (priority >= TO_INT(OBJ_PROCESS_GET_PRIORITY(active)) /* && ints_enabled */)
    alwaysPreempt = true;

  if (IS_NIL(processOOP) || is_process_terminating(processOOP))
    /* The process was terminated - nothing to resume, fail */
    return (false);

  /* We have no active process, activate this guy instantly.  */
  if (IS_NIL(activeOOP)) {
    activate_process(processOOP);
    return (true);
  }

  processLists = GET_PROCESS_LISTS();
  processList = ARRAY_AT(processLists, priority);

  if (alwaysPreempt) {
    /* We're resuming a process with a *equal or higher* priority, so sleep
       the current one and activate the new one */
    if (OBJ_PROCESS_GET_MY_LIST(active) != _gst_nil_oop)
      sleep_process(activeOOP);
    activate_process(processOOP);
  } else {
    /* this process has a lower priority than the active one, so the
       policy is that it doesn't preempt the currently running one.
       Anyway, it must be the first in its priority queue - so don't
       put it to sleep.  */
    add_first_link(processList, processOOP);
  }

  return (true);
}

OOP activate_process(OOP processOOP) {
  gst_object process;
  int priority;
  OOP processLists;
  OOP processList;

  if (IS_NIL(processOOP))
    return processOOP;

  /* 2002-19-12: tried get_active_process instead of get_scheduled_process.  */
  if (processOOP != get_active_process()) {
    process = OOP_TO_OBJ(processOOP);
    priority = TO_INT(OBJ_PROCESS_GET_PRIORITY(process));
    processLists = GET_PROCESS_LISTS();
    processList = ARRAY_AT(processLists, priority);
    add_first_link(processList, processOOP);
  }

  SET_EXCEPT_FLAG(true);
  switch_to_process = processOOP;
  return processOOP;
}

#ifdef ENABLE_PREEMPTION
RETSIGTYPE
preempt_smalltalk_process(int sig) {
  time_to_preempt = true;
  SET_EXCEPT_FLAG(true);
}
#endif

mst_Boolean is_process_terminating(OOP processOOP) {
  gst_object process;

  process = OOP_TO_OBJ(processOOP);
  return (IS_NIL(OBJ_PROCESS_GET_SUSPENDED_CONTEXT(process)));
}

mst_Boolean is_process_ready(OOP processOOP) {
  gst_object process;
  int priority;
  OOP processLists;
  OOP processList;

  process = OOP_TO_OBJ(processOOP);
  priority = TO_INT(OBJ_PROCESS_GET_PRIORITY(process));
  processLists = GET_PROCESS_LISTS();
  processList = ARRAY_AT(processLists, priority);

  /* check if process is in the priority queue */
  return (OBJ_PROCESS_GET_MY_LIST(process) == processList);
}

void sleep_process(OOP processOOP) {
  gst_object process;
  int priority;
  OOP processLists;
  OOP processList;

  process = OOP_TO_OBJ(processOOP);
  priority = TO_INT(OBJ_PROCESS_GET_PRIORITY(process));
  processLists = GET_PROCESS_LISTS();
  processList = ARRAY_AT(processLists, priority);

  /* add process to end of priority queue */
  add_last_link(processList, processOOP);
}

mst_Boolean would_reschedule_process() {
  OOP processLists, processListOOP;
  int priority, activePriority;
  OOP processOOP;
  gst_object process;
  gst_object processList;

  if (!IS_NIL(switch_to_process))
    return false;

  processOOP = get_scheduled_process();
  process = OOP_TO_OBJ(processOOP);
  activePriority = TO_INT(OBJ_PROCESS_GET_PRIORITY(process));
  processLists = GET_PROCESS_LISTS();
  priority = NUM_OOPS(OOP_TO_OBJ(processLists));
  do {
    assert(priority > 0);
    processListOOP = ARRAY_AT(processLists, priority);
  } while (is_empty(processListOOP) && --priority >= activePriority);

  processList = OOP_TO_OBJ(processListOOP);
  return (priority < activePriority ||
          (priority == activePriority
           /* If the same priority, check if the list has the
              current process as the sole element.  */
           && OBJ_SEMAPHORE_GET_FIRST_LINK(processList) ==
                  OBJ_SEMAPHORE_GET_LAST_LINK(processList) &&
           OBJ_SEMAPHORE_GET_FIRST_LINK(processList) == processOOP));
}

OOP highest_priority_process(void) {
  OOP processLists, processListOOP;
  int priority;
  OOP processOOP;
  gst_object processList;

  processLists = GET_PROCESS_LISTS();
  priority = NUM_OOPS(OOP_TO_OBJ(processLists));
  for (; priority > 0; priority--) {
    processListOOP = ARRAY_AT(processLists, priority);
    if (!is_empty(processListOOP)) {
      processOOP = remove_first_link(processListOOP);
      if (processOOP == get_scheduled_process()) {
        add_last_link(processListOOP, processOOP);
        _gst_check_process_state();

        /* If there's only one element in the list, discard this
           priority.  */
        processList = OOP_TO_OBJ(processListOOP);
        if (OBJ_SEMAPHORE_GET_FIRST_LINK(processList) ==
            OBJ_SEMAPHORE_GET_LAST_LINK(processList))
          continue;

        processOOP = remove_first_link(processListOOP);
      }

      return (processOOP);
    }
  }
  return (_gst_nil_oop);
}

OOP next_scheduled_process(void) {
  OOP processOOP;
  gst_object processor;

  processOOP = highest_priority_process();

  if (!IS_NIL(processOOP))
    return (processOOP);

  if (is_process_ready(get_scheduled_process()))
    return (_gst_nil_oop);

  processor = OOP_TO_OBJ(_gst_processor_oop);
  OBJ_PROCESSOR_SCHEDULER_SET_ACTIVE_PROCESS(processor, _gst_nil_oop);

  return (_gst_nil_oop);
}

void _gst_check_process_state(void) {
  OOP processLists, processListOOP, processOOP;
  int priority, n;
  gst_object processList;
  gst_object process;

  processLists = GET_PROCESS_LISTS();
  priority = NUM_OOPS(OOP_TO_OBJ(processLists));
  for (n = 0; priority > 0; --priority) {
    processListOOP = ARRAY_AT(processLists, priority);
    processList = OOP_TO_OBJ(processListOOP);

    if (IS_NIL(OBJ_SEMAPHORE_GET_FIRST_LINK(processList)) &&
        IS_NIL(OBJ_SEMAPHORE_GET_LAST_LINK(processList)))
      continue;

    /* Sanity check the first and last link in the process list.  */
    if (IS_NIL(OBJ_SEMAPHORE_GET_FIRST_LINK(processList)) ||
        IS_NIL(OBJ_SEMAPHORE_GET_LAST_LINK(processList)))
      abort();

    for (processOOP = OBJ_SEMAPHORE_GET_FIRST_LINK(processList);
         !IS_NIL(processOOP); processOOP = OBJ_PROCESS_GET_NEXT_LINK(process), n++) {
      process = OOP_TO_OBJ(processOOP);
      if (OBJ_PROCESS_GET_MY_LIST(process) != processListOOP)
        abort();

#if 0
	  /* This is false when a process has just finished initializing
	     itself.  */
	  if (process->priority != FROM_INT (priority))
	    abort ();
#endif

      /* Sanity check the last link in the process list.  */
      if (IS_NIL(OBJ_PROCESS_GET_NEXT_LINK(process)) &&
          processOOP != OBJ_SEMAPHORE_GET_LAST_LINK(processList))
        abort();

      /* Check (rather brutally) for loops in the process lists.  */
      if (++n > _gst_mem.ot_size)
        abort();
    }
  }
}

/* Mainly for being invoked from a debugger */
void _gst_print_process_state(void) {
  OOP processLists, processListOOP, processOOP;
  int priority;
  gst_object processList;
  gst_object process;

  processLists = GET_PROCESS_LISTS();
  priority = NUM_OOPS(OOP_TO_OBJ(processLists));

  processOOP = get_scheduled_process();
  process = OOP_TO_OBJ(processOOP);
  if (processOOP == _gst_nil_oop)
    printf("No active process\n");
  else
    printf("Active process: <Proc %p prio: %td next %p context %p>\n",
           processOOP, TO_INT(OBJ_PROCESS_GET_PRIORITY(process)), OBJ_PROCESS_GET_NEXT_LINK(process),
           OBJ_PROCESS_GET_SUSPENDED_CONTEXT(process));

  for (; priority > 0; priority--) {
    processListOOP = ARRAY_AT(processLists, priority);
    processList = OOP_TO_OBJ(processListOOP);

    if (IS_NIL(OBJ_SEMAPHORE_GET_FIRST_LINK(processList)))
      continue;

    printf("  Priority %d: First %p last %p ", priority,
           OBJ_SEMAPHORE_GET_FIRST_LINK(processList),
           OBJ_SEMAPHORE_GET_LAST_LINK(processList));

    for (processOOP = OBJ_SEMAPHORE_GET_FIRST_LINK(processList);
         !IS_NIL(processOOP); processOOP = OBJ_PROCESS_GET_NEXT_LINK(process)) {
      process = OOP_TO_OBJ(processOOP);
      printf("\n    <Proc %p prio: %td context %p> ", processOOP,
             TO_INT(OBJ_PROCESS_GET_PRIORITY(process)), OBJ_PROCESS_GET_SUSPENDED_CONTEXT(process));
    }

    printf("\n");
  }
}

OOP semaphore_new(int signals) {
  gst_object sem;
  OOP semaphoreOOP;

  sem = instantiate(_gst_semaphore_class, &semaphoreOOP);
  OBJ_SEMAPHORE_SET_SIGNALS(sem, FROM_INT(signals));

  return (semaphoreOOP);
}

void _gst_init_process_system(void) {
  gst_object processor;
  int i;

  processor = OOP_TO_OBJ(_gst_processor_oop);
  if (IS_NIL(OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_LISTS(processor))) {
    gst_object processLists;

    processLists = instantiate_with(_gst_array_class, NUM_PRIORITIES,
                                    &OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_LISTS(processor));

    for (i = 0; i < NUM_PRIORITIES; i++)
      processLists->data[i] = semaphore_new(0);
  }

  if (IS_NIL(OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_TIME_SLICE(processor)))
    OBJ_PROCESSOR_SCHEDULER_SET_PROCESS_TIME_SLICE(processor, FROM_INT(DEFAULT_PREEMPTION_TIMESLICE));

  /* No process is active -- so highest_priority_process() need not
     worry about discarding an active process.  */
  OBJ_PROCESSOR_SCHEDULER_SET_ACTIVE_PROCESS(processor, _gst_nil_oop);
  switch_to_process = _gst_nil_oop;
  activate_process(highest_priority_process());
  set_preemption_timer();
}

OOP create_callin_process(OOP contextOOP) {
  OOP processListsOOP;
  gst_object processor;
  gst_object initialProcess;
  OOP initialProcessOOP, initialProcessListOOP, nameOOP;
  inc_ptr inc = INC_SAVE_POINTER();

  processor = OOP_TO_OBJ(_gst_processor_oop);
  processListsOOP = OBJ_PROCESSOR_SCHEDULER_GET_PROCESS_LISTS(processor);
  initialProcessListOOP = ARRAY_AT(processListsOOP, 4);

  nameOOP = _gst_string_new("call-in process");
  INC_ADD_OOP(nameOOP);

  initialProcess =
      instantiate(_gst_callin_process_class, &initialProcessOOP);

  INC_ADD_OOP(initialProcessOOP);
  OBJ_PROCESS_SET_PRIORITY(initialProcess, FROM_INT(USER_SCHEDULING_PRIORITY));
  OBJ_PROCESS_SET_INTERRUPT_LOCK(initialProcess, _gst_nil_oop);
  OBJ_PROCESS_SET_SUSPENDED_CONTEXT(initialProcess, contextOOP);
  OBJ_PROCESS_SET_NAME(initialProcess, nameOOP);
  INC_RESTORE_POINTER(inc);

  /* Put initialProcessOOP in the root set */
  add_first_link(initialProcessListOOP, initialProcessOOP);

  return (initialProcessOOP);
}

int _gst_get_var(enum gst_var_index index) {
  switch (index) {
  case GST_DECLARE_TRACING:
    return (_gst_declare_tracing);
  case GST_EXECUTION_TRACING:
    return (_gst_execution_tracing);
  case GST_EXECUTION_TRACING_VERBOSE:
    return (verbose_exec_tracing);
  case GST_GC_MESSAGE:
    return (_gst_gc_message);
  case GST_VERBOSITY:
    return (_gst_verbosity);
  case GST_MAKE_CORE_FILE:
    return (_gst_make_core_file);
  case GST_REGRESSION_TESTING:
    return (_gst_regression_testing);
  case GST_NO_LINE_NUMBERS:
    return (_gst_omit_line_numbers);
  default:
    return (-1);
  }
}

int _gst_set_var(enum gst_var_index index, int value) {
  int old = _gst_get_var(index);
  if (value < 0)
    return -1;

  switch (index) {
  case GST_DECLARE_TRACING:
    _gst_declare_tracing = value;
    break;
  case GST_EXECUTION_TRACING:
    _gst_execution_tracing = value;
    break;
  case GST_EXECUTION_TRACING_VERBOSE:
    verbose_exec_tracing = value;
    break;
  case GST_GC_MESSAGE:
    _gst_gc_message = value;
    break;
  case GST_VERBOSITY:
    _gst_verbosity = value;
    break;
  case GST_MAKE_CORE_FILE:
    _gst_make_core_file = value;
    break;
  case GST_REGRESSION_TESTING:
    _gst_regression_testing = true;
    break;
  case GST_NO_LINE_NUMBERS:
    _gst_omit_line_numbers = value;
    break;
  default:
    return (-1);
  }

  return old;
}

void _gst_init_interpreter(void) {
  unsigned int i;

#ifdef ENABLE_JIT_TRANSLATION
  _gst_init_translator();
  ip[current_thread_id] = 0;
#else
  ip[current_thread_id] = NULL;
#endif

  _gst_this_context_oop[current_thread_id] = _gst_nil_oop;
  for (i = 0; i < MAX_LIFO_DEPTH; i++)
    lifo_contexts[current_thread_id][i].flags = F_POOLED | F_CONTEXT;

  _gst_init_async_events();
  _gst_init_process_system();
}

void _gst_init_context(void) {

  chunk[current_thread_id] = chunks[current_thread_id] - 1;
  free_lifo_context[current_thread_id] = lifo_contexts[current_thread_id];

  for (size_t i = 0; i < MAX_LIFO_DEPTH; i++) {
    lifo_contexts[current_thread_id][i].flags = F_POOLED | F_CONTEXT;
  }
}

OOP _gst_prepare_execution_environment(void) {
  gst_object dummyContext;
  OOP dummyContextOOP, processOOP;
  inc_ptr inc = INC_SAVE_POINTER();

  empty_context_stack();
  dummyContext = alloc_stack_context(4);
  OBJ_SET_CLASS(dummyContext, _gst_method_context_class);
  OBJ_METHOD_CONTEXT_SET_PARENT_CONTEXT(dummyContext, _gst_nil_oop);
  OBJ_METHOD_CONTEXT_SET_METHOD(dummyContext, _gst_get_termination_method());
  OBJ_METHOD_CONTEXT_SET_FLAGS(
      dummyContext, (OOP)(MCF_IS_METHOD_CONTEXT | MCF_IS_EXECUTION_ENVIRONMENT |
                          MCF_IS_UNWIND_CONTEXT));
  OBJ_METHOD_CONTEXT_SET_RECEIVER(dummyContext, _gst_nil_oop);
  OBJ_METHOD_CONTEXT_SET_IP_OFFSET(dummyContext, FROM_INT(0));
  OBJ_METHOD_CONTEXT_SET_SP_OFFSET(dummyContext, FROM_INT(-1));

  OBJ_METHOD_CONTEXT_SET_NATIVE_IP(
      dummyContext, DUMMY_NATIVE_IP); /* See empty_context_stack */

  dummyContextOOP =
      alloc_oop(dummyContext, _gst_mem.active_flag | F_POOLED | F_CONTEXT);

  INC_ADD_OOP(dummyContextOOP);
  processOOP = create_callin_process(dummyContextOOP);

  INC_RESTORE_POINTER(inc);
  return (processOOP);
}

OOP _gst_nvmsg_send(OOP receiver, OOP sendSelector, OOP *args, int sendArgs) {
  inc_ptr inc = INC_SAVE_POINTER();
#if 0
  OOP dirMessageOOP;
#endif
  OOP processOOP, currentProcessOOP;
  OOP result;
  gst_object process;
  int i;

  processOOP = _gst_prepare_execution_environment();
  INC_ADD_OOP(processOOP);

  _gst_check_process_state();
  /* _gst_print_process_state (); */
  /* _gst_show_backtrace (stdout); */

  if (reentrancy_jmp_buf && !reentrancy_jmp_buf->suspended++)
    suspend_process(reentrancy_jmp_buf->processOOP);

  currentProcessOOP = get_active_process();
  change_process_context(processOOP);

  PUSH_OOP(receiver);
  for (i = 0; i < sendArgs; i++)
    PUSH_OOP(args[i]);

  if (!sendSelector)
    send_block_value(sendArgs, sendArgs);
  else if (OOP_CLASS(sendSelector) == _gst_symbol_class)
    SEND_MESSAGE(sendSelector, sendArgs);
  else
    _gst_send_method(sendSelector);

  process = OOP_TO_OBJ(currentProcessOOP);

  if (!IS_NIL(currentProcessOOP) &&
      TO_INT(OBJ_PROCESS_GET_PRIORITY(process)) > USER_SCHEDULING_PRIORITY)
    ACTIVE_PROCESS_YIELD();

  result = _gst_interpret(processOOP);
  INC_ADD_OOP(result);

  /* Re-enable the previously executing process *now*, because a
     primitive might expect the current stack pointer to be that
     of the process that was executing.  */
  if (reentrancy_jmp_buf && !--reentrancy_jmp_buf->suspended &&
      !is_process_terminating(reentrancy_jmp_buf->processOOP)) {
    resume_process(reentrancy_jmp_buf->processOOP, true);
    if (!IS_NIL(switch_to_process))
      change_process_context(switch_to_process);
  }

  INC_RESTORE_POINTER(inc);
  return (result);
}

void set_preemption_timer(void) {
#ifdef ENABLE_PREEMPTION
  gst_processor_scheduler processor;
  int timeSlice;

  processor = (gst_processor_scheduler)OOP_TO_OBJ(_gst_processor_oop);
  timeSlice = TO_INT(processor->processTimeslice);

  time_to_preempt = false;
  if (timeSlice > 0)
    _gst_sigvtalrm_every(timeSlice, preempt_smalltalk_process);
#endif
}

void _gst_invalidate_method_cache(void) {
  int i;

  /* Only do this if some code was run since the last cache cleanup,
     as it is quite expensive.  */
  if (!_gst_sample_counter)
    return;

#ifdef ENABLE_JIT_TRANSLATION
  _gst_reset_inline_caches();
#else
  at_cache_class = at_put_cache_class = size_cache_class = class_cache_class =
      NULL;
#endif

  _gst_cache_misses = _gst_sample_counter = 0;

  for (i = 0; i < METHOD_CACHE_SIZE; i++) {
    method_cache[i].selectorOOP = NULL;
  }
}

void _gst_copy_processor_registers(void) {
  copy_semaphore_oops();

  /* Get everything into the main OOP table first.  */
  if (_gst_this_context_oop[current_thread_id])
    MAYBE_COPY_OOP(_gst_this_context_oop[current_thread_id]);

  /* everything else is pointed to by _gst_this_context_oop, either
     directly or indirectly, or has been copyed when scanning the
     registered roots.  */
}

void copy_semaphore_oops(void) {
  async_queue_entry *sig;

  for (sig = queued_async_signals; sig != &queued_async_signals_tail;
       sig = sig->next)
    MAYBE_COPY_OOP(sig->data);
  for (sig = queued_async_signals_sig; sig != &queued_async_signals_tail;
       sig = sig->next)
    MAYBE_COPY_OOP(sig->data);

  /* there does seem to be a window where this is not valid */
  if (single_step_semaphore)
    MAYBE_COPY_OOP(single_step_semaphore);

  /* there does seem to be a window where this is not valid */
  MAYBE_COPY_OOP(switch_to_process);
}

void _gst_mark_processor_registers(void) {
  mark_semaphore_oops();
  if (_gst_this_context_oop[current_thread_id])
    MAYBE_MARK_OOP(_gst_this_context_oop[current_thread_id]);

  /* everything else is pointed to by _gst_this_context_oop, either
     directly or indirectly, or has been marked when scanning the
     registered roots.  */
}

void mark_semaphore_oops(void) {
  async_queue_entry *sig;

  for (sig = queued_async_signals; sig != &queued_async_signals_tail;
       sig = sig->next)
    MAYBE_MARK_OOP(sig->data);
  for (sig = queued_async_signals_sig; sig != &queued_async_signals_tail;
       sig = sig->next)
    MAYBE_MARK_OOP(sig->data);

  /* there does seem to be a window where this is not valid */
  if (single_step_semaphore)
    MAYBE_MARK_OOP(single_step_semaphore);

  /* there does seem to be a window where this is not valid */
  MAYBE_MARK_OOP(switch_to_process);
}

void _gst_fixup_object_pointers(void) {
  gst_object thisContext;

  if (!IS_NIL(_gst_this_context_oop[current_thread_id])) {
    /* Create real OOPs for the contexts here.  If we do it while copying,
       the newly created OOPs are in to-space and are never scanned! */
    empty_context_stack();

    thisContext = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
#ifdef DEBUG_FIXUP
    fflush(stderr);
    printf("\nF sp %x %d    ip[current_thread_id] %x %d	_gst_this_method[current_thread_id] %x  thisContext %x",
           sp, sp - OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext), ip[current_thread_id],
           ip[current_thread_id] - method_base, _gst_this_method[current_thread_id]->object, thisContext);
    fflush(stdout);
#endif
    OBJ_METHOD_CONTEXT_SET_METHOD(thisContext, _gst_this_method[current_thread_id]);
    OBJ_METHOD_CONTEXT_SET_RECEIVER(thisContext, _gst_self[current_thread_id]);
    OBJ_METHOD_CONTEXT_SET_SP_OFFSET(
        thisContext,
        FROM_INT(sp - OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext)));
    OBJ_METHOD_CONTEXT_SET_IP_OFFSET(thisContext, FROM_INT(ip[current_thread_id] - method_base));
  }
}

void _gst_restore_object_pointers(void) {
  gst_object thisContext;

  /* !!! The objects can move after the growing or compact phase. But,
     all this information is re-computable, so we pick up
     _gst_this_method to adjust the ip and _gst_literals accordingly,
     and we also pick up the context to adjust sp and the temps
     accordingly.  */

  if (!IS_NIL(_gst_this_context_oop[current_thread_id])) {
    thisContext = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
    _gst_temporaries[current_thread_id] = OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext);

#ifndef OPTIMIZE /* Mon Jul 3 01:21:06 1995 */
    /* these should not be necessary */
    if (_gst_this_method[current_thread_id] != OBJ_METHOD_CONTEXT_METHOD(thisContext)) {
      printf("$$$$$$$$$$$$$$$$$$$ GOT ONE!!!!\n");
      printf("this method %O\n", _gst_this_method[current_thread_id]);
      printf("this context %O\n", OBJ_METHOD_CONTEXT_RECEIVER(thisContext));
      abort();
    }
    if (_gst_self[current_thread_id] != OBJ_METHOD_CONTEXT_RECEIVER(thisContext)) {
      printf("$$$$$$$$$$$$$$$$$$$ GOT ONE!!!!\n");
      printf("self %O\n", _gst_self[current_thread_id]);
      printf("this context %O\n", OBJ_METHOD_CONTEXT_RECEIVER(thisContext));
      abort();
    }
#endif /* OPTIMIZE Mon Jul 3 01:21:06 1995 */

    SET_THIS_METHOD(_gst_this_method[current_thread_id], GET_CONTEXT_IP(thisContext));
    sp = TO_INT(OBJ_METHOD_CONTEXT_SP_OFFSET(thisContext)) +
         OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext);

#ifdef DEBUG_FIXUP
    fflush(stderr);
    printf("\nR sp %x %d    ip[current_thread_id] %x %d	_gst_this_method[current_thread_id] %x  thisContext %x\n",
           sp, sp - OBJ_METHOD_CONTEXT_CONTEXT_STACK(thisContext), ip[current_thread_id],
           ip[current_thread_id] - method_base, _gst_this_method[current_thread_id]->object, thisContext);
    fflush(stdout);
#endif
  }

  SET_EXCEPT_FLAG(true); /* force to import registers */
}

static RETSIGTYPE interrupt_on_signal(int sig) {
  if (reentrancy_jmp_buf)
    stop_execution();
  else {
    _gst_set_signal_handler(sig, SIG_DFL);
    raise(sig);
  }
}

static void backtrace_on_signal_1(mst_Boolean is_serious_error,
                                  mst_Boolean c_backtrace) {
  static int reentering = -1;

  /* Avoid recursive signals */
  reentering++;

  if ((reentrancy_jmp_buf && reentrancy_jmp_buf->interpreter) && !reentering &&
      ip[current_thread_id] && !_gst_gc_running)
    _gst_show_backtrace(stderr);
  else {
    if (is_serious_error)
      _gst_errorf("Error occurred while not in byte code interpreter!!");

#ifdef HAVE_EXECINFO_H
    /* Don't print a backtrace, for example, if exiting during a
       compilation.  */
    if (c_backtrace && !reentering) {
      PTR array[11];
      size_t size = backtrace(array, 11);
      backtrace_symbols_fd(array + 1, size - 1, STDERR_FILENO);
    }
#endif
  }

  reentering--;
}

static RETSIGTYPE backtrace_on_signal(int sig) {
  _gst_errorf("%s", strsignal(sig));
  _gst_set_signal_handler(sig, backtrace_on_signal);
  backtrace_on_signal_1(sig != SIGTERM, sig != SIGTERM);
  _gst_set_signal_handler(sig, SIG_DFL);
  raise(sig);
}

#ifdef SIGUSR1
static RETSIGTYPE user_backtrace_on_signal(int sig) {
  _gst_set_signal_handler(sig, user_backtrace_on_signal);
  backtrace_on_signal_1(false, true);
}
#endif

void _gst_init_signals(void) {
  if (!_gst_make_core_file) {
#ifdef ENABLE_JIT_TRANSLATION
    _gst_set_signal_handler(SIGILL, backtrace_on_signal);
#endif
    _gst_set_signal_handler(SIGABRT, backtrace_on_signal);
  }
  _gst_set_signal_handler(SIGTERM, backtrace_on_signal);
  _gst_set_signal_handler(SIGINT, interrupt_on_signal);
#ifdef SIGUSR1
  _gst_set_signal_handler(SIGUSR1, user_backtrace_on_signal);
#endif
}

void _gst_show_backtrace(FILE *fp) {
  OOP contextOOP;
  gst_object context;
  gst_compiled_block block;
  gst_compiled_method method;
  gst_method_info methodInfo;

  empty_context_stack();
  for (contextOOP = _gst_this_context_oop[current_thread_id]; !IS_NIL(contextOOP);
       contextOOP = OBJ_METHOD_CONTEXT_PARENT_CONTEXT(context)) {
    context = OOP_TO_OBJ(contextOOP);
    if ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(context) ==
        (MCF_IS_METHOD_CONTEXT | MCF_IS_DISABLED_CONTEXT))
      continue;

    /* printf ("(OOP %p)", context->method); */
    fprintf(fp, "(ip[current_thread_id] %d)", TO_INT(OBJ_METHOD_CONTEXT_IP_OFFSET(context)));
    if ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(context) & MCF_IS_METHOD_CONTEXT) {
      OOP receiver, receiverClass;

      if ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(context) &
          MCF_IS_EXECUTION_ENVIRONMENT) {
        if (IS_NIL(OBJ_METHOD_CONTEXT_PARENT_CONTEXT(context)))
          fprintf(fp, "<bottom>\n");
        else
          fprintf(fp, "<unwind point>\n");
        continue;
      }

      if ((intptr_t)OBJ_METHOD_CONTEXT_FLAGS(context) & MCF_IS_UNWIND_CONTEXT)
        fprintf(fp, "<unwind> ");

      /* a method context */
      method =
          (gst_compiled_method)OOP_TO_OBJ(OBJ_METHOD_CONTEXT_METHOD(context));
      methodInfo = (gst_method_info)OOP_TO_OBJ(method->descriptor);
      receiver = OBJ_METHOD_CONTEXT_RECEIVER(context);
      if (IS_INT(receiver))
        receiverClass = gst_small_integer_class;

      else
        receiverClass = OOP_CLASS(receiver);

      if (receiverClass == methodInfo->class)
        fprintf(fp, "%O", receiverClass);
      else
        fprintf(fp, "%O(%O)", receiverClass, methodInfo->class);
    } else {
      /* a block context */
      block =
          (gst_compiled_block)OOP_TO_OBJ(OBJ_METHOD_CONTEXT_METHOD(context));
      method = (gst_compiled_method)OOP_TO_OBJ(block->method);
      methodInfo = (gst_method_info)OOP_TO_OBJ(method->descriptor);

      fprintf(fp, "[] in %O", methodInfo->class);
    }
    fprintf(fp, ">>%O\n", methodInfo->selector);
  }
}

void _gst_show_stack_contents(void) {
  gst_object context;
  OOP *walk;
  mst_Boolean first;

  if (IS_NIL(_gst_this_context_oop[current_thread_id]))
    return;

  context = OOP_TO_OBJ(_gst_this_context_oop[current_thread_id]);
  for (first = true, walk = OBJ_METHOD_CONTEXT_CONTEXT_STACK(context);
       walk <= sp; first = false, walk++) {
    if (!first)
      printf(", ");

    printf("%O", *walk);
  }
  printf("\n\n");
}

static inline mst_Boolean cached_index_oop_primitive(OOP rec, OOP idx,
                                                     intptr_t spec) {
  OOP result;
  if (!IS_INT(idx))
    return (true);

  result = index_oop_spec(rec, OOP_TO_OBJ(rec), TO_INT(idx), spec);
  if UNCOMMON (!result)
    return (true);

  POP_N_OOPS(1);
  SET_STACKTOP(result);
  return (false);
}

static inline mst_Boolean
cached_index_oop_put_primitive(OOP rec, OOP idx, OOP val, intptr_t spec) {
  if (!IS_INT(idx))
    return (true);

  if UNCOMMON (!index_oop_put_spec(rec, OOP_TO_OBJ(rec), TO_INT(idx), val,
                                   spec))
    return (true);

  POP_N_OOPS(2);
  SET_STACKTOP(val);
  return (false);
}

static inline intptr_t execute_primitive_operation(int primitive,
                                                   volatile int numArgs) {
  prim_table_entry *pte = &_gst_primitive_table[primitive];

  intptr_t result = pte->func(pte->id, numArgs);
  last_primitive = primitive;
  return result;
}

prim_table_entry *_gst_get_primitive_attributes(int primitive) {
  return &_gst_default_primitive_table[primitive];
}

void _gst_set_primitive_attributes(int primitive, prim_table_entry *pte) {
  if (pte)
    _gst_primitive_table[primitive] = *pte;
  else
    _gst_primitive_table[primitive] = _gst_default_primitive_table[0];
}

void push_jmp_buf(interp_jmp_buf *jb, int for_interpreter, OOP processOOP) {
  jb->next = reentrancy_jmp_buf;
  jb->processOOP = processOOP;
  jb->suspended = 0;
  jb->interpreter = for_interpreter;
  jb->interrupted = false;
  _gst_register_oop(processOOP);
  reentrancy_jmp_buf = jb;
}

mst_Boolean pop_jmp_buf(void) {
  interp_jmp_buf *jb = reentrancy_jmp_buf;
  reentrancy_jmp_buf = jb->next;

  if (jb->interpreter && !is_process_terminating(jb->processOOP))
    _gst_terminate_process(jb->processOOP);

  _gst_unregister_oop(jb->processOOP);
  return jb->interrupted && reentrancy_jmp_buf;
}

void stop_execution(void) {
  reentrancy_jmp_buf->interrupted = true;

  if (reentrancy_jmp_buf->interpreter &&
      !is_process_terminating(reentrancy_jmp_buf->processOOP)) {
    _gst_abort_execution = "userInterrupt";
    SET_EXCEPT_FLAG(true);
    if (get_active_process() != reentrancy_jmp_buf->processOOP)
      resume_process(reentrancy_jmp_buf->processOOP, true);
  } else
    longjmp(reentrancy_jmp_buf->jmpBuf, 1);
}

mst_Boolean parse_method_from_stream_with_protection(OOP currentClass,
                                                     OOP currentCategory) {
  interp_jmp_buf jb;
  OOP methodOOP;

  push_jmp_buf(&jb, false, get_active_process());
  if (setjmp(jb.jmpBuf) == 0)
    methodOOP = _gst_parse_method_from_stream(currentClass, currentCategory);
  else
    methodOOP = _gst_nil_oop;

  PUSH_OOP(methodOOP);
  return pop_jmp_buf();
}

mst_Boolean parse_stream_with_protection(OOP currentNamespace) {
  interp_jmp_buf jb;

  push_jmp_buf(&jb, false, get_active_process());
  if (setjmp(jb.jmpBuf) == 0)
    _gst_parse_stream(currentNamespace);

  return pop_jmp_buf();
}
