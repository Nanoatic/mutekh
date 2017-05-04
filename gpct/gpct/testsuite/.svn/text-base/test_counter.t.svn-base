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

m4_changecom
#include <INCLUDE>

struct test_object_s
{
  CONTAINER_ENTRY_TYPE(TESTCONT)	list_entry;
  unsigned int				id;
};

#define CONTAINER_COUNTER_test

CONTAINER_TYPE		(test, TESTCONT, TESTTYPE, TESTTYPE_EXTRA);

CONTAINER_KEY_TYPE	(test, PTR, TESTALGO, TESTKEY, TESTKEYTYPE_EXTRA);
CONTAINER_KEY_PROTOTYPE	(test, static, list, TESTKEY);

CONTAINER_PROTOTYPE	(test, static, list);
CONTAINER_FUNC		(test, TESTCONT, static, list, TESTFUNC_EXTRA);

CONTAINER_KEY_FUNC	(test, TESTCONT, static, list, TESTKEY, TESTKEYFUNC_EXTRA);

#define ENTRY_COUNT	8

int main()
{
  test_root_t	list;
  struct test_object_s *o;
  struct test_object_s	obj[ENTRY_COUNT] =
    {
      {
	.id = 16546,
      },
      {
	.id = 36873,
      },
      {
	.id = 64357,
      },
      {
	.id = 3146,
      },
      {
	.id = 3,
      },
      {
	.id = 64357,
      },
      {
	.id = 3146,
      },
      {
	.id = 3,
      },
    };

  unsigned int		i;

  puts("Container counter test (" NAME_STR(TESTCONT) ")");

  list_init(&list);

  /* push entries */

  assert(list_isempty(&list));
  assert(!list_check(&list));

  for (i = 0; i < ENTRY_COUNT; i++)
    {
      assert(list_count(&list) == i);
      assert(list_push(&list, &obj[i]) == 1);
      assert(!list_check(&list));
      assert(!list_isempty(&list));
    }

  list_remove(&list, &obj[2]);
  assert(!list_check(&list));
  assert(list_count(&list) == ENTRY_COUNT - 1);

  o = list_pop(&list);
  assert(!list_check(&list));
  assert(list_count(&list) == ENTRY_COUNT - 2);

  list_pushback(&list, o);
  assert(!list_check(&list));
  assert(list_count(&list) == ENTRY_COUNT - 1);

  list_popback(&list);
  assert(!list_check(&list));
  assert(list_count(&list) == ENTRY_COUNT - 2);

  list_clear(&list);
  assert(!list_check(&list));
  assert(list_count(&list) == 0);

  return 0;
}

