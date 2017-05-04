// example_dring.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_dring.h>

struct myitem
{
  const char			*data;
};

CONTAINER_TYPE(myring, DRING, int, 2, 8)
CONTAINER_FUNC(myring, DRING, static, myfunc);

int main()
{
  myring_root_t ring;

  myfunc_init(&ring);

  assert(myfunc_size(&ring) == 0);

  myfunc_push(&ring, 0);
  assert(myfunc_size(&ring) == 2);
  myfunc_push(&ring, 0);
  assert(myfunc_size(&ring) == 2);

  myfunc_push(&ring, 0);
  assert(myfunc_size(&ring) == 4);

  myfunc_push(&ring, 0);

  myfunc_push(&ring, 0);
  assert(myfunc_size(&ring) == 8);

  myfunc_push(&ring, 0);
  myfunc_push(&ring, 0);

  assert(myfunc_push(&ring, 0));
  assert(!myfunc_push(&ring, 0));

  assert(myfunc_size(&ring) == 8);

  myfunc_destroy(&ring);

  return 0;
}

