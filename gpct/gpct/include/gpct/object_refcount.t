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

#ifndef GPCT_OBJECT_REFCOUNT_H_
#define GPCT_OBJECT_REFCOUNT_H_

#include <stdarg.h>

#include <gpct/_platform.h>
#include <gpct/_atomic.h>
#include <gpct/_object.h>
#include <gpct/_helpers.h>

#define GPCT_OBJ_REFCOUNT_ENTRY_FIELDS(name) \
 gpct_atomic_t           refcnt;



/**
   Object manager with simple reference counting. the delete()
   function is not available here.
 */

/* backslash-region-begin */
#define GPCT_OBJ_REFCOUNT_FUNC(attr, name, prefix, f)

attr GPCT_OBJ_PROTO_CONSTRUCT(name, prefix)
{
  name##_object_t obj = storage;
  gpct_error_t err;

  GPCT_ASSERT(storage != NULL);
  gpct_atomic_init(&obj->f.refcnt);
  gpct_atomic_set(&obj->f.refcnt, 1);
  obj->f.storage_free = sfree;

  err = name##_constructor(obj, ap);

  /* prevent memset(obj, 0, sizeof(*obj)) in constructor */
  GPCT_ASSERT(gpct_atomic_get(&obj->f.refcnt) >= 1);
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
  GPCT_STR_ERROR("delete() is illegal with reference counting");
  return 0;
}

attr GPCT_OBJ_PROTO_REFNEW(name, prefix)
{
  GPCT_ASSERT(gpct_atomic_get(&object->f.refcnt) >= 0);

  gpct_atomic_inc(&object->f.refcnt);

  return object;
}

attr GPCT_OBJ_PROTO_REFCOUNT(name, prefix)
{
  return gpct_atomic_get(&object->f.refcnt);
}

attr GPCT_OBJ_PROTO_REFDROP(name, prefix)
{
  GPCT_ASSERT(gpct_atomic_get(&object->f.refcnt) > 0);

  if (gpct_atomic_dec(&object->f.refcnt))
    return;

  name##_destructor(object);

  /* give a chance to destructor to abort */
  if (gpct_atomic_get(&object->f.refcnt) > 0)
    return;

  gpct_atomic_destroy(&object->f.refcnt);
  if (object->f.storage_free != NULL)
    object->f.storage_free(object);
}
/* backslash-region-end */



/* backslash-region-begin */
#define GPCT_OBJ_REFCOUNT_INITIALIZER
{
  .storage_free = NULL,
  .refcnt = GPCT_ATOMIC_INITIALIZER(1)
}
/* backslash-region-end */

#endif

