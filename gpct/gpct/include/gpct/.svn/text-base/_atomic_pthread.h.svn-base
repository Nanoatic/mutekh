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

#ifndef GPCT_ATOMIC_PTHREAD_H_
#define GPCT_ATOMIC_PTHREAD_H_

#if !(defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(GPCT_CONFIG_NOWARNING)
# warning pthread requires _REENTRANT or _THREAD_SAFE to be defined
#endif

/* Use pthread spinlock */

#if defined (GPCT_CONFIG_ATOMIC_PTHREAD_SPIN)

# if (_XOPEN_SOURCE < 600) && !defined(GPCT_CONFIG_NOWARNING)
#  warning pthread spinlock requires _XOPEN_SOURCE >= 600
# endif

# define GPCT_ATOMIC_LOCKTYPE pthread_spinlock_t
# define GPCT_ATOMIC_LOCK(lock) \
  { gpct_error_t err = pthread_spin_lock(lock); GPCT_ASSERT(!err); }
# define GPCT_ATOMIC_UNLOCK(lock) \
  { gpct_error_t err = pthread_spin_unlock(lock); GPCT_ASSERT(!err); }

/* Use pthread mutex */

#elif defined (GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX)

# define GPCT_ATOMIC_LOCKTYPE pthread_mutex_t
# define GPCT_ATOMIC_LOCK(lock) \
  { gpct_error_t err = pthread_mutex_lock(lock); GPCT_ASSERT(!err); }
# define GPCT_ATOMIC_UNLOCK(lock) \
  { gpct_error_t err = pthread_mutex_unlock(lock); GPCT_ASSERT(!err); }

#else
# error No atomic pthread locking mechanism selected
#endif

#include <pthread.h>

typedef int_fast8_t     gpct_atomic_int_t;

typedef struct  gpct_atomic_s
{
  volatile gpct_atomic_int_t val;
  GPCT_ATOMIC_LOCKTYPE lock;
}               gpct_atomic_t;

/* static value init */
#ifdef GPCT_CONFIG_ATOMIC_PTHREAD_SPIN
# define GPCT_ATOMIC_INITIALIZER(v)     { .val = (v), .lock = PTHREAD_SPINLOCK_INITIALIZER }
#else
# define GPCT_ATOMIC_INITIALIZER(v)     { .val = (v), .lock = PTHREAD_MUTEX_INITIALIZER }
#endif

/* init atomic value lock if needed */
GPCT_INTERNAL void
gpct_atomic_init(gpct_atomic_t *a)
{
  gpct_error_t err;
#ifdef GPCT_CONFIG_ATOMIC_PTHREAD_SPIN
  err = pthread_spin_init(&a->lock, PTHREAD_PROCESS_PRIVATE);
#else
  err = pthread_mutex_init(&a->lock, NULL);
#endif
  GPCT_ASSERT(!err);
}

/* destroy atomic value lock if needed */
GPCT_INTERNAL void
gpct_atomic_destroy(gpct_atomic_t *a)
{
  gpct_error_t err;
#ifdef GPCT_CONFIG_ATOMIC_PTHREAD_SPIN
  err = pthread_spin_destroy(&a->lock);
#else
  err = pthread_mutex_destroy(&a->lock);
#endif
  GPCT_ASSERT(!err);
}

/* set atomic value */
GPCT_INTERNAL void
gpct_atomic_set(gpct_atomic_t *a, gpct_atomic_int_t v)
{
  a->val = v;
}

/* get atomic value */
GPCT_INTERNAL gpct_atomic_int_t
gpct_atomic_get(gpct_atomic_t *a)
{
  return a->val;
}

/* increment atomic value return true if not 0 */
GPCT_INTERNAL gpct_bool_t
gpct_atomic_inc(gpct_atomic_t *a)
{
  gpct_atomic_int_t     res;

  GPCT_ATOMIC_LOCK(&a->lock);
  res = ++a->val;
  GPCT_ATOMIC_UNLOCK(&a->lock);

  return res != 0;
}

/* decrement atomic value return true if not 0 */
GPCT_INTERNAL gpct_bool_t
gpct_atomic_dec(gpct_atomic_t *a)
{
  gpct_atomic_int_t     res;

  GPCT_ATOMIC_LOCK(&a->lock);
  res = --a->val;
  GPCT_ATOMIC_UNLOCK(&a->lock);

  return res != 0;
}

/* bit test */
GPCT_INTERNAL gpct_bool_t
gpct_atomic_bit_test(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  return (a->val & (1 << n)) != 0;
}

/* bit test and set */
GPCT_INTERNAL gpct_bool_t
gpct_atomic_bit_test_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     res;
  gpct_atomic_int_t     mask = (1 << n);

  GPCT_ATOMIC_LOCK(&a->lock);
  res = a->val & mask;
  a->val |= mask;
  GPCT_ATOMIC_UNLOCK(&a->lock);

  return res != 0;
}

/* bit test and clear */
GPCT_INTERNAL gpct_bool_t
gpct_atomic_bit_test_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  gpct_atomic_int_t     res;
  gpct_atomic_int_t     mask = (1 << n);

  GPCT_ATOMIC_LOCK(&a->lock);
  res = a->val & mask;
  a->val &= ~mask;
  GPCT_ATOMIC_UNLOCK(&a->lock);

  return res != 0;
}

/* bit clear */
GPCT_INTERNAL void
gpct_atomic_bit_clr(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  GPCT_ATOMIC_LOCK(&a->lock);
  a->val &= ~(1 << n);
  GPCT_ATOMIC_UNLOCK(&a->lock);
}

/* bit set */
GPCT_INTERNAL void
gpct_atomic_bit_set(gpct_atomic_t *a, gpct_atomic_int_t n)
{
  GPCT_ATOMIC_LOCK(&a->lock);
  a->val |= (1 << n);
  GPCT_ATOMIC_UNLOCK(&a->lock);
}

#endif

