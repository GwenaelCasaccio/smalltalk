## CFLAGS=-O0 -g
##AM_CFLAGS = $(LIBGST_CFLAGS) -flto
AM_CFLAGS = $(LIBGST_CFLAGS)
AM_CPPFLAGS = $(RELOC_CPPFLAGS) \
  -I$(top_srcdir)/lib-src -I$(top_builddir)/lib-src \
  $(INCFFI) $(INCLIGHTNING) $(INCSNPRINTFV) $(INCSIGSEGV) $(INCLTDL)

noinst_PROGRAMS = minilua
minilua_SOURCES = minilua.c

noinst_HEADERS = \ 
	dasm_arm.h     \
	dasm_mips.h    \
	dasm_ppc.h     \
	dasm_proto.h   \
	dasm_x86.h
	$(BUILT_SOURCES)


