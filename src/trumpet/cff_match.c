/*            Copyright (C) 2003, 2004, 2005, 2006, 2007 Stijn van Dongen
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
#include "tingea/hash.h"
#include "tingea/ting.h"
#include "tingea/types.h"
#include "tingea/err.h"
#include "tingea/io.h"


int main
(  int   argc
,  char* argv[]
)
   {  mcxIO* xf
   ;  char* pat
   ;  mcxstatus stat

   ;  if (argc < 3)
      mcxDie(1, "match", "Usage: match <pat> <stream>")

   ;  xf = mcxIOnew(argv[2], "r")
   ;  pat = argv[1]
   ;  mcxIOopen(xf, EXIT_ON_FAIL)

   ;  stat =  mcxIOfind(xf, pat, RETURN_ON_FAIL)
   ;  fprintf(stdout, "%s! at pos %ld\n", stat ? "Zilch" : "Eureka", (long) xf->bc)

   ;  mcxIOfree(&xf)
   ;  return 0
;  }



