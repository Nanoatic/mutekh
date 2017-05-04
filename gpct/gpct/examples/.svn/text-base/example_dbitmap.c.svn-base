// example_dbitmap.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_dbitmap.h>

CONTAINER_TYPE(mybitmap, DBITMAP, unsigned int, 5);
CONTAINER_FUNC(mybitmap, DBITMAP, static, myfunc);

int main()
{
  mybitmap_root_t	bitmap;

  myfunc_init(&bitmap);

  myfunc_resize(&bitmap, 42);

  myfunc_set(&bitmap, 3, 1);
  myfunc_set(&bitmap, 11, 1);

  printf("2: %i\n", myfunc_get(&bitmap, 2));
  printf("3: %i\n", myfunc_get(&bitmap, 3));

  CONTAINER_FOREACH(mybitmap, DBITMAP, &bitmap,
  {
    printf("%i is set\n", index);
  });

  return 0;
}

