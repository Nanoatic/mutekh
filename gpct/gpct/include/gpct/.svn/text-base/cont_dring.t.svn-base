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

#ifndef GPCT_DRING_H_
#define GPCT_DRING_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_ring.h>
#include <gpct/_helpers.h>

#define GPCT_CONT_DRING_ARRAY_BASED      1
#define GPCT_CONT_DRING_POINTER_BASED    0
#define GPCT_CONT_DRING_ORDERED          1
#define GPCT_CONT_DRING_SORTED           0

/***********************************************************************
 *      types
 */

typedef gpct_empty_t gpct_DRING_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_DRING_TYPE(name, type, minsize, maxsize)

typedef type                            name##_itembase_t;

typedef struct                          name##_root_s
{
  gpct_index_t                          first;
  size_t                                count;
  size_t                                size;
  name##_itembase_t                     *data;
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
}                                       name##_root_t;

typedef gpct_sindex_t                   name##_index_t;
typedef name##_itembase_t               name##_item_t;
typedef gpct_DRING_entry_t              name##_entry_t;

static const name##_item_t
name##_gpct_null_item = { 0 };

static const name##_index_t
name##_gpct_null_index = -1;

static const size_t
name##_gpct_ringminsize = minsize;

static const size_t
name##_gpct_ringmaxsize = maxsize;

GPCT_INTERNAL size_t
gpct_cont_ring_##name##_extend(name##_root_t *root, size_t s)
{
  if (root->count + s <= root->size)
    return s;

  if (root->size < maxsize)
    {
      size_t newsize = root->size ? root->size * 2 : minsize;
      name##_itembase_t *data = gpct_realloc(root->data, newsize * sizeof (name##_item_t));

      if (data != NULL)
        {
          root->data = data;
          root->first &= (root->size - 1);

          /* copy buffer old wrapping part to new second half */
       /* for (i = root->size; i < root->first + root->count; i++)
            data[i] = data[i - root->size]; */
          gpct_memcpy(data, root->size, data,
                      0, root->first + root->count - root->size,
                      sizeof(name##_item_t));

          root->size = newsize;
        }
    }

  return root->size - root->count;
}
/* backslash-region-end */

#define GPCT_CONT_DRING_SIZE(name, root) ((root)->size)
#define GPCT_CONT_DRING_SIZE_MOD(name, root, x)                         \
 (((size_t)(x)) & ((root)->size - 1))


/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_DRING_ROOT_INITIALIZER(name, lockname)
{
  .first = 0,
  .count = 0,
  .size = 0,
  .data = NULL,
  .lock = GPCT_LOCK_INITIALIZER(lockname),
}
/* backslash-region-end */

#define GPCT_CONT_DRING_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_DRING_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_DRING_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_RINGS_FUNC(attr, name, prefix, lockname, DRING)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  if (!gpct_cont_ring_size_ispow2(name##_gpct_ringminsize))
    GPCT_STR_ERROR("DRING minimal size must be a power of 2");
  if (!gpct_cont_ring_size_ispow2(name##_gpct_ringmaxsize))
    GPCT_STR_ERROR("DRING maximal size must be a power of 2");
  if (name##_gpct_ringminsize > name##_gpct_ringmaxsize)
    GPCT_STR_ERROR("DRING minimal size > maximal size");

  root->first = 0;
  root->count = 0;
  root->size = 0;
  root->data = NULL;

  return GPCT_LOCK_INIT(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  if (root->data)
    {
      gpct_free(root->data);
      root->data = NULL;
    }

  GPCT_LOCK_DESTROY(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  if (root->size == 0)
    return 0;

  return !(root->data != NULL
           && root->count >= 0
           && root->count <= root->size
           && root->size <= name##_gpct_ringmaxsize
           && root->size >= name##_gpct_ringminsize
           && gpct_cont_ring_size_ispow2(root->size));
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return root->count == name##_gpct_ringmaxsize;
}

/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

#define GPCT_CONT_DRING_FOREACH_FROM(name, root, idx, ...)         \
 GPCT_CONT_RINGS_FOREACH_FROM(name, DRING, root, idx, __VA_ARGS__)

#define GPCT_CONT_DRING_FOREACH(name, root, ...)                   \
 GPCT_CONT_DRING_FOREACH_FROM(name, root, -1, __VA_ARGS__)

#define GPCT_CONT_DRING_FOREACH_REVERSE_FROM(name, root, idx, ...) \
 GPCT_CONT_RINGS_FOREACH_REVERSE_FROM(name, DRING, root, idx, __VA_ARGS__)

#define GPCT_CONT_DRING_FOREACH_REVERSE(name, root, ...)           \
 GPCT_CONT_DRING_FOREACH_REVERSE_FROM(name, root, (root)->count, __VA_ARGS__)

#define GPCT_CONT_DRING_FOREACH_UNORDERED(name, root, ...)         \
 GPCT_CONT_DRING_FOREACH(name, root, __VA_ARGS__)

/***********************************************************************
 *      key field based functions
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_DRING_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)
GPCT_CONT_LOOKUP_FUNC(attr, name, DRING, prefix,
                      lockname, keyfield);
/* backslash-region-end */

#endif

