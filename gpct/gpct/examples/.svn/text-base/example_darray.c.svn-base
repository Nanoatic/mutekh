// example_darray.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_darray.h>

struct myitem
{
  const char			*data;
};

CONTAINER_TYPE(myarray, DARRAY, int, 2, 8)
CONTAINER_FUNC(myarray, DARRAY, static, myfunc);

int main()
{
  myarray_root_t array;

  myfunc_init(&array);

  assert(myfunc_size(&array) == 0);

  myfunc_push(&array, 1);
  assert(myfunc_size(&array) == 2);
  myfunc_push(&array, 2);
  assert(myfunc_size(&array) == 2);

  myfunc_push(&array, 3);
  assert(myfunc_size(&array) == 4);

  myfunc_push(&array, 4);

  myfunc_push(&array, 5);
  assert(myfunc_size(&array) == 8);

  myfunc_push(&array, 6);
  myfunc_push(&array, 7);

  assert(myfunc_push(&array, 8)); /* last item slot filled here */

  assert(!myfunc_push(&array, 0));

  assert(myfunc_size(&array) == 8);

  myfunc_destroy(&array);

  return 0;
}

