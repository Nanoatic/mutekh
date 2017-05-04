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

#ifndef GPCT_LOCK_PTHREAD_SPIN_H_
#define GPCT_LOCK_PTHREAD_SPIN_H_

#if !(defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(GPCT_CONFIG_NOWARNING)
# warning pthread requires _REENTRANT or _THREAD_SAFE to be defined
#endif

#if (_XOPEN_SOURCE < 600) && !defined(GPCT_CONFIG_NOWARNING)
# warning pthread spin locks requires _XOPEN_SOURCE >= 600
#endif

#include <pthread.h>
#include <gpct/_platform.h>
#include <gpct/_cont_access.h>

#define GPCT_LOCK_PTHREAD_SPIN_SPINNING 1
#define GPCT_LOCK_PTHREAD_SPIN_READ_WRITE 0
#define GPCT_LOCK_PTHREAD_SPIN_RECURSIVE 0

#if defined(PTHREAD_SPINLOCK_INITIALIZER)
# define gpct_lock_PTHREAD_SPIN_initializer    PTHREAD_SPINLOCK_INITIALIZER
# define GPCT_LOCK_PTHREAD_SPIN_STATIC_INIT 1

#elif !defined(GPCT_CONFIG_NOWARNING)
# warning pthread spin lock static initializer is not available on your platform
# define GPCT_LOCK_PTHREAD_SPIN_STATIC_INIT 0
#endif

typedef pthread_spinlock_t gpct_lock_PTHREAD_SPIN_type_t;

GPCT_INTERNAL gpct_error_t
gpct_lock_PTHREAD_SPIN_init(pthread_spinlock_t *lock)
{
  return pthread_spin_init(lock, PTHREAD_PROCESS_SHARED);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_SPIN_destroy(pthread_spinlock_t *lock)
{
  gpct_error_t err = pthread_spin_destroy(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_SPIN_wrlock(pthread_spinlock_t *lock)
{
  gpct_error_t err = pthread_spin_lock(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_SPIN_rdlock(pthread_spinlock_t *lock)
{
  gpct_error_t err = pthread_spin_lock(lock);
  GPCT_ASSERT(!err);
}

GPCT_INTERNAL void
gpct_lock_PTHREAD_SPIN_unlock(pthread_spinlock_t *lock)
{
  gpct_error_t err = pthread_spin_unlock(lock);
  GPCT_ASSERT(!err);
}

#endif

