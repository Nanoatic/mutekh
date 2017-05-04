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

#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef CONTAINER_LOCK_test
#include <gpct/lock_pthread_mutex.h>
#endif

m4_changecom
#include <INCLUDE>

CONTAINER_TYPE(test, TESTCONT, TESTTYPE, TESTTYPE_EXTRA);
CONTAINER_FUNC(test, TESTCONT, static inline, array);

int main()
{
  int		i;
  test_root_t	array;
  TESTTYPE		t[16];
  TESTTYPE		*ptr = t;

  puts("Container array realloc test (" NAME_STR(TESTCONT) ", " NAME_STR(TESTTYPE) ")");

  assert(array_init(&array) == 0);

  assert(array_count(&array) == 0);
  assert(array_size(&array) == 0);

  assert(!array_check(&array));

  assert(array_pushback(&array, 'b') == 1);
  /* >b * * * */

  assert(array_pushback(&array, 'a') == 1);
  /* >b a * * */

  assert(array_push(&array, 'r') == 1);
  /* b a * >r */
  assert(array_size(&array) == 4);

  ptr = t;
  CONTAINER_FOREACH(test, TESTCONT, &array,
  {
    *ptr++ = item;
  });
  assert(ptr == t + 3);

  for (i = 0; i < 3; i++)
    assert(t[i] == "rba"[i]);

  assert(array_push(&array, 'u') == 1);
  /* b a >u r */
  assert(array_size(&array) == 4);
  assert(!array_check(&array));

  assert(array_pushback(&array, 'y') == 1);
  /* * * >u r b a y * */
  assert(array_size(&array) == 8);
  assert(!array_check(&array));

  ptr = t;
  CONTAINER_FOREACH(test, TESTCONT, &array,
  {
    *ptr++ = item;
  });
  assert(ptr == t + 5);

#ifdef GPCT_CONT_RING_H_
  assert(array.first == 2);
#endif
  assert(array.count == 5);

  for (i = 0; i < 5; i++)
    assert(t[i] == "urbay"[i]);

  assert(!array_check(&array));

#ifdef CONTAINER_LOCK_test
  puts("Container locking test (" NAME_STR(TESTCONT) ")");

  array_wrlock(&array);
  assert(pthread_mutex_trylock(&array.lock) != 0);
  array_unlock(&array);

  assert(pthread_mutex_trylock(&array.lock) == 0);
  pthread_mutex_unlock(&array.lock);
#endif

  array_destroy(&array);

  return 0;
}

