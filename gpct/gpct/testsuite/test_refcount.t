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

#include <gpct/object_refcount.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

m4_changecom
#include <INCLUDE>

struct test_object_s;

OBJECT_TYPE		(test_obj, REFCOUNT, struct test_object_s);
OBJECT_PROTOTYPE	(test_obj, static, obj);

struct test_object_s
{
  CONTAINER_ENTRY_TYPE(TESTCONT)	list_entry;
  test_obj_entry_t			obj_entry;
  char					*name;
  unsigned int				mask;
};

#define CONTAINER_OBJ_test	obj

CONTAINER_TYPE		(test, TESTCONT, TESTTYPE, TESTTYPE_EXTRA);
CONTAINER_PROTOTYPE	(test, static, list);

m4_ifdef(`TESTALGO', `
CONTAINER_KEY_TYPE	(test, PTR, TESTALGO, TESTKEY, TESTKEYTYPE_EXTRA);
CONTAINER_KEY_PROTOTYPE	(test, static, list, TESTKEY);
')

static OBJECT_CONSTRUCTOR(test_obj)
{
  obj->name = va_arg(ap, char *);
  obj->mask = va_arg(ap, unsigned int);

#ifdef DEBUG
  printf("object alloc %p\n", obj);
#endif

  return 0;
}

static unsigned int	drop_mask = 0;

static OBJECT_DESTRUCTOR(test_obj)
{
#ifdef DEBUG
  printf("object free %p\n", obj);
#endif
  drop_mask |= obj->mask;
}

OBJECT_FUNC(test_obj, REFCOUNT, static, obj, obj_entry);
CONTAINER_FUNC(test, TESTCONT, static, list, TESTFUNC_EXTRA);

m4_ifdef(`TESTALGO', `
CONTAINER_KEY_FUNC	(test, TESTCONT, static, list, TESTKEY, TESTKEYFUNC_EXTRA);
')

static unsigned int	iter_mask = 0;

static CONTAINER_ITERATOR(test, object_iterator)
{
#ifdef DEBUG
  printf("object iter %p %s\n", item, item->name);
#endif
  iter_mask |= item->mask;

  return 0;
}

static unsigned int popcount(unsigned int x)
{
  int	i, c;

  for (c = i = 0; i < sizeof(x) * 8; i++)
    if (x & (1 << i))
      c++;

  return c;
}

int main(void)
{
  test_root_t	list;
  struct test_object_s	*obj, *obj2;

  puts("Reference counting test (" NAME_STR(TESTCONT)")");

  list_init(&list);

  obj = obj_new(NULL, "foo", 0x01);
  assert(list_push(&list, obj) == 1);
  obj_refdrop(obj);

  obj = obj_new(NULL, "bar", 0x10);
  assert(list_pushback(&list, obj) == 1);
  obj_refdrop(obj);

  obj = obj_new(NULL, "tee", 0x100);
  assert(list_push(&list, obj) == 1);
  obj_refdrop(obj);

  assert(list_count(&list) == 3);

  /* iterate over object list */

  assert(list_foreach(&list, object_iterator) == 0);
  assert(iter_mask == 0x111);

  /* pop an object and drop it */
  obj = list_pop(&list);
  assert(drop_mask == 0);

  obj_refdrop(obj);
  assert(popcount(drop_mask) == 1);

  /* pop an object, push it back, replace it */
  obj = list_pop(&list);
  assert(popcount(drop_mask) == 1);

  assert(list_push(&list, obj) == 1);  
  obj_refdrop(obj);
  assert(popcount(drop_mask) == 1);

  obj2 = obj_new(NULL, "replace", 0x1000);
#if CONTAINER_DEF(TESTCONT, ARRAY_BASED)
# if CONTAINER_DEF(TESTCONT, SORTED) || !CONTAINER_DEF(TESTCONT, ORDERED)
  list_remove(&list, 0);
  list_pushback(&list, obj2);
# else
  assert(list_set(&list, 0, obj2) == 0);
# endif
#else
# if CONTAINER_DEF(TESTCONT, SORTED) || !CONTAINER_DEF(TESTCONT, ORDERED)
  list_remove(&list, obj);
  list_pushback(&list, obj2);
# else
  assert(list_set(&list, obj, obj2) == 0);
# endif
#endif

  assert(popcount(drop_mask) == 2);
  obj_refdrop(obj2);
  assert(popcount(drop_mask) == 2);

  /* iterate over object list */
  iter_mask = 0;
  assert(list_foreach(&list, object_iterator) == 0);
  assert(iter_mask & 0x1000);
  assert(popcount(iter_mask) == 2);

  /* remove replacement object */

#if CONTAINER_DEF(TESTCONT, ARRAY_BASED)
  assert(list_remove(&list, 0) == 0);
#else
  assert(list_remove(&list, obj2) == 0);
#endif
  assert(popcount(drop_mask) == 3);

  /* pop and refdrop last object */
  obj_refdrop(list_pop(&list));
  assert(drop_mask == 0x1111);

  return 0;
}

