/*
  Copyright (c) 2006, 2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2006, 2008 Center for Bioinformatics, University of Hamburg

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

#include <assert.h>
#include "core/undef.h"
#include "extended/gff3_output.h"

void gff3_output_leading(GT_GenomeFeature *gf, GT_GenFile *outfp)
{
  GtGenomeNode *gn;
  assert(gf);
  gn = (GtGenomeNode*) gf;
  gt_genfile_xprintf(outfp, "%s\t%s\t%s\t%lu\t%lu\t",
                     gt_str_get(gt_genome_node_get_seqid(gn)),
                     gt_genome_feature_get_source(gf),
                     gt_genome_feature_get_type(gf),
                     gt_genome_node_get_start(gn),
                     gt_genome_node_get_end(gn));
  if (gt_genome_feature_score_is_defined(gf))
    gt_genfile_xprintf(outfp, "%.3g", gt_genome_feature_get_score(gf));
  else
    gt_genfile_xfputc('.', outfp);
  gt_genfile_xprintf(outfp, "\t%c\t%c\t",
                     GT_STRAND_CHARS[gt_genome_feature_get_strand(gf)],
                     GT_PHASE_CHARS[gt_genome_feature_get_phase(gf)]);
}
