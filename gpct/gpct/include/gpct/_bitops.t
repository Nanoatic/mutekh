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

#ifndef GPCT_BITOPS_H_
#define GPCT_BITOPS_H_

#include <gpct/_platform.h>

# define GPCT_BIT_TEST(type, data, bit)       ((data) & ((type)1 << (bit)) ? 1 : 0)
# define GPCT_BIT_SET(type, data, bit)        ((data) |= ((type)1 << (bit)))
# define GPCT_BIT_CLR(type, data, bit)        ((data) &= ~((type)1 << (bit)))

/* backslash-region-begin */
# define GPCT_BIT_SETVALUE(type, data, bit, value)
  {
    if (value)
      GPCT_BIT_SET(type, data, bit);
    else
      GPCT_BIT_CLR(type, data, bit);
  }
/* backslash-region-end */

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)

/* backslash-region-begin */
# define GPCT_POPCOUNT(n)
  (__builtin_types_compatible_p(typeof(n), GPCT_ULONG) ? __builtin_popcountl(n) :
  __builtin_types_compatible_p(typeof(n), GPCT_ULONGLONG) ? __builtin_popcountll(n) :
  __builtin_popcount(n))
/* backslash-region-end */

/* backslash-region-begin */
# define GPCT_BIT_FFS(n)
  (__builtin_types_compatible_p(typeof(n), GPCT_ULONG) ? __builtin_ffsl(n) :
  __builtin_types_compatible_p(typeof(n), GPCT_ULONGLONG) ? __builtin_ffsll(n) :
  __builtin_ffs(n))
/* backslash-region-end */

/* backslash-region-begin */
# define GPCT_BIT_FFSR(n)
({
  typeof(n) gpct_n = (n);

  gpct_n ? (__builtin_types_compatible_p(typeof(gpct_n), GPCT_ULONG) ? sizeof(GPCT_ULONG) * 8 - __builtin_clzl(gpct_n) :
  __builtin_types_compatible_p(typeof(gpct_n), GPCT_ULONGLONG) ? sizeof(GPCT_ULONGLONG) * 8 - __builtin_clzll(gpct_n) :
  sizeof(GPCT_UINT) * 8 - __builtin_clz(gpct_n)) : 0;
})
/* backslash-region-end */

#elif __GNUC__

/* backslash-region-begin */
# define GPCT_POPCOUNT(n)
({
  uint_fast8_t        gpct_x, gpct_count = 0;
  typeof(n)     gpct_w = n;

  for (gpct_x = 0; gpct_x < sizeof(gpct_w) * 8; gpct_x++)
    if (GPCT_BIT_TEST(typeof(n), gpct_w, gpct_x))
      gpct_count++;

  gpct_count;
})
/* backslash-region-end */


/* backslash-region-begin */
# define GPCT_BIT_FFS(n)
({
  uint_fast8_t        gpct_x, gpct_pos = 0;
  typeof(n)     gpct_w = n;

  for (gpct_x = 0; gpct_x < sizeof(gpct_w) * 8; gpct_x++)
    if (GPCT_BIT_TEST(typeof(n), gpct_w, gpct_x))
      {
        gpct_pos = gpct_x + 1;
        break;
      }

  gpct_pos;
})
/* backslash-region-end */


/* backslash-region-begin */
# define GPCT_BIT_FFSR(n)
({
  int_fast8_t   gpct_x;
  uint_fast8_t  gpct_pos = 0;
  typeof(n)     gpct_w = n;

  for (gpct_x = sizeof(gpct_w) * 8 - 1; gpct_x >= 0; gpct_x--)
    if (GPCT_BIT_TEST(typeof(n), gpct_w, gpct_x))
      {
        gpct_pos = gpct_x + 1;
        break;
      }

  gpct_pos;
})
/* backslash-region-end */

#else

GPCT_INTERNAL uint_fast8_t
GPCT_POPCOUNT(uintmax_t gpct_word)
{
  uint_fast8_t        gpct_x, gpct_count = 0;

  for (gpct_x = 0; gpct_x < sizeof(gpct_word) * 8; gpct_x++)
    if (GPCT_BIT_TEST(uintmax_t, gpct_word, gpct_x))
      gpct_count++;

  return gpct_count;
}

GPCT_INTERNAL uint_fast8_t
GPCT_BIT_FFS(uintmax_t gpct_word)
{
  uint_fast8_t        gpct_x, gpct_pos = 0;

  for (gpct_x = 0; gpct_x < sizeof(gpct_word) * 8; gpct_x++)
    if (GPCT_BIT_TEST(uintmax_t, gpct_word, gpct_x))
      {
        gpct_pos = gpct_x + 1;
        break;
      }

  return gpct_pos;
}

GPCT_INTERNAL uint_fast8_t
GPCT_BIT_FFSR(uintmax_t gpct_word)
{
  int_fast8_t   gpct_x;
  uint_fast8_t  gpct_pos = 0;

  for (gpct_x = sizeof(gpct_word) * 8 - 1; gpct_x >= 0; gpct_x--)
    if (GPCT_BIT_TEST(uintmax_t, gpct_word, gpct_x))
      {
        gpct_pos = gpct_x + 1;
        break;
      }

  return gpct_pos;
}

#endif

#endif

