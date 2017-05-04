// example_custom_access.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <assert.h>
#include <gpct/cont_hashlist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(HASHLIST) hash_entry;
  const char *data;
};

static const char * get_item_data(struct myitem *item)
{
  return item->data;
}

CONTAINER_TYPE    (myhash, HASHLIST, struct myitem, hash_entry, 11);
CONTAINER_KEY_TYPE(myhash, CUSTOM, STRING, get_item_data(myhash_item), data_key);

CONTAINER_FUNC    (myhash, HASHLIST, static, myfunc, data_key);
CONTAINER_KEY_FUNC(myhash, HASHLIST, static, myfunc, data_key);

int main()
{
  myhash_root_t hash;
  struct myitem a = { .data = "foo" };

  myfunc_init(&hash);
  myfunc_push(&hash, &a);
  assert(myfunc_lookup(&hash, "foo") == &a);

  return 0;
}
