/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef ARC4_H_
#define ARC4_H_

#include <hexo/types.h>

#define ARC4_S_SIZE	256
#define ARC4_IV_SIZE	8

typedef uint8_t	arc4_S_t[ARC4_S_SIZE];

/* initialize S buffer with identity */

static inline void arc4_s_init(arc4_S_t S)
{
  size_t a;

  for (a = 0; a < ARC4_S_SIZE; a++)
    S[a] = a;
}

/* do key permutation on buffer */

static inline void arc4_s_permut(arc4_S_t S, const uint8_t *key, size_t len)
{
  register uint_fast8_t a, b;

  for (a = b = 0; a < ARC4_S_SIZE; a++)
    {
      uint8_t tmp;

      tmp = S[a];
      b = (uint8_t)(b + tmp + key[a % len]);

      S[a] = S[b];		/* swap S[a] and S[b] */
      S[b] = tmp;
    }
}

/* get arc4 stream */

static inline void arc4_stream(arc4_S_t S, uint8_t *data, size_t data_len)
{
  register uint_fast8_t a = 0, b = 0;

  while (data_len--)
    {
      uint8_t tmp;

      a = (uint8_t)(a + 1);
      b = (uint8_t)(b + S[a]);

      tmp = S[b];
      S[b] = S[a];
      S[a] = tmp;

      *data++ = S[(uint8_t)(S[a] + S[b])];
    }
}

#endif

