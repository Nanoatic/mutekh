// example_bitmap.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_bitmap.h>

CONTAINER_TYPE(mybitmap, BITMAP, unsigned int, 37);
CONTAINER_FUNC(mybitmap, BITMAP, static, myfunc);

int main()
{
  mybitmap_root_t	bitmap;

  myfunc_init(&bitmap);

  myfunc_set(&bitmap, 3, 1);
  myfunc_set(&bitmap, 11, 1);

  printf("2: %i\n", myfunc_get(&bitmap, 2));
  printf("3: %i\n", myfunc_get(&bitmap, 3));

  CONTAINER_FOREACH(mybitmap, BITMAP, &bitmap,
  {
    printf("%i is set\n", index);
  });

  return 0;
}

