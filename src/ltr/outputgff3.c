/*
  Copyright (c) 2007 David Ellinghaus <d.ellinghaus@ikmb.uni-kiel.de>
  Copyright (c) 2007 Center for Bioinformatics, University of Hamburg

  Permission to use, copy, modify, and distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "core/fa.h"
#include "core/str.h"
#include "match/echoseq.h"
#include "match/encseq-def.h"
#include "ltr/ltrharvest-opt.h"
#include "ltr/repeattypes.h"

typedef struct
{
  unsigned long idcounterRepregion,
                idcounterRetrotrans,
                idcounterLTR,
                idcounterTSD,
                idcounterMotif;
} LTRcounter;

static void showboundaries(FILE *fp,
                           const LTRharvestoptions *lo,
                           const LTRboundaries *boundaries,
                           unsigned long contignumber,
                           Seqpos offset,
                           LTRcounter *ltrc)
{

  /* repeat-region */
  fprintf(fp, "seq%lu\tLTRharvest\trepeat_region\t" FormatSeqpos
              "\t" FormatSeqpos "\t" ".\t?\t.\tID=RepeatReg%lu\n",
      contignumber,
      /* increase boundary position by one for output */
      PRINTSeqposcast(boundaries->leftLTR_5 -offset + 1
                  - boundaries->lenleftTSD),
      PRINTSeqposcast(boundaries->rightLTR_3 -offset + 1
                  + boundaries->lenrightTSD),
      ltrc->idcounterRepregion++ );

  /* LTR retrotransposon */
  fprintf(fp, "seq%lu\tLTRharvest\tLTR_retrotransposon\t"
              FormatSeqpos "\t" FormatSeqpos "\t"
              ".\t?\t.\tID=LTRret%lu;Parent=RepeatReg%lu\n",
      contignumber,
      /* increase boundary position by one for output */
      PRINTSeqposcast(boundaries->leftLTR_5 -offset + 1),
      PRINTSeqposcast(boundaries->rightLTR_3 -offset  + 1),
      ltrc->idcounterRetrotrans++,
      ltrc->idcounterRepregion-1 );

  /* LTRs */
  fprintf(fp, "seq%lu\tLTRharvest\tlong_terminal_repeat\t"
              FormatSeqpos "\t" FormatSeqpos "\t"
              ".\t?\t.\tID=LTR%lu;Parent=LTRret%lu\n",
      contignumber,
      /* increase boundary position by one for output */
      PRINTSeqposcast(boundaries->leftLTR_5 -offset + 1),
      PRINTSeqposcast(boundaries->leftLTR_3 -offset + 1),
      ltrc->idcounterLTR++,
      ltrc->idcounterRetrotrans-1 );
  fprintf(fp, "seq%lu\tLTRharvest\tlong_terminal_repeat\t"
              FormatSeqpos "\t" FormatSeqpos "\t"
              ".\t?\t.\tID=LTR%lu;Parent=LTRret%lu\n",
      contignumber,
      /* increase boundary position by one for output */
      PRINTSeqposcast(boundaries->rightLTR_5 -offset + 1),
      PRINTSeqposcast(boundaries->rightLTR_3 -offset + 1),
      ltrc->idcounterLTR++,
      ltrc->idcounterRetrotrans-1 );

  if (lo->minlengthTSD > 1U)
  {
    /* TSDs */
    fprintf(fp, "seq%lu\tLTRharvest\ttarget_site_duplication\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=TSD%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->leftLTR_5 -offset + 1
                  - boundaries->lenleftTSD),
        PRINTSeqposcast(boundaries->leftLTR_5 -offset),
        ltrc->idcounterTSD++,
        ltrc->idcounterRepregion-1 );

    fprintf(fp, "seq%lu\tLTRharvest\ttarget_site_duplication\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=TSD%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->rightLTR_3 -offset + 2),
        PRINTSeqposcast(boundaries->rightLTR_3 -offset + 1
                  + boundaries->lenrightTSD),
        ltrc->idcounterTSD++,
        ltrc->idcounterRepregion-1 );
  }

  if (lo->motif.allowedmismatches < 4U)
  {
    fprintf(fp, "seq%lu\tLTRharvest\tinverted_repeat\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=Motif%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->leftLTR_5 -offset + 1),
        PRINTSeqposcast(boundaries->leftLTR_5 -offset + 2),
        ltrc->idcounterMotif++,
        ltrc->idcounterRepregion-1 );
    fprintf(fp, "seq%lu\tLTRharvest\tinverted_repeat\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=Motif%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->leftLTR_3 -offset),
        PRINTSeqposcast(boundaries->leftLTR_3 -offset + 1),
        ltrc->idcounterMotif++,
        ltrc->idcounterRepregion-1 );

    fprintf(fp, "seq%lu\tLTRharvest\tinverted_repeat\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=Motif%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->rightLTR_5 -offset + 1),
        PRINTSeqposcast(boundaries->rightLTR_5 -offset + 2),
        ltrc->idcounterMotif++,
        ltrc->idcounterRepregion-1 );
    fprintf(fp, "seq%lu\tLTRharvest\tinverted_repeat\t"
                FormatSeqpos "\t" FormatSeqpos "\t"
                ".\t?\t.\tID=Motif%lu;Parent=RepeatReg%lu\n",
        contignumber,
        /* increase boundary position by one for output */
        PRINTSeqposcast(boundaries->rightLTR_3 -offset),
        PRINTSeqposcast(boundaries->rightLTR_3 -offset + 1),
        ltrc->idcounterMotif++,
        ltrc->idcounterRepregion-1 );
  }
}

void printgff3format(const LTRharvestoptions *lo,
                     const Encodedsequence *encseq)
{
  LTRboundaries *boundaries;
  unsigned long seqnum, i, contignumber,
                *descendtab = NULL,
                desclen, numofdbsequences;
  LTRcounter ltrc;
  const char *desptr = NULL;
  FILE *fp;
  Seqinfo seqinfo;

  numofdbsequences = getencseqnumofdbsequences(encseq);

  fp = gt_fa_xfopen(gt_str_get(lo->str_gff3filename), "w");

  /* for getting descriptions */
  descendtab = calcdescendpositions(encseq);

  ltrc.idcounterRepregion = ltrc.idcounterRetrotrans
                          = ltrc.idcounterLTR
                          = ltrc.idcounterTSD
                          = ltrc.idcounterMotif = 0;

  if (lo->arrayLTRboundaries.nextfreeLTRboundaries == 0)
  {
    /* no LTR-pairs predicted */
  }
  else
  {
    fprintf(fp, "##gff-version 3\n");
    /* print output sorted by contignumber */
    for (seqnum = 0; seqnum < numofdbsequences; seqnum++)
    {
      /* contig is first sequence, and only one sequence in multiseq */
      getencseqSeqinfo(&seqinfo,encseq,seqnum);
      fprintf(fp, "##sequence-region seq%lu 1 " FormatSeqpos "\n",
                  seqnum, PRINTSeqposcast(seqinfo.seqlength));
      /* write description of sequence */
      fprintf(fp, "# ");
      desptr = retrievesequencedescription(&desclen,
                                           encseq,
                                           descendtab,
                                           seqnum);
      for (i=0; i < desclen; i++)
      {
        fprintf(fp, "%c", desptr[i]);
      }
      fprintf(fp, "\n");

      for (i = 0; i < lo->arrayLTRboundaries.nextfreeLTRboundaries; i++)
      {
        boundaries = &(lo->arrayLTRboundaries.spaceLTRboundaries[i]);
        contignumber = boundaries->contignumber;
        if ( (!boundaries->skipped) && contignumber == seqnum)
        {
          showboundaries(fp,
                         lo,
                         boundaries,
                         contignumber,
                         seqinfo.seqstartpos,
                         &ltrc);
        }
      }
    }
  }
  gt_free(descendtab);
  gt_fa_xfclose(fp);
}
