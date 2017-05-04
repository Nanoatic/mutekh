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

    atomic integer operations dispatch

*/

#ifndef GPCT_ATOMIC_H_
#define GPCT_ATOMIC_H_

/* Try to guess atomic mechanism if none already defined */

#if !(defined(GPCT_CONFIG_ATOMIC_PTHREAD_SPIN) || defined(GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX) || defined(GPCT_CONFIG_ATOMIC_GCC) || defined(GPCT_CONFIG_ATOMIC_NONE) || defined(GPCT_CONFIG_ATOMIC_OTHER))

# warning No atomic operation mechanism defined, guessing ...

# if (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || __GNUC__ > 4)
#  define GPCT_CONFIG_ATOMIC_GCC
#  warning Using gcc atomic builtins
  
# elif defined(_REENTRANT) || defined(_THREAD_SAFE)
#  if (_XOPEN_SOURCE >= 600)
#   define GPCT_CONFIG_ATOMIC_PTHREAD_SPIN
#   warning Using pthread spinlock locking
#  else
#   define GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX
#   warning Using pthread mutex locking
#  endif

# else
#  error Unable to select an atomic operation mechanism, please define one using GPCT_CONFIG_ATOMIC_* macros
# endif

#endif

/* Include selected atomic mechanism header */

#if defined(GPCT_CONFIG_ATOMIC_PTHREAD_SPIN) || defined (GPCT_CONFIG_ATOMIC_PTHREAD_MUTEX)
# include <gpct/_atomic_pthread.h>

#elif defined(GPCT_CONFIG_ATOMIC_GCC)
# include <gpct/_atomic_gcc.h>

#elif defined(GPCT_CONFIG_ATOMIC_NONE)
# include <gpct/_atomic_none.h>

#elif defined(GPCT_CONFIG_ATOMIC_OTHER)
 /* Using user predefined atomic mechanism */

#else
# error No atomic operation mechanism defined
#endif

GPCT_INTERNAL void gpct_atomic_init(gpct_atomic_t *a);
GPCT_INTERNAL void gpct_atomic_destroy(gpct_atomic_t *a);
GPCT_INTERNAL void gpct_atomic_set(gpct_atomic_t *a, gpct_atomic_int_t v);
GPCT_INTERNAL gpct_atomic_int_t gpct_atomic_get(gpct_atomic_t *a);
GPCT_INTERNAL gpct_bool_t gpct_atomic_inc(gpct_atomic_t *a);
GPCT_INTERNAL gpct_bool_t gpct_atomic_dec(gpct_atomic_t *a);
GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test(gpct_atomic_t *a, gpct_atomic_int_t n);
GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_set(gpct_atomic_t *a, gpct_atomic_int_t n);
GPCT_INTERNAL gpct_bool_t gpct_atomic_bit_test_clr(gpct_atomic_t *a, gpct_atomic_int_t n);
GPCT_INTERNAL void gpct_atomic_bit_clr(gpct_atomic_t *a, gpct_atomic_int_t n);
GPCT_INTERNAL void gpct_atomic_bit_set(gpct_atomic_t *a, gpct_atomic_int_t n);

#endif

