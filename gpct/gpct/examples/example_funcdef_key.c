// example_funcdef_key.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <assert.h>
#include <gpct/cont_slist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(SLIST)   list_entry;
  const char                    *data;
};

CONTAINER_TYPE(mylist, SLIST, struct myitem, list_entry);
CONTAINER_FUNC(mylist, SLIST, static, myfunc);

CONTAINER_KEY_TYPE(mylist, PTR, STRING, data);
CONTAINER_KEY_FUNC(mylist, SLIST, static, myfunc_data, data);

int main()
{
  mylist_root_t list;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&list);
  myfunc_push(&list, &a);
  myfunc_push(&list, &b);

  assert(myfunc_data_lookup(&list, "foo") == &a);
  assert(myfunc_data_lookup(&list, "bar") == &b);

  return 0;
}

