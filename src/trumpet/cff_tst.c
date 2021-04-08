/*            Copyright (C) 2005 Stijn van Dongen
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

int main
(  int   argc
,  char* argv[]
)
   {  mcxTing* t = mcxTingNew(argv[1])
   ;  const char* str = argv[2]
   ;  unsigned ofs = atoi(argv[3])
   ;  mcxTingSplice(t, str, ofs, 0, 5)
   ;  fprintf(stdout, "%s\n", t->str)
   ;  return 0
;  }




