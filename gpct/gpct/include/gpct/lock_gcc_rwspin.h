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

#ifndef GPCT_LOCK_GCC_RWSPIN_H_
#define GPCT_LOCK_GCC_RWSPIN_H_

#include <gpct/_lock_gcc.h>
#include <gpct/_platform.h>
#include <gpct/_cont_access.h>

typedef struct
{
  volatile GPCT_INT a;
} gpct_lock_GCC_RWSPIN_type_t;

/* rwlock state word format (32bits example)
   S W CCCCCC CCCCCCCC CCCCCCCC CCCCCCCC
 */

/* S: write lock spin bit, used to spin between writers and block new readers */
#define GPCT_GCC_RWSPIN_WRSPIN     (1 << (sizeof(GPCT_INT) * 8 - 1))
/* W: write locked and no more reader in critical section */
#define GPCT_GCC_RWSPIN_WRLOCKED   (1 << (sizeof(GPCT_INT) * 8 - 2))
/* All MSB write status bits */
#define GPCT_GCC_RWSPIN_WRITE	(GPCT_GCC_RWSPIN_WRSPIN | GPCT_GCC_RWSPIN_WRLOCKED)
/* C: All LSB reader counter bits */
#define GPCT_GCC_RWSPIN_READ	(~GPCT_GCC_RWSPIN_WRITE)

#define gpct_lock_GCC_RWSPIN_initializer     { 0 }
#define GPCT_LOCK_GCC_RWSPIN_STATIC_INIT 1
#define GPCT_LOCK_GCC_RWSPIN_SPINNING 1
#define GPCT_LOCK_GCC_RWSPIN_READ_WRITE 1
#define GPCT_LOCK_GCC_RWSPIN_RECURSIVE 0

GPCT_INTERNAL gpct_error_t
gpct_lock_GCC_RWSPIN_init(gpct_lock_GCC_RWSPIN_type_t *lock)
{
  lock->a = 0;
  return 0;
}

GPCT_INTERNAL void
gpct_lock_GCC_RWSPIN_destroy(gpct_lock_GCC_RWSPIN_type_t *lock)
{
}

GPCT_INTERNAL void
gpct_lock_GCC_RWSPIN_wrlock(gpct_lock_GCC_RWSPIN_type_t *lock)
{
  /* Spin to acquire the write lock and block new readers */
  while (__sync_fetch_and_or(&lock->a, GPCT_GCC_RWSPIN_WRSPIN)
	 & GPCT_GCC_RWSPIN_WRSPIN) {
    GPCT_GCC_SPIN_PAUSE();
  }

  /* Wait for all readers to leave critical section */
  do {
    GPCT_GCC_SPIN_PAUSE();
  } while (lock->a & GPCT_GCC_RWSPIN_READ);

  /* Effective write locked section entry, next unlock will be
     a write unlock. */
  __sync_fetch_and_or(&lock->a, GPCT_GCC_RWSPIN_WRLOCKED);
}

GPCT_INTERNAL void
gpct_lock_GCC_RWSPIN_rdlock(gpct_lock_GCC_RWSPIN_type_t *lock)
{
  /* Try to acquire a read lock */
  while (__sync_fetch_and_add(&lock->a, 1) & GPCT_GCC_RWSPIN_WRITE) {
    /* A write lock is actually pending so changes must be
       reverted. We may generate few noise by doing this but it
       can not confuse writers and may happend only once per cpu. */
    __sync_fetch_and_sub(&lock->a, 1);

    /* Spin wait for write request end */
    do { 
      GPCT_GCC_SPIN_PAUSE();
    } while (lock->a & GPCT_GCC_RWSPIN_WRITE);
  }
}

GPCT_INTERNAL void
gpct_lock_GCC_RWSPIN_unlock(gpct_lock_GCC_RWSPIN_type_t *lock)
{
  register GPCT_INT a = lock->a;

  GPCT_ASSERT(a != 0);

  /* If unlock is called we hold either some rdlocks or a wrlock and
     the WRLOCKED bit can not change here, so we may safely test current
     lock type before taking action. */
  if (a & GPCT_GCC_RWSPIN_WRLOCKED)
    /* Reset all write bits, next may be either a read or write lock
       with equal chances */
    __sync_fetch_and_and(&lock->a, ~GPCT_GCC_RWSPIN_WRITE);
  else
    /* Release a readlock */
    __sync_fetch_and_sub(&lock->a, 1);
}

#endif

