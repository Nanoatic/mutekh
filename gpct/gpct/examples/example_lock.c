// example_lock.c GPCT example
// Copyright Alexandre Becoulet (C) 2009,
// Released under the GPL v3, see COPYING

#include <gpct/lock_pthread_mutex.h>
#include <gpct/cont_slist.h>

struct myitem
{
  CONTAINER_ENTRY_TYPE(SLIST)   list_entry;
  const char                    *data;
};

#define CONTAINER_LOCK_mylist PTHREAD_MUTEX

CONTAINER_TYPE       (mylist, SLIST, struct myitem, list_entry);
CONTAINER_FUNC       (mylist, SLIST, static, myfunc);
CONTAINER_FUNC_NOLOCK(mylist, SLIST, static, myfunc_nolock);

int main()
{
  mylist_root_t list;

  struct myitem a = { .data = "foo" };
  struct myitem b = { .data = "bar" };

  myfunc_init(&list);

  myfunc_wrlock(&list);          /* single write lock batched accesses */
  myfunc_nolock_push(&list, &a);
  myfunc_nolock_push(&list, &b);
  myfunc_unlock(&list);

  myfunc_pop(&list);             /* multiple write locked accesses */
  myfunc_pop(&list);

  return 0;
}
