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

#ifndef GPCT_OBJECT_SIMPLE_H_
#define GPCT_OBJECT_SIMPLE_H_

#include <stdarg.h>

#include <gpct/_platform.h>
#include <gpct/_object.h>
#include <gpct/_helpers.h>

#define GPCT_OBJ_SIMPLE_ENTRY_FIELDS(name)

/**
   Object manager with simple reference counting. the delete()
   function is not available here.
 */

/* backslash-region-begin */
#define GPCT_OBJ_SIMPLE_FUNC(attr, name, prefix, f)

attr GPCT_OBJ_PROTO_CONSTRUCT(name, prefix)
{
  name##_object_t obj = storage;
  gpct_error_t err;

  GPCT_ASSERT(storage != NULL);
  obj->f.storage_free = sfree;

  err = name##_constructor(obj, ap);

  GPCT_ASSERT(obj->f.storage_free == sfree);

  if (err && sfree != NULL)
    sfree(storage);

  return err;
}

attr GPCT_OBJ_PROTO_NEW(name, prefix)
{
  name##_object_t obj = storage;
  object_storage_free_t *sfree = NULL;
  va_list ap;

  if (obj == NULL)
    {
      obj = gpct_malloc(sizeof(*obj));
      sfree = &gpct_free;
    }

  if (obj != NULL)
    {
      va_start(ap, storage);
      if (prefix##_construct(obj, sfree, ap))
        return NULL;
      va_end(ap);
    }

  return obj;
}

attr GPCT_OBJ_PROTO_DELETE(name, prefix)
{
  name##_destructor(object);
  if (object->f.storage_free != NULL)
    object->f.storage_free(object);
  return 0;
}

attr GPCT_OBJ_PROTO_REFNEW(name, prefix)
{
  GPCT_ERROR_NA(name);
  return NULL;
}

attr GPCT_OBJ_PROTO_REFCOUNT(name, prefix)
{
  GPCT_ERROR_NA(name);
  return 1;
}

attr GPCT_OBJ_PROTO_REFDROP(name, prefix)
{
  GPCT_ERROR_NA(name);
}
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_OBJ_SIMPLE_INITIALIZER
{
  .storage_free = NULL,
}
/* backslash-region-end */

#endif

