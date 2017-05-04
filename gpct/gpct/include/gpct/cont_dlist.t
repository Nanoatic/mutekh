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

    Double Linked List template.

    The list is implemented with head and tail pointers in container
    root. Null pointers are used as list terminators.
*/

#ifndef GPCT_CONT_DLIST_H_
#define GPCT_CONT_DLIST_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_entry.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_pushpop_array.h>
#include <gpct/_cont_lists.h>

#define GPCT_CONT_DLIST_ARRAY_BASED     0
#define GPCT_CONT_DLIST_POINTER_BASED   1
#define GPCT_CONT_DLIST_ORDERED         1
#define GPCT_CONT_DLIST_SORTED          0
#define GPCT_CONT_DLIST_REORDER         1

/***********************************************************************
 *      types
 */

typedef struct                          gpct_dlist_entry_s
{
  struct gpct_dlist_entry_s             *next;
  struct gpct_dlist_entry_s             *prev;
}                                       gpct_DLIST_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_DLIST_TYPE(name, type, entryfield)

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  struct gpct_dlist_entry_s             *head;
  struct gpct_dlist_entry_s             *tail;
}                                       name##_root_t;

typedef struct gpct_dlist_entry_s       name##_entry_t;
typedef type                            name##_itembase_t;
typedef name##_itembase_t *             name##_item_t;
typedef name##_itembase_t *             name##_index_t;

static const name##_item_t
name##_gpct_null_item = NULL;

static const name##_index_t
name##_gpct_null_index = NULL;

static const uintptr_t
name##_gpct_offsetof = offsetof(name##_itembase_t, entryfield);

GPCT_CONT_ORPHAN_EMPTY_FUNC(DLIST, name, CONTAINER_ORPHAN_CHK_##name)
/* backslash-region-end */


/***********************************************************************
 *      orphan checking
 */

GPCT_INTERNAL void
gpct_orphan_DLIST__set(struct gpct_dlist_entry_s *e)
{
  e->next = e;
}

GPCT_INTERNAL gpct_bool_t
gpct_orphan_DLIST__chk(struct gpct_dlist_entry_s *e)
{
  return e != e->next;
}

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .head = NULL,
  .tail = NULL,
}
/* backslash-region-end */

#define GPCT_CONT_DLIST_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_DLIST_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

#define GPCT_CONT_DLIST_ISEND(root, entry)      ((entry) == NULL)
#define GPCT_CONT_DLIST_EMPTY(root)             ((root)->head = (root)->tail = NULL)

/* backslash-region-begin */
#define GPCT_CONT_DLIST_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_LISTS_INIT_FUNC(attr, name, DLIST, prefix, lockname)

GPCT_CONT_LISTS_ROOT_FUNC(attr, name, DLIST, prefix, lockname, head)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  gpct_error_t          res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t *index_e = GPCT_CONT_GET_ENTRY(name, index);
    name##_entry_t *item_e = GPCT_CONT_GET_ENTRY(name, item);

    if (GPCT_CONT_ORPHAN_CHK(DLIST, name, index_e))
      {
        name##_entry_t *next;
        name##_entry_t *prev;

        GPCT_CONT_REFNEW(name, item);

        next = item_e->next = index_e->next;
        prev = item_e->prev = index_e->prev;
        *(prev ? &prev->next : &root->head) = item_e;
        *(next ? &next->prev : &root->tail) = item_e;

        GPCT_CONT_ORPHAN_SET(DLIST, name, index_e);
        GPCT_CONT_REFDROP(name, index);
        res = 0;
      }
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, DLIST,
                               prefix, lockname,
                               NEXT, HEAD, next, head)

GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, DLIST,
                               prefix, lockname,
                               PREV, TAIL, prev, tail)

GPCT_CONT_LISTS_COUNT_FUNC(attr, name, DLIST, prefix,
                           lockname, head)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix)
{
  name##_entry_t        *index_e;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  index_e = GPCT_CONT_GET_ENTRY(name, index);

  if (GPCT_CONT_ORPHAN_CHK(DLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      item_e->next = index_e;
      item_e->prev = index_e->prev;
      *(index_e->prev ? &index_e->prev->next : &root->head) = item_e;
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

  if (GPCT_CONT_ORPHAN_CHK(DLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      item_e->prev = index_e;
      item_e->next = index_e->next;
      *(index_e->next ? &index_e->next->prev : &root->tail) = item_e;
      index_e->next = item_e;

      res = 1;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_CLEAR_FUNC(attr, name, DLIST, prefix, lockname, head)

GPCT_INTERNAL void
prefix##_remove_        (name##_root_t *root,
                         name##_entry_t *entry)
{
  name##_entry_t *next = entry->next;
  name##_entry_t *prev = entry->prev;

  *(prev ? &prev->next : &root->head) = next;
  *(next ? &next->prev : &root->tail) = prev;

  GPCT_CONT_ORPHAN_SET(DLIST, name, entry);
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  gpct_error_t  res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t      *index_e = GPCT_CONT_GET_ENTRY(name, index);

    if (GPCT_CONT_ORPHAN_CHK(DLIST, name, index_e))
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
  name##_entry_t        *head;
  name##_entry_t        *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  head = root->head;
  item_e = GPCT_CONT_GET_ENTRY(name, item);
  item_e->prev = 0;
  item_e->next = head;
  *(head ? &head->prev : &root->tail) = item_e;
  root->head = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  name##_entry_t        *tail;
  name##_entry_t        *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  tail = root->tail;
  item_e = GPCT_CONT_GET_ENTRY(name, item);

  item_e->next = 0;
  item_e->prev = tail;
  *(tail ? &tail->next : &root->head) = item_e;
  root->tail = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP(name, prefix)
{
  name##_entry_t        *head;
  name##_item_t         item = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((head = root->head))
    {
      item = GPCT_CONT_GET_ITEM(name, head);
      prefix##_remove_(root, head);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POPBACK(name, prefix)
{
  name##_entry_t        *tail;
  name##_item_t         item = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((tail = root->tail))
    {
      item = GPCT_CONT_GET_ITEM(name, tail);
      prefix##_remove_(root, tail);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_CONT_PUSHPOP_ARRAY_FUNC(attr, name, prefix, prefix, lockname);

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, DLIST, prefix,
                                 lockname, head, next)

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)
GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, DLIST, prefix,
                                 lockname, tail, prev)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  name##_entry_t        *entry;
  gpct_error_t          res = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (entry = root->head; !res && entry != NULL; entry = entry->next)
    {
      if (entry->next == entry || entry->prev == entry)
        res |= 1;

      if (root->head == entry && entry->prev != NULL)
        res |= 2;

      if (root->tail == entry && entry->next != NULL)
        res |= 4;

      if (entry->next != NULL && entry->next->prev != entry)
        res |= 8;

      if (entry->prev != NULL && entry->prev->next != entry)
        res |= 16;

      if (entry->next == root->head)
        res |= 32;

      if (entry->prev == root->tail)
        res |= 64;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_FOREACH(name, root, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(DLIST, name, root,
                              (root)->head, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_DLIST_FOREACH_FROM(name, root, index, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(DLIST, name, root,
			      GPCT_CONT_GET_ENTRY(name, index)->next, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_DLIST_FOREACH_REVERSE(name, root, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(DLIST, name, root,
                              (root)->tail, prev, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_DLIST_FOREACH_REVERSE_FROM(name, root, index, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(DLIST, name, root,
			      GPCT_CONT_GET_ENTRY(name, index)->prev, prev, __VA_ARGS__)
/* backslash-region-end */


#define GPCT_CONT_DLIST_FOREACH_UNORDERED(name, root, ...)         \
 GPCT_CONT_DLIST_FOREACH(name, root, __VA_ARGS__)


/***********************************************************************
 *      key field based functions
 */

GPCT_CONT_LISTS_SORT_FUNC(DLIST, gpct_DLIST_entry_t)




/*
 * DLIST links rebuild after merge/sort template code
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_REBUILD_CODE(name, lockname, cmp, entry, root)
{
  name##_entry_t *prev;
  root->head = entry;

  for (prev = NULL; entry != NULL; entry = entry->next)
    {
      entry->prev = prev;
      prev = entry;
    }

  root->tail = prev;
}
/* backslash-region-end */





/*
 * DLIST sort glue function template, call sort and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_SORT_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->head != NULL && root->head->next != NULL)
    {
      name##_entry_t *tmp = gpct_DLIST_sort(root->head, cmp);

      GPCT_CONT_DLIST_REBUILD_CODE(name, lockname, cmp, tmp, root);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */





/*
 * DLIST merge glue function template, call merge and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_MERGE_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &src->lock);
  GPCT_LOCK_WRLOCK(lockname, &dest->lock);

  if (src->head != NULL)
    {
      if (dest->head == NULL)
	{
	  /* move from src to dest */
	  dest->head = src->head;
	  dest->tail = src->tail;
	}
      else
	{
	  name##_entry_t *tmp = gpct_DLIST_merge(dest->head, src->head, cmp);

	  GPCT_CONT_DLIST_REBUILD_CODE(name, lockname, cmp, tmp, dest);
	}

      GPCT_CONT_DLIST_EMPTY(src);
    }

  GPCT_LOCK_UNLOCK(lockname, &dest->lock);
  GPCT_LOCK_UNLOCK(lockname, &src->lock);
}
/* backslash-region-end */





/*
 * DLIST Ordered insertion function template
 */

/* backslash-region-begin */
#define GPCT_CONT_DLIST_ORDERED_INSERT_FUNCCODE(name, lockname, cmp)
{
  name##_entry_t        *index_e, *item_e, **prev;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  item_e = GPCT_CONT_GET_ENTRY(name, item);

  index_e = NULL;
  for (prev = &root->head; *prev != NULL; (index_e = *prev), (prev = &(*prev)->next))
    if (cmp(item_e, *prev))
      break;

  GPCT_CONT_REFNEW(name, item);

  item_e->next = *prev;
  item_e->prev = index_e;
  *(*prev ? &(*prev)->prev : &root->tail) = item_e;
  *prev = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */





/* backslash-region-begin */
#define GPCT_CONT_KEY_DLIST_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)

GPCT_CONT_LOOKUP_FUNC(attr, name, DLIST, prefix, lockname, keyfield);

GPCT_CONT_LISTS_CMP_FUNC(name, prefix, DLIST, keyfield)

attr GPCT_CONT_PROTO_SORT_ASCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_SORT_DESCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_SORT_STABLE_ASCEND(name, prefix, keyfield)
{
  prefix##_sort_ascend(root);
}

attr GPCT_CONT_PROTO_SORT_STABLE_DESCEND(name, prefix, keyfield)
{
  prefix##_sort_descend(root);
}

attr GPCT_CONT_PROTO_SORTED_ASCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(DLIST, name, lockname, prefix##_gpct_cmp_ascend, root->head)

attr GPCT_CONT_PROTO_SORTED_DESCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(DLIST, name, lockname, prefix##_gpct_cmp_descend, root->head)

attr GPCT_CONT_PROTO_INSERT_ASCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_INSERT_DESCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_MERGE_ASCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_MERGE_DESCEND(name, prefix, keyfield)
GPCT_CONT_DLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)
/* backslash-region-end */



#endif

