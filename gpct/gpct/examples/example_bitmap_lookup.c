// example_bitmap_lookup.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <assert.h>
#include <stdio.h>
#include <gpct/cont_bitmap.h>

CONTAINER_TYPE(mybitmap, BITMAP, unsigned int, 37);
CONTAINER_FUNC(mybitmap, BITMAP, static, myfunc);

CONTAINER_KEY_FUNC(mybitmap, BITMAP, static, myfunc);

int main()
{
  mybitmap_root_t	bitmap;
  mybitmap_index_t	i;

  myfunc_init(&bitmap);

  myfunc_set(&bitmap, 3, 1);
  myfunc_set(&bitmap, 4, 1);
  myfunc_set(&bitmap, 5, 1);
  myfunc_set(&bitmap, 11, 1);
  myfunc_set(&bitmap, 33, 1);

  i = myfunc_lookup(&bitmap, 1);
  printf("first bit set found at %i\n", i);
  assert(i == 3);

  i = myfunc_lookup_next(&bitmap, i, 0);
  printf("next bit cleared found at %i\n", i);
  assert(i == 6);

  /* find all cleared bits */
  for (i = myfunc_lookup     (&bitmap, 0);
       !   myfunc_isnull     (i);
       i = myfunc_lookup_next(&bitmap, i, 0))
    printf("bit set found at %i\n", i);

  return 0;
}

