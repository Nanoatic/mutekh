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

#ifndef GPCT_ARRAY_H_
#define GPCT_ARRAY_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_array.h>

#define GPCT_CONT_ARRAY_ARRAY_BASED      1
#define GPCT_CONT_ARRAY_POINTER_BASED    0
#define GPCT_CONT_ARRAY_ORDERED          1
#define GPCT_CONT_ARRAY_SORTED           0

/***********************************************************************
 *      types
 */

typedef gpct_empty_t gpct_ARRAY_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_ARRAY_TYPE(name, type, size)

typedef type                            name##_itembase_t;

typedef struct                          name##_root_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  size_t                                count;
  name##_itembase_t                     data[size];
}                                       name##_root_t;

typedef gpct_sindex_t                   name##_index_t;
typedef name##_itembase_t               name##_item_t;
typedef gpct_ARRAY_entry_t              name##_entry_t;

static const name##_item_t
name##_gpct_null_item = { 0 };

static const name##_index_t
name##_gpct_null_index = -1;

static const size_t
name##_gpct_arraysize = size;

GPCT_INTERNAL size_t
gpct_cont_array_##name##_extend(name##_root_t *root, size_t s)
{
  size_t max = name##_gpct_arraysize - root->count;
  return max > s ? s : max;
}
/* backslash-region-end */

#define GPCT_CONT_ARRAY_SIZE(name, root) name##_gpct_arraysize

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_ARRAY_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .count = 0,
}
/* backslash-region-end */

#define GPCT_CONT_ARRAY_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_ARRAY_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_ARRAY_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_ARRAYS_FUNC(attr, name, prefix, lockname, ARRAY)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
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
  return !(root->count >= 0 && root->count <= name##_gpct_arraysize);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return root->count == name##_gpct_arraysize;
}
/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

#define GPCT_CONT_ARRAY_FOREACH_FROM(name, root, idx, ...)         \
 GPCT_CONT_ARRAYS_FOREACH_FROM(name, ARRAY, root, idx, __VA_ARGS__)

#define GPCT_CONT_ARRAY_FOREACH(name, root, ...)                   \
 GPCT_CONT_ARRAY_FOREACH_FROM(name, root, -1, __VA_ARGS__)

#define GPCT_CONT_ARRAY_FOREACH_REVERSE_FROM(name, root, idx, ...) \
 GPCT_CONT_ARRAYS_FOREACH_REVERSE_FROM(name, ARRAY, root, idx, __VA_ARGS__)

#define GPCT_CONT_ARRAY_FOREACH_REVERSE(name, root, ...)           \
 GPCT_CONT_ARRAY_FOREACH_REVERSE_FROM(name, root, (root)->count, __VA_ARGS__)

#define GPCT_CONT_ARRAY_FOREACH_UNORDERED(name, root, ...)         \
 GPCT_CONT_ARRAY_FOREACH(name, root, __VA_ARGS__)

/***********************************************************************
 *      key field based functions
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_ARRAY_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)
GPCT_CONT_LOOKUP_FUNC(attr, name, ARRAY, prefix,
                      lockname, keyfield);
/* backslash-region-end */


#endif

