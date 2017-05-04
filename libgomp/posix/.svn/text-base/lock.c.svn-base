/* Copyright (C) 2005, 2008, 2009 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU OpenMP Library (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  

   MutekH port by Alexandre Becoulet, 2010

*/

/* This is the default PTHREADS implementation of the public OpenMP
   locking primitives.

   Because OpenMP uses different entry points for normal and recursive
   locks, and pthreads uses only one entry point, a system may be able
   to do better and streamline the locking as well as reduce the size
   of the types exported.  */

/* We need Unix98 extensions to get recursive locks.  On Tru64 UNIX V4.0F,
   the declarations are available without _XOPEN_SOURCE, which actually
   breaks compilation.  */
#ifndef __osf__
#define _XOPEN_SOURCE 500
#endif

#include "libgomp.h"

void
gomp_init_lock_30 (omp_lock_t *lock)
{
  sem_init (lock, 0, 1);
}

void
gomp_destroy_lock_30 (omp_lock_t *lock)
{
  sem_destroy (lock);
}

void
gomp_set_lock_30 (omp_lock_t *lock)
{
  while (sem_wait (lock) != 0)
    ;
}

void
gomp_unset_lock_30 (omp_lock_t *lock)
{
  sem_post (lock);
}

omp_int_t
gomp_test_lock_30 (omp_lock_t *lock)
{
  return sem_trywait (lock) == 0;
}

void
gomp_init_nest_lock_30 (omp_nest_lock_t *lock)
{
  sem_init (&lock->lock, 0, 1);
  lock->count = 0;
  lock->owner = NULL;
}

void
gomp_destroy_nest_lock_30 (omp_nest_lock_t *lock)
{
  sem_destroy (&lock->lock);
}

void
gomp_set_nest_lock_30 (omp_nest_lock_t *lock)
{
  void *me = gomp_icv (true);

  if (lock->owner != me)
    {
      while (sem_wait (&lock->lock) != 0)
	;
      lock->owner = me;
    }
  lock->count++;
}

void
gomp_unset_nest_lock_30 (omp_nest_lock_t *lock)
{
  if (--lock->count == 0)
    {
      lock->owner = NULL;
      sem_post (&lock->lock);
    }
}

omp_int_t
gomp_test_nest_lock_30 (omp_nest_lock_t *lock)
{
  void *me = gomp_icv (true);

  if (lock->owner != me)
    {
      if (sem_trywait (&lock->lock) != 0)
	return 0;
      lock->owner = me;
    }

  return ++lock->count;
}

ialias (omp_init_lock)
ialias (omp_init_nest_lock)
ialias (omp_destroy_lock)
ialias (omp_destroy_nest_lock)
ialias (omp_set_lock)
ialias (omp_set_nest_lock)
ialias (omp_unset_lock)
ialias (omp_unset_nest_lock)
ialias (omp_test_lock)
ialias (omp_test_nest_lock)


