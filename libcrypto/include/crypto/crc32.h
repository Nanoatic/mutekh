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
 * @short IEEE 802.3 32 bits Cyclic Redundancy Check algorithm
 */

#ifndef _CRC32_H_
#define _CRC32_H_

#include <crypto/crypto.h>

struct crypto_crc32_ctx_s {
  uint32_t crc;
};

CRYPTO_HASH_INIT(crypto_crc32_init);
CRYPTO_HASH_UPDATE(crypto_crc32_update);
CRYPTO_HASH_GET(crypto_crc32_get);

extern struct crypto_hash_algo_s crypto_crc32;

#endif

