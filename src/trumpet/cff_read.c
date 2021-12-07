/*            Copyright (C) 2003, 2004, 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tingea/io.h"
#include "tingea/err.h"


static void report_xf
(  mcxIO* xf
)
   {  mcxTell
      (  "xf counters"
      ,  "xbc <%ld> xlc <%ld> xlo <%ld> xlo_ <%ld>"
      ,  (long) xf->bc
      ,  (long) mcxIOlc(xf)
      ,  (long) xf->lo
      ,  (long) xf->lo_
      )
;  }


int main
(  int   argc
,  char* argv[]
)
   {  mcxIO* xf = mcxIOnew(argc > 1 ? argv[1] : "-", "r")
   ;  int mode = argc > 2 ? atoi(argv[2]) : 0
   ;  int xmode = argc > 3 ? atoi(argv[3]) : 0
   ;  int c
   ;  mcxTing* line = mcxTingEmpty(NULL, 160)
   ;  int n_read
   ;  char buf[4097]

   ;  mcxIOopen(xf, EXIT_ON_FAIL)
#define TRUMP_READLINE_SHOW (MCX_READLINE_DOT << 1)
   ;  mcxbool show = xmode & TRUMP_READLINE_SHOW ? TRUE : FALSE

   ;  BIT_OFF(xmode, TRUMP_READLINE_SHOW)

;fprintf(stderr, "xmode %d\n", xmode)

   ;  if (!mode)
      fprintf
      (  stderr,
"Usage: read <fname> [<mode>] [<xmode>]\n"
"mode  1    fgetc\n"
"mode  2    mcxIOstep\n"
"mode  3    mcxIOreadLine\n"
"mode  4    mcxIOreadFile\n"
"mode  5    read\n"
"mode  6    read + strchr(\\n)\n"
"mode  7    read + strcspn(\\r\\n)\n"
"mode  8    output character counts\n"
"  xmodes\n"
"     CHOMP %d\n"
"     SKIP %d\n"
"     PAR %d\n"
"     BSC %d\n"
"     DOT %d\n"
"    show %d\n"
   ,  MCX_READLINE_CHOMP
   ,  MCX_READLINE_SKIP_EMPTY
   ,  MCX_READLINE_PAR
   ,  MCX_READLINE_BSC
   ,  MCX_READLINE_DOT
   ,  TRUMP_READLINE_SHOW
      )
   ,  exit(0)
   ;  if (mode == 1)
      while ((c = fgetc(xf->fp)) != EOF)
      ;

   ;  if (mode == 2)
      {  while ((c = mcxIOstep(xf)) != EOF)
         if (0)
         fprintf(stderr, "[%c-%ld]", c, mcxIOlc(xf))
      ;  report_xf(xf)
   ;  }

      if (mode == 3)
      {  long lc = 0
      ;  mcxstatus status
      ;  while
         (  STATUS_OK
         == (status = mcxIOreadLine(xf, line, xmode))
         )
         {  if (line->len)
            lc++
         ;  if (show)
            mcxTell(NULL, "[%s]", line->str)
      ;  }

         mcxTell
         (  NULL
         ,  "last line <%s>\n"
            "line count <%ld>"
         ,  line->str
         ,  lc
         )
      ;  report_xf(xf)
   ;  }

      if (mode == 4)
         mcxIOreadFile(xf, line)
      ,  mcxTell(NULL, "string size <%ld>", (long) line->len)
      ,  report_xf(xf)

   ;  if (mode == 5)
      while (read(fileno(xf->fp), buf, 4096) > 0)
      ;

   ;  if (mode == 6)
      while ((n_read = read(fileno(xf->fp), buf, 4096) > 0))
      {  const char* s
      ;  buf[n_read] = '\0'
      ;  if ((s = strchr(buf, '\n')))
         buf[s-buf] = 'a'
   ;  }

      if (mode == 7)
      while ((n_read = read(fileno(xf->fp), buf, 4097) > 0))
      {  int n
      ;  buf[n_read] = '\0'
      ;  n = strcspn(buf, "\r\n")
      ;  buf[n] = '\n'
   ;  }

      if (mode == 8)
      {  unsigned long tally[256] = { 0 }
      ;  int i
      ;  while ((c = fgetc(xf->fp)) != EOF)
         tally[c]++
      ;  for (i=0;i<256;i++)
         fprintf(stdout, "%d\t%c\t%lu\n", i, i > 31 && i < 127 ? i : '-', tally[i])
   ;  }

      return 0
;  }


/* last line */
