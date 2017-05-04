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

#ifndef GPCT_ATOMIC_NONE_H_
#define GPCT_ATOMIC_NONE_H_

#include <stdint.h>

typedef int_fast8_t             gpct_atomic_int_t;
typedef gpct_atomic_int_t       gpct_atomic_t;

# define GPCT_ATOMIC_INITIALIZER(v)     v

GPCT_INTERNAL void gpct_atomic_init(gpct_atomic_t *a)
{
}

GPCT_INTERNAL void gpct_atomic_destroy(gpct_atomic_t *a)
{
}

GPCT_INTERNAL void gpct_atomic_set(gpct_atomic_t *a, gpct_atomic_int_t v)
{
  *a = v;
}

GPCT_INTERNAL gpct_atomic_int_t gpct_atomic_get(gpct_atomic_t *a)
{
  return *a;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_inc(gpct_atomic_t *a)
{
  return ++(*a) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_dec(gpct_atomic_t *a)
{
  return --(*a) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return (*a & (1 << n)) != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     res;
  gpct_atomic_int_t     mask = (1 << n);

  res = *a & mask;
  *a |= mask;

  return res != 0;
}

GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     res;
  gpct_atomic_int_t     mask = (1 << n);

  res = *a & mask;
  *a &= ~mask;

  return res != 0;
}

GPCT_INTERNAL void gpct_atomic_bit_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  *a &= ~(1 << n);
}

GPCT_INTERNAL void gpct_atomic_bit_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  *a |= (1 << n);
}

#endif

