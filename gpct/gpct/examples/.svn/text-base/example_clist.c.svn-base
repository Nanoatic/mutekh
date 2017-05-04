// example_clist.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_clist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(CLIST)	list_entry;
  const char			*data;
};

CONTAINER_TYPE(mylist, CLIST, struct myitem, list_entry);
CONTAINER_FUNC(mylist, CLIST, static, myfunc);

int main()
{
  mylist_root_t	list;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&list);
  myfunc_push(&list, &a);
  myfunc_push(&list, &b);

  puts(myfunc_pop(&list)->data);
  puts(myfunc_pop(&list)->data);

  return 0;
}

