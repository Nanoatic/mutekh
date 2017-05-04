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

#ifndef GPCT_DBITMAP_H_
#define GPCT_DBITMAP_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_bitops.h>
#include <gpct/_cont_bitmap.h>

#define GPCT_CONT_DBITMAP_ARRAY_BASED      0
#define GPCT_CONT_DBITMAP_POINTER_BASED    0
#define GPCT_CONT_DBITMAP_ORDERED          1
#define GPCT_CONT_DBITMAP_SORTED           0

/* backslash-region-begin */
#define GPCT_CONT_DBITMAP_TYPE(name, type, init_size, ...)

struct                                  name##_bitmap_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  type          *data;
  size_t        size;
};

typedef gpct_sindex_t                   name##_index_t;
typedef gpct_sindex_t                   name##_itembase_t;
typedef gpct_bool_t                     name##_item_t;
typedef struct name##_bitmap_s          name##_root_t;
typedef type                            name##_entry_t;

static const size_t
name##_gpct_init_size = init_size;

static const name##_index_t
name##_gpct_null_index = -1;

typedef gpct_bool_t gpct_##name##_keyfield_valuetype_t;

/* backslash-region-end */

#define GPCT_CONT_DBITMAP_BITCOUNT(root, name) ((root)->size * sizeof (name##_entry_t) * 8)
#define GPCT_CONT_DBITMAP_BYTECOUNT(root, name) ((root)->size * sizeof (name##_entry_t))
#define GPCT_CONT_DBITMAP_WORDCOUNT(root, name) ((root)->size)

/* backslash-region-begin */
#define GPCT_CONT_DBITMAP_FUNC(attr, name, prefix, lockname, algo, ...)
 GPCT_CONT_BITMAPS_FUNC(attr, name, prefix, lockname, DBITMAP)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  size_t wc = GPCT_CONT_BITMAP_ALIGN_SIZE(name##_gpct_init_size, name##_entry_t);

  if (name##_gpct_init_size > 0
      && (root->data = calloc(wc, sizeof (name##_entry_t))) == NULL)
    return -1;

  root->size = wc;

  if (GPCT_LOCK_INIT(lockname, &root->lock))
    {
      free(root->data);
      return -1;
    }

  return 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  free(root->data);
  GPCT_LOCK_DESTROY(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_RESIZE(name, prefix)
{
  size_t wc = GPCT_CONT_BITMAP_ALIGN_SIZE(size, name##_entry_t);
  void *data;
  gpct_error_t res = -1;
  unsigned int i;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if ((data = realloc(root->data, wc * sizeof (name##_entry_t))) != NULL || wc == 0)
    {
      root->data = data;
      res = 0;

      for (i = root->size; i < wc; i++)
        root->data[i] = 0;

      root->size = wc;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_KEY_DBITMAP_FUNC(attr, name, prefix, lockname, algo)
 GPCT_CONT_KEY_BITMAPS_FUNC(attr, name, prefix, lockname, DBITMAP)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_DBITMAP_FOREACH(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH(name, root, DBITMAP, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_DBITMAP_FOREACH_REVERSE(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH_REVERSE(name, root, DBITMAP, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_DBITMAP_FOREACH_UNORDERED(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH(name, root, DBITMAP, __VA_ARGS__)
/* backslash-region-end */

#endif

