
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tingea/types.h"
#include "tingea/hash.h"
#include "tingea/ting.h"
#include "tingea/types.h"
#include "tingea/io.h"
#include "tingea/opt.h"
#include "tingea/err.h"
#include "tingea/alloc.h"

#define FPTELL stderr


/* TODO: diagnostic output: reduced != ignored,
 * because reduce takes into account max_genome, whereas ignored does not.
*/

const char* usagelines[] =
{  "Usage: reap <fasta-file> <ssaha-file>"
,  NULL
}  ;


static const char* me = "reap";
const char* syntax = "reap -fasta-sl <fname> -ssaha <fname> [options]";

unsigned basemap[256] = { 0 };

static void basemap_init ( void )
   {  dim i
   ;  for (i=0;i<256;i++)
      basemap[i] = 5
   ;  basemap['A'] = basemap['a'] = 0
   ;  basemap['C'] = basemap['c'] = 1
   ;  basemap['G'] = basemap['g'] = 2
   ;  basemap['T'] = basemap['t'] = 3
   ;  basemap['U'] = basemap['u'] = 3     /* everything < 4 is coding */
   ;  basemap['N'] = basemap['n'] = 4     /* masked */
   ;  basemap['X'] = basemap['x'] = 4     /* masked */

   ;  basemap['\n'] = 8    /* bit 8 set */
   ;  basemap['\r'] = 9    /* bit 8 set */
;  }


typedef struct
{  dim      depth
;  dim      length
;
}  readattr ;


typedef struct
{  unsigned min_alnlen
;  float    min_identity
;  float    min_alnratio
;  unsigned max_gaplen
;  unsigned max_start
;  int      do_filter
;  int      do_divide
;  unsigned max_genome
;  const char* bugme
;
}  readpar  ;


                              /* if we ever keep everything in memory, then we
                               * could economize by taking members out from chrhit below.
                              */
typedef struct
{  unsigned    index
;  unsigned    start
;  unsigned    end
;  unsigned    score          /* currently redundant after first filtering */
;  float       identity       /* currently redundant after first filtering */
;  unsigned    depth          /* copied from seqattr, so currently redundant */
;  unsigned    length         /* copied from seqattr, so currently redundant */
;  unsigned    n_places       /* number of places where it maps */
;  char        revstrand
;
}  chrhit   ;


typedef struct
{  unsigned    pos            /* zero means unused */
;  float       count_fwd
;  float       count_rev
;
}  hcell       ;


static int cmp_track_position(const void* k1, const void* k2)
   {  const hcell* h1 = k1, *h2 = k2
   ;  if (!h1->pos)
      return h2->pos ? 1 : (h1 > h2)
   ;  if (!h2->pos)
      return -1
   ;  return h1->pos < h2->pos ? -1 : 1
;  }


typedef struct
{  hcell*      cells          /* size hmask + 1 */
;  unsigned    hmask          /* at least 1<<14 */
;  dim         n_used
;  
}  harray      ;


#define hashwell(position, mask)    (position & mask)
#define HASH_JUMP 34567          /* should be coprime with mask */


#define HSEARCH(lvalue, cells, themask, thepos)    \
   {  unsigned hhh = hashwell(thepos, themask)     \
   ;  dim jj = 0                                   \
   ;  while (cells[hhh].pos != 0 && cells[hhh].pos != thepos)     \
      {  hhh += HASH_JUMP                          \
      ;  hhh &= themask                            \
      ;  if (jj++ > themask)                       \
         mcxDie(1, "hash", "error")                \
   ;  }                                            \
      lvalue = cells + hhh                         \
   ;  if (!cells[hhh].pos)                         \
      cells[hhh].pos = thepos                      \
;  }

static dim count_max = 0;

#define HINC(ha, thepos, revstrand, depth)         \
   {  hcell* ce                                    \
   ;  HSEARCH(ce, ha->cells, ha->hmask, thepos)    \
   ;  if (!ce->count_fwd && !ce->count_rev)        \
      ha->n_used++                                 \
   ;  if (revstrand)                               \
      ce->count_rev += depth                       \
   ;  else                                         \
      ce->count_fwd += depth                       \
;  }


#define WIDTH 3

static void harray_new(harray* h)
   {  h->hmask    =  (1<<WIDTH) - 1
   ;  h->cells    =  mcxAlloc(sizeof h->cells[0] * (h->hmask+1), EXIT_ON_FAIL)
   ;  h->n_used   =  0
   ;  memset(h->cells, 0, sizeof h->cells[0] * (h->hmask+1))
;  }


static void harray_double(harray* ha, dim n_new,  const char* name)
   {  unsigned newmask = 2 * ha->hmask + 1
   ;  hcell* newcells
   ;  dim j

   ;  while (newmask < 2 * (ha->n_used + n_new))
      newmask = 2 * newmask + 1

   ;  newcells = mcxAlloc(sizeof newcells[0] * (newmask+1), EXIT_ON_FAIL)  /* fixme 4G limit */
   ;  memset(newcells, 0, sizeof newcells[0] * (newmask+1))

   ;  if (ha->hmask + 1 >= 1 << 31)
      mcxDie(1, me, "sequence %s too big", name)

   ;  for (j=0;j<ha->hmask+1;j++)
      {  dim p = ha->cells[j].pos
      ;  if (p > 0)
         {  hcell* c
         ;  HSEARCH(c, newcells, newmask, p)
         ;  c[0] = ha->cells[j]
      ;  }
      }
      free(ha->cells)
   ;  ha->cells = newcells
   ;  ha->hmask = newmask
;  }


typedef struct
{  mcxTing*    name
;  harray      incident
;  dim         n_mapped
;  dim         n_ignored
;
}  chrtrack    ;              /* not a brilliant name, no */


typedef struct
{  chrtrack*   tracks
;  dim         n_track
;  chrhit*     hits
;  dim         n_hit
;
}  hitfilter   ;


static int cmp_tracks_name(const void* n1, const void* n2)
   {  const chrtrack* t1 = n1, *t2 = n2
   ;  const mcxTing*  s1 = t1->name, *s2 = t2->name
   ;  if (s1->len != s2->len)
      return s1->len < s2->len ? -1 : 1
   ;  return strcmp(s1->str, s2->str)
;  }



static dim reduce_single_query(chrhit* hits, dim n_hit, chrtrack* track, int debug)
   {  dim best_length   =  0
   ;  float best_identity =  0.0
   ;  chrhit*  dst      =  hits
   ;  dim j

   ;  for (j=0;j<n_hit;j++)
      {  chrhit* src = hits+j
      ;  if (src->length > best_length)  best_length = src->length
      ;  if (src->identity > best_identity ) best_identity = src->identity
   ;  }

if (debug) fprintf(stderr, "------> %d %f\n", (int) best_length, (float) best_identity);
      for (j=0;j<n_hit;j++)
      {  chrhit* src = hits+j
      ;  const char* ok = "discard"
      ;  if
         (  (src->length == best_length)
         || (src->length+1 == best_length && src->identity >= best_identity)
         )
         {  if (dst != src)
            dst[0] = src[0]
         ;  dst++
         ;  ok = "pass"
      ;  }
         else
         track[src->index].n_ignored++
;if (debug) fprintf(stderr, "-> test %d %f (%s)\n", src->length, (float) src->identity, ok);
   ;  }
      return (dst - hits)
;  }


static void accumulate_tracks(hitfilter* flt, int divide)
   {  chrhit* hits = flt->hits
   ;  dim n_hit = flt->n_hit
   ;  chrtrack* tracks = flt->tracks
   ;  dim j

   ;  mcxTell("track", "begin accumulating reads")

   ;  for (j=0;j<n_hit;j++)
      {  chrhit* hit = hits+j
      ;  harray* ha = &tracks[hit->index].incident
      ;  dim n_new = hit->end - hit->start + 1
      ;  float delta = hit->depth
      ;  dim i

      ;  if (divide) delta /= hit->n_places
      ;  if (delta < 0.01001) delta = 0.01001

      ;  if ((ha->n_used + n_new) * 2 > ha->hmask)
         harray_double(ha, n_new, tracks->name->str)
      ;  for (i=hit->start; i<=hit->end; i++)
         HINC(ha, i, hit->revstrand, delta)
      ;  tracks[hit->index].n_mapped++
   ;  }
   }



enum
{  MY_OPT_FASTASL
,  MY_OPT_SSAHA
,  MY_OPT_OUTPUT
,  MY_OPT_DIVIDE
,  MY_OPT_HELP
,  MY_OPT_APROPOS
,  MY_OPT_MINIDENTITY
,  MY_OPT_MINALNRATIO
,  MY_OPT_MINALNLEN
,  MY_OPT_MAXGAPLEN
,  MY_OPT_MAXSTART
,  MY_OPT_MAXGENOME
,  MY_OPT_NOFILTER
,  MY_OPT_BUGME
}  ;


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
,  {  "--filter-off"
   ,  MCX_OPT_DEFAULT
   ,  MY_OPT_NOFILTER
   ,  NULL
   ,  "turn off all filtering modes"
   }
,  {  "--divide-depth"
   ,  MCX_OPT_DEFAULT
   ,  MY_OPT_DIVIDE
   ,  NULL
   ,  "divide read depth over hits"
   }
,  {  "-max-genome"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MAXGENOME
   ,  "<num>"
   ,  "maximum allowed hits to genome (0 denotes 0~0)"
   }
,  {  "-max-start"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MAXSTART
   ,  "<num>"
   ,  "maximum allowed start position"
   }
,  {  "-max-gap-len"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MAXGAPLEN
   ,  "<num>"
   ,  "maximum total gap length"
   }
,  {  "-min-aln-ratio"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MINALNRATIO
   ,  "<num>"
   ,  "minimum ratio of aligned sequence"
   }
,  {  "-min-identity"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MINIDENTITY
   ,  "<num>"
   ,  "minimum identity"
   }
,  {  "-min-aln-len"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_MINALNLEN
   ,  "<num>"
   ,  "minimum alignment length"
   }
,  {  "-fasta-sl"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_FASTASL
   ,  "<fname>"
   ,  "fasta file for reads, single line for a sequence, depth_<num> annotation"
   }
,  {  "-trace"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_BUGME
   ,  "<query-id>"
   ,  "emit diagnostics for query-id"
   }
,  {  "-o"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_OUTPUT
   ,  "<fname>"
   ,  "output file"
   }
,  {  "-ssaha"
   ,  MCX_OPT_HASARG
   ,  MY_OPT_SSAHA
   ,  "<fname>"
   ,  "ssaha result file"
   }
,  {  NULL
   ,  0
   ,  0
   ,  NULL
   ,  NULL
   }
}  ;


static void read_fasta(mcxIO* xffasta, mcxHash* seqattr)
   {  int expect_id  =  1
   ;  readattr*  attr=  NULL
   ;  mcxTing  *line =  mcxTingEmpty(NULL, 160)
   ;  dim n_read = 0

   ;  while(STATUS_OK == mcxIOreadLine(xffasta, line, MCX_READLINE_CHOMP))
      {  mcxTing* key = NULL
      ;  mcxKV*   kv = NULL

      ;  if (expect_id)
         {  dim n_char_id
         ;  ulong depth
         ;  const char* wrong = NULL

         ;  do    /* fixme funcify */
            {  if ((uchar) line->str[0] != '>')
               {  wrong = "expect identifier"; break; }
            ;  if (!(n_char_id = strcspn(line->str+1, " \t\r\n")))
               {  wrong = "expect valid identifier"; break; }
            ;  if (1 != sscanf(line->str+1+n_char_id, " depth_%lu", &depth))
               {  wrong = "no depth"; break; }
         ;  }
            while(0)
         ;  if (wrong)
            mcxDie(1, me, "%s at line %lu", wrong, (ulong) xffasta->lc)

         ;  key = mcxTingNNew(line->str+1, n_char_id)
         ;  kv =  mcxHashSearch(key, seqattr, MCX_DATUM_INSERT)
         ;  if (kv->key != key)
            mcxDie(1, me, "sequence <%s> already present", key->str)
         ;  if (!(kv->val = attr = malloc(sizeof attr[0])))
            mcxDie(1, me, "unable to malloc")
         ;  attr->depth = depth
         ;  expect_id = 0
         ;  n_read++
      ;  }
         else
         {  dim length = line->len
         ;  while (length && basemap[(uchar) line->str[length-1]] >= 4)
            length--
         ;  attr->length = length
         ;  expect_id = 1
      ;  }
      }
      mcxTell("fasta read", "parsed %lu identifier/sequence pairs", (ulong) n_read)
;  }


static void read_ssaha(mcxIO* xfssaha, mcxHash* seqattr, readpar par, hitfilter* flt)
   {  mcxTing  *line    =  mcxTingEmpty(NULL, 160)
   ;  mcxTing  *idquery =  mcxTingEmpty(NULL, 160)
   ;  mcxTing  *idtrack =  mcxTingEmpty(NULL, 160)
   ;  dim n_skip_reduce = 0, n_skip_gap = 0, n_skip_start = 0, n_skip_length = 0, n_skip_identity = 0, n_skip_ratio = 0
   ;  dim n_skip_union = 0
   ;  dim n_taken = 0
   ;  mcxHash  *chrmap   =  mcxHashNew(1<<12, mcxTingCThash, mcxTingCmp)

   ;  const char* query_previous = NULL
   ;  dim      n_hit   =  0
   ;  dim      ofs_hits =  0
   ;  dim      n_hit_alloc =  8
   ;  chrhit*  hits     =  mcxAlloc(n_hit_alloc * sizeof hits[0], EXIT_ON_FAIL)

   ;  dim      n_track  =  0, track_id = 0
   ;  dim      n_track_alloc =  1<<16
   ;  chrtrack*  tracks =  mcxAlloc(n_track_alloc * sizeof tracks[0], EXIT_ON_FAIL)

   ;  while(STATUS_OK == mcxIOreadLine(xfssaha, line, MCX_READLINE_CHOMP))
      {  ulong score
      ;  ulong qstart, qend, rstart, rend
      ;  char str[2]
      ;  int revstrand
      ;  ulong alnlen
      ;  float identity
      ;  mcxKV* kvtrack, *kvquery
      ;  readattr* qattr

      ;  mcxTingEnsure(idquery, line->len)
      ;  mcxTingEnsure(idtrack, line->len)

      ;  if
         (  10
         != sscanf
            (  line->str
            ,  "%*s %lu %s %s %lu %lu %lu %lu %[CF+-] %lu %f"
            ,  &score
            ,  idquery->str, idtrack->str
            ,  &qstart, &qend
            ,  &rstart, &rend
            ,  (char*) &str     /* dangersign fixme string cast */
            ,  &alnlen
            ,  &identity
            )
         )
         mcxDie(1, me, "no match at line %lu", (ulong) xfssaha->lc)
;if(0)fprintf(stderr, "query %s track %s rstart %d\n", idquery->str, idtrack->str, (int) rstart)
      ;  idquery->len = strlen(idquery->str)
      ;  idtrack->len = strlen(idtrack->str)
      ;  revstrand = str[0] == 'F' ? 0 : 1

      ;  kvquery = mcxHashSearch(idquery, seqattr, MCX_DATUM_FIND)
      ;  if (!kvquery)
         mcxDie(1, me, "cannot find query <%s>", idquery->str)
      ;  qattr = kvquery->val

      ;  if (rstart > rend) { ulong t = rend; rend = rstart; rstart = t; }
      ;  if (qstart > qend) { ulong t = qend; qend = qstart; qstart = t; }

      ;  if (par.do_filter)
         {  const char* skip = NULL
         ;  do
            {  if (alnlen < par.min_alnlen)
               {  n_skip_length++
               ;  skip = "minimum alignment length threshold"
            ;  }
               if (identity < par.min_identity)
               {  n_skip_identity++
               ;  skip = "identity threshold"
            ;  }
               if (1.0 * alnlen * 1.0 / qattr->length < par.min_alnratio)
               {  n_skip_ratio++
               ;  skip = "aligned fraction"
            ;  }
               if (alnlen != rend - rstart + 1)
               {  n_skip_gap++
               ;  skip = "gap in alignment"
            ;  }
               if (qstart > par.max_start)
               {  n_skip_start++
               ;  skip = "start threshold"
            ;  }
            }
            while (0)
         ;  if (skip)
            {  if (par.bugme && !strcmp(par.bugme, idquery->str))
               mcxTell(me, "skipping query %s: %s", par.bugme, skip)
            ;  continue
         ;  }
         }

         kvtrack = mcxHashSearch(idtrack, chrmap, MCX_DATUM_INSERT)

      ;  if (!n_hit)
         query_previous = ((mcxTing*) kvquery->key)->str

      ;  if (kvtrack->key == idtrack)                /* newly inserted */
         {  kvtrack->val = ULONG_TO_VOID n_track
         ;  idtrack = mcxTingEmpty(NULL, idtrack->mxl)
         ;  track_id = n_track
         ;  if (n_track >= n_track_alloc)
               n_track_alloc *= 1.4
            ,  tracks = mcxRealloc(tracks, n_track_alloc * sizeof hits[0], EXIT_ON_FAIL)
         ;  tracks[n_track].name = kvtrack->key      /* dangersign: shallow copy */
         ;  harray_new(&tracks[n_track].incident)
         ;  tracks[n_track].n_mapped = 0
         ;  tracks[n_track].n_ignored = 0
         ;  n_track++
      ;  }
         else                                        /* already exists */
         track_id = VOID_TO_ULONG kvtrack->val

      ;  if (strcmp(query_previous, ((mcxTing*) (kvquery->key))->str))      /* kvquery->key->str is persistent */
         {  dim j
         ;  int debug = par.bugme && !strcmp(query_previous, par.bugme)
                                                      /* fixme: check offset arithmetic */
         ;  n_taken = reduce_single_query(hits + ofs_hits, n_hit - ofs_hits, tracks, debug)
;if (debug) fprintf(stderr, "for seq_13707 have %d from %d\n", (int) n_taken, (int) (n_hit -ofs_hits))
         ;  if (par.max_genome && n_taken > par.max_genome)
            n_taken = 0

         ;  query_previous = ((mcxTing*) (kvquery->key))->str
         ;  for (j=0;j<n_taken;j++)
            hits[ofs_hits+j].n_places = n_taken
;if(0)fprintf(stderr, "%d taken from %d\n", (int) n_taken, (int) (n_hit - ofs_hits))
         ;  n_skip_reduce += n_hit - ofs_hits - n_taken
         ;  n_hit   =  ofs_hits + n_taken
         ;  ofs_hits =  n_hit
      ;  }

if(0)fprintf(stderr, "%d %s %s\n", (int) n_hit, query_previous, ((mcxTing*) (kvquery->key))->str);

         {  if (n_hit >= n_hit_alloc)
               n_hit_alloc *= 1.4
            ,  hits = mcxRealloc(hits, n_hit_alloc * sizeof hits[0], EXIT_ON_FAIL)
         ;  hits[n_hit].index = track_id
;if(0)fprintf(stderr, "write to track %s\n", tracks[track_id].name->str)
         ;  hits[n_hit].start = rstart
         ;  hits[n_hit].end   = rend
         ;  hits[n_hit].revstrand= revstrand
         ;  hits[n_hit].depth = qattr->depth
         ;  hits[n_hit].length= alnlen
         ;  hits[n_hit].score = score
         ;  hits[n_hit].identity = identity
         ;  n_hit++
      ;  }
      }

      {  dim j                      /* fixme duplication ugly. */
      ;  n_taken
         =  reduce_single_query
            (  hits + ofs_hits
            ,  n_hit - ofs_hits
            ,  tracks
            ,  par.bugme && !strcmp(query_previous, par.bugme)
            )
      ;  if (par.max_genome && n_taken > par.max_genome)
         n_taken = 0
      ;  for (j=0;j<n_taken;j++)
         hits[ofs_hits+j].n_places = n_taken

      ;  n_skip_reduce += n_hit - ofs_hits - n_taken
      ;  n_hit = ofs_hits + n_taken
      ;  n_skip_union = n_skip_gap + n_skip_start + n_skip_length + n_skip_identity + n_skip_ratio
   ;  }

      mcxTell
      (  "ssaha read"
      ,  "from %lu lines, kept %lu reduced %lu skipped %lu"
      ,  (ulong) xfssaha->lc
      ,  (ulong) n_hit
      ,  (ulong) n_skip_reduce
      ,  (ulong) n_skip_union
      )
   ;  mcxTell
      (  "ssaha read"
      ,  "skipped length=%lu ident=%lu ratio=%lu gap=%lu, start=%lu"
      ,  (ulong) n_skip_length
      ,  (ulong) n_skip_identity
      ,  (ulong) n_skip_ratio
      ,  (ulong) n_skip_gap
      ,  (ulong) n_skip_start
      )
   ;  mcxTell("ssaha read", "found %lu tracks", (ulong) n_track)

   ;  flt->n_track=  n_track
   ;  flt->tracks =  tracks
   ;  flt->hits   =  hits
   ;  flt->n_hit  =  n_hit
;  }


static void destroy_and_write_tracks(hitfilter* flt, mcxIO* xfout)
   {  dim i, bases_used = 0, reads_ignore = 0, reads_mapped = 0
   ;  for (i=0;i<flt->n_track;i++)
      {  dim u =  flt->tracks[i].incident.n_used
      ;  dim m =  flt->tracks[i].n_mapped
      ;  dim ig=  flt->tracks[i].n_ignored
      ;  bases_used  += u
      ;  reads_mapped+= m
      ;  reads_ignore+= ig
   ;  }

      mcxTell
      (  "track"
      ,  "writing (%d bases mapped, %d reads mapped, %d reads ignored)"
      ,  (int) bases_used
      ,  (int) reads_mapped
      ,  (int) reads_ignore
      )

                     /* WARNING: SORTING THE TRACKS IS A DESTRUCTIVE OPERATION: HASH
                      * is no longer usable as a hash.
                     */
   ;  for (i=0;i<flt->n_track;i++)
      {  dim track_size = 0, j = 0
      ;  hcell* hc = flt->tracks[i].incident.cells
      ;  const char* curname = flt->tracks[i].name->str

      ;  qsort
         (  flt->tracks[i].incident.cells
         ,  flt->tracks[i].incident.hmask+1
         ,  sizeof(hcell)
         ,  cmp_track_position
         )

      ;  while (hc->pos > 0)
         {  fprintf
            (  xfout->fp
            ,  "%s:%lu\t%lu\t%lu\n"
            ,  curname
            ,  (ulong) hc->pos
            ,  (ulong) (hc->count_fwd + 0.99)
            ,  (ulong) (hc->count_rev + 0.99)
            )
         ;  track_size++
         ;  hc++
      ;  }
         if (track_size != flt->tracks[i].incident.n_used)
         mcxDie(1, me, "inconsistent mapped base count after sorting, track <%s>", flt->tracks[i].name->str)
   ;  }
   }


int main
(  int   argc
,  char* argv[]
)
   {  u32     (*strhash)(const void* str)   =  mcxTingCThash
   ;  mcxmode   rlmode  =  MCX_READLINE_CHOMP
   ;  mcxKV    *kv

   ;  mcxIO    *xffasta =  NULL
   ;  mcxIO    *xfssaha  =  NULL
   ;  mcxIO    *xfout   =  mcxIOnew("-", "w")

   ;  mcxHash  *seqattr  =  mcxHashNew(1<<16, strhash, mcxTingCmp)

   ;  mcxstatus parseStatus = STATUS_OK
   ;  mcxOption* opts, *opt
   ;  int n_arg_read = 0
   ;  int n_arg_trailing = 0

   ;  int a

   ;  readpar  par      =  { 0 }
   ;  hitfilter flt     =  { NULL, 0, NULL, 0 }

   ;  par.min_alnlen    =  15
   ;  par.min_identity  =  90.0
   ;  par.min_alnratio  =  0.6
   ;  par.max_gaplen    =  0
   ;  par.max_start     =  2
   ;  par.do_filter     =  1
   ;  par.do_divide     =  0
   ;  par.max_genome    =  100
   ;  par.bugme         =  NULL

   ;  basemap_init()

   ;  mcxOptAnchorSortById(options, sizeof(options)/sizeof(mcxOptAnchor) -1)
   ;  opts = mcxOptParse(options, (char**) argv, argc, 1, 0, &parseStatus)

   ;  if (parseStatus != STATUS_OK)
      mcxDie(1, me, "initialization failed")

   ;  for (opt=opts;opt->anch;opt++)
      {  mcxOptAnchor* anch = opt->anch

      ;  switch(anch->id)
         {  case MY_OPT_HELP
         :  case MY_OPT_APROPOS
         :  mcxOptApropos(stdout, me, syntax, 20, MCX_OPT_DISPLAY_SKIP, options)
         ;  return 0
         ;

            case  MY_OPT_MINALNRATIO
         :  par.min_alnratio = atof(opt->val)
         ;  break
         ;

            case  MY_OPT_MINIDENTITY
         :  par.min_identity = atof(opt->val)
         ;  break
         ;

            case  MY_OPT_MINALNLEN
         :  par.min_alnlen = atoi(opt->val)
         ;  break
         ;

            case  MY_OPT_MAXGAPLEN
         :  par.max_gaplen = atoi(opt->val)
         ;  break
         ;

            case  MY_OPT_MAXGENOME
         :  par.max_genome = atoi(opt->val)
         ;  break
         ;

            case  MY_OPT_MAXSTART
         :  par.max_start = atoi(opt->val)
         ;  break
         ;

            case  MY_OPT_DIVIDE
         :  par.do_divide = 1
         ;  break
         ;

            case  MY_OPT_BUGME
         :  par.bugme = opt->val
         ;  break
         ;

            case  MY_OPT_NOFILTER
         :  par.do_filter = 0
         ;  break
         ;

            case  MY_OPT_SSAHA
         :  xfssaha = mcxIOnew(opt->val, "r")
         ;  break
         ;

            case  MY_OPT_FASTASL
         :  xffasta = mcxIOnew(opt->val, "r")
         ;  break
         ;

            case  MY_OPT_OUTPUT
         :  mcxIOrenew(xfout, opt->val, "w")
         ;  break
         ;
         }
      }

   ;  if (!xffasta || !xfssaha)
      mcxDie(1, me, "-fasta-sl and -ssaha options are both required")

   ;  mcxIOopen(xffasta, EXIT_ON_FAIL)
   ;  mcxIOopen(xfssaha, EXIT_ON_FAIL)
   ;  mcxIOopen(xfout, EXIT_ON_FAIL)

   ;  read_fasta(xffasta, seqattr)
   
   ;  read_ssaha(xfssaha, seqattr, par, &flt)

      /* fixme todo: reinstate the accounting part of reduce_single_query
       * it does in the current setup not need to know at how many places a read maps.
      */

   ;  accumulate_tracks(&flt, par.do_divide)

   ;  qsort(flt.tracks, flt.n_track, sizeof flt.tracks[0], cmp_tracks_name)

   ;  destroy_and_write_tracks(&flt, xfout)

   ;  return 0
;  }



#if 0

      {  mcxHashWalk*  walk = mcxHashWalkInit(seqattr)
      ;  dim ct =  0
      ;  while((kv = mcxHashWalkStep(walk, NULL)))
         {  mcxTing* key = kv->key
         ;  readattr*  par = kv->val
         ;  ct++
;fprintf(stdout, "[%s] %lu %lu\n", key->str, (ulong) par->depth, (ulong) par->length)
      ;  }
         mcxHashWalkFree(&walk)
   ;  }

#endif


