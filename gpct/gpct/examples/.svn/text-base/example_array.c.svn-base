// example_array.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_array.h>

struct myitem
{
  const char			*data;
};

CONTAINER_TYPE(myarray, ARRAY, struct myitem, 16)
CONTAINER_FUNC(myarray, ARRAY, static, myfunc);

int main()
{
  myarray_root_t array;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&array);
  myfunc_pushback(&array, a);
  myfunc_pushback(&array, b);

  puts(myfunc_popback(&array).data);
  puts(myfunc_popback(&array).data);

  return 0;
}

