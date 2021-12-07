/*            Copyright (C) 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tingea/rand.h"

static void print_bits
(  unsigned u
)
   {  char bits[32]
   ;  int i = 0
   ;  while (u || i < 32)
      {  bits[i++] = u & 1 ? '1' : '0'
      ;  u >>= 1
   ;  }
      for (i=31;i>=0;i--)
      putc(bits[i], stdout)
;  }


int main
(  int   argc
,  char* argv[]
)
   {  double x[1<<20]
#if 0
   ;  srandom(s)

   ;  while (i++ < n)
      {  unsigned s = mcxSeed(i)
      ;  printf("|  %12u ", s)
      ;  print_bits(s)
      ;  putc('\n', stdout)
      ;  if (m)
         {  int j = 0
         ;  srandom(s)
         ;  while (j++<m)
            {  unsigned u = random()
            ;  printf(">> %12ld ", u)
            ;  print_bits(u)
            ;  putc('\n', stdout)
         ;  }
         }
      }
#endif
   ;  return 0
;  }



