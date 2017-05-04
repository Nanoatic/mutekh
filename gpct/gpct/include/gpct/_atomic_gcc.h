/*

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr> (C) 2009

    dummy (non)atomic integer operations

*/

#ifndef GPCT_ATOMIC_GCC_H_
#define GPCT_ATOMIC_GCC_H_

#if !(((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || __GNUC__ > 4)
# error GCC atomic operations not supported by the compiler, gcc >= 4.1.0 required
#endif

#include <gpct/_platform.h>

typedef GPCT_INT     gpct_atomic_int_t;

typedef struct gpct_atomic_s
{
  volatile gpct_atomic_int_t val;
} gpct_atomic_t;

# define GPCT_ATOMIC_INITIALIZER(v)     { v }

GPCT_INTERNAL void gpct_atomic_init(gpct_atomic_t *a)
{
}

GPCT_INTERNAL void gpct_atomic_destroy(gpct_atomic_t *a)
{
}

GPCT_INTERNAL void gpct_atomic_set(gpct_atomic_t *a, gpct_atomic_int_t v)
{
  a->val = v;
}

GPCT_INTERNAL gpct_atomic_int_t gpct_atomic_get(gpct_atomic_t *a)
{
  return a->val;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_inc(gpct_atomic_t *a)
{
  return __sync_add_and_fetch(&a->val, 1) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_dec(gpct_atomic_t *a)
{
  return __sync_sub_and_fetch(&a->val, 1) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return (a->val & (1 << n)) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     mask = (1 << n);

  return (__sync_fetch_and_or(&a->val, mask) & mask) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     mask = (1 << n);

  return (__sync_fetch_and_and(&a->val, ~mask) & mask) != 0;
}

GPCT_INTERNAL void gpct_atomic_bit_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     mask = (1 << n);

  __sync_fetch_and_and(&a->val, ~mask);
}

GPCT_INTERNAL void gpct_atomic_bit_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     mask = (1 << n);

  __sync_fetch_and_or(&a->val, mask);
}

#endif

