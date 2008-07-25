/*
  Copyright (c) 2008 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2008 Center for Bioinformatics, University of Hamburg

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
#include "libgtext/bitfield.h"

void bitfield_set_bit(Bitfield *bitfield, unsigned int bit)
{
  assert(bitfield && bit < 8U * sizeof (Bitfield));
  *bitfield |= 1U << bit;
}

void bitfield_unset_bit(Bitfield *bitfield, unsigned int bit)
{
  assert(bitfield && bit < 8U * sizeof (Bitfield));
  *bitfield &= ~(1U << bit);
}

bool bitfield_bit_is_set(Bitfield bitfield, unsigned int bit)
{
  assert(bit < 8U * sizeof (Bitfield));
  if (bitfield & 1U << bit)
    return true;
  return false;
}
