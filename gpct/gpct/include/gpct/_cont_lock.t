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

    Container locking stuff

*/

#ifndef GPCT_CONT_LOCK_H_
#define GPCT_CONT_LOCK_H_

#include <gpct/_platform.h>

/* Lock the container root for modification */
#define GPCT_CONT_PROTO_WRLOCK(name, prefix)                    \
GPCT_UNUSED void                                                            \
prefix##_wrlock         (name##_root_t *root)

/* Lock the container root for read */
#define GPCT_CONT_PROTO_RDLOCK(name, prefix)                    \
GPCT_UNUSED void                                                            \
prefix##_rdlock         (name##_root_t *root)

/* Unlock the container root for read */
#define GPCT_CONT_PROTO_UNLOCK(name, prefix)                    \
GPCT_UNUSED void                                                            \
prefix##_unlock         (name##_root_t *root)

/***********************************************************************/

#define GPCT_LOCK_TYPE(lockname)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _type_t)

#define GPCT_LOCK_INITIALIZER(lockname)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _initializer)

#define GPCT_LOCK_INIT(lockname, lock)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _init)(lock)

#define GPCT_LOCK_DESTROY(lockname, lock)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _destroy)(lock)

#define GPCT_LOCK_WRLOCK(lockname, lock)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _wrlock)(lock)

#define GPCT_LOCK_RDLOCK(lockname, lock)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _rdlock)(lock)

#define GPCT_LOCK_UNLOCK(lockname, lock)  GPCT_CPP_CONCAT(gpct_lock_, lockname, _unlock)(lock)

/***********************************************************************/

/* backslash-region-begin */
#define GPCT_CONT_LOCK_FUNC(attr, name, prefix, lockname)

attr GPCT_CONT_PROTO_WRLOCK(name, prefix)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);
}

attr GPCT_CONT_PROTO_RDLOCK(name, prefix)
{
  GPCT_LOCK_RDLOCK(lockname, &root->lock);
}

attr GPCT_CONT_PROTO_UNLOCK(name, prefix)
{
  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */

/***********************************************************************/

typedef gpct_empty_t gpct_lock_NOLOCK_type_t;

#define GPCT_LOCK_NOLOCK_INITIALIZER    {}

/* backslash-region-begin */
#define GPCT_LOCK_EMPTY_LOCK_FUNC(lockname)
GPCT_INTERNAL void lockname##_wrlock(void *lock) {}
GPCT_INTERNAL void lockname##_rdlock(void *lock) {}
GPCT_INTERNAL void lockname##_unlock(void *lock) {}
GPCT_INTERNAL gpct_error_t lockname##_init(void *lock) { return 0; }
GPCT_INTERNAL void lockname##_destroy(void *lock) {}
enum { lockname##_initializer = 0 };
/* backslash-region-end */

//static const gpct_empty_t lockname##_initializer = {};
GPCT_LOCK_EMPTY_LOCK_FUNC(gpct_lock_NOLOCK)

#define         LOCK_FEATURE(lockname, test) \
  GPCT_CPP_CONCAT(GPCT_LOCK_, lockname, _##test)

#endif

