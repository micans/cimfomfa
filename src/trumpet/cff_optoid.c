/*            Copyright (C) 2005 Stijn van Dongen
 *
 * This file is part of cimfomfa.  You can redistribute and/or modify cimfomfa
 * under the terms of the GNU General Public License; either version 2 of the
 * License or (at your option) any later version.  You should have received a
 * copy of the GPL along with cimfomfa, in the file COPYING.
*/

/* Description:
 *   template file for tingea option processing.
*/


#include <stdio.h>
#include <stdlib.h>

#include "tingea/ting.h"
#include "tingea/opt.h"
#include "tingea/io.h"
#include "tingea/err.h"

enum
{  MY_OPT_EGG
,  MY_OPT_OUTPUT

,  MY_OPT_VERSION = MY_OPT_OUTPUT + 2
,  MY_OPT_HELP
,  MY_OPT_APROPOS
,  MY_OPT_007
}  ;


static const char* me    =  "optoid";

const char* syntax = "optoid [-a <num>]";

mcxOptAnchor options[] =
{  {  "--apropos"
   ,  MCX_OPT_DEFAULT | MCX_OPT_INFO
   ,  MY_OPT_APROPOS
   ,  NULL
   ,  "print this help"
   }
,  {  "-h"
   ,  MCX_OPT_DEFAULT | MCX_OPT_INFO
   ,  MY_OPT_HELP
   ,  NULL
   ,  "print this help"
   }
,  {  "-a"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_EGG
   ,  "<egg>"
   ,  "new answer"
   }
,  {  "-o"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_OUTPUT
   ,  "<fname>"
   ,  "output file name"
   }
,  {  "-v"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_VERSION
   ,  NULL
   ,  "print version and exit"
   }
,  {  "--007"
   ,  MCX_OPT_DEFAULT | MCX_OPT_HIDDEN
   ,  MY_OPT_007
   ,  NULL
   ,  "\tMi\tDshow *all* options"
   }
,  {  NULL
   ,  0
   ,  0
   ,  NULL
   ,  NULL
   }
}  ;


int main
(  int   argc
,  char* argv[]
)
   {  mcxIO*   xfout    =  mcxIOnew("-", "w")
   ;  mcxstatus status  =  STATUS_FAIL
   ;  int i, j

   ;  mcxstatus parseStatus = STATUS_OK
   ;  mcxOption* opts, *opt
   ;  int n_arg_read = 0
   ;  int n_arg_trailing = 0
   ;  const char* statement = "mind and language are a double helix"

   ;  mcxOptAnchorSortById(options, sizeof(options)/sizeof(mcxOptAnchor) -1)

   ;  if ( !(
      opts =
      mcxOptExhaust
      (options, (char**) argv, argc, 1, &n_arg_read, &parseStatus)
         )  )
      exit(0)

    /*
      opts = mcxOptParse(options, (char**) argv, argc, 1, 0, &parseStatus)
    */

   ;  for (opt=opts;opt->anch;opt++)
      {  mcxOptAnchor* anch = opt->anch

      ;  switch(anch->id)
         {  case MY_OPT_HELP
         :  case MY_OPT_APROPOS
         :  mcxOptApropos(stdout, me, syntax, 20, MCX_OPT_DISPLAY_SKIP, options)
         ;  return 0
         ;

            case MY_OPT_007
         :  mcxOptApropos
            (  stdout, me, syntax, 20
            ,  MCX_OPT_DISPLAY_SKIP | MCX_OPT_DISPLAY_HIDDEN
            ,  options
            )
         ;  return 0
         ;

            case  MY_OPT_EGG
         :  statement = opt->val
         ;  break
         ;

            case  MY_OPT_OUTPUT
         :  mcxIOrenew(xfout, opt->val, "w")
         ;  break
         ;

            case  MY_OPT_VERSION
         :  fprintf(stdout, "0\n")
         ;  break
         ;
         }
      }

      n_arg_trailing = argc - n_arg_read - 1

   ;  if (mcxIOopen(xfout, RETURN_ON_FAIL))
      {  mcxErr(me, "cannot open file %s", xfout->fn->str)
      ;  mcxIOfree(&xfout)
      ;  return 1
   ;  }

      fprintf(xfout->fp, "first of all: %s.\n", statement)

   ;  for (i=n_arg_read+1,j=2;i<argc;i++,j++)
      fprintf(xfout->fp, "%12d: %s.\n", j, argv[i])

   ;  mcxIOclose(xfout)
   ;  mcxIOfree(&xfout)
   ;  return 0
;  }



