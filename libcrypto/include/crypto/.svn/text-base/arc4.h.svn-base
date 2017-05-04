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

/**
 * @file
 * @module{Cryptographic algorithms}
 * @short ARC4 cipher and pseudo random stream generator
 */

#ifndef ARC4_H_
#define ARC4_H_

#include <crypto/crypto.h>

/** ARC4 algorithm internal state */
struct crypto_arc4_ctx_s {
  uint8_t state[256];
};

CRYPTO_STREAM_INIT(crypto_arc4_init);
CRYPTO_STREAM_UPDATE(crypto_arc4_update);
CRYPTO_STREAM_GETSTREAM(crypto_arc4_getstream);
CRYPTO_STREAM_XORSTREAM(crypto_arc4_xorstream);

extern struct crypto_stream_algo_s crypto_arc4;

#endif

