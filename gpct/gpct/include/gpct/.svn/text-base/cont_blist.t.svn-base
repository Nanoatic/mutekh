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

    Simple Linked List with Back pointer template.

    The list is implemented with head pointer only in container
    root. Entriee contain a next pointer and a prev double pointer to
    field `next' of the previous item or list head.

    Equivalent to SLIST with fast remove operation.

*/

#ifndef GPCT_BLIST_H_
#define GPCT_BLIST_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_entry.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_pushpop_array.h>
#include <gpct/_cont_lists.h>

#define GPCT_CONT_BLIST_ARRAY_BASED     0
#define GPCT_CONT_BLIST_POINTER_BASED   1
#define GPCT_CONT_BLIST_ORDERED         1
#define GPCT_CONT_BLIST_SORTED          0
#define GPCT_CONT_BLIST_REORDER         1

/***********************************************************************
 *      types
 */

typedef struct                          gpct_blist_entry_s
{
  struct gpct_blist_entry_s             *next;
  struct gpct_blist_entry_s             **prev;
}                                       gpct_BLIST_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_BLIST_TYPE(name, type, entryfield)

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  struct gpct_blist_entry_s             *head;
}                                       name##_root_t;

typedef struct gpct_blist_entry_s       name##_entry_t;
typedef type                            name##_itembase_t;
typedef name##_itembase_t *             name##_item_t;
typedef name##_itembase_t *             name##_index_t;

static const name##_item_t
name##_gpct_null_item = NULL;

static const name##_index_t
name##_gpct_null_index = NULL;

static const uintptr_t
name##_gpct_offsetof = offsetof(name##_itembase_t, entryfield);

GPCT_CONT_ORPHAN_EMPTY_FUNC(BLIST, name, CONTAINER_ORPHAN_CHK_##name)
/* backslash-region-end */


/***********************************************************************
 *      orphan checking
 */

GPCT_INTERNAL void
gpct_orphan_BLIST__set(struct gpct_blist_entry_s *e)
{
  e->next = e;
}

GPCT_INTERNAL gpct_bool_t
gpct_orphan_BLIST__chk(struct gpct_blist_entry_s *e)
{
  return e != e->next;
}

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .head = NULL,
}
/* backslash-region-end */

#define GPCT_CONT_BLIST_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_BLIST_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */


#define GPCT_CONT_BLIST_ISEND(root, entry)      ((entry) == NULL)
#define GPCT_CONT_BLIST_EMPTY(root)             ((root)->head = NULL)

/* backslash-region-begin */
#define GPCT_CONT_BLIST_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_LISTS_INTERNAL_FUNC(attr, name, prefix)

GPCT_CONT_LISTS_INIT_FUNC(attr, name, BLIST, prefix, lockname)

GPCT_CONT_LISTS_ROOT_FUNC(attr, name, BLIST, prefix, lockname, head)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  gpct_error_t          res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t *index_e = GPCT_CONT_GET_ENTRY(name, index);
    name##_entry_t *item_e = GPCT_CONT_GET_ENTRY(name, item);

    if (GPCT_CONT_ORPHAN_CHK(BLIST, name, index_e))
      {
        name##_entry_t **prev;

        GPCT_CONT_REFNEW(name, item);

        prev = item_e->prev = index_e->prev;
        if ((item_e->next = index_e->next))
          item_e->next->prev = &item_e->next;
        *prev = item_e;

        GPCT_CONT_ORPHAN_SET(BLIST, name, index_e);
        GPCT_CONT_REFDROP(name, index);

        res = 0;
      }
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, BLIST,
                               prefix, lockname,
                               NEXT, HEAD, next, head)

GPCT_CONT_LISTS_PREV_TAIL_FUNC(attr, name, BLIST, prefix, lockname)

GPCT_CONT_LISTS_COUNT_FUNC(attr, name, BLIST, prefix, lockname, head)

GPCT_NONNULL(1, 2, 3)
attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix)
{
  name##_entry_t        *index_e;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  index_e = GPCT_CONT_GET_ENTRY(name, index);

  if (GPCT_CONT_ORPHAN_CHK(BLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      item_e->prev = index_e->prev;
      index_e->prev = &item_e->next;
      item_e->next = index_e;
      *item_e->prev = item_e;

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

  if (GPCT_CONT_ORPHAN_CHK(BLIST, name, index_e))
    {
      name##_entry_t    *item_e;

      GPCT_CONT_REFNEW(name, item);

      item_e = GPCT_CONT_GET_ENTRY(name, item);
      if ((item_e->next = index_e->next) != NULL)
        index_e->next->prev = &item_e->next;
      index_e->next = item_e;
      item_e->prev = &index_e->next;

      res = 1;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_LISTS_CLEAR_FUNC(attr, name, BLIST, prefix, lockname, head)

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  gpct_error_t          res = -1;
  name##_entry_t        *index_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  index_e = GPCT_CONT_GET_ENTRY(name, index);

  if (GPCT_CONT_ORPHAN_CHK(BLIST, name, index_e))
    {
      name##_entry_t *next = index_e->next;
      name##_entry_t **prev = index_e->prev;

      if ((*prev = next))
        next->prev = prev;

      GPCT_CONT_ORPHAN_SET(BLIST, name, index_e);
      GPCT_CONT_REFDROP(name, index);

      res = 0;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  name##_entry_t    *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  item_e = GPCT_CONT_GET_ENTRY(name, item);
  item_e->prev = &root->head;
  item_e->next = root->head;
  if (root->head != NULL)
    root->head->prev = &item_e->next;
  root->head = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP(name, prefix)
{
  name##_item_t item = NULL;
  name##_entry_t *entry;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((entry = root->head) != NULL)
    {
      name##_entry_t *next = entry->next;

      if ((root->head = next))
        next->prev = entry->prev;

      item = GPCT_CONT_GET_ITEM(name, entry);
      GPCT_CONT_ORPHAN_SET(BLIST, name, entry);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_INTERNAL
GPCT_CONT_PROTO_PUSHBACK(name, prefix##_gpct)
{
  name##_entry_t        *item_e;
  name##_entry_t        **plast;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  item_e = GPCT_CONT_GET_ENTRY(name, item);
  plast = prefix##_gpct_pprev(root, NULL);

  item_e->next = NULL;
  item_e->prev = plast;
  *plast = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1, 2)
GPCT_DEPRECATED
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  return prefix##_gpct_pushback(root, item);
}

GPCT_INTERNAL
GPCT_CONT_PROTO_POPBACK(name, prefix##_gpct)
{
  name##_entry_t        **ptail, *tail;
  name##_item_t         item = NULL;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->head && (ptail = prefix##_gpct_ptail(root)))
    {
      tail = *ptail;
      *ptail = NULL;
      item = GPCT_CONT_GET_ITEM(name, tail);
      GPCT_CONT_ORPHAN_SET(BLIST, name, tail);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
GPCT_DEPRECATED
attr GPCT_CONT_PROTO_POPBACK(name, prefix)
{
  return prefix##_gpct_popback(root);
}

GPCT_DEPRECATED attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix);
GPCT_DEPRECATED attr GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix);
GPCT_CONT_PUSHPOP_ARRAY_FUNC(attr, name, prefix,
                             prefix##_gpct, lockname);

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, BLIST, prefix,
                                 lockname, head, next)

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)
GPCT_CONT_LISTS_FOREACH_REVERSE_FUNCCODE(attr, name, BLIST,
                                         prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  name##_entry_t        *entry;
  gpct_error_t          res = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (entry = root->head; !res && entry != NULL; entry = entry->next)
    {
      if (entry->next == entry)
        res |= 1;

      if (root->head == entry && entry->prev != &root->head)
        res |= 2;

      if (entry->next != NULL && entry->next->prev != &entry->next)
        res |= 4;

      if (entry->next == root->head)
        res |= 8;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_FOREACH(name, root, ...)
GPCT_CONT_LISTS_FOREACH_BASE(BLIST, name, root, (root)->head, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_BLIST_FOREACH_FROM(name, root, index, ...)
 GPCT_CONT_LISTS_FOREACH_BASE(BLIST, name, root,
			      GPCT_CONT_GET_ENTRY(name, index)->next, next, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_BLIST_FOREACH_REVERSE(name, root, ...)
GPCT_CONT_LISTS_FOREACH_REVERSE_BASE(name, root, NULL, __VA_ARGS__)
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_BLIST_FOREACH_REVERSE_FROM(name, root, index, ...)
GPCT_CONT_LISTS_FOREACH_REVERSE_BASE(name, root,
				     GPCT_CONT_GET_ENTRY(name, index), __VA_ARGS__)
/* backslash-region-end */

#define GPCT_CONT_BLIST_FOREACH_UNORDERED(name, root, ...)         \
 GPCT_CONT_BLIST_FOREACH(name, root, __VA_ARGS__)


/***********************************************************************
 *      key field based functions
 */

GPCT_CONT_LISTS_SORT_FUNC(BLIST, gpct_BLIST_entry_t)




/*
 * BLIST links rebuild after merge/sort template code
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_REBUILD_CODE(name, lockname, cmp, entry, root)
{
  name##_entry_t **prev;
  root->head = entry;

  for (prev = &root->head; entry != NULL; entry = entry->next)
    {
      entry->prev = prev;
      prev = &entry->next;
    }
}
/* backslash-region-end */




/*
 * BLIST sort glue function template, call sort and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_SORT_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->head != NULL && root->head->next != NULL)
    {
      name##_entry_t *tmp = gpct_BLIST_sort(root->head, cmp);
      GPCT_CONT_BLIST_REBUILD_CODE(name, lockname, cmp, tmp, root)
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */




/*
 * BLIST merge glue function template, call merge and rebuild prev links
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_MERGE_FUNCCODE(name, lockname, cmp)
{
  GPCT_LOCK_WRLOCK(lockname, &src->lock);
  GPCT_LOCK_WRLOCK(lockname, &dest->lock);

  if (src->head != NULL)
    {
      if (dest->head == NULL)
	{
	  /* move from src to dest */
	  dest->head = src->head;
	  dest->head->prev = &dest->head;
	}
      else
	{
	  name##_entry_t *tmp = gpct_BLIST_merge(dest->head, src->head, cmp);

	  GPCT_CONT_BLIST_REBUILD_CODE(name, lockname, cmp, tmp, dest);
	}

      GPCT_CONT_BLIST_EMPTY(src);
    }

  GPCT_LOCK_UNLOCK(lockname, &dest->lock);
  GPCT_LOCK_UNLOCK(lockname, &src->lock);
}
/* backslash-region-end */




/*
 * BLIST Ordered insertion function template
 */

/* backslash-region-begin */
#define GPCT_CONT_BLIST_ORDERED_INSERT_FUNCCODE(name, lockname, cmp)
{
  name##_entry_t        *item_e, **prev;
  size_t                res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  item_e = GPCT_CONT_GET_ENTRY(name, item);

  for (prev = &root->head; *prev != NULL; prev = &(*prev)->next)
    if (cmp(item_e, *prev))
      break;

  GPCT_CONT_REFNEW(name, item);

  item_e->next = *prev;
  item_e->prev = prev;
  if (*prev)
    (*prev)->prev = &item_e->next;
  *prev = item_e;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */




/* backslash-region-begin */
#define GPCT_CONT_KEY_BLIST_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)

GPCT_CONT_LOOKUP_FUNC(attr, name, BLIST, prefix, lockname, keyfield);

GPCT_CONT_LISTS_CMP_FUNC(name, prefix, BLIST, keyfield)

attr GPCT_CONT_PROTO_SORT_ASCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_SORT_DESCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_SORT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_SORT_STABLE_ASCEND(name, prefix, keyfield)
{
  prefix##_sort_ascend(root);
}

attr GPCT_CONT_PROTO_SORT_STABLE_DESCEND(name, prefix, keyfield)
{
  prefix##_sort_descend(root);
}

attr GPCT_CONT_PROTO_SORTED_ASCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(BLIST, name, lockname, prefix##_gpct_cmp_ascend, root->head)

attr GPCT_CONT_PROTO_SORTED_DESCEND(name, prefix, keyfield)
GPCT_CONT_LISTS_SORTED_FUNCCODE(BLIST, name, lockname, prefix##_gpct_cmp_descend, root->head)

attr GPCT_CONT_PROTO_INSERT_ASCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_INSERT_DESCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_ORDERED_INSERT_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)

attr GPCT_CONT_PROTO_MERGE_ASCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_ascend)

attr GPCT_CONT_PROTO_MERGE_DESCEND(name, prefix, keyfield)
GPCT_CONT_BLIST_MERGE_FUNCCODE(name, lockname, prefix##_gpct_cmp_descend)
/* backslash-region-end */




#endif

