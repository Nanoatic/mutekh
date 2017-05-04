// example_obj_refcount.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#define GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX

#include <stdio.h>
#include <gpct/object_refcount.h>

OBJECT_TYPE     (myobj, REFCOUNT, struct myitem);
OBJECT_PROTOTYPE(myobj, static, myobjfunc);

struct myitem
{
  myobj_entry_t			obj_entry;
  const char			*data;
};

#define CONTAINER_OBJ_mylist myobjfunc

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

int main()
{
  struct myitem *a;

  a = myobjfunc_new(NULL, "foo");
  assert(myobjfunc_refcount(a) == 1);

  myobjfunc_refnew(a);
  assert(myobjfunc_refcount(a) == 2);

  myobjfunc_refdrop(a);
  assert(myobjfunc_refcount(a) == 1);

  myobjfunc_refdrop(a);

  return 0;
}

