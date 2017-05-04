// example_container_refcount.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#define GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX

#include <stdio.h>
#include <gpct/cont_slist.h>
#include <gpct/object_refcount.h>

OBJECT_TYPE     (myobj, REFCOUNT, struct myitem);
OBJECT_PROTOTYPE(myobj, static, myobjfunc);

struct myitem
{
  CONTAINER_ENTRY_TYPE(SLIST)	list_entry;
  myobj_entry_t			obj_entry;
  const char			*data;
};

#define CONTAINER_OBJ_mylist myobjfunc

CONTAINER_TYPE     (mylist, SLIST, struct myitem, list_entry);
CONTAINER_PROTOTYPE(mylist, static, myfunc);

static OBJECT_CONSTRUCTOR(myobj)
{
  obj->data = va_arg(ap, const char *);

  return 0;
}

static OBJECT_DESTRUCTOR(myobj)
{
  printf("%s droped\n", obj->data);
}

OBJECT_FUNC   (myobj, REFCOUNT, static, myobjfunc, obj_entry);
CONTAINER_FUNC(mylist, SLIST, static, myfunc);

#define myobjfunc_new(...) myobjfunc_new(NULL, __VA_ARGS__)

int main()
{
  mylist_root_t	list;

  struct myitem *a;

  myfunc_init(&list);

  a = myobjfunc_new("foo");
  myfunc_push(&list, a);
  myobjfunc_refdrop(a);

  a = myobjfunc_new("bar");
  myfunc_push(&list, a);
  myobjfunc_refdrop(a);

  puts("clear");

  myfunc_clear(&list);

  return 0;
}

