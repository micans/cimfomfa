/*            Copyright (C) 2002, 2003, 2004, 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "tingea/ting.h"
#include "tingea/ding.h"
#include "tingea/alloc.h"
#include "tingea/minmax.h"
#include "tingea/err.h"
#include "tingea/types.h"
#include "tingea/ding.h"
#include "tingea/let.h"


int main
(  int   argc
,  char* argv[]
)
   {  long ival
   ;  double fval
   ;  int flags
   ;  int debug = argc > 2
   ;  telRaam *raam

   ;  if (argc <= 1)
      exit(1)

   ;  if (argc > 3)     /* terrible, fixme */
      trmDebug()

   ;  raam = trmInit(argv[argc-1])

   ;  if (trmParse(raam))
      {  mcxErr("main", "parse error")
      ;  trmExit(raam)
      ;  exit(1)
   ;  }

      if (debug)
      trmDump(raam, "after parse")

   ;  flags = trmEval(raam, &ival, &fval)

   ;  if (debug)
      trmDump(raam, "after eval")

   ;  if (flags < 0)
      printf ("evaluation error occurred\n")
   ;  else if (trmIsNan(flags))
      printf ("arithmetic exception\n")
   ;  else if (trmIsNum(flags))
      printf ("%ld\n", ival)
   ;  else
      printf ("%.15g\n", fval)

   ;  if (trmExit(raam))
      mcxErr("main", "unexpected free error")
   ;  return 0
;  }



