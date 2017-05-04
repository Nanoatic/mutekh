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
  unsigned int				id;
  char					*name;
  uint8_t				blob[12];
  unsigned int				mask;
  struct aggregate_s			agg;
};

#ifdef TESTAGG
# define TESTAGG_OP	&
#else
# define TESTAGG_OP
#endif

CONTAINER_TYPE		(test, TESTCONT, TESTTYPE, TESTTYPE_EXTRA);

CONTAINER_KEY_TYPE	(test, PTR, TESTALGO, TESTKEY, TESTKEYTYPE_EXTRA);
CONTAINER_KEY_PROTOTYPE	(test, static, list, TESTKEY);

CONTAINER_PROTOTYPE	(test, static, list);
CONTAINER_FUNC		(test, TESTCONT, static, list, TESTFUNC_EXTRA);

CONTAINER_KEY_FUNC	(test, TESTCONT, static, list, TESTKEY, TESTKEYFUNC_EXTRA);

static unsigned int	iter_mask = 0;

static CONTAINER_ITERATOR(test, hash_iterator)
{
#ifdef DEBUG
  printf("object iter %p %s\n", item, item->name);
#endif
  iter_mask ^= item->mask;

  return 0;
}

#define ENTRY_COUNT	8
#define FIRST_DUP	5

int main(void)
{
  test_root_t	list;
  struct test_object_s	obj[ENTRY_COUNT] =
    {
      {
	.id = 16546,
	.name = "foo",
	.blob = "aksieprhqjdh",
	.agg = { .a = 123, .b = 456 },
	.mask = 1,
      },
      {
	.id = 36873,
	.name = "bar",
	.blob = "anwjshdy0123",
	.agg = { .a = 123, .b = 789 },
	.mask = 2,
      },
      {
	.id = 64357,
	.name = "prettylongname",
	.blob = "mwnqidhx0123",
	.agg = { .a = 456, .b = 789 },
	.mask = 4,
      },
      {
	.id = 3146,
	.name = "test",
	.blob = "0123bawidpqj",
	.agg = { .a = 416, .b = 913 },
	.mask = 8,
      },
      {
	.id = 3,
	.name = "",
	.blob = "0123duarbxjd",
	.agg = { .a = 416, .b = 813 },
	.mask = 16,
      },
      /* duplicate entries */
      {
	.id = 64357,
	.name = "prettylongname",
	.blob = "mwnqidhx0123",
	.agg = { .a = 456, .b = 789 },
	.mask = 32,
      },
      {
	.id = 3146,
	.name = "test",
	.blob = "0123bawidpqj",
	.agg = { .a = 416, .b = 913 },
	.mask = 64,
      },
      {
	.id = 3,
	.name = "",
	.blob = "0123duarbxjd",
	.agg = { .a = 416, .b = 813 },
	.mask = 128,
      },
    };

  unsigned int		i;

  puts("Container hash and compare test (" NAME_STR(TESTCONT) ", " NAME_STR(TESTALGO)")");

  list_init(&list);

  /* push entries */

  for (i = 0; i < FIRST_DUP; i++)
    assert(list_push(&list, &obj[i]) == 1);

  /* iterate over object list */

  assert(list_foreach(&list, hash_iterator) == 0);
  assert(iter_mask == (1 << FIRST_DUP) - 1);

  /* test lookup */

  for (i = 0; i < FIRST_DUP; i++)
    {
      test_index_t	idx = list_lookup(&list, TESTAGG_OP obj[i].TESTKEY);
      struct test_object_s *ent = list_get(&list, idx);

#ifdef DEBUG
      printf("ent %p, obj %p %s\n", ent, &obj[i], obj[i].name);
#endif
      assert(ent == &obj[i]);

      idx = list_lookup_next(&list, idx, TESTAGG_OP obj[i].TESTKEY);
      assert(list_isnull(idx));
    }

  for (i = 0; i < FIRST_DUP; i++)
    {
      test_index_t	idx = list_lookup_last(&list, TESTAGG_OP obj[i].TESTKEY);
      struct test_object_s *ent = list_get(&list, idx);

      assert(ent == &obj[i]);

      idx = list_lookup_prev(&list, idx, TESTAGG_OP obj[i].TESTKEY);
      assert(list_isnull(idx));
    }

  /* test remove_key */
#if 0
  assert(!list_remove_key(&list, TESTAGG_OP obj[3].TESTKEY));
  assert(list_isnull(list_lookup(&list, TESTAGG_OP obj[3].TESTKEY)));
  assert(list_push(&list, &obj[3]) == 1);
#endif
  /* add entries with duplicated key values */

  for (i = FIRST_DUP; i < ENTRY_COUNT; i++)
    assert(list_push(&list, &obj[i]) == 1);

  /* test lookup with duplicate entries */

  for (iter_mask = i = 0; i < FIRST_DUP; i++)
    {
      test_index_t	idx;

      for (idx = list_lookup(&list, TESTAGG_OP obj[i].TESTKEY); !list_isnull(idx);
	   idx = list_lookup_next(&list, idx, TESTAGG_OP obj[i].TESTKEY))
	{
	  struct test_object_s *ent = list_get(&list, idx);

	  iter_mask ^= ent->mask;
	}
    }

  assert(iter_mask == (1 << ENTRY_COUNT) - 1);

  for (iter_mask = i = 0; i < FIRST_DUP; i++)
    {
      test_index_t	idx;

      for (idx = list_lookup_last(&list, TESTAGG_OP obj[i].TESTKEY); !list_isnull(idx);
	   idx = list_lookup_prev(&list, idx, TESTAGG_OP obj[i].TESTKEY))
	{
	  struct test_object_s *ent = list_get(&list, idx);

	  iter_mask ^= ent->mask;
	}
    }

  assert(iter_mask == (1 << ENTRY_COUNT) - 1);

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

