// example_iterate.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_slist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(SLIST)	list_entry;
  const char			*data;
};

CONTAINER_TYPE(mylist, SLIST, struct myitem, list_entry);
CONTAINER_FUNC(mylist, SLIST, static, myfunc);

CONTAINER_ITERATOR(mylist, myiter_func)
{
  char *str = va_arg(ap, char *);

  printf("%s %s\n", str, item->data);
  return 0;
}

int main()
{
  mylist_root_t	list;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&list);
  myfunc_push(&list, &a);
  myfunc_push(&list, &b);

  myfunc_foreach(&list, myiter_func, "data:");

  return 0;
}

