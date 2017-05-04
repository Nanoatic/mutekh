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

#include <gpct/cont_bitmap.h>

#ifdef CONTAINER_LOCK_test
#include <gpct/lock_pthread_mutex.h>
#endif

#include <assert.h>
#include <string.h>
#include <stdio.h>

CONTAINER_TYPE(test, BITMAP, TESTTYPE, 256);
CONTAINER_FUNC(test, BITMAP, static inline, bitmap);
CONTAINER_KEY_FUNC(test, BITMAP, static inline, bitmap);

static unsigned long long	prod = 1;

static CONTAINER_ITERATOR(test, iter)
{
  prod *= item;

#ifdef DEBUG
  printf("%lu\n", item);
#endif

  return 0;
}

int main()
{
  test_root_t	bitmap;
  test_index_t	i;

  puts("Testing BITMAP with type TESTTYPE");

  bitmap_init(&bitmap);

  assert(sizeof(bitmap.data) * 8 == 256);

  assert(bitmap_isempty(&bitmap));
  assert(bitmap_count(&bitmap) == 0);

  assert(!bitmap_get(&bitmap, 200));
  assert(!bitmap_set(&bitmap, 200, 1));
  assert(bitmap_get(&bitmap, 200));

  assert(!bitmap_isempty(&bitmap));
  assert(bitmap_count(&bitmap) == 1);

  assert(bitmap_set(&bitmap, 200, 1));
  assert(bitmap_get(&bitmap, 200));
  assert(bitmap_set(&bitmap, 200, 0));
  assert(!bitmap_get(&bitmap, 200));

  assert(bitmap_count(&bitmap) == 0);

  assert(!bitmap_set(&bitmap, 125, 1));
  assert(!bitmap_set(&bitmap, 103, 1));
  assert(!bitmap_set(&bitmap, 128, 1));
  assert(!bitmap_set(&bitmap, 129, 1));

  assert(bitmap_count(&bitmap) == 4);

  assert(bitmap_set(&bitmap, 129, 0));

  assert(bitmap_count(&bitmap) == 3);

  assert(!bitmap_set(&bitmap, 101, 1));
  assert(!bitmap_set(&bitmap, 105, 1));

  bitmap_foreach(&bitmap, iter);

  assert(prod == 17477040000LL);

  prod = 1;

  CONTAINER_FOREACH(test, BITMAP, &bitmap,
  {
    prod *= index;
  });

  assert(prod == 17477040000LL);

  prod = 1;

  CONTAINER_FOREACH_REVERSE(test, BITMAP, &bitmap,
  {
    prod *= index;
  });

  assert(prod == 17477040000LL);

  prod = 1;

  for (i = bitmap_lookup     (&bitmap, 1);
       !   bitmap_isnull     (i);
       i = bitmap_lookup_next(&bitmap, i, 1))
    prod *= i;

  assert(prod == 17477040000LL);

  bitmap_clear(&bitmap);
  assert(bitmap_isempty(&bitmap));

#ifdef CONTAINER_LOCK_test
  puts("Container locking test");

  bitmap_wrlock(&bitmap);
  assert(pthread_mutex_trylock(&bitmap.lock) != 0);
  bitmap_unlock(&bitmap);

  assert(pthread_mutex_trylock(&bitmap.lock) == 0);
  pthread_mutex_unlock(&bitmap.lock);
#endif

  bitmap_destroy(&bitmap);

  return 0;
}

