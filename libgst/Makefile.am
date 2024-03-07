LEX_OUTPUT_ROOT = lex.yy
## CFLAGS=-O0 -g
##AM_CFLAGS = $(LIBGST_CFLAGS) -flto
AM_CFLAGS = $(LIBGST_CFLAGS) -Wall -Wextra
AM_CPPFLAGS = $(RELOC_CPPFLAGS)	\
	-I$(top_srcdir)/dynasm \
	-I$(top_srcdir)/lib-src \
	$(INCFFI) \
	$(INCSNPRINTFV) \
	$(INCSIGSEGV) \
	$(INCLTDL)

BUILT_SOURCES = jit-x64.c
CLEANFILES = jit-x64.c
include_HEADERS = gstpub.h gst.h
lib_LTLIBRARIES = libgst.la

# definitions for libgst.la

libgst_la_LIBADD=$(top_builddir)/lib-src/library.la \
	$(LIBSIGSEGV) $(LIBFFI) $(LIBSNPRINTFV) $(LIBREADLINE) $(LIBLTDL) \
	$(LIBGMP) $(LIBTHREAD)

libgst_la_DEPENDENCIES=$(top_builddir)/lib-src/library.la $(LIBSNPRINTFV)

# libgst_la_LDFLAGS = -flto -version-info $(VERSION_INFO) -no-undefined -bindir $(bindir)
libgst_la_LDFLAGS = -version-info $(VERSION_INFO) -no-undefined -bindir $(bindir)

libgst_la_SOURCES = gstpub.c files.c gst-parse.c lex.c \
		str.c tree.c byte.c comp.c \
		sym.c dict.c oop.c opt.c \
		save.c cint.c heap.c input.c \
		sysdep.c callin.c mpz.c barrier.c \
		print.c alloc.c re.c interp.c \
		real.c sockets.c events.c object_pointer.c \
		forward_object.c heap/heap.c heap/tlab.c jit-x64.c \
		method_dictionary.c sip_hash.c bootstrap.c \
		object.cpp object_table.cpp alloc_new.cpp test_case_export.cpp

jit-x64.lo: AM_CFLAGS = $(LIBGST_CFLAGS)

dist_noinst_DATA = valgrind.supp builtins.gperf jit-x64.dasc

noinst_HEADERS = gstpriv.h files.h lex.h str.h \
		re.h tree.h byte.h interp.h \
		comp.h	sym.h dict.h oop.h \
		save.h cint.h sysdep.h	callin.h \
		gstpub.h opt.h mpz.h md-config.h \
		heap.h	real.h oop.inl dict.inl \
		interp.inl interp-bc.inl sockets.h comp.inl \
		input.h events.h print.h alloc.h \
		gst-parse.h prims.inl superop1.inl superop2.inl \
	 	object_pointer.h forward_object.h vm.inl stack.inl \
		barrier.h primitive_process.inl primitive_process_scheduling.inl primitive_semaphore.inl \
		semaphore.h processor_scheduler.h \
		sysdep/common/files.c sysdep/common/time.c sysdep/cygwin/files.c \
		sysdep/cygwin/findexec.c sysdep/cygwin/mem.c sysdep/cygwin/signals.c \
		sysdep/cygwin/time.c sysdep/cygwin/timer.c sysdep/posix/files.c	\
		sysdep/posix/findexec.c sysdep/posix/mem.c sysdep/posix/signals.c \
		sysdep/posix/time.c sysdep/posix/timer.c sysdep/win32/files.c \
		sysdep/win32/findexec.c sysdep/win32/mem.c sysdep/win32/signals.c \
		sysdep/win32/time.c sysdep/win32/timer.c sysdep/posix/events.c \
		sysdep/win32/events.c sysdep/cygwin/events.c heap/heap.h heap/tlab.h \
		method_dictionary.h sip_hash.h bootstrap.h $(BUILT_SOURCES) \
		object.h object_table.h object_data_alloc.h alloc_new.h

# rules for invoking gperf
# not fully idiot-proof but only to be run by maintainers

%.inl: %.gperf
	@opts="$< `$(SED) -ne /.*gperf/!d -e s///p -e q $< | \
	    $(SED) 's,$$(srcdir),$(srcdir),g'`"; \
	  echo $(GPERF) $$opts " > $@"; \
	  for i in a b c d e f g h j; do \
	    if test $$i = j; then \
	      eval $(GPERF) $$opts > $@ && break; \
	    else \
	      eval $(GPERF) $$opts > $@ 2>/dev/null && break; \
	      echo Retrying...; sleep 1; \
	    fi; \
	  done

$(srcdir)/jit-x64.c: Makefile
	../dynasm/minilua ../dynasm/dynasm.lua -D x64 -o jit-x64.c jit-x64.dasc

builtins.inl: builtins.gperf

dist-hook:
	for i in $(STAMP_FILES); do \
	  echo timestamp > $(distdir)/$$i; \
	done

