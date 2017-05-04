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

#ifndef GPCT_DARRAY_H_
#define GPCT_DARRAY_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_cont_lookup.h>
#include <gpct/_cont_array.h>
#include <gpct/_helpers.h>

#define GPCT_CONT_DARRAY_ARRAY_BASED      1
#define GPCT_CONT_DARRAY_POINTER_BASED    0
#define GPCT_CONT_DARRAY_ORDERED          1
#define GPCT_CONT_DARRAY_SORTED           0

/***********************************************************************
 *      types
 */

typedef gpct_empty_t gpct_DARRAY_entry_t;

/* backslash-region-begin */
#define GPCT_CONT_DARRAY_TYPE(name, type, initialsize, maxsize)

typedef type                            name##_itembase_t;

typedef struct                          name##_root_s
{
  size_t                                count;
  size_t                                size;
  name##_itembase_t                     *data;
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
}                                       name##_root_t;

typedef gpct_sindex_t                   name##_index_t;
typedef name##_itembase_t               name##_item_t;
typedef gpct_DARRAY_entry_t              name##_entry_t;

static const name##_item_t
name##_gpct_null_item = { 0 };

static const name##_index_t
name##_gpct_null_index = -1;

static const size_t
name##_gpct_arrayinitialsize = initialsize;

static const size_t
name##_gpct_arraymaxsize = maxsize;

GPCT_INTERNAL size_t
gpct_cont_array_##name##_extend(name##_root_t *root, size_t s)
{
  size_t res;

  GPCT_ASSERT(name##_gpct_arrayinitialsize > 0);

  if (root->count + s <= root->size)
    return s;

  if (root->size < maxsize)
    {
      size_t newsize = root->size ? root->size * 2 : initialsize;
      name##_itembase_t *data = gpct_realloc(root->data, newsize * sizeof (name##_item_t));

      if (data != NULL)
        {
          root->data = data;
          root->size = newsize;
        }
    }

  res = root->size - root->count;
  return res > s ? s : res;
}
/* backslash-region-end */

#define GPCT_CONT_DARRAY_SIZE(name, root) ((root)->size)

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_DARRAY_ROOT_INITIALIZER(name, lockname)
{
  .count = 0,
  .size = 0,
  .data = NULL,
  .lock = GPCT_LOCK_INITIALIZER(lockname),
}
/* backslash-region-end */

#define GPCT_CONT_DARRAY_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_DARRAY_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_DARRAY_FUNC(attr, name, prefix, lockname, ...)

GPCT_CONT_ARRAYS_FUNC(attr, name, prefix, lockname, DARRAY)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  if (name##_gpct_arrayinitialsize > name##_gpct_arraymaxsize)
    GPCT_STR_ERROR("DARRAY minimal size > maximal size");

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
attr GPCT_CONT_PROTO_RESIZE(name, prefix)
{
  name##_itembase_t *data;
  gpct_error_t res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (size > name##_gpct_arraymaxsize)
    res = -1;
  else if (size < root->count)
    res = -1;
  else if (size == 0)
    {
      free(root->data);
      root->size = 0;
      root->data = NULL;
    }
  else
    {
      data = gpct_realloc(root->data, size * sizeof (name##_item_t));

      if (data != NULL)
        {
          root->data = data;
          root->size = size;
        }
      else
        res = -1;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CHECK(name, prefix)
{
  if (root->size == 0)
    return 0;

  return !(root->data != NULL
           && root->count >= 0
           && root->count <= root->size
           && root->size <= name##_gpct_arraymaxsize
           && root->size >= name##_gpct_arrayinitialsize);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return root->count == name##_gpct_arraymaxsize;
}
/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */

#define GPCT_CONT_DARRAY_FOREACH_FROM(name, root, idx, ...)        \
 GPCT_CONT_ARRAYS_FOREACH_FROM(name, DARRAY, root, idx, __VA_ARGS__)

#define GPCT_CONT_DARRAY_FOREACH(name, root, ...)                  \
 GPCT_CONT_DARRAY_FOREACH_FROM(name, root, -1, __VA_ARGS__)

#define GPCT_CONT_DARRAY_FOREACH_REVERSE_FROM(name, root, idx, ...) \
 GPCT_CONT_ARRAYS_FOREACH_REVERSE_FROM(name, DARRAY, root, idx, __VA_ARGS__)

#define GPCT_CONT_DARRAY_FOREACH_REVERSE(name, root, ...)          \
 GPCT_CONT_DARRAY_FOREACH_REVERSE_FROM(name, root, (root)->count, __VA_ARGS__)

#define GPCT_CONT_DARRAY_FOREACH_UNORDERED(name, root, ...)        \
 GPCT_CONT_DARRAY_FOREACH(name, root, __VA_ARGS__)

/***********************************************************************
 *      key field based functions
 */

/* backslash-region-begin */
#define GPCT_CONT_KEY_DARRAY_FUNC(attr, name, prefix,
                                 lockname, keyfield, ...)
GPCT_CONT_LOOKUP_FUNC(attr, name, DARRAY, prefix,
                      lockname, keyfield);
/* backslash-region-end */


#endif

