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

#ifndef GPCT_BITMAP_H_
#define GPCT_BITMAP_H_

#include <gpct/_cont_access.h>
#include <gpct/_cont_keyfield.h>
#include <gpct/_bitops.h>
#include <gpct/_cont_bitmap.h>

#define GPCT_CONT_BITMAP_ARRAY_BASED      0
#define GPCT_CONT_BITMAP_POINTER_BASED    0
#define GPCT_CONT_BITMAP_ORDERED          1
#define GPCT_CONT_BITMAP_SORTED           0

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_TYPE(name, type, size, ...)

struct                                  name##_bitmap_s
{
  GPCT_LOCK_TYPE(CONTAINER_LOCK_##name) lock;
  type          data[GPCT_CONT_BITMAP_ALIGN_SIZE(size, type)];
};

typedef gpct_sindex_t                   name##_index_t;
typedef gpct_sindex_t                   name##_itembase_t;
typedef gpct_bool_t                     name##_item_t;
typedef struct name##_bitmap_s          name##_root_t;
typedef type                            name##_entry_t;

static const size_t
name##_gpct_size = size;

static const name##_index_t
name##_gpct_null_index = -1;

typedef gpct_bool_t gpct_##name##_keyfield_arg_t;
/* backslash-region-end */

#define GPCT_CONT_BITMAP_BITCOUNT(root, name) (name##_gpct_size)
#define GPCT_CONT_BITMAP_BYTECOUNT(root, name) (sizeof ((name##_root_t*)0)->data)
#define GPCT_CONT_BITMAP_WORDCOUNT(root, name) ((sizeof ((name##_root_t*)0)->data) / sizeof(name##_entry_t))

/***********************************************************************
 *      intializer
 */

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_ROOT_INITIALIZER(name, lockname)
{
  .lock = GPCT_LOCK_INITIALIZER(lockname),
  .data = { 0 },
}
/* backslash-region-end */

#define GPCT_CONT_BITMAP_ROOT_DECLARATOR(name, lockname, symbol) \
 name##_root_t symbol = GPCT_CONT_BITMAP_ROOT_INITIALIZER(name, lockname)

/***********************************************************************
 *      access functions
 */

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_FUNC(attr, name, prefix, lockname, algo, ...)
 GPCT_CONT_BITMAPS_FUNC(attr, name, prefix, lockname, BITMAP)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  gpct_index_t     i;

  for (i = 0; i < GPCT_CONT_BITMAP_WORDCOUNT(root, name); i++)
    root->data[i] = 0;

  return GPCT_LOCK_INIT(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  GPCT_LOCK_DESTROY(lockname, &root->lock);
}

/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_KEY_BITMAP_FUNC(attr, name, prefix, lockname, algo)
 GPCT_CONT_KEY_BITMAPS_FUNC(attr, name, prefix, lockname, BITMAP)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_FOREACH(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH(name, root, BITMAP, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_FOREACH_REVERSE(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH_REVERSE(name, root, BITMAP, __VA_ARGS__)
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BITMAP_FOREACH_UNORDERED(name, root, ...)
 GPCT_CONT_BITMAPS_FOREACH(name, root, BITMAP, __VA_ARGS__)
/* backslash-region-end */


#endif

