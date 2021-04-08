/*            Copyright (C) 2004, 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

#include <stdio.h>
#include <stdlib.h>

#include "tingea/list.h"


int main
(  int   argc
,  char* argv[]
)
   {  mcxLink* src = mcxListSource(2, MCX_GRIM_ARITHMETIC)
   ;  mcxLink* a = mcxLinkSpawn(src, "a")
   ;  mcxLink* b = mcxLinkAfter(a, "b")
   ;  mcxLink* d = mcxLinkAfter(b, "d")
   ;  mcxLink* c = mcxLinkAfter(b, "c")
   ;  mcxLink* e = mcxLinkAfter(d, "e")
   ;  mcxLink* x = mcxLinkBefore(a, "_")
   ;  mcxLink* t = c
   ;  int i = 0;

   ;  mcxLinkClose(e, x)

   ;  while (t && i < 20)
      {  if (t->val)
         fprintf(stdout, "link has value <%s>\n", (char*) t->val)
      ;  t = t->prev
      ;  i++
   ;  }

      mcxListFree(&src, NULL)
   ;  return 0
;  }



