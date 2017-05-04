// example_ring.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_ring.h>

struct myitem
{
  const char			*data;
};

CONTAINER_TYPE(myring, RING, struct myitem, 16)
CONTAINER_FUNC(myring, RING, static, myfunc);

int main()
{
  myring_root_t ring;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&ring);
  myfunc_push(&ring, a);
  myfunc_push(&ring, b);

  puts(myfunc_pop(&ring).data);
  puts(myfunc_pop(&ring).data);

  return 0;
}

