/*            Copyright (C) 2004, 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/


#include "tingea/ting.h"
#include "tingea/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char* bar = "bar";

int main
(  int   argc
,  char* argv[]
)
   {  int fni = fileno(stdin)
   ;  int fno = fileno(stdout)
   ;  int ttyi= isatty(fni)
   ;  int ttyo= isatty(fno)
   ;  const char* mode = NULL

   ;  fprintf(stderr, "STDIN  fileno=%d isatty=%d\n", fni, ttyi)
   ;  fprintf(stderr, "STDOUT fileno=%d isatty=%d\n", fno, ttyo)

   ;  mode =  fseek(stdin, 0, SEEK_CUR) ? "unseekable" : "seekable"
   ;  fprintf(stderr, "STDIN %s for length 0\n", mode)

   ;  return 0
;  }

