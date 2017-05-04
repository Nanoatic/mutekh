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
 * @short MD5 cryptographic hash algorithm
 */

#ifndef _MD5_H_
#define _MD5_H_

#include <crypto/crypto.h>

struct crypto_md5_ctx_s {
  uint32_t buf[4];
  uint32_t bits[2];
  uint8_t in[64];
};

CRYPTO_HASH_INIT(crypto_md5_init);
CRYPTO_HASH_UPDATE(crypto_md5_update);
CRYPTO_HASH_GET(crypto_md5_get);

extern struct crypto_hash_algo_s crypto_md5;

#endif

