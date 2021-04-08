/*     Copyright (C) 2001, 2002, 2003, 2004, 2005 Stijn van Dongen
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
#include "tingea/io.h"
#include "tingea/opt.h"
#include "tingea/alloc.h"

#define FPTELL stderr

const char* usagelines[] =
{  "Usage: hashfile <file fname> [options]* [search-strings]*"
,  "   Options:"
,  "   -b <#buckets>, -lb <2log-of-#buckets>"
,  "        Number of buckets initially created."
,  "  -hf <dp|bj|ct|bd|ge|djb|elf|svd1|svd2|svd>"
,  "        Hash function to use:"
,  "        o  Daniel Philips (default)"
,  "        o  Bob Jenkins"
,  "        o  Chris Torek"
,  "        o  Berkely Databse"
,  "        o  GNU Emacs"
,  "        o  Dan Bernstein"
,  "        o  UNIX ELF"
,  "        o  Some random and less random attempts of mine"
,  "   -load <load>, -lload <2log-of-load>"
,  "        Hash doubles when *average* bucket size exceeds load."
,  "   --build    Exit after building hash."
,  "   --const    Disable hash growing (how un-Dutch)."
,  "   --walk     Walk entire hash after creation."
,  "   --show     Walk entire hash after creation, print all buckets."
,  "   -egg <string> Look for an egg dressed in string."
,  NULL
}  ;


int main
(  int   argc
,  char* argv[]
)
   {  int            a
   ;  int            ct          =  0
   ;  int            found       =  0
   ;  int            hashoptions =  0
   ;  int            n_buckets   =  1024
   ;  float          load        =  1.0
   ;  mcxbool        consthash   =  FALSE
   ;  mcxbool        buildonly   =  FALSE
   ;  mcxbool        walkhash    =  FALSE
   ;  mcxbool        searchegg   =  FALSE
   ;  mcxbool        sortit      =  FALSE
   ;  mcxbool        show        =  FALSE
   ;  mcxTing**      key_buf     =  NULL
   ;  int            window      =  0
   ;  const char*    pattern     =  "egg"
   ;  mcxTing*        eggs       =  mcxTingEmpty(NULL, 30)
   ;  mcxmode        rlmode      =  MCX_READLINE_CHOMP
   ;  u32            (*strhash)(const void* str)   =  mcxTingCThash
   ;  mcxTing        *line = mcxTingEmpty(NULL, 160)
   ;  mcxKV          *kv

   ;  mcxIO          *xf
   ;  mcxHash*       hash
   ;  mcxHashSettings   settings
   ;  const char* me = "hashfile"
   ;  u8             n_bits = 0
   ;  dim            n_lines = 0

   ;  if (argc < 2 || !strcmp(argv[1], "-h"))
      goto help

   ;  xf =  mcxIOnew(argv[1], "r")
   ;  mcxIOopen(xf, EXIT_ON_FAIL)

   ;  a = 2
   ;  while (a < argc)
      {  if (!strcmp(argv[a], "-b"))
         {  if (a++ + 1 <argc)
            n_buckets = atoi(argv[a]) 
         ;  else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "-lb"))
         {  if (a++ + 1 <argc)
            {  int   l_buckets = atoi(argv[a]) 
            ;  n_buckets   =  2
            ;  while (--l_buckets)
               n_buckets <<=  1
         ;  }
            else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "--const"))
         {  consthash = TRUE
      ;  }
         else if (!strcmp(argv[a], "--sort"))
         {  sortit = TRUE
      ;  }
         else if (!strcmp(argv[a], "--build"))
         {  buildonly = TRUE
      ;  }
         else if (!strcmp(argv[a], "--walk"))
         {  walkhash = TRUE
      ;  }
         else if (!strcmp(argv[a], "--show"))
         {  show = TRUE
         ;  walkhash = TRUE 
      ;  }
         else if (!strcmp(argv[a], "-window"))
         {  if (a++ + 1 <argc)
            window = atoi(argv[a]) 
         ;  else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "-egg"))
         {  if (a++ + 1 <argc)
            pattern = argv[a] 
         ;  else
            goto arg_missing
         ;  searchegg = TRUE
      ;  }
         else if (!strcmp(argv[a], "-load"))
         {  if (a++ + 1 <argc)
            load = atof(argv[a]) 
         ;  else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "-lload"))
         {  if (a++ + 1 <argc)
            {  int iload = 2
            ;  int x = atoi(argv[a]) 
            ;  while (--x)
               iload <<= 1
            ;  load = iload
         ;  }
            else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "-hf"))
         {  if (a++ + 1 <argc)
            {  if (!(strhash = mcxTingHFieByName(argv[a])))
               fprintf
               (  stderr
               ,  "[hashfile] hash option <%s> not supported\n", argv[a]
               )
               ,  exit(1)
         ;  }
            else
            goto arg_missing
      ;  }
         else if (!strcmp(argv[a], "-h"))
         {  help:
         ;  mcxUsage(stdout, me, usagelines)
         ,  exit(0)
      ;  }
         else if (0)
         {  arg_missing:
         ;  fprintf
            (  stdout
            ,  "[hashfile] Flag %s needs argument; see help (-h)\n" 
            ,  argv[argc-1]
            )
         ;  exit(1)
      ;  }
         else
         break

      ;  a++
   ;  }

      hash = mcxHashNew(n_buckets, strhash, mcxTingCmp)

   ;  if (consthash)
      hashoptions |= MCX_HASH_OPT_CONSTANT

   ;  mcxHashSetOpts(hash, load, hashoptions)

   ;  mcxHashGetSettings(hash, &settings)
   ;  n_buckets = settings.n_buckets

   ;  fprintf(FPTELL, "\n---> building hash ..\n")

   ;  if (window)
      key_buf = mcxAlloc(window * sizeof(mcxTing*), EXIT_ON_FAIL)

   ;  while(STATUS_OK == mcxIOreadLine(xf, line, rlmode))
      {  mcxTing* key   =  mcxTingNew(line->str)
      ;  mcxKV*   kv    =  mcxHashSearch(key, hash, MCX_DATUM_INSERT)
      ;  char*    ptrct

      ;  if (!kv)
         fprintf(FPTELL, ">>> >>> void kv for key <%s>!\n", key->str)

      ;  else if (!kv->key)
            fprintf(FPTELL, ">>> >>> void object for key <%s>!\n", key->str)
         ,  exit(1)

      ;  else if (kv->key != key)
         mcxTingFree(&key)

      ;  else if (strcmp(((mcxTing*)kv->key)->str, key->str))
         fprintf
         (  FPTELL
         ,  ">>> >>> diff key_i(%s) key_o(%s)\n"
         ,  ((mcxTing*)kv->key)->str
         ,  key->str
         )

      ;  if (key && window) 
         {  if (n_lines >= window)
            {  mcxTing* key2  =  key_buf[n_lines % window]
            ;  mcxKV* kv      =  mcxHashSearch(key2, hash, MCX_DATUM_DELETE)
            ;  if (!kv)
               fprintf
               (  FPTELL
               ,  ">>> >>> lost key_o(%s)\n"
               ,  key2->str
               )
            ;  mcxTingFree(&key2)
         ;  }
            key_buf[n_lines % window] = key
      ;  }

         n_lines++

      ;  ptrct = kv->val
      ;  ptrct++
      ;  kv->val = ptrct
      ;  ct++
   ;  }

      fprintf
      (FPTELL, "done building hash (%lu insertions, %lu lines).\n", (unsigned long) ct, (unsigned long) xf->lc)
   ;  mcxHashGetSettings(hash, &settings)
   ;  fprintf
      (  FPTELL
      ,  "hash stats %lu entries, %lu buckets initial, %lu buckets final\n"
      ,  (unsigned long) settings.n_entries
      ,  (unsigned long) n_buckets
      ,  (unsigned long) settings.n_buckets
      )
   ;  {  dim t = settings.n_buckets -1
      ;  while (t)
            n_bits++
         ,  t >>= 1
   ;  }
      fprintf
      (  FPTELL
      ,  "hash settings: [load %.3f] [mask %ld] [bits %d]\n"
      ,  settings.load
      ,  (long) (settings.n_buckets -1)
      ,  (int) n_bits
      )

   ;  if (buildonly)
      exit(0)

   ;  if (walkhash)
      {  mcxHashWalk*  walk = mcxHashWalkInit(hash)
      ;  dim bucketidx = -1, i_bucket = 0

      ;  fprintf(FPTELL, "---> walking hash ..\n")
      ;  ct =  0
      ;  found =  0

      ;  while((kv = mcxHashWalkStep(walk, &i_bucket)))
         {  ct++
         ;  if (mcxHashSearch(kv->key, hash, MCX_DATUM_FIND))
            found++
         ;  if (searchegg && strstr(((mcxTing*)kv->key)->str, pattern))
            fprintf(FPTELL, "() %s\n", ((mcxTing*)kv->key)->str)
         ;  else if (show)
            {  if (i_bucket != bucketidx)
               {  bucketidx =  i_bucket
               ;  fprintf(FPTELL, "-> %lu\n", (unsigned long) i_bucket)
            ;  }
               fprintf(FPTELL, "[%s]\n", ((mcxTing*)kv->key)->str)
         ;  }
      ;  }
         fprintf(FPTELL, "done walking hash (%d walked, %d found).\n", ct, found)
      ;  mcxHashWalkFree(&walk)
   ;  }

      while (a < argc)
      {  mcxTingWrite(eggs, argv[a])
      ;  kv                =  mcxHashSearch(eggs, hash, MCX_DATUM_FIND)

      ;  if (kv)
         fprintf(FPTELL, "---> present: <%s>\n", ((mcxTing*)kv->key)->str)
      ;  else
         fprintf(FPTELL, "--->  absent:  <%s>\n", eggs->str)

      ;  a++
   ;  }

      if (sortit)
      {  dim n_entries = 0, i
      ;  void** obs = mcxHashKVs(hash, &n_entries, mcxPKeyTingCmp, 0)
      ;  for (i=0;i<n_entries;i++)
         {  mcxKV*  kv   =    obs[i]
         ;  mcxTing* tg  =    kv->key
         ;  int     lct  =    ((char*) kv->val - (char*) 0)
         ;  while (--lct >= 0)
               fputs(tg->str, stdout)
            ,  fputc('\n', stdout)
      ;  }
      }

      mcxHashStats(FPTELL, hash)

   ;  fprintf(FPTELL, "---> deleting hash ..\n")
   ;  mcxHashFree(&hash, mcxTingRelease, NULL)
   ;  mcxFree(key_buf)
   ;  mcxIOfree(&xf)
   ;  mcxTingFree(&eggs)
   ;  mcxTingFree(&line)
   ;  fprintf(FPTELL, "done deleting hash.\n")
   ;  fprintf(FPTELL, "\n")

   ;  return 0
;  }



