/*            Copyright (C) 2008 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/


#include "tingea/ting.h"
#include "tingea/io.h"
#include "tingea/ding.h"
#include "tingea/heap.h"
#include "tingea/rand.h"
#include "tingea/array.h"
#include "tingea/err.h"
#include "tingea/tok.h"
#include "tingea/types.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>


static int cmp_long (const void* a, const void* b)
   {  long au = *((long*)a)
   ;  long bu = *((long*)b)
   ;  return au < bu ? -1 : au > bu ? 1 : 0
;  }

static const char* me = "unit_ceilfloor";


static long binarySearchCeil2(long a[], long n, long x)
{
    long lo = 0, hi = n;

    while(lo < hi)
    {
        long mid = lo + (hi - lo)/2;

        if(a[mid] < x) lo = mid + 1;
        else           hi = mid;
    }

    return hi;
}

static long binarySearchCeil(long a[],long n,long ele)
{
  long lower=0;
  long upper=n-1;

  long mid;
  long pos=-1;
  while(lower<=upper)
  {
    mid=(lower+upper)/2;
    if (a[mid]==ele)
    {
        while(mid>=0 && a[mid]==ele)
            mid--;
        return mid+1;
    }
    else if(a[mid]<ele)
        lower=mid+1;
    else
    {
        pos=mid;
        upper=mid-1;
    }
  }
  return pos;
}


static int mymain
(  int   argc
,  char* argv[]
)
   {  int N = atoi(argv[1])
   ;  int n = atoi(argv[2])
   ;  int k = atoi(argv[3])
   ;  int i
   ;  unsigned long seed = mcxSeed(getpid())
   ;  long* f = malloc(sizeof f[0] * N)
   ;  long* omega = f+N-1

   ;  fprintf(stdout, "seed %ld\n", seed)
   ;  srandom(seed)

   ;  if (n <= 0)
      mcxDie(1, me, "need nonnegative range, mfraid")

   ;  while (k-- > 0)
      {  fputc('.', stdout)

                           /* fill the array */
      ;  for (i=0;i<N;i++)
         f[i] = random() % n
      ;  qsort(f, N, sizeof f[0], cmp_long)

                           /* and search it for possible members */
      ;  for (i=0;i<n;i++)
         {  long pivot = i
                              /* largest <= pivot */
         ;  long* flr = mcxBsearchFloor(&pivot, f, N, sizeof f[0], cmp_long)

                              /* smallest >= pivot */
         ;  long* cl  = mcxBsearchCeil(&pivot, f, N, sizeof f[0], cmp_long)
         ;  long  ttt = binarySearchCeil2(f, N, pivot)

         ;  if (cl && ttt != cl - f)
            {  fprintf(stdout, "mine %d yours %d\n", (int) cl[0], (int) ttt)
            ;  for (i=0;i<N;i++)
               fprintf(stdout, " %d -> %d\n", (int) i, (int) f[i])
            ;  exit(0)
         ;  }

            if
            (  (flr && flr[0] > pivot)                /* does not satisfy condition */
            || (flr && flr < omega && flr[1] <= pivot)/* there is a larger one */
            || (!flr && N && f[0] <= pivot)           /* none found, but one exists */
            )
            mcxDie(1, "tst floor", "beu")

         ;  if
            (  (cl && cl[0] < pivot)                  /* does not satisfy condition */
            || (cl && cl > f && cl[-1] >= pivot)      /* there is a smaller one */
            || (!cl && N && f[0] >= pivot)            /* none found, but one exists */
            )
            mcxDie(1, "tst ceil", "beu")

         ;  if (flr && cl && flr > cl)                /* multiple identical elements */
            fprintf(stdout, "[%ld]", (long) (flr-cl))
      ;  }
         fputc('\n', stdout)
   ;  }
      return 0
;  }


int main
(  int argc
,  char* argv[]
)
   {  if (argc != 4)
      mcxDie(1, me, "need <COUNT> <RANGE> <REPEAT> arguments")
   ;  return mymain(argc, argv)
;  }


