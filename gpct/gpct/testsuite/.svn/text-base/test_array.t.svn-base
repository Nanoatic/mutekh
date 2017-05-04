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

static CONTAINER_ITERATOR(test, iter)
{
  TESTTYPE	**ptr = va_arg(ap, TESTTYPE**);

  **ptr = item & ~32;
  (*ptr)++;

  return 0;
}

int main()
{
  int		i;
  test_root_t	array;
  TESTTYPE	t[16];
  TESTTYPE	*ptr = t;

  puts("Container array operations test (" NAME_STR(TESTCONT) ", " NAME_STR(TESTTYPE) ")");

  assert(array_init(&array) == 0);

  assert(array_count(&array) == 0);
  assert(array_size(&array) == 8);

  assert(!array_check(&array));

  /* simple push pop tests */

  assert(array_push(&array, 'b') == 1);
  assert(array_push(&array, 'a') == 1);
  assert(array_push(&array, 'r') == 1);

  assert(array_foreach(&array, iter, &ptr) == 0);
  assert(ptr == t + 3);

  for (i = 0; i < 3; i++)
    assert(t[i] == "RAB"[i]);

  ptr = t;
  CONTAINER_FOREACH(test, TESTCONT, &array,
  {
    *ptr++ = item;
  });
  assert(ptr == t + 3);

  for (i = 0; i < 3; i++)
    assert(t[i] == "rab"[i]);

  assert(!array_check(&array));

  assert(array_count(&array) == 3);
  assert(array_pop(&array) == 'r');
  assert(array_count(&array) == 2);
  assert(array_pop(&array) == 'a');
  assert(array_count(&array) == 1);
  assert(array_pop(&array) == 'b');
  assert(array_count(&array) == 0);

  assert(!array_check(&array));

  /* simple pushback popback tests */

  assert(array_pushback(&array, 'b') == 1);
  assert(array_pushback(&array, 'a') == 1);
  assert(array_pushback(&array, 'r') == 1);

  assert(array_count(&array) == 3);
  assert(array_popback(&array) == 'r');
  assert(array_count(&array) == 2);
  assert(array_popback(&array) == 'a');
  assert(array_count(&array) == 1);
  assert(array_popback(&array) == 'b');
  assert(array_count(&array) == 0);

  assert(!array_check(&array));

  /* mixed push pop pushback popback tests */

  assert(array_pushback(&array, 'b') == 1);
  assert(array_push(&array, 'a') == 1);
  assert(array_pushback(&array, 'r') == 1);
  assert(array_push(&array, 'i') == 1);

  assert(!array_check(&array));

  assert(array_pop(&array) == 'i');
  assert(array_pop(&array) == 'a');
  assert(array_pop(&array) == 'b');
  assert(array_pop(&array) == 'r');

  assert(!array_check(&array));

  assert(array_pushback(&array, 'b') == 1);
  assert(array_push(&array, 'a') == 1);
  assert(array_pushback(&array, 'r') == 1);
  assert(array_push(&array, 'i') == 1);

  assert(!array_check(&array));

  assert(array_popback(&array) == 'r');
  assert(array_popback(&array) == 'b');
  assert(array_popback(&array) == 'a');
  assert(array_popback(&array) == 'i');

  assert(!array_check(&array));

  assert(array_count(&array) == 0);

  /* test empty pop */

  assert(array_pop(&array) == 0);
  assert(array_count(&array) == 0);

  assert(!array_check(&array));

  /* test isnull head tail next prev */

  assert(array_isnull(array_head(&array)));
  assert(array_isnull(array_tail(&array)));
  assert(array_push(&array, 'a') == 1);
  assert(!array_isnull(array_head(&array)));
  assert(!array_isnull(array_tail(&array)));
  assert(array_push(&array, 'b') == 1);

  assert(!array_check(&array));

  assert(array_head(&array) == 0);
  assert(array_tail(&array) == 1);

  assert(array_next(&array, 0) == 1);
  assert(array_prev(&array, 1) == 0);
  assert(array_isnull(array_prev(&array, 0)));
  assert(array_isnull(array_next(&array, 1)));

  /* test set get */

  assert(array_get(&array, 0) == 'b');
  assert(array_get(&array, 1) == 'a');
  array_set(&array, 0, 'c');
  assert(array_get(&array, 0) == 'c');

  assert(!array_check(&array));

  /* test remove */

  assert(array_pushback(&array, 'd') == 1);

  array_remove(&array, 1);
  assert(array_pop(&array) == 'c');
  assert(array_pop(&array) == 'd');

  assert(!array_check(&array));

  /* test array functions */

  {
    TESTTYPE	t[] = { 'b', 'a', 'r', 'i' };
    assert(array_push_array(&array, t, 4) == 4);
  }

  assert(!array_check(&array));

  assert(array_pop(&array) == 'i');
  assert(array_pop(&array) == 'r');
  assert(array_pop(&array) == 'a');
  assert(array_pop(&array) == 'b');

  assert(!array_check(&array));

  {
    TESTTYPE	t[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    assert(array_push_array(&array, t, 9) == 8);
  }

  assert(array_push(&array, 'a') == 0);

  assert(!array_check(&array));

  assert(array_pop(&array) == '8');
  assert(array_popback(&array) == '1');

  assert(!array_check(&array));

  assert(array_count(&array) == 6);

  assert(array_pop_array(&array, t, 3) == 3);

  for (i = 0; i < 3; i++)
    assert(t[i] == "765"[i]);

  assert(!array_check(&array));

  assert(array_popback_array(&array, t, 5) == 3);

  for (i = 0; i < 3; i++)
    assert(t[i] == "234"[i]);

  assert(!array_check(&array));

  {
    TESTTYPE	t[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    assert(array_pushback_array(&array, t, 9) == 8);
  }

  assert(!array_check(&array));

  for (i = 0; i < array_size(&array); i++)
    assert(array_getindex(&array, array_getptr(&array, i)) == i);

  assert(array_pop_array(&array, t, 4) == 4);

  for (i = 0; i < 4; i++)
    assert(t[i] == "1234"[i]);

  assert(array_pop_array(&array, t, 6) == 4);

  for (i = 0; i < 4; i++)
    assert(t[i] == "5678"[i]);

  assert(array_count(&array) == 0);

  /* test insertion */

  {
    TESTTYPE	t[] = { '1', '2', '3', '4' };
    assert(array_pushback_array(&array, t, 4) == 4);
  }

  assert(array_insert_pre(&array, 2, 'a') == 1);
  assert(array_insert_pre(&array, 0, 'A') == 1);
  assert(array_insert_post(&array, 5, 'z') == 1);
  assert(array_insert_post(&array, 2, 'Z') == 1);
  assert(array_insert_pre(&array, 3, 'a') == 0);
  assert(array_insert_post(&array, 3, 'Z') == 0);

  assert(array_pop_array(&array, t, 8) == 8);

  for (i = 0; i < 8; i++)
    assert(t[i] == "A12Za34z"[i]);

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

