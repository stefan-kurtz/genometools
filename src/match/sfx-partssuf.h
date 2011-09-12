/*
  Copyright (c) 2007 Stefan Kurtz <kurtz@zbh.uni-hamburg.de>
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

#ifndef SFX_PARTSSUF_H
#define SFX_PARTSSUF_H

#include "core/logger.h"
#include "core/codetype.h"
#include "sfx-maprange.h"
#include "bcktab.h"

typedef struct GtSuftabparts GtSuftabparts;

GtSuftabparts *gt_suftabparts_new(unsigned int numofparts,
                               const GtBcktab *bcktab,
                               const GtSfxmappedrange *mappedmarkprefixbuckets,
                               unsigned long numofsuffixestoinsert,
                               unsigned long fullspecials,
                               GtLogger *logger);

void gt_suftabparts_delete(GtSuftabparts *suftabparts);

GtCodetype gt_suftabparts_mincode(unsigned int part,
                                  const GtSuftabparts *suftabparts);

GtCodetype gt_suftabparts_maxcode(unsigned int part,
                                  const GtSuftabparts *suftabparts);

unsigned long gt_suftabparts_offset(unsigned int part,
                                    const GtSuftabparts *suftabparts);

unsigned long gt_suftabparts_sumofwdith(unsigned int part,
                                        const GtSuftabparts *suftabparts);

unsigned long gt_suftabparts_widthofpart(unsigned int part,
                                         const GtSuftabparts *suftabparts);

unsigned long gt_suftabparts_largest_width(const GtSuftabparts *suftabparts);

unsigned int gt_suftabparts_numofparts(const GtSuftabparts *suftabparts);

unsigned long gt_suftabparts_largestsizemappedpartwise(
                                       const GtSuftabparts *suftabparts);

#endif
