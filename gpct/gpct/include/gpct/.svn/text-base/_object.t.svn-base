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

#ifndef GPCT_OBJECT_H_
#define GPCT_OBJECT_H_

#include <stdarg.h>

#include <gpct/_platform.h>

#define OBJECT_STORAGE_FREE(fcn) void (fcn)(void * storage)
typedef OBJECT_STORAGE_FREE(object_storage_free_t);

/* backslash-region-begin */
#define OBJECT_TYPE(name, algo, type)

typedef type * name##_object_t;
typedef type name##_object_base_t;

typedef struct name##_entry_s
{
  object_storage_free_t *storage_free;
  GPCT_OBJ_##algo##_ENTRY_FIELDS(name);
} name##_entry_t;
/* backslash-region-end */


/***********************************************************************
 *      user defined object related functions prototypes
 */

/**
   Object contructor function provided by user.

   @param obj allocated object ready for initialization.
   @param ap stdarg object used to access extra parameters passed to new()

   Some gpct related fields may already be initialized and should not
   be overwritten with a global memset operation.
*/
#define OBJECT_CONSTRUCTOR(name)                                \
gpct_error_t                                                    \
(name##_constructor)(name##_object_t obj,                       \
                     va_list ap)

/**
   Object destructor called before freeing object storage area
 */
#define OBJECT_DESTRUCTOR(name)                                 \
void                                                            \
(name##_destructor)(name##_object_t obj)


/***********************************************************************
 *      object access functions prototypes
 */


/**
   Create a new object by allocating storage area and calling its
   constrctor. Object ref count is set to 1 if any.

   @param storage pointer to user provided storage area for
   object. Use NULL pointer to let gpct allocate storage for the
   object. if a storage area pointer is specified, no automatic
   storage freeing will occure after calling object destructor.

   @return constructor result, NULL if failed
 */
#define GPCT_OBJ_PROTO_NEW(name, prefix)                        \
name##_object_t                                                 \
prefix##_new    (void *storage, ...)

/**
   Create a new object by calling its constrctor. Object ref count is
   set to 1 if any.

   @param storage pointer to storage area for object
   @param free pointer to storage freeing function.
   @return constructor result, NULL if failed
*/
#define GPCT_OBJ_PROTO_CONSTRUCT(name, prefix)                  \
gpct_error_t                                                    \
prefix##_construct(void *storage,                               \
                   object_storage_free_t *sfree,                \
                   va_list ap)

/**
   Mark an object for deletion if available depending on deletion
   policy.
*/
#define GPCT_OBJ_PROTO_DELETE(name, prefix)                     \
gpct_error_t                                                    \
prefix##_delete(name##_object_t object)

/**
   Get a new reference to an object. May fail if _object.has been
   marked for deletion.

   @return pointer to object, NULL if failed
*/
#define GPCT_OBJ_PROTO_REFNEW(name, prefix)                     \
name##_object_t                                                 \
prefix##_refnew (name##_object_t object)

/**
   Drop a reference to an object. Destrctor will be called if
   reference count if set to one.
*/
#define GPCT_OBJ_PROTO_REFDROP(name, prefix)                    \
void                                                            \
prefix##_refdrop(name##_object_t object)

/**
   Get reference conter value
*/
#define GPCT_OBJ_PROTO_REFCOUNT(name, prefix)                   \
size_t                                                          \
prefix##_refcount(name##_object_t object)

/* backslash-region-begin */
#define OBJECT_PROTOTYPE(name, attr, prefix)

GPCT_UNUSED attr GPCT_OBJ_PROTO_NEW(name, prefix);
GPCT_UNUSED attr GPCT_OBJ_PROTO_DELETE(name, prefix);
GPCT_UNUSED attr GPCT_OBJ_PROTO_REFNEW(name, prefix);
GPCT_UNUSED attr GPCT_OBJ_PROTO_REFDROP(name, prefix);
GPCT_UNUSED attr GPCT_OBJ_PROTO_REFCOUNT(name, prefix);
/* backslash-region-end */

#define OBJECT_FUNC(name, algo, attr, prefix, ...)              \
 GPCT_OBJ_##algo##_FUNC(attr, name, prefix, __VA_ARGS__)

#define OBJECT_INITIALIZER(name, algo)          GPCT_OBJ_##algo##_INITIALIZER

#endif

