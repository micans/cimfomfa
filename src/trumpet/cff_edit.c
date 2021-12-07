/*  Copyright (C) 2007 Leopold Parts & Stijn van Dongen & Leopold Parts
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tingea/types.h"
#include "tingea/alloc.h"
#include "tingea/hash.h"
#include "tingea/array.h"
#include "tingea/ding.h"
#include "tingea/ting.h"
#include "tingea/types.h"
#include "tingea/minmax.h"
#include "tingea/err.h"
#include "tingea/io.h"


typedef struct
{  mcxTing**   lines
;  dim         n_lines
;  dim         n_lines_max
;
}  mcxLines    ;


static mcxstatus mcxIOreadLines
(  mcxLines*   ln
,  mcxIO*      xf
,  mcxOnFail   ON_FAIL
,  mcxbits     rlmode
)
   {  mcxTing* line =   mcxTingEmpty(NULL, 0)
   ;  dim n_lines_alloc = 16
   ;  mcxstatus status = STATUS_OK

   ;  ln->n_lines =  0
   ;  ln->lines   =  NULL
   ;  ln->n_lines_max = 0

   ;  if (mcxIOtestOpen(xf, ON_FAIL))
      return STATUS_FAIL

   ;  if (!(ln->lines = mcxAlloc(n_lines_alloc * sizeof(mcxTing), ON_FAIL)))
      return STATUS_FAIL

   ;  while(STATUS_OK == mcxIOreadLine(xf, line, rlmode))
      {  status = STATUS_FAIL
      ;  if
         (  n_lines_alloc <= ln->n_lines
         && mcxResize
            (  &ln->lines
            ,  sizeof(mcxTing)
            ,  &n_lines_alloc
            ,  ln->n_lines * 2
            ,  ON_FAIL
            )
         )
         break

      ;  ln->lines[ln->n_lines++] = line
      ;  line = mcxTingEmpty(NULL, 0)
      ;  status = STATUS_OK
   ;  }

      mcxTingFree(&line)
   ;  ln->n_lines_max = n_lines_alloc
   ;  return status
;  }


int main
(  int   argc
,  char* argv[]
)
   {  mcxIO* xf = mcxIOnew(argc > 1 ? argv[1] : "-", "r")
   ;  dim lo = argc > 2 ? atoi(argv[2]) : 0
   ;  dim hi = argc > 3 ? atoi(argv[3]) : 0
   ;  dim i
   ;  mcxLines ln

   ;  mcxIOreadLines(&ln, xf, EXIT_ON_FAIL, MCX_READLINE_CHOMP)

   ;  if (!hi)
      hi = ln.n_lines

   ;  for (i=lo;i<hi;i++)
      {  dim j
      ;  for (j=i+1;j<ln.n_lines;j++)
         {  int lcs = 0
         ;  int ed = mcxEditDistance(ln.lines[i]->str, ln.lines[j]->str, &lcs)
         ;  fprintf(stdout, "%s\t%s\t%d\t%d\n", ln.lines[i]->str, ln.lines[j]->str, (int) ed, (int) lcs)
      ;  }
      }
      return 0
;  }



