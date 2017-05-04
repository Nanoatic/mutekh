/* -*- c -*-

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr> (C) 2009

    Circular Double Linked List template.

    The list is implemented with item structure as head/tail root.
*/

#ifndef GPCT_CONT_CLIST_H_
#define GPCT_CONT_CLIST_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_entry.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_pushpop_array.h>
#include <gpct/_cont_lists.h>

#define GPCT_CONT_CLIST_ARRAY_BASED     0
#define GPCT_CONT_CLIST_POINTER_BASED   1
#define GPCT_CONT_CLIST_ORDERED         1
#define GPCT_CONT_CLIST_SORTED          0
#define GPCT_CONT_CLIST_REORDER         1

/***********************************************************************
 *      types
 */

typedef struct                          gpct_clist_entry_s
{
  struct gpct_clist_entry_s             *next;
  struct gpct_clist_entry_s             *prev;
}                                       gpct_CLIST_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_CLIST_TYPE(name, type, entryfield)

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  struct gpct_clist_entry_s     ht;
}                                       name##_root_t;

typedef struct gpct_clist_entry_s       name##_entry_t;
typedef type                            name##_itembase_t;
typedef name##_itembase_t *             name##_item_t;
typedef name##_itembase_t *             name##_index_t;

static const name##_item_t
name##_gpct_null_item = NULL;

static const name##_index_t
name##_gpct_null_index = NULL;

static const uintptr_t
name##_gpct_offsetof = offsetof(name##_itembase_t, entryfield);

GPCT_CONT_ORPHAN_EMPTY_FUNC(CLIST, name, CONTAINER_ORPHAN_CHK_##name)
/* backslash-region-end */


/***********************************************************************
 *      orphan checking
 */

GPCT_INTERNAL void
gpct_orphan_CLIST__set(struct gpct_clist_entry_s *e)
{
  e->next = NULL;
}

GPCT_INTERNAL gpct_bool_t
gpct_orphan_CLIST__chk(struct gpct_clist_entry_s *e)
{
  return e->next != NULL;
}

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_ROOT_DECLARATOR(name, lockname, symbol)
name##_root_t symbol =
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .ht = { .next = &symbol.ht, .prev = &symbol.ht }
}
/* backslash-region-end */

/***********************************************************************
 *      access functions
 */

#define GPCT_CONT_CLIST_ISEND(root, entry)      ((entry) == &(root)->ht)
#define GPCT_CONT_CLIST_EMPTY(root)             ((root)->ht.next = (root)->ht.prev = &(root)->ht)

/* backslash-region-begin */
#define GPCT_CONT_CLIST_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_LISTS_INIT_FUNC(attr, name, CLIST, prefix, lockname)

GPCT_CONT_LISTS_ROOT_FUNC(attr, name, CLIST, prefix, lockname, ht.next)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  gpct_error_t          res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t *index_e = GPCT_CONT_GET_ENTRY(name, index);
    name##_entry_t *item_e = GPCT_CONT_GET_ENTRY(name, item);

    if (GPCT_CONT_ORPHAN_CHK(CLIST, name, index_e))
      {
        GPCT_CONT_REFNEW(name, item);

        (item_e->prev = index_e->prev)->next = item_e;
        (item_e->next = index_e->next)->prev = item_e;

        GPCT_CONT_ORPHAN_SET(CLIST, name, index_e);
        GPCT_CONT_REFDROP(name, index);
        res = 0;
      }
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, CLIST,
                               prefix, lockname,
                               NEXT, HEAD, next, ht.next)

GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, CLIST,
                               prefix, lockname,
                               PREV, TAIL, prev, ht.prev)

GPCT_CONT_LISTS_COUNT_FUNC(attr, name, CLIST, prefix,
                           lockname, ht.next)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix)
{
  name##_entry_t        *index_e;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  index_e = GPCT_CONT_GET_ENTRY(name, index);

  if (GPCT_CONT_ORPHAN_CHK(CLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      item_e->next = index_e;
      item_e->prev = index_e->prev;
      index_e->prev->next = item_e;
      index_e->prev = item_e;

      res = 1;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_INSERT_POST(name, prefix)
{
  name##_entry_t        *index_e;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  index_e = GPCT_CONT_GET_ENTRY(name, index);

  if (GPCT_CONT_ORPHAN_CHK(CLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      item_e->prev = index_e;
      item_e->next = index_e->next;
      index_e->next->prev = item_e;
      index_e->next = item_e;

      res = 1;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_CLEAR_FUNC(attr, name, CLIST,
                           prefix, lockname, ht.next)

GPCT_INTERNAL void
prefix##_remove_        (name##_root_t *root,
                         name##_entry_t *entry)
{
  entry->prev->next = entry->next;
  entry->next->prev = entry->prev;

  GPCT_CONT_ORPHAN_SET(CLIST, name, entry);
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  gpct_error_t  res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t      *index_e = GPCT_CONT_GET_ENTRY(name, index);

    if (GPCT_CONT_ORPHAN_CHK(CLIST, name, index_e))
      {
        prefix##_remove_(root, index_e);
        GPCT_CONT_REFDROP(name, index);

        res = 0;
      }

    GPCT_LOCK_UNLOCK(lockname, &root->lock);
  }

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  name##_entry_t    *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  item_e = GPCT_CONT_GET_ENTRY(name, item);
  item_e->prev = &root->ht;
  root->ht.next->prev = item_e;
  item_e->next = root->ht.next;
  root->ht.next = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  name##_entry_t    *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  item_e = GPCT_CONT_GET_ENTRY(name, item);
  item_e->next = &root->ht;
  root->ht.prev->next = item_e;
  item_e->prev = root->ht.prev;
  root->ht.prev = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP(name, prefix)
{
  name##_item_t         item = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((root->ht.next != &root->ht))
    {
      name##_entry_t    *head = root->ht.next;

      item = GPCT_CONT_GET_ITEM(name, head);
      prefix##_remove_(root, head);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POPBACK(name, prefix)
{
  name##_item_t         item = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((root->ht.next != &root->ht))
    {
      name##_entry_t    *tail = root->ht.prev;

      item = GPCT_CONT_GET_ITEM(name, tail);
      prefix##_remove_(root, tail);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_CONT_PUSHPOP_ARRAY_FUNC(attr, name, prefix, prefix, lockname);

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, CLIST, prefix,
                                 lockname, ht.next, next)

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)
GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, CLIST, prefix,
                                 lockname, ht.prev, prev)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  name##_entry_t        *entry;
  gpct_bool_t           res = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  entry = &root->ht;
  do
    {
      if (entry->next == NULL)
        res |= 1;

      if (entry->prev == NULL)
        res |= 2;

      if (entry->next->prev != entry)
        res |= 4;

      if (entry->prev->next != entry)
        res |= 8;

      entry = entry->next;
    }
  while (!res && (entry != &root->ht));

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */



/***********************************************************************
 *      iteration macros
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_FOREACH(name, root, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(CLIST, name, root,
                              (root)->ht.next, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_CLIST_FOREACH_FROM(name, root, index, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(CLIST, name, root,
			      GPCT_CONT_GET_ENTRY(name, index)->next, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_CLIST_FOREACH_REVERSE(name, root, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(CLIST, name, root,
                              (root)->ht.prev, prev, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_CLIST_FOREACH_REVERSE_FROM(name, root, index, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(CLIST, name, root,
			      GPCT_CONT_GET_ENTRY(name, index)->prev, prev, __VA_ARGS__)
/* backslash-region-end */


#define GPCT_CONT_CLIST_FOREACH_UNORDERED(name, root, ...)         \
 GPCT_CONT_CLIST_FOREACH(name, root, __VA_ARGS__)


/***********************************************************************
 *      key field based functions
 */

/* linked list merge/sort functions */
GPCT_CONT_LISTS_SORT_FUNC(CLIST, gpct_CLIST_entry_t);




/*
 * CLIST links rebuild after merge/sort template code
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_REBUILD_CODE(name, lockname, cmp, entry, root)
{
  name##_entry_t *prev;

  root->ht.next = entry;

  /* rebuild prev links */
  for (prev = &root->ht ; entry != NULL; entry = entry->next)
    {
      entry->prev = prev;
      prev = entry;
    }

  /* update links with root */
  prev->next = &root->ht;
  root->ht.prev = prev;
}
/* backslash-region-end */





/*
 * CLIST sort glue function template, call sort and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_SORT_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->ht.next != root->ht.prev)
    {
      name##_entry_t *tmp;

      /* terminate list with a NULL for sort */
      root->ht.prev->next = NULL;
      tmp = root->ht.next = gpct_CLIST_sort(root->ht.next, cmp);

      GPCT_CONT_CLIST_REBUILD_CODE(name, lockname, cmp, tmp, root);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */





/*
 * CLIST merge glue function template, call merge and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_MERGE_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &src->lock);
  GPCT_LOCK_WRLOCK(lockname, &dest->lock);

  if (src->ht.next != &src->ht)
    {
      if (dest->ht.next == &dest->ht)
	{
	  /* move from src to dest */
	  dest->ht = src->ht;
	  dest->ht.prev->next =
	    dest->ht.next->prev = &dest->ht;
	}
      else
	{
	  name##_entry_t *tmp;

	  /* terminate lists with a NULL for merge operation */
	  src->ht.prev->next = NULL;
	  dest->ht.prev->next = NULL;

	  tmp = gpct_CLIST_merge(dest->ht.next, src->ht.next, cmp);

	  GPCT_CONT_CLIST_REBUILD_CODE(name, lockname, cmp, tmp, dest);
	}

      GPCT_CONT_CLIST_EMPTY(src);
    }

  GPCT_LOCK_UNLOCK(lockname, &dest->lock);
  GPCT_LOCK_UNLOCK(lockname, &src->lock);
}
/* backslash-region-end */





/*
 * CLIST Ordered insertion function template
 */

/* backslash-region-begin */
#define GPCT_CONT_CLIST_ORDERED_INSERT_FUNCCODE(name, lockname, cmp)
{
  name##_entry_t        *index_e, *item_e;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  item_e = GPCT_CONT_GET_ENTRY(name, item);

  for (index_e = root->ht.next; index_e != &root->ht; index_e = index_e->next)
    if (cmp(item_e, index_e))
      break;

  GPCT_CONT_REFNEW(name, item);

  item_e->next = index_e;
  item_e->prev = index_e->prev;
  index_e->prev->next = item_e;
  index_e->prev = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */





/* backslash-region-begin */
#define GPCT_CONT_KEY_CLIST_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)

GPCT_CONT_LOOKUP_FUNC(attr, name, CLIST, prefix, lockname, keyfield);

GPCT_CONT_LISTS_CMP_FUNC(name, prefix, CLIST, keyfield)

attr GPCT_CONT_PROTO_SORT_ASCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_SORT_DESCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_SORT_STABLE_ASCEND(name, prefix, keyfield)
{
  prefix##_sort_ascend(root);
}

attr GPCT_CONT_PROTO_SORT_STABLE_DESCEND(name, prefix, keyfield)
{
  prefix##_sort_descend(root);
}

attr GPCT_CONT_PROTO_SORTED_ASCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(CLIST, name, lockname, prefix##_gpct_cmp_ascend, root->ht.next)

attr GPCT_CONT_PROTO_SORTED_DESCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(CLIST, name, lockname, prefix##_gpct_cmp_descend, root->ht.next)

attr GPCT_CONT_PROTO_INSERT_ASCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_INSERT_DESCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_MERGE_ASCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_MERGE_DESCEND(name, prefix, keyfield)
GPCT_CONT_CLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)
/* backslash-region-end */



#endif

