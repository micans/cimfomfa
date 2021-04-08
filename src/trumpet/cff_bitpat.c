/*      Copyright (C) 2005, 2006, 2007 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tingea/ting.h"
#include "tingea/ding.h"


int main
(  int argc
,  char* argv[]
) 
   {  int i
   ;  int a = 81679993
   ;  mcxTing* t = mcxTingEmpty(NULL, 32)

   ;  mcxMemPrint(t, &a, sizeof(a), MCX_MEMPRINT_REVERSE)
   ;  fprintf(stdout, "%s\n", t->str)

   ;  for (i=0;i<15;i++)
      {  signed short sh = i
      ;  mcxMemPrint(t, &sh, sizeof sh, MCX_MEMPRINT_REVERSE)
      ;  fprintf(stdout, "%s\n", t->str)
   ;  }

      for (i=0;i<15;i++)
      {  double f = i
      ;  mcxMemPrint(t, &f, sizeof f, MCX_MEMPRINT_REVERSE)
      ;  fprintf(stdout, "%s\n", t->str)
   ;  }
      return 0
;  }

