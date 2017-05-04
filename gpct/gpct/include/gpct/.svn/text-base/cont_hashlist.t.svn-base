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

    Linked list (same as BLIST) with hash table on list head.
*/

#ifndef GPCT_CONT_HASHLIST_H_
#define GPCT_CONT_HASHLIST_H_

#include <stddef.h>

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_entry.h>
#include <gpct/_cont_pushpop_array.h>

#define GPCT_CONT_HASHLIST_ARRAY_BASED    0
#define GPCT_CONT_HASHLIST_POINTER_BASED  1
#define GPCT_CONT_HASHLIST_ORDERED        0
#define GPCT_CONT_HASHLIST_SORTED         0

/***********************************************************************
 *      types
 */

typedef struct                          gpct_hashlist_entry_s
{
  struct gpct_hashlist_entry_s          *next;
  struct gpct_hashlist_entry_s          **prev;
}                                       gpct_HASHLIST_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_HASHLIST_TYPE(name, type, entryfield, size)

GPCT_CONT_COUNTER_TYPE(name)

typedef struct                          name##_root_s
{
  struct gpct_hashlist_entry_s          *head[size];
  GPCT_CONT_COUNTER_T(name)             counter;
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
}                                       name##_root_t;

typedef struct gpct_hashlist_entry_s    name##_entry_t;
typedef type                            name##_itembase_t;
typedef name##_itembase_t *             name##_item_t;
typedef name##_itembase_t *             name##_index_t;

static const name##_item_t
name##_gpct_null_item = NULL;

static const name##_index_t
name##_gpct_null_index = NULL;

static const uintptr_t
name##_gpct_offsetof = offsetof(name##_itembase_t, entryfield);

static const size_t
name##_gpct_hashsize = (size);

GPCT_CONT_COUNTER_FUNC(name)
GPCT_CONT_ORPHAN_EMPTY_FUNC(HASHLIST, name, CONTAINER_ORPHAN_CHK_##name)
/* backslash-region-end */


/***********************************************************************
 *      orphan checking
 */

GPCT_INTERNAL void
gpct_orphan_HASHLIST__set(struct gpct_hashlist_entry_s *e)
{
  e->next = e;
}

GPCT_INTERNAL gpct_bool_t
gpct_orphan_HASHLIST__chk(struct gpct_hashlist_entry_s *e)
{
  return e != e->next;
}


/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_HASHLIST_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .head = { 0 },
}
/* backslash-region-end */

#define GPCT_CONT_HASHLIST_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_HASHLIST_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_HASHLIST_FUNC(attr, name, prefix, lockname, keyfield)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISORPHAN(name, prefix)
{
  return !GPCT_CONT_ORPHAN_CHK(HASHLIST, name, GPCT_CONT_GET_ENTRY(name, item));
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ORPHAN(name, prefix)
{
  GPCT_CONT_ORPHAN_SET(HASHLIST, name, GPCT_CONT_GET_ENTRY(name, item));
}

attr GPCT_CONT_PROTO_ISNULL(name, prefix)
{
  return !index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISEMPTY(name, prefix)
{
  gpct_bool_t           res;
  uintptr_t             i;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  if (GPCT_CONT_COUNTER_ENABLED(name))
    res = GPCT_CONT_COUNTER_GET(name, root) == 0;
  else
    {
      res = 1;
      for (i = 0; i < name##_gpct_hashsize; i++)
        if (root->head[i])
          {
            res = 0;
            break;
          }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return 0;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_GET(name, prefix)
{
  return index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETINDEX(name, prefix)
{
  return ptr;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETPTR(name, prefix)
{
  return index;
}

GPCT_DEPRECATED
GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_NEXT(name, prefix)
{
  name##_index_t                item;
  name##_entry_t                *entry;
  uintptr_t                     i;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  if (!(entry = GPCT_CONT_GET_ENTRY(name, index)->next))
    for (i = (GPCT_CONT_HASH_FIELD(name, keyfield, index)
              % name##_gpct_hashsize) + 1;
         !entry && i < name##_gpct_hashsize; i++)
        entry = root->head[i];

  item = entry
       ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, entry))
       : NULL;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_DEPRECATED
GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_HEAD(name, prefix)
{
  name##_index_t                item;
  name##_entry_t                *entry = 0;
  uintptr_t                     i;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; !entry && i < name##_gpct_hashsize; i++)
    entry = root->head[i];

  item = entry
       ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, entry))
       : NULL;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_COUNT(name, prefix)
{
  uintptr_t             i;
  size_t                s;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  name##_entry_t        *entry;

  if (GPCT_CONT_COUNTER_ENABLED(name))
    s = GPCT_CONT_COUNTER_GET(name, root);
  else
    for (s = i = 0; i < name##_gpct_hashsize; i++)
      for (entry = root->head[i]; entry; s++)
        entry = entry->next;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return s;
}

attr GPCT_CONT_PROTO_SIZE(name, prefix)
{
  return name##_gpct_hashsize;
}

GPCT_INTERNAL
GPCT_NONNULL(1)
void prefix##_remove_   (name##_entry_t *entry)
{
  name##_entry_t *next = entry->next;
  name##_entry_t **prev = entry->prev;

  if ((*prev = next))
    next->prev = prev;

  GPCT_CONT_ORPHAN_SET(HASHLIST, name, entry);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  uintptr_t             i;
  name##_entry_t        *entry, *next;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; i < name##_gpct_hashsize; i++)
    {
      for (entry = root->head[i]; entry; entry = next)
        {
          GPCT_CONT_ORPHAN_SET(HASHLIST, name, entry);
          next = entry->next;
          GPCT_CONT_REFDROP(name, GPCT_CONT_GET_ITEM(name, entry));
        }
      root->head[i] = NULL;
    }

  GPCT_CONT_COUNTER_SET(name, root, 0);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  gpct_error_t          res = -1;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    name##_entry_t      *index_e = GPCT_CONT_GET_ENTRY(name, index);

    if (GPCT_CONT_ORPHAN_CHK(HASHLIST, name, index_e))
      {
        prefix##_remove_(index_e);
        GPCT_CONT_REFDROP(name, index);
        GPCT_CONT_COUNTER_ADD(name, root, -1);
        res = 0;
      }
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  name##_entry_t        **entry, *entry_, *item_e;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);
  GPCT_CONT_REFNEW(name, item);

  entry = &root->head[GPCT_CONT_HASH_FIELD(name, keyfield, item)
                      % name##_gpct_hashsize];
  entry_ = *entry;
  item_e = GPCT_CONT_GET_ENTRY(name, item);

  item_e->prev = entry;
  item_e->next = entry_;
  if (entry_ != NULL)
    entry_->prev = &item_e->next;
  *entry = item_e;
  GPCT_CONT_COUNTER_ADD(name, root, 1);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  return prefix##_push(root, item);
}

GPCT_INTERNAL
GPCT_NONNULL(1)
GPCT_CONT_PROTO_POP(name, prefix##_gpct)
{
  name##_index_t                item = 0;
  name##_entry_t                *entry = 0;
  uintptr_t                     i;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; i < name##_gpct_hashsize; i++)
    if ((entry = root->head[i]))
      break;

  if (entry)
    {
      item = GPCT_CONT_GET_ITEM(name, entry);
      prefix##_remove_(entry);
      GPCT_CONT_COUNTER_ADD(name, root, -1);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
GPCT_DEPRECATED
attr GPCT_CONT_PROTO_POP(name, prefix)
{
  return prefix##_gpct_pop(root);
}

GPCT_NONNULL(1)
GPCT_DEPRECATED
attr GPCT_CONT_PROTO_POPBACK(name, prefix)
{
  return prefix##_gpct_pop(root);
}

attr GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    prefix##_push(root, item[i]);

  return size;
}

GPCT_DEPRECATED
attr GPCT_CONT_PROTO_POP_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    item[i] = prefix##_gpct_pop(root);

  return size;
}

attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    prefix##_push(root, item[i]);

  return size;
}

GPCT_DEPRECATED
attr GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    item[i] = prefix##_gpct_pop(root);

  return size;
}

GPCT_INTERNAL intptr_t
prefix##_foreach_(name##_root_t *root,
                  CONTAINER_ITERATOR(name, * const fcn),
                  va_list ap)
{
  va_list               aq;
  uintptr_t             i;
  name##_entry_t        *entry;
  intptr_t              res = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; !res && i < name##_gpct_hashsize; i++)
    for (entry = root->head[i]; !res && entry; entry = entry->next)
      {
        va_copy(aq, ap);
        res = fcn(GPCT_CONT_GET_ITEM(name, entry), aq);
        va_end(aq);
      }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
{
  intptr_t      res;
  va_list       ap;

  va_start(ap, fcn);
  res = prefix##_foreach_(root, fcn, ap);
  va_end(ap);

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)
{
  intptr_t      res;
  va_list       ap;

  va_start(ap, fcn);
  res = prefix##_foreach_(root, fcn, ap);
  va_end(ap);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  uintptr_t             i;

  for (i = 0; i < name##_gpct_hashsize; i++)
    root->head[i] = NULL;

  GPCT_CONT_COUNTER_SET(name, root, 0);

  return GPCT_LOCK_INIT(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  GPCT_LOCK_DESTROY(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  uintptr_t             i;
  name##_entry_t        *entry;
  gpct_error_t          res = 0;
  size_t                count = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; !res && i < name##_gpct_hashsize; i++)
    for (entry = root->head[i]; !res && entry != NULL; entry = entry->next)
      {
        if (entry->next == entry)
          res |= 1;

        if (root->head[i] == entry && entry->prev != &root->head[i])
          res |= 2;

        if (entry->next != NULL && entry->next->prev != &entry->next)
          res |= 4;

        if (entry->next == root->head[i])
          res |= 8;

        count++;
      }

  if (!res && GPCT_CONT_COUNTER_ENABLED(name))
    if (GPCT_CONT_COUNTER_GET(name, root) != count)
      res |= 16;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */


/* backslash-region-begin */
#define GPCT_CONT_HASHLIST_FOREACH(name, root, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  uintptr_t             gpct_i;
  name##_entry_t        *gpct_entry;
  name##_entry_t        *gpct_next;

  for (gpct_i = 0; gpct_i < name##_gpct_hashsize; gpct_i++)
    for (gpct_entry = (root)->head[gpct_i];
         gpct_entry; gpct_entry = gpct_next)
      {
        GPCT_CONT_LABEL_DECL(gpct_continue);
        name##_item_t   item = GPCT_CONT_GET_ITEM(name, gpct_entry);
        GPCT_UNUSED name##_index_t      index = item;
        gpct_next = gpct_entry->next;

        { __VA_ARGS__ }
        GPCT_CONT_LABEL(gpct_continue);
      }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */


#define GPCT_CONT_HASHLIST_FOREACH_REVERSE(name, root, ...)        \
 GPCT_CONT_HASHLIST_FOREACH(name, root, __VA_ARGS__)

#define GPCT_CONT_HASHLIST_FOREACH_UNORDERED(name, root, ...)      \
 GPCT_CONT_HASHLIST_FOREACH(name, root, __VA_ARGS__)

#define GPCT_CONT_HASHLIST_FOREACH_FROM(name, root, index, ...)    \
 GPCT_ERROR_NA(name);

#define GPCT_CONT_HASHLIST_FOREACH_REVERSE_FROM(name, root, index, ...) \
 GPCT_ERROR_NA(name);


/***********************************************************************
 *      key field based functions
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_HASHLIST_FUNC(attr, name, prefix,
                                    lockname, keyfield, ...)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP(name, prefix, keyfield)
{
  gpct_key_hash_t       i;
  name##_entry_t        *entry;
  name##_item_t         item = NULL;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  i = GPCT_CONT_HASH_ARG(name, keyfield, value)
    % name##_gpct_hashsize;

  for (entry = root->head[i]; entry; entry = entry->next)
    {
      item = GPCT_CONT_GET_ITEM(name, entry);

      if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
        {
          item = GPCT_CONT_REFNEW(name, item);
          break;
        }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return entry ? item : NULL;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP_LAST(name, prefix, keyfield)
{
  return prefix##_lookup(root, value);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP_NEXT(name, prefix, keyfield)
{
  name##_entry_t        *entry;
  name##_item_t         item;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (entry = GPCT_CONT_GET_ENTRY(name, index)->next;
       entry; entry = entry->next)
    {
      item = GPCT_CONT_GET_ITEM(name, entry);

      if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
        {
          item = GPCT_CONT_REFNEW(name, item);
          break;
        }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return entry ? item : NULL;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP_PREV(name, prefix, keyfield)
{
  return prefix##_lookup_next(root, index, value);
}

GPCT_CONT_REMOVE_KEY_FUNC(attr, name, HASHLIST, prefix,
			       lockname, keyfield)
/* backslash-region-end */



#endif

