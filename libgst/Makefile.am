LEX_OUTPUT_ROOT = lex.yy
## CFLAGS=-O0 -g
AM_CFLAGS = $(LIBGST_CFLAGS)
AM_LFLAGS = -Cfe -o$(LEX_OUTPUT_ROOT).c
AM_YFLAGS = -vy
AM_CPPFLAGS = $(RELOC_CPPFLAGS) \
  -I$(top_srcdir)/lib-src -I$(top_builddir)/lib-src \
  $(INCFFI) $(INCLIGHTNING) $(INCSNPRINTFV) $(INCSIGSEGV) $(INCLTDL)

include_HEADERS = gstpub.h gst.h
lib_LTLIBRARIES = libgst.la
EXTRA_PROGRAMS = genbc genvm
CLEANFILES = genbc$(EXEEXT) genvm$(EXEEXT) \
  genbc-decl.stamp genbc-impl.stamp genvm-parse.stamp

# definitions for libgst.la

libgst_la_LIBADD=$(top_builddir)/lib-src/library.la \
	$(LIBSIGSEGV) $(LIBFFI) $(LIBSNPRINTFV) $(LIBREADLINE) $(LIBLTDL) \
	$(LIBGMP) $(LIBTHREAD)

libgst_la_DEPENDENCIES=$(top_builddir)/lib-src/library.la $(LIBSNPRINTFV)

libgst_la_LDFLAGS = -version-info $(VERSION_INFO) -no-undefined \
	-export-symbols-regex "^gst_.*" -bindir $(bindir)

libgst_la_SOURCES = \
       gstpub.c    files.c       gst-parse.c    lex.c        \
       str.c       tree.c        byte.c         comp.c       \
       sym.c       dict.c        oop.c  	opt.c        \
       save.c      cint.c    	 heap.c	        input.c      \
       sysdep.c    callin.c      mpz.c        \
       print.c	   alloc.c	 re.c	        interp.c     \
       real.c	   sockets.c	 events.c

# definitions for genbc

genbc_SOURCES = \
       genbc-decl.y genbc-impl.y genbc-scan.l genbc.c

genbc_LDADD = @LIBSNPRINTFV@ $(top_builddir)/lib-src/library.la

# definitions for genvm

genvm_SOURCES = \
       genvm-parse.y genvm-scan.l

genvm_LDADD = @LIBSNPRINTFV@ $(top_builddir)/lib-src/library.la

# manually give dependencies involving BUILT_SOURCES.  The input .l/.y files
# change rarely, so do not bother changing the timestamp file only if the
# output changed.

genbc-decl.stamp: genbc-decl.c genbc-decl.y; echo stamp > $@
genbc-impl.stamp: genbc-impl.c genbc-impl.y; echo stamp > $@
genvm-parse.stamp: genvm-parse.c genvm-parse.y; echo stamp > $@

genbc.o: genbc-decl.stamp genbc-impl.stamp
genbc-scan.o: genbc-decl.stamp genbc-impl.stamp
genbc-decl.o: genbc-decl.stamp genbc-impl.stamp
genbc-impl.o: genbc-decl.stamp genbc-impl.stamp
genvm-scan.o: genvm-parse.stamp

BUILT_SOURCES = match.h builtins.inl vm.inl \
	genbc-decl.c genbc-impl.c genbc-scan.c genbc-decl.h genbc-impl.h \
	genvm-parse.c genvm-scan.c genvm-parse.h

dist_noinst_DATA = valgrind.supp prims.inl byte.def vm.def builtins.gperf
STAMP_FILES = match.stamp vm.stamp

noinst_HEADERS = \
	gstpriv.h files.h lex.h str.h re.h \
	tree.h byte.h interp.h comp.h \
	sym.h dict.h oop.h save.h cint.h \
	sysdep.h callin.h gstpub.h opt.h mpz.h \
	md-config.h heap.h real.h oop.inl \
	dict.inl interp.inl interp-bc.inl \
	sockets.h comp.inl input.h events.h \
	print.h alloc.h gst-parse.h prims.inl \
	genbc.h genbc-decl.h \
	genbc-impl.h genvm-parse.h genvm.h \
	superop1.inl superop2.inl \
	sysdep/common/files.c sysdep/common/time.c sysdep/cygwin/files.c \
	sysdep/cygwin/findexec.c sysdep/cygwin/mem.c sysdep/cygwin/signals.c \
	sysdep/cygwin/time.c sysdep/cygwin/timer.c sysdep/posix/files.c \
	sysdep/posix/findexec.c sysdep/posix/mem.c sysdep/posix/signals.c \
	sysdep/posix/time.c sysdep/posix/timer.c sysdep/win32/files.c \
	sysdep/win32/findexec.c sysdep/win32/mem.c sysdep/win32/signals.c \
	sysdep/win32/time.c sysdep/win32/timer.c sysdep/posix/events.c \
	sysdep/win32/events.c sysdep/cygwin/events.c \
	$(BUILT_SOURCES)

# rules for invoking genbc
# Try to economize in the rebuilds, by avoiding unnecessary
# changes to the timestamp of match.h

$(srcdir)/match.h: $(srcdir)/match.stamp
	@:

$(srcdir)/match.stamp: byte.def byte.c opt.c
	@$(MAKE) genbc$(EXEEXT)
	@echo "./genbc$(EXEEXT) $(srcdir)/byte.def $(srcdir)/byte.c $(srcdir)/opt.c > match.h"; \
	  ./genbc$(EXEEXT) $(srcdir)/byte.def $(srcdir)/byte.c $(srcdir)/opt.c > _match.h
	@if cmp _match.h $(srcdir)/match.h > /dev/null 2>&1; then \
	  echo match.h is unchanged; \
	  rm _match.h; \
	else \
	  mv _match.h $(srcdir)/match.h; \
	fi
	@echo timestamp > $(srcdir)/match.stamp

# rules for invoking genvm
# Try to economize in the rebuilds, by avoiding unnecessary
# changes to the timestamp of vm.inl

$(srcdir)/vm.inl: $(srcdir)/vm.stamp
	@:

$(srcdir)/vm.stamp: vm.def
	@$(MAKE) genvm$(EXEEXT)
	@echo "./genvm$(EXEEXT) < $(srcdir)/vm.def | awk '{ /^#/ && gsub(/__oline__/,NR+1); print }' > vm.inl"; \
	  ./genvm$(EXEEXT) < $(srcdir)/vm.def | awk '{ /^#/ && gsub(/__oline__/,NR+1); print }' > _vm.inl
	@if cmp _vm.inl $(srcdir)/vm.inl > /dev/null 2>&1; then \
	  echo vm.inl is unchanged; \
	  rm _vm.inl; \
	else \
	  mv _vm.inl $(srcdir)/vm.inl; \
	fi
	@echo timestamp > $(srcdir)/vm.stamp

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

builtins.inl: builtins.gperf

dist-hook:
	for i in $(STAMP_FILES); do \
	  echo timestamp > $(distdir)/$$i; \
	done

