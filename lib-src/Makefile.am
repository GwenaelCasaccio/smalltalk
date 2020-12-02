noinst_LTLIBRARIES = library.la

library_la_LIBADD =  @LTLIBOBJS@ $(LIBTHREAD) $(LIBADD_DL) \
         $(SOCKET_LIBS)

library_la_DEPENDENCIES =  @LTLIBOBJS@ 

# These are to be included *always*, the rest are driven by configure.in
library_la_SOURCES = \
       getopt.c getopt1.c md5.c avltrees.c rbtrees.c lock.c \
       strspell.c regex.c socketx.c pointer-set.c

noinst_HEADERS = \
       obstack.h poll_.h getopt.h md5.h ansidecl.h strspell.h lock.h \
       avltrees.h rbtrees.h getaddrinfo.h \
       socketx.h regex.h pointer-set.h

DISTCLEANFILES = poll.h

