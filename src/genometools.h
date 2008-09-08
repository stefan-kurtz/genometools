/*
  Copyright (c) 2003-2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2003-2008 Center for Bioinformatics, University of Hamburg

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

#ifndef GENOMETOOLS_H
#define GENOMETOOLS_H

/* The GenomeTools ``all-in-one'' header.
   Include only this header if you program against the libgenometools.
*/

/* the core module */
#include "core/array_api.h"
#include "core/error_api.h"
#include "core/fptr_api.h"
#include "core/ma_api.h"
#include "core/range_api.h"
#include "core/str_api.h"
#include "core/unused_api.h"

/* the extended module */
#include "extended/feature_type_factory_any.h"
#include "extended/sequence_region_api.h"

#ifndef WITHOUT_CAIRO
/* the AnnotationSketch module (depends on Cairo) */
#include "annotationsketch/canvas_api.h"
#include "annotationsketch/canvas_cairo_context_api.h"
#include "annotationsketch/canvas_cairo_file_api.h"
#include "annotationsketch/color_api.h"
#include "annotationsketch/diagram_api.h"
#include "annotationsketch/feature_index_api.h"
#include "annotationsketch/graphics_api.h"
#include "annotationsketch/image_info_api.h"
#include "annotationsketch/recmap_api.h"
#include "annotationsketch/style_api.h"
#endif

#endif
