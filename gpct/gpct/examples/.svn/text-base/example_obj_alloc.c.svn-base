// example_obj_alloc.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/object_simple.h>

OBJECT_TYPE     (myobj, SIMPLE, struct myitem);
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

OBJECT_FUNC   (myobj, SIMPLE, static, myobjfunc, obj_entry);

int main()
{
  struct myitem *a;

  a = myobjfunc_new(NULL, "foo");
  myobjfunc_delete(a);

  return 0;
}

