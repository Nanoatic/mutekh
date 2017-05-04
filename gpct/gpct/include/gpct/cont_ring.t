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

#ifndef GPCT_RING_H_
#define GPCT_RING_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_ring.h>

#define GPCT_CONT_RING_ARRAY_BASED      1
#define GPCT_CONT_RING_POINTER_BASED    0
#define GPCT_CONT_RING_ORDERED          1
#define GPCT_CONT_RING_SORTED           0

/***********************************************************************
 *      types
 */

typedef gpct_empty_t gpct_RING_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_RING_TYPE(name, type, size)

typedef type                            name##_itembase_t;

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name)         lock;
  gpct_index_t                          first;
  size_t                                count;
  name##_itembase_t                     data[size];
}                                       name##_root_t;

typedef gpct_sindex_t                   name##_index_t;
typedef name##_itembase_t               name##_item_t;
typedef gpct_RING_entry_t               name##_entry_t;

static const name##_item_t
name##_gpct_null_item = { 0 };

static const name##_index_t
name##_gpct_null_index = -1;

static const size_t
name##_gpct_ringsize = size;

GPCT_INTERNAL size_t
gpct_cont_ring_##name##_extend(name##_root_t *root, size_t s)
{
  size_t max = name##_gpct_ringsize - root->count;
  return max > s ? s : max;
}
/* backslash-region-end */

#define GPCT_CONT_RING_SIZE(name, root) name##_gpct_ringsize
#define GPCT_CONT_RING_SIZE_MOD(name, root, x)                          \
 (((size_t)(x)) & (name##_gpct_ringsize - 1))


/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_RING_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .first = 0,
  .count = 0,
}
/* backslash-region-end */

#define GPCT_CONT_RING_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_RING_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_RING_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_RINGS_FUNC(attr, name, prefix, lockname, RING)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  if (!gpct_cont_ring_size_ispow2(name##_gpct_ringsize))
    GPCT_STR_ERROR("RING size must be a power of 2");

  root->first = 0;
  root->count = 0;

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
  return !(root->count >= 0 && root->count <= name##_gpct_ringsize);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return root->count == name##_gpct_ringsize;
}
/* backslash-region-end */



/***********************************************************************
 *      iteration macros
 */

/* backslash-region-begin */
#define GPCT_CONT_RING_FOREACH_FROM(name, root, idx, ...)
 GPCT_CONT_RINGS_FOREACH_FROM(name, RING, root, idx, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_RING_FOREACH(name, root, ...)
 GPCT_CONT_RING_FOREACH_FROM(name, root, -1, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_RING_FOREACH_REVERSE_FROM(name, root, idx, ...)
 GPCT_CONT_RINGS_FOREACH_REVERSE_FROM(name, RING, root, idx, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_RING_FOREACH_REVERSE(name, root, ...)
 GPCT_CONT_RING_FOREACH_REVERSE_FROM(name, root, (root)->count, __VA_ARGS__)
/* backslash-region-end */

#define GPCT_CONT_RING_FOREACH_UNORDERED(name, root, ...)          \
 GPCT_CONT_RING_FOREACH(name, root, __VA_ARGS__)

/***********************************************************************
 *      key field based functions
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_RING_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)
GPCT_CONT_LOOKUP_FUNC(attr, name, RING, prefix,
                      lockname, keyfield);
/* backslash-region-end */


#endif

