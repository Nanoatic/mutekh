// example_orphan.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_blist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(BLIST)	list_entry;
  const char			*data;
};

#define CONTAINER_ORPHAN_CHK_mylist

CONTAINER_TYPE(mylist, BLIST, struct myitem, list_entry);
CONTAINER_FUNC(mylist, BLIST, static, myfunc);

int main()
{
  mylist_root_t	list;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&list);
  myfunc_push(&list, &a);
  myfunc_push(&list, &b);

  assert(!myfunc_remove(&list, &a));
  assert( myfunc_remove(&list, &a));

  myfunc_pop(&list);
  assert( myfunc_remove(&list, &b));

  return 0;
}

