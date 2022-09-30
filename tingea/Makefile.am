## Process this file with automake to produce Makefile.in
## $Id: Makefile.am-assimilate,v 1.3 2005-12-07 16:58:40 flux Exp $
                                                                                
AM_CPPFLAGS = -I$(top_srcdir)

instdir=$(libdir)

#  inst_LIBRARIES = libtingea.a
inst_LTLIBRARIES = libtingea.la
libtingea_la_SOURCES = alloc.c array.c rand.c equate.c opt.c io.c \
 err.c hash.c heap.c let.c list.c gralloc.c tok.c ting.c ding.c \
 tr.c types.c
# libtingea_la_SOURCES = $(libtingea_a_SOURCES)

library_includedir = $(includedir)/tingea

library_include_HEADERS = alloc.h array.h rand.h equate.h hash.h heap.h let.h io.h \
   opt.h err.h ting.h ding.h list.h gralloc.h tok.h \
   compile.h getpagesize.h types.h inttypes.h minmax.h tr.h

