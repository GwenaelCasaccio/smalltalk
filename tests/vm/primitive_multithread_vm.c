#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libgst/gstpriv.h"

#define PRIM_SUCCEEDED return (false)

OOP highest_priority_process(void);
void change_process_context(OOP newProcess);
void empty_context_stack(void);

OOP switch_to_process[100];
bool async_queue_enabled[100] = { true };
async_queue_entry queued_async_signals_tail[100];
async_queue_entry *queued_async_signals[100] = { &queued_async_signals_tail[0] };
async_queue_entry *queued_async_signals_sig[100] = { &queued_async_signals_tail[0] };
_Atomic(void *const *) dispatch_vec_per_thread[100] = { NULL };
thread_local void *const *global_normal_bytecodes;
bool _gst_interp_need_to_wait[100] = { false };

OOP *sp[100];

void __wrap__gst_init_context(void);

#include "libgst/primitive_process.inl"

OOP highest_priority_process(void) {
  abort();
}

void change_process_context(OOP newProcess) {
  abort();
}

void empty_context_stack(void) {
  abort();
}


OOP _gst_processor_oop[100];
OOP _gst_this_context_oop[100];
OOP _gst_nil_oop;
thread_local unsigned long _gst_sample_counter;
thread_local unsigned long _gst_primitives_executed;

void __wrap_nomemory(int fatal) {
  check_expected(fatal);

  function_called();

  return ;
}
void __wrap__gst_init_context(void) {
  abort();
}

void __wrap__gst_invalidate_method_cache(void) {
  abort();
}

void __wrap__gst_check_process_state(void) {
  abort();
}

OOP __wrap__gst_interpret(OOP processOOP) {
  abort();
}

void __wrap_set_except_flag_for_thread(bool reset, size_t thread_id) {
  abort();
}

static void should_create_new_vm_thread(void **state) {

  (void) state;

}

int main(void) {
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test(should_create_new_vm_thread),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
