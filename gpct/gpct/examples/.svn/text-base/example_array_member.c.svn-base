// example_array_member.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_array.h>

struct myitem
{
  int a;
};

CONTAINER_TYPE(mystructarray, ARRAY, struct myitem, 16)
CONTAINER_FUNC(mystructarray, ARRAY, static, mystructfunc);

CONTAINER_KEY_TYPE(mystructarray, MEMBER, SCALAR, a)
CONTAINER_KEY_FUNC(mystructarray, ARRAY, static, mystructfunc, a);

int main()
{
  mystructarray_root_t array;
  struct myitem items[3] = { { .a = 7 }, { .a = 11 }, { .a = 5 } };

  mystructfunc_init(&array);
  mystructfunc_pushback_array(&array, items, 3);

  printf("index :%i\n", mystructfunc_lookup(&array, 11));

  return 0;
}
