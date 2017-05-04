// example_array_ptr.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_array.h>

struct myitem
{
  int a;
};

CONTAINER_TYPE(myptrarray, ARRAY, struct myitem *, 16)
CONTAINER_FUNC(myptrarray, ARRAY, static, myptrfunc);

CONTAINER_KEY_TYPE(myptrarray, PTR, SCALAR, a)
CONTAINER_KEY_FUNC(myptrarray, ARRAY, static, myptrfunc, a);

int main()
{
  myptrarray_root_t array;
  struct myitem items[3] = { { .a = 7 }, { .a = 11 }, { .a = 5 } };

  myptrfunc_init(&array);
  myptrfunc_pushback(&array, &items[0]);
  myptrfunc_pushback(&array, &items[1]);
  myptrfunc_pushback(&array, &items[2]);

  printf("index :%i\n", myptrfunc_lookup(&array, 11));

  return 0;
}
