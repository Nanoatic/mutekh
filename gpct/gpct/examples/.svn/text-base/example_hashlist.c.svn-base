// example_hashlist.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <stdio.h>
#include <gpct/cont_hashlist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(HASHLIST)        hash_entry;
  const char                            *data;
};

CONTAINER_TYPE    (myhash, HASHLIST, struct myitem, hash_entry, 11);
CONTAINER_KEY_TYPE(myhash, PTR, STRING, data);

CONTAINER_FUNC    (myhash, HASHLIST, static, myfunc, data);
CONTAINER_KEY_FUNC(myhash, HASHLIST, static, myfunc, data);

int main()
{
  myhash_root_t hash;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&hash);
  myfunc_push(&hash, &a);
  myfunc_push(&hash, &b);

  assert(myfunc_lookup(&hash, "foo") == &a);
  assert(myfunc_lookup(&hash, "bar") == &b);

  return 0;
}

