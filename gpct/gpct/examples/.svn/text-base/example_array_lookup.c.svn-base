// example_array_lookup.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_array.h>

CONTAINER_TYPE(myintarray, ARRAY, int, 16)
CONTAINER_FUNC(myintarray, ARRAY, static, myintfunc);

CONTAINER_KEY_TYPE(myintarray, DIRECT, SCALAR)
CONTAINER_KEY_FUNC(myintarray, ARRAY, static, myintfunc);

int main()
{
  myintarray_root_t array;

  myintfunc_init(&array);
  myintfunc_pushback(&array, 7);
  myintfunc_pushback(&array, 11);
  myintfunc_pushback(&array, 5);

  printf("index :%i\n", myintfunc_lookup(&array, 11));

  return 0;
}

