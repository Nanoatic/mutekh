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
 * @module{Hexo}
 * @internal
 * @short Hexo specific platform definition for GPCT
 */

#ifndef __GPCT_PLATFORM_HEXO_H__
#define __GPCT_PLATFORM_HEXO_H__

#include <hexo/decls.h>

C_HEADER_BEGIN

#ifdef GPCT_PLATFORM_H_
# error gpct_platform_hexo.h must be included before any gpct header
#endif

#include <hexo/error.h>
#include <hexo/types.h>
#include <hexo/atomic.h>

#define GPCT_CONFIG_NOPLATFORM
#define GPCT_CONFIG_ATOMIC_OTHER

#define GPCT_INT	int_fast8_t
#define GPCT_UINT	uint_fast8_t
#define GPCT_ULONG	__compiler_ulong_t
#define GPCT_ULONGLONG	__compiler_ulonglong_t

typedef bool_t		gpct_bool_t;
typedef	error_t		gpct_error_t;
typedef atomic_t	gpct_atomic_t;
typedef atomic_int_t	gpct_atomic_int_t;
typedef size_t		gpct_index_t;
typedef ssize_t		gpct_sindex_t;

/* static value init */
#define GPCT_ATOMIC_INITIALIZER(v)	ATOMIC_INITIALIZER(v)

static inline void gpct_atomic_init(gpct_atomic_t *a)
{
}

static inline void gpct_atomic_destroy(gpct_atomic_t *a)
{
}

static inline void gpct_atomic_set(gpct_atomic_t *a, gpct_atomic_int_t v)
{
  atomic_set(a, v);
}

static inline gpct_atomic_int_t gpct_atomic_get(gpct_atomic_t *a)
{
  return atomic_get(a);
}

static inline gpct_bool_t gpct_atomic_inc(gpct_atomic_t *a)
{
  return atomic_inc(a);
}

static inline gpct_bool_t gpct_atomic_dec(gpct_atomic_t *a)
{
  return atomic_dec(a);
}

static inline gpct_bool_t gpct_atomic_bit_test(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return atomic_bit_test(a, n);
}

static inline gpct_bool_t gpct_atomic_bit_test_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return atomic_bit_testset(a, n);
}

static inline gpct_bool_t gpct_atomic_bit_test_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return atomic_bit_testclr(a, n);
}

static inline void gpct_atomic_bit_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return atomic_bit_clr(a, n);
}

static inline void gpct_atomic_bit_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return atomic_bit_set(a, n);
}

C_HEADER_END

#endif

