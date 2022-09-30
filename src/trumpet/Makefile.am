## Process this file with automake to produce Makefile.in
## $Id: Makefile.am,v 1.13 2007-02-27 11:49:42 flux Exp $

AM_CPPFLAGS = -I$(top_srcdir) -I..

## make sure these do _not_ get build or installed by default
bin_PROGRAMS   =
noinst_PROGRAMS=  cff_match cff_pagesize cff_reap cff_read  cff_tst  cff_ttwhy  cff_link  cff_sow  cff_optoid  cff_tring  cff_rand  cff_bitpat  cff_edit  cff_unit_ceilfloor cff_hashfile cff_let


cff_ttwhy_SOURCES  =  cff_ttwhy.c
cff_unit_ceilfloorSOURCES  =  cff_unit_ceilfloor.c
cff_tst_SOURCES    =  cff_tst.c
cff_hashfile_SOURCES = cff_hashfile.c
cff_reap_SOURCES = cff_reap.c
cff_optoid_SOURCES =  cff_optoid.c
cff_bitpat_SOURCES =  cff_bitpat.c
cff_let_SOURCES    =  cff_let.c
cff_read_SOURCES   =  cff_read.c
cff_sow_SOURCES    =  cff_sow.c
cff_tring_SOURCES  =  cff_tring.c
cff_rand_SOURCES   =  cff_rand.c
cff_link_SOURCES   =  cff_link.c
cff_match_SOURCES  =  cff_match.c
cff_edit_SOURCES   =  cff_edit.c
cff_pagesize_SOURCES  =  cff_pagesize.c

cff_let_LDFLAGS = -lm
cff_hashfile_LDFLAGS = -lm

#LDADD = ../../tingea/libtingea.a
LDADD = ../../tingea/libtingea.la

