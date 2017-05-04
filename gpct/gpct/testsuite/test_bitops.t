/* -*- c -*-

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

*/

#include "testsuite.h"

#include <gpct/_bitops.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
  uint8_t	a = 0xA5;
  uint32_t	c = 0xA5A5A5A5;
#if defined (__GNUC__)
  uint64_t	d = 0xA500000000000000;
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
  puts("Bits operations test (gcc builtins)");
#elif defined (__GNUC__)
  puts("Bits operations test (gcc)");
#else
  puts("Bits operations test (no gcc)");
#endif

  assert(GPCT_BIT_TEST(uint8_t, a, 0));
  assert(!GPCT_BIT_TEST(uint8_t, a, 1));

  GPCT_BIT_SET(uint8_t, a, 1);
  assert(a == (0xA5 | 0x02));

  GPCT_BIT_SET(uint8_t, a, 1);
  assert(a == (0xA5 | 0x02));

  GPCT_BIT_CLR(uint8_t, a, 1);
  assert(a == 0xA5);

  GPCT_BIT_CLR(uint8_t, a, 1);
  assert(a == 0xA5);

#if defined (__GNUC__)
  assert(!GPCT_BIT_TEST(uint64_t, d, 31));
  assert(!GPCT_BIT_TEST(uint64_t, d, 32));
  assert(GPCT_BIT_TEST(uint64_t, d, 56));
  assert(!GPCT_BIT_TEST(uint64_t, d, 57));
#endif

  assert(GPCT_POPCOUNT(a) == 4);
  assert(GPCT_POPCOUNT(0x111) == 3);
  assert(GPCT_POPCOUNT(c) == 16);

  assert(GPCT_BIT_FFS(a) == 1);
  assert(GPCT_BIT_FFSR(a) == 8);
  GPCT_BIT_CLR(uint8_t, a, 0);
  GPCT_BIT_CLR(uint8_t, a, 7);
  assert(GPCT_BIT_FFS(a) == 3);
  assert(GPCT_BIT_FFSR(a) == 6);
  a = 0;
  assert(GPCT_BIT_FFS(a) == 0);
  assert(GPCT_BIT_FFSR(a) == 0);

#if defined (__GNUC__)
  assert(GPCT_BIT_FFS(d) == 57);
  assert(GPCT_BIT_FFSR(d) == 64);
#endif

  return 0;
}

