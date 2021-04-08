
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tingea/getpagesize.h"

int main
(  int argc
,  char* argv[]
)
   {  size_t ps = getpagesize()
   ;  fprintf(stdout, "%zd\n", ps)
   ;  return 0
;  }

