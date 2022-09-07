nodist_check_DATA = gst.im

AUTOTEST = $(AUTOM4TE) --language=autotest
TESTSUITE = $(srcdir)/testsuite

dist_noinst_DATA = \
atlocal.in local.at testsuite.at $(TESTSUITE) package.m4 \
ackermann.ok ackermann.st arrays.ok arrays.st ary3.ok ary3.st blocks.ok \
blocks.st chars.ok chars.st classes.ok classes.st cobjects.ok cobjects.st \
compiler.ok compiler.st dates.ok dates.st delays.ok delays.st except.ok \
except.st exceptions.ok exceptions.st fibo.ok fibo.st fileext.ok fileext.st \
floatmath.ok floatmath.st getopt.ok getopt.st geometry.ok geometry.st hash.ok \
hash.st hash2.ok hash2.st heapsort.ok heapsort.st intmath.ok intmath.st \
lists.ok lists.st lists1.ok lists1.st lists2.ok lists2.st matrix.ok \
matrix.st methcall.ok methcall.st mutate.ok mutate.st nestedloop.ok \
nestedloop.st objects.ok objects.st objinst.ok \
objinst.st processes.ok processes.st prodcons.ok prodcons.st quit.ok \
quit.st random-bench.ok random-bench.st sets.ok \
sets.st sieve.ok sieve.st strcat.ok strcat.st strings.ok strings.st \
pools.ok pools.st Ansi.st AnsiDB.st AnsiInit.st AnsiLoad.st AnsiRun.st \
stcompiler.st stcompiler.ok shape.st shape.ok streams.st streams.ok \
xlat.st xlat.ok multithread.st multithread.ok \
multithreaded_semaphore.st multithreaded_semaphore.ok

CLEANFILES = gst.im
DISTCLEANFILES = atconfig

.PHONY: regress gst.im

regress: gst.im
	cd $(srcdir) || exit 1; \
	for test in $(TESTS); do \
	  result=`echo $$test | $(SED) 's/st$$/ok/'`; \
	  @abs_top_builddir@/gst --image=@abs_top_builddir@/gst.im -r $$test 2>&1 | tee $$result; \
	done

gst.im: ../kernel/stamp-classes AnsiLoad.st Ansi.st AnsiDB.st
	echo "PackageLoader fileInPackage: #SUnit. ObjectMemory snapshot: 'gst.im'" | ./gst --image=../gst.im -
	cd $(srcdir) && @abs_builddir@/gst -S --image=@abs_top_builddir@/tests/gst.im AnsiLoad.st

$(srcdir)/package.m4: $(top_srcdir)/configure.ac
	{                                      \
	    echo '# Signature of the current package.'; \
	    echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])'; \
	    echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])'; \
	    echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])'; \
	    echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])'; \
	    echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	  } >'$(srcdir)/package.m4'

check-local: gst atlocal atconfig $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

clean-local:
	-$(SHELL) '$(TESTSUITE)' --clean

installcheck-local: atlocal atconfig $(TESTSUITE)
	if test -z "$(DESTDIR)"; then \
	  $(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS) AUTOTEST_PATH=$(bindir); \
	fi

$(TESTSUITE): $(srcdir)/testsuite.at $(srcdir)/package.m4 $(srcdir)/local.at
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@

TESTS = from_int grey_list barrier forward_object forward_object_alloc heap_test tlab_test integration_tlab jit_x64 object_pointer_test bootstrap_test primitive_multithread_vm_test primitive_process_test primitive_atomic_test semaphore_lock_test processor_scheduler_test primitive_semaphore_test \
method_dictionary_test

check_PROGRAMS = from_int grey_list barrier forward_object forward_object_alloc heap_test tlab_test integration_tlab jit_x64 object_pointer_test bootstrap_test primitive_multithread_vm_test primitive_process_test primitive_atomic_test semaphore_lock_test processor_scheduler_test primitive_semaphore_test \
method_dictionary_test

from_int_SOURCES = vm/from_int.c
from_int_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV)
from_int_LDADD = $(top_builddir)/libgst/libgst.la $(LIBSIGSEGV) $(LIBSNPRINTFV) $(top_builddir)/lib-src/library.la @ICON@
from_int_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
from_int_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

grey_list_SOURCES = vm/grey_list.c
grey_list_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV)
grey_list_LDADD = $(top_builddir)/libgst/libgst.la $(LIBSNPRINTFV) $(top_builddir)/lib-src/library.la @ICON@
grey_list_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
grey_list_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

barrier_SOURCES = vm/barrier.c
barrier_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV)
barrier_LDADD = $(top_builddir)/libgst/libgst.la $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
barrier_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
barrier_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

forward_object_SOURCES = vm/forward_object.c
forward_object_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
forward_object_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
forward_object_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
forward_object_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,_gst_heap_create -Wl,--wrap,nomemory -Wl,--wrap,_gst_heap_sbrk -Wl,--wrap,_gst_display_oop_short -Wl,--wrap,_gst_display_oop -Wl,--wrap,pthread_mutex_lock -Wl,--wrap,pthread_mutex_unlock -Wl,--wrap,_gst_finished_incremental_gc -Wl,--wrap,_gst_sweep_oop -Wl,--wrap,xcalloc

forward_object_alloc_SOURCES = vm/forward_object_alloc.c
forward_object_alloc_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
forward_object_alloc_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
forward_object_alloc_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
forward_object_alloc_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,_gst_detach_oop_arena_entry -Wl,--wrap,_gst_alloc_oop_arena_entry -Wl,--wrap,nomemory

heap_test_SOURCES = vm/heap_test.c
heap_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
heap_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
heap_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
heap_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,posix_memalign -Wl,--wrap,perror

tlab_test_SOURCES = vm/tlab_test.c
tlab_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
tlab_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
tlab_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
tlab_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_scavenge -Wl,--wrap,xcalloc -Wl,--wrap,set_except_flag_for_thread -Wl,--wrap,_gst_vm_global_barrier_wait -Wl,--wrap,_gst_vm_end_barrier_wait

integration_tlab_SOURCES = integration/tlab_int.c
integration_tlab_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
integration_tlab_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
integration_tlab_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
integration_tlab_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_scavenge -Wl,--wrap,xcalloc -Wl,--wrap,set_except_flag_for_thread -Wl,--wrap,_gst_vm_global_barrier_wait -Wl,--wrap,_gst_vm_end_barrier_wait

jit_x64_SOURCES = vm/jit_x64.c
jit_x64_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS) $(UNICORN_CFLAGS)
jit_x64_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(UNICORN_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@ -lunicorn
jit_x64_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
jit_x64_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

object_pointer_test_SOURCES = vm/object_pointer.c
object_pointer_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
object_pointer_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
object_pointer_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
object_pointer_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

bootstrap_test_SOURCES = vm/bootstrap.c
bootstrap_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
bootstrap_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
bootstrap_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
bootstrap_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_alloc_obj -Wl,--wrap,_gst_errorf -Wl,--wrap,_gst_alloc_words -Wl,--wrap,_gst_make_class_variable_dictionary -Wl,--wrap,xfree -Wl,--wrap,xmalloc -Wl,--wrap,_gst_inc_grow_registry -Wl,--wrap,_gst_swap_objects -Wl,--wrap,_gst_intern_string -Wl,--wrap,_gst_init_symbols_pass1 -Wl,--wrap,_gst_init_builtin_objects_classes -Wl,--wrap,_gst_init_symbols_pass2 -Wl,--wrap,_gst_tenure_all_survivors -Wl,--wrap,_gst_restore_symbols -Wl,--wrap,_gst_set_primitive_attributes -Wl,--wrap,_gst_set_primitive_attributes -Wl,--wrap,_gst_get_primitive_attributes -Wl,--wrap,_gst_make_instance_variable_array -Wl,--wrap,_gst_relocate_path -Wl,--wrap,_gst_register_oop -Wl,--wrap,_gst_make_pool_array -Wl,--wrap,_gst_init_process_system -Wl,--wrap,_gst_relocate_path -Wl,--wrap,_gst_is_pipe

method_dictionary_test_SOURCES = vm/method_dictionary.c
method_dictionary_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
method_dictionary_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
method_dictionary_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
method_dictionary_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_alloc_obj -Wl,--wrap,_gst_errorf -Wl,--wrap,_gst_alloc_words -Wl,--wrap,_gst_make_class_variable_dictionary -Wl,--wrap,xfree -Wl,--wrap,xmalloc -Wl,--wrap,_gst_inc_grow_registry -Wl,--wrap,_gst_swap_objects -Wl,--wrap,_gst_intern_string -Wl,--wrap,_gst_init_symbols_pass1 -Wl,--wrap,_gst_init_builtin_objects_classes -Wl,--wrap,_gst_init_symbols_pass2 -Wl,--wrap,_gst_tenure_all_survivors -Wl,--wrap,_gst_restore_symbols -Wl,--wrap,_gst_set_primitive_attributes -Wl,--wrap,_gst_set_primitive_attributes -Wl,--wrap,_gst_get_primitive_attributes -Wl,--wrap,_gst_make_instance_variable_array -Wl,--wrap,_gst_relocate_path -Wl,--wrap,_gst_register_oop -Wl,--wrap,_gst_make_pool_array -Wl,--wrap,_gst_init_process_system -Wl,--wrap,_gst_relocate_path -Wl,--wrap,_gst_is_pipe -Wl,--wrap,new_num_fields -Wl,--wrap,instantiate -Wl,--wrap,instantiate_with -Wl,--wrap,scramble -Wl,--wrap,new_num_fields

primitive_multithread_vm_test_SOURCES = vm/primitive_multithread_vm.c
primitive_multithread_vm_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
primitive_multithread_vm_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
primitive_multithread_vm_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
primitive_multithread_vm_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_init_context -Wl,--wrap,_gst_invalidate_method_cache -Wl,--wrap,_gst_check_process_state -Wl,--wrap,_gst_interpret -Wl,--wrap,set_except_flag_for_thread -Wl,--wrap,pthread_create -Wl,--wrap,_gst_vm_global_barrier_wait -Wl,--wrap,_gst_vm_end_barrier_wait -Wl,--wrap,perror -Wl,--wrap,pthread_cond_signal -Wl,--wrap,empty_context_stack -Wl,--wrap,pthread_exit -Wl,--wrap,_gst_alloc_oop_arena_entry_init -Wl,--wrap,gst_allocate_in_heap -Wl,--wrap,highest_priority_process -Wl,--wrap,change_process_context -Wl,--wrap,GET_PROCESS_LISTS -Wl,--wrap,add_first_link

primitive_process_test_SOURCES = vm/primitive_process.c
primitive_process_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
primitive_process_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
primitive_process_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
primitive_process_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,suspend_process -Wl,--wrap,resume_process -Wl,--wrap,is_process_ready -Wl,--wrap,is_process_terminating -Wl,--wrap,get_active_process -Wl,--wrap,active_process_yield -Wl,--wrap,_gst_sync_wait

primitive_atomic_test_SOURCES = vm/primitive_atomic.c
primitive_atomic_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
primitive_atomic_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
primitive_atomic_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
primitive_atomic_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS)

semaphore_lock_test_SOURCES = vm/semaphore_lock.c
semaphore_lock_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
semaphore_lock_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
semaphore_lock_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
semaphore_lock_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,perror

processor_scheduler_test_SOURCES = vm/processor_scheduler.c
processor_scheduler_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
processor_scheduler_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
processor_scheduler_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
processor_scheduler_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,perror

primitive_semaphore_test_SOURCES = vm/primitive_semaphore.c
primitive_semaphore_test_CFLAGS = -I$(top_builddir)/lib-src/ $(INCSNPRINTFV) $(CMOCKA_CFLAGS)
primitive_semaphore_test_LDADD = $(top_builddir)/libgst/libgst.la $(CMOCKA_LIBS) $(LIBSNPRINTFV) $(LIBTHREAD) $(top_builddir)/lib-src/library.la @ICON@
primitive_semaphore_test_DEPENDENCIES = $(top_builddir)/libgst/libgst.la $(top_builddir)/lib-src/library.la @ICON@
primitive_semaphore_test_LDFLAGS = -export-dynamic $(RELOC_LDFLAGS) $(LIBFFI_EXECUTABLE_LDFLAGS) -Wl,--wrap,nomemory -Wl,--wrap,_gst_sync_signal -Wl,--wrap,_gst_sync_wait -Wl,--wrap,wait_for_semaphore -Wl,--wrap,signal_and_broadcast_for_semaphore -Wl,--wrap,STACKTOP -Wl,--wrap,FROM_INT -Wl,--wrap,TO_INT -Wl,--wrap,SET_STACKTOP_BOOLEAN -Wl,--wrap,POP_OOP
