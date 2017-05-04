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
#include <stdlib.h>

#ifdef CONTAINER_LOCK_test
#include <gpct/lock_pthread_mutex.h>
#endif

m4_changecom
#include <INCLUDE>

struct aggregate_s
{
  long int				a;
  long int				b;
};

struct test_object_s
{
  CONTAINER_ENTRY_TYPE(TESTCONT)	list_entry;
  int					id;
  char					*name;
  uint8_t				blob[12];
  unsigned int				mask;
  struct aggregate_s			agg;
};

CONTAINER_TYPE		(test, TESTCONT, TESTTYPE, TESTTYPE_EXTRA);

CONTAINER_KEY_TYPE	(test, PTR, TESTALGO, TESTKEY, TESTKEYTYPE_EXTRA);
CONTAINER_KEY_PROTOTYPE	(test, static, list, TESTKEY);

CONTAINER_PROTOTYPE	(test, static, list);
CONTAINER_FUNC		(test, TESTCONT, static, list, TESTFUNC_EXTRA);

CONTAINER_KEY_FUNC	(test, TESTCONT, static, list, TESTKEY, TESTKEYFUNC_EXTRA);

unsigned int		i;

static CONTAINER_ITERATOR(test, iter_func)
{
  i = i * item->id + item->id;  
  return 0;
}

int main()
{
  test_root_t	list;
  struct test_object_s	obj[] =
    {
      {
	.id = 3,
	.name = "foo",
      },
      {
	.id = 5,
	.name = "bar",
      },
      {
	.id = 7,
	.name = "prettylongname",
      },
      {
	.id = 11,
	.name = "test",
      },
      {
	.id = 13,
	.name = "foo",
      },
#define ENTRY_COUNT	5

      {
	.id = 0,
	.name = "min",
      },
      {
	.id = 30,
	.name = "max",
      },
      {
	.id = 7,
	.name = "double",
      },
      {
	.id = 10,
	.name = "any",
      },

      {
	.id = -1,
	.name = "MIN",
      },
      {
	.id = 12,
	.name = "MAX",
      },
      {
	.id = 20,
	.name = "DOUBLE",
      },
      {
	.id = 32,
	.name = "ANY",
      },
    };

#if CONTAINER_DEF(TESTCONT, ARRAY_BASED)
  static const int idx[ENTRY_COUNT] = { 0, 1, 1, 2, 2 };
#else
  struct test_object_s *idx[ENTRY_COUNT] = { &obj[0], &obj[0], &obj[0], &obj[0], &obj[0] };
#endif

  puts("Container ordered insertion test (" NAME_STR(TESTCONT) ", " NAME_STR(TESTALGO)")");

  list_init(&list);

  /* push entries */

  assert(list_push(&list, &obj[0]) == 1);
  assert(!list_check(&list));
  /* 0 */

  assert(list_insert_pre(&list, idx[0], &obj[1]) == 1);
  assert(!list_check(&list));
  /* 1 0 */

  assert(list_insert_post(&list, idx[1], &obj[2]) == 1);
  assert(!list_check(&list));
  /* 1 0 2 */

  assert(list_insert_pre(&list, idx[2], &obj[3]) == 1);
  assert(!list_check(&list));
  /* 1 3 0 2 */

  assert(list_insert_post(&list, idx[3], &obj[4]) == 1);
  assert(!list_check(&list));
  /* 1 3 0 4 2 */


  i = 1;
  CONTAINER_FOREACH(test, TESTCONT, &list,
  {
    i = i * item->id + item->id;
  });

  assert(!list_check(&list));
  assert(i == 33404);


  i = 1;
  list_foreach(&list, iter_func);
  assert(i == 33404);


  i = 1;
  CONTAINER_FOREACH_REVERSE(test, TESTCONT, &list,
  {
    i = i * item->id + item->id;
  });
  assert(i == 32400);


  i = 1;
  list_foreach_reverse(&list, iter_func);
  assert(i == 32400);

#if CONTAINER_DEF(TESTCONT, REORDER)
  puts("Container sorting test (" NAME_STR(TESTCONT) ", " NAME_STR(TESTALGO)")");

  list_sort_ascend(&list);
  assert(!list_check(&list));
  assert(list_issorted_ascend(&list));
  assert(!list_issorted_descend(&list));

  for (i = 5; i <= 8; i++)
    {
      list_insert_ascend(&list, &obj[i]);
      assert(!list_check(&list));
      assert(list_issorted_ascend(&list));
      assert(!list_issorted_descend(&list));
    }
  assert(list_count(&list) == 9);

  list_sort_descend(&list);
  assert(!list_check(&list));
  assert(!list_issorted_ascend(&list));
  assert(list_issorted_descend(&list));

  for (i = 9; i <= 12; i++)
    {
      list_insert_descend(&list, &obj[i]);
      assert(!list_check(&list));
      assert(!list_issorted_ascend(&list));
      assert(list_issorted_descend(&list));
    }
  assert(list_count(&list) == 13);

  list_clear(&list);
  test_root_t	list2;
  list_init(&list2);

  list_sort_descend(&list2);
  list_sort_ascend(&list2);

  for (i = 0; i <= 8; i++)
    {
      list_insert_ascend(&list, &obj[i]);
      assert(!list_check(&list));
    }
  assert(list_issorted_ascend(&list));
  assert(!list_issorted_descend(&list));

  list_merge_ascend(&list2, &list);
  assert(!list_check(&list2));
  assert(!list_check(&list));
  assert(list_count(&list) == 0);
  assert(list_count(&list2) == 9);
  assert(list_issorted_ascend(&list2));
  assert(!list_issorted_descend(&list2));

  list_sort_descend(&list2);
  assert(list_count(&list2) == 9);

  for (i = 9; i <= 12; i++)
    {
      list_insert_descend(&list, &obj[i]);
      assert(!list_check(&list));
    }
  assert(!list_issorted_ascend(&list));
  assert(list_issorted_descend(&list));
  assert(list_count(&list) == 4);

  list_merge_descend(&list2, &list);
  assert(!list_check(&list2));
  assert(!list_check(&list));
  assert(list_count(&list2) == 13);
  assert(list_count(&list) == 0);
  assert(list_issorted_descend(&list2));

  list_destroy(&list2);

#endif

  /* Check locking effectiveness */

#ifdef CONTAINER_LOCK_test
  puts("Container locking test (" NAME_STR(TESTCONT) ")");

  list_wrlock(&list);
  assert(pthread_mutex_trylock(&list.lock) != 0);
  list_unlock(&list);

  assert(pthread_mutex_trylock(&list.lock) == 0);
  pthread_mutex_unlock(&list.lock);
#endif

  list_destroy(&list);

  return 0;
}

