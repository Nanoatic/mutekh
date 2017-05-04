/*

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

#ifndef GPCT_LOCK_PTHREAD_MUTEX_H_
#define GPCT_LOCK_PTHREAD_MUTEX_H_

#if !(defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(GPCT_CONFIG_NOWARNING)
# warning pthread requires _REENTRANT or _THREAD_SAFE to be defined
#endif

#include <pthread.h>
#include <gpct/_platform.h>
#include <gpct/_cont_access.h>

/*************************************************************
 *      PTHREAD_MUTEX mutex lock functions
 */

#define gpct_lock_PTHREAD_MUTEX_initializer     PTHREAD_MUTEX_INITIALIZER
#define GPCT_LOCK_PTHREAD_MUTEX_STATIC_INIT 1
#define GPCT_LOCK_PTHREAD_MUTEX_SPINNING 0
#define GPCT_LOCK_PTHREAD_MUTEX_READ_WRITE 0
#define GPCT_LOCK_PTHREAD_MUTEX_RECURSIVE 0

typedef pthread_mutex_t gpct_lock_PTHREAD_MUTEX_type_t;

GPCT_INTERNAL gpct_error_t
gpct_lock_PTHREAD_MUTEX_init(pthread_mutex_t *lock)
{
  return pthread_mutex_init(lock, 0);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_MUTEX_destroy(pthread_mutex_t *lock)
{
  gpct_error_t err = pthread_mutex_destroy(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_MUTEX_wrlock(pthread_mutex_t *lock)
{
  gpct_error_t err = pthread_mutex_lock(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_MUTEX_rdlock(pthread_mutex_t *lock)
{
  gpct_error_t err = pthread_mutex_lock(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_MUTEX_unlock(pthread_mutex_t *lock)
{
  gpct_error_t err = pthread_mutex_unlock(lock);
  GPCT_ASSERT(!err);
}

#endif

