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

    GCC Atomic operations based spin locks
    http://gcc.gnu.org/onlinedocs/gcc-4.4.1/gcc/Atomic-Builtins.html
*/

#ifndef GPCT_LOCK_GCC_SPIN_H_
#define GPCT_LOCK_GCC_SPIN_H_

#include <gpct/_lock_gcc.h>
#include <gpct/_platform.h>
#include <gpct/_cont_access.h>

typedef struct
{
  volatile GPCT_INT a;
} gpct_lock_GCC_SPIN_type_t;

#define gpct_lock_GCC_SPIN_initializer     { 0 }
#define GPCT_LOCK_GCC_SPIN_STATIC_INIT 1
#define GPCT_LOCK_GCC_SPIN_SPINNING 1
#define GPCT_LOCK_GCC_SPIN_READ_WRITE 0
#define GPCT_LOCK_GCC_SPIN_RECURSIVE 0

GPCT_INTERNAL gpct_error_t
gpct_lock_GCC_SPIN_init(gpct_lock_GCC_SPIN_type_t *lock)
{
  lock->a = 0;
  return 0;
}

GPCT_INTERNAL void
gpct_lock_GCC_SPIN_destroy(gpct_lock_GCC_SPIN_type_t *lock)
{
}

GPCT_INTERNAL void
gpct_lock_GCC_SPIN_wrlock(gpct_lock_GCC_SPIN_type_t *lock)
{
  while (__sync_lock_test_and_set(&lock->a, 1))
    {
      GPCT_GCC_SPIN_PAUSE();
    }
}

GPCT_INTERNAL void
gpct_lock_GCC_SPIN_rdlock(gpct_lock_GCC_SPIN_type_t *lock)
{
  gpct_lock_GCC_SPIN_wrlock(lock);
}

GPCT_INTERNAL void
gpct_lock_GCC_SPIN_unlock(gpct_lock_GCC_SPIN_type_t *lock)
{
  __sync_lock_release(&lock->a);
}

#endif

