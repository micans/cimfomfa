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
#include "tingea/tr.h"


enum
{  MY_OPT_SRC
,  MY_OPT_DST
,  MY_OPT_DEL
,  MY_OPT_SQS
,  MY_OPT_SUB
,  MY_OPT_DEBUG
,  MY_OPT_HELP = MY_OPT_SUB + 2
,  MY_OPT_APROPOS
}  ;


static const char* me  =  "tring";

const char* syntax
=  "tring [-x <spec> -y <spec>] [-d <spec>] [-s <spec>] [-a <string>]";

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
   ,  MY_OPT_SUB
   ,  "<subject>"
   ,  "tring that"
   }
,  {  "-d"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_DEL
   ,  "<spec>"
   ,  "delete spec"
   }
,  {  "-debug"
   ,  MCX_OPT_DEFAULT
   ,  MY_OPT_DEBUG
   ,  NULL
   ,  "debug mode"
   }
,  {  "-s"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_SQS
   ,  "<spec>"
   ,  "squash spec"
   }
,  {  "-x"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_SRC
   ,  "<spec>"
   ,  "source spec"
   }
,  {  "-y"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_DST
   ,  "<spec>"
   ,  "dest spec"
   }
,  {  NULL
   ,  0
   ,  0
   ,  NULL
   ,  NULL
   }
}  ;


const char* printable =
   "abcdefghijklmnopqrstuvwxyz"  \
               "~!@#$%^&*()_+"   \
                "0123456789-="   \
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"   \
                         " \t"   \
          "[]\\{}|;':\",./<>?" ;


int main
(  int   argc
,  char* argv[]
)
   {  mcxstatus status  =  STATUS_FAIL
   ;  mcxTing* subject  =  mcxTingNew(printable)
   ;  mcxTing* splash   =  NULL
   ;  mcxTR tr
   ;  int i, j

   ;  int n_arg_trailing = 0
   ;  const char* src = NULL
   ;  const char* dst = NULL
   ;  const char* del = NULL
   ;  const char* sqs = NULL

   ;  mcxstatus parseStatus = STATUS_OK
   ;  mcxOption* opts, *opt

   ;  mcxOptAnchorSortById(options, sizeof(options)/sizeof(mcxOptAnchor) -1)

   ;  if
      (!(opts = mcxOptParse(options, (char**) argv, argc, 1, 0, &parseStatus)))
      exit(0)

   ;  for (opt=opts;opt->anch;opt++)
      {  mcxOptAnchor* anch = opt->anch

      ;  switch(anch->id)
         {  case MY_OPT_HELP
         :  case MY_OPT_APROPOS
         :  mcxOptApropos(stdout, me, syntax, 20, MCX_OPT_DISPLAY_SKIP, options)
         ;  return 0
         ;

            case MY_OPT_DEBUG
         :  mcx_tr_debug = TRUE
         ;  break
         ;

            case MY_OPT_SUB
         :  mcxTingWrite(subject, opt->val)
         ;  break
         ;

            case MY_OPT_DST
         :  dst = opt->val 
         ;  break
         ;

            case MY_OPT_DEL
         :  del = opt->val 
         ;  break
         ;

            case MY_OPT_SQS
         :  sqs = opt->val 
         ;  break
         ;

            case MY_OPT_SRC
         :  src = opt->val 
         ;  break
         ;
         }
      }

   ;  status = mcxTRloadTable(&tr, src, dst, del, sqs, 0)
   ;  if (status)
      {  if (mcx_tr_err)
         mcxErr(me, "load error: %s", mcx_tr_err)
      ;  return 1
   ;  }

      splash = mcxTRsplash(&tr, MCX_TR_SOURCE | MCX_TR_SQUASH | MCX_TR_DELETE)
   ;  fprintf(stdout, "splash [%s]\n", splash->str)

   ;  subject->len = mcxTRtranslate(subject->str, &tr)
   ;  fprintf(stdout, "result [%s]\n", subject->str) 

   ;  return 0
;  }



