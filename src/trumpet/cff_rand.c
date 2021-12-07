/*            Copyright (C) 2005, 2006, 2007 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/


#define REVERT 1


#include "tingea/ting.h"
#include "tingea/io.h"
#include "tingea/ding.h"
#include "tingea/heap.h"
#include "tingea/rand.h"
#include "tingea/err.h"
#include "tingea/tok.h"
#include "tingea/types.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

ulong N = 1 << 20;


static double mcxNormalSample2
(  void
)
   {  return mcxNormalSample(3.0, 1.0)
;  }


int main
(  int   argc
,  char* argv[]
)
   {  double *x
   ;  dim i
   ;  double avg = 0.0, stddev = 0.0, stddev_kahan = 0.0, sum = 0.0
   ;  double carry = 0.0
   ;  double (*gaus)(void) = mcxNormalBoxMuller
   ;  double radius = 3.0
   ;  double sdev = 1.0
   ;  double mean = 0.0

   ;  if (argc == 1)
      {  fprintf(stdout, "arguments: N radius sdev mean\n")
      ;  exit(0)
   ;  }

      if (argc > 1)
      N = atol(argv[1])
   ;  if (argc > 2)
      radius = atof(argv[2])
   ;  if (argc > 3)
      sdev = atof(argv[3])
   ;  if (argc > 4)
      mean = atof(argv[4])

   ;  x = malloc (sizeof x[0] * N)

   ;  srandom(mcxSeed(14))

   ;  for (i=0;i<N;i++)
      {  double t = mcxNormalCut(radius, sdev)
      ;  if (t)
         fprintf(stdout, "%f\n", mean + t)
   ;  }
      return 0
;  }



