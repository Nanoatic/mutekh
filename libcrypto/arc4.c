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

#include <crypto/arc4.h>

#define ARC4_S_SIZE	256

/* initialize S buffer with identity */

CRYPTO_STREAM_INIT(crypto_arc4_init)
{
  struct crypto_arc4_ctx_s *ctx = ctx_;
  size_t a;

  for (a = 0; a < ARC4_S_SIZE; a++)
    ctx->state[a] = a;
}

/* do key permutation on buffer */

CRYPTO_STREAM_UPDATE(crypto_arc4_update)
{
  struct crypto_arc4_ctx_s *ctx = ctx_;
  uint8_t *state = ctx->state;

  register uint_fast8_t a, b;

  for (a = b = 0; a < ARC4_S_SIZE; a++)
    {
      uint8_t tmp;

      tmp = state[a];
      b = (uint8_t)(b + tmp + key[a % keylen]);

      state[a] = state[b];		/* swap state[a] and state[b] */
      state[b] = tmp;
    }
}

/* get arc4 stream */

#define ARC4_PROCESS(op)					\
{								\
  struct crypto_arc4_ctx_s *ctx = ctx_;				\
  uint8_t *state = ctx->state;					\
								\
  register uint_fast8_t a = 0, b = 0;				\
								\
  while (data_len--)						\
    {								\
      uint8_t tmp;						\
								\
      a = (uint8_t)(a + 1);					\
      b = (uint8_t)(b + state[a]);				\
								\
      tmp = state[b];						\
      state[b] = state[a];					\
      state[a] = tmp;						\
								\
      if (data)							\
	*data++ op state[(uint8_t)(state[a] + state[b])];	\
    }								\
}

CRYPTO_STREAM_GETSTREAM(crypto_arc4_getstream) ARC4_PROCESS( = )
CRYPTO_STREAM_XORSTREAM(crypto_arc4_xorstream) ARC4_PROCESS( ^= )

struct crypto_stream_algo_s crypto_arc4 = {
  .f_init = crypto_arc4_init,
  .f_update = crypto_arc4_update,
  .f_getstream = crypto_arc4_getstream,
  .f_xorstream = crypto_arc4_xorstream,
  .ctx_size = sizeof (struct crypto_arc4_ctx_s),
  .min_key = 1,
  .max_key = 256,
};

