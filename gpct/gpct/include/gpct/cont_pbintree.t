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

*/

#ifndef GPCT_CONT_PBINTREE_H_
#define GPCT_CONT_PBINTREE_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_entry.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_pushpop_array.h>
#include <gpct/_cont_bintree.h>
#include <gpct/_cont_bintree_iter.h>
#include <gpct/_cont_bintree_up.h>

#define GPCT_CONT_PBINTREE_ARRAY_BASED    0
#define GPCT_CONT_PBINTREE_POINTER_BASED  1
#define GPCT_CONT_PBINTREE_ORDERED        1
#define GPCT_CONT_PBINTREE_SORTED         1

/***********************************************************************
 *      types
 */

typedef struct                          gpct_pbintree_entry_s
{
  struct gpct_pbintree_entry_s          *child[2];
  struct gpct_pbintree_entry_s          *parent;
}                                       gpct_PBINTREE_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_PBINTREE_TYPE(name, type, entryfield, stacksize)

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  struct gpct_pbintree_entry_s          *root;
  size_t                                count;
}                                       name##_root_t;

typedef struct gpct_pbintree_entry_s       name##_entry_t;
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
name##_gpct_stacksize = stacksize;

GPCT_CONT_ORPHAN_EMPTY_FUNC(PBINTREE, name, CONTAINER_ORPHAN_CHK_##name)
/* backslash-region-end */

/***********************************************************************
 *      orphan checking
 */

GPCT_INTERNAL void
gpct_orphan_PBINTREE__set(struct gpct_pbintree_entry_s *e)
{
  e->child[0] = e;
}

GPCT_INTERNAL gpct_bool_t
gpct_orphan_PBINTREE__chk(struct gpct_pbintree_entry_s *e)
{
  return e->child[0] != e;
}

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_PBINTREE_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .root = { .root = NULL }
}
/* backslash-region-end */

#define GPCT_CONT_PBINTREE_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_PBINTREE_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_PBINTREE_FUNC(attr, name, prefix, lockname, keyfield)

GPCT_CONT_BINTREES_UP_INTERNAL_FUNC(attr, name, prefix, keyfield)
GPCT_CONT_BINTREES_ROOT_FUNC(attr, name, PBINTREE, prefix, lockname)
GPCT_CONT_BINTREES_ACCESS_FUNC(attr, name, PBINTREE, prefix, lockname, keyfield)
GPCT_CONT_BINTREES_ROOT_UP_FUNC(attr, name, PBINTREE, prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_NEXT(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, PBINTREE, prefix, lockname, 1)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PREV(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, PBINTREE, prefix, lockname, 0)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  size_t count = 0;
  gpct_error_t res = 0;
  name##_entry_t *node;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (node = root->root; node && node->child[1]; )
    node = node->child[1];

  {
    GPCT_CONT_BINTREES_UP_FOREACH_UNORDERED_BASE(BINTREE, name, root, {

      GPCT_CONT_BINTREES_SYMMETRIC_OPS(n, {

        if (gpct_node->child[n] != NULL &&
            gpct_node->child[n]->parent != gpct_node)
          res |= 4;

      });

      count++;
    });
  }

  res |= (count != root->count);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

#define GPCT_CONT_PBINTREE_FOREACH(name, root, ...)                \
 GPCT_CONT_BINTREES_UP_FOREACH_BASE(PBINTREE, name, root, 1, __VA_ARGS__)

#define GPCT_CONT_PBINTREE_FOREACH_REVERSE(name, root, ...)        \
 GPCT_CONT_BINTREES_UP_FOREACH_BASE(PBINTREE, name, root, 0, __VA_ARGS__)

#define GPCT_CONT_PBINTREE_FOREACH_UNORDERED(name, root, ...)      \
 GPCT_CONT_BINTREES_UP_FOREACH_UNORDERED_BASE(PBINTREE, name, root, __VA_ARGS__)

#endif

