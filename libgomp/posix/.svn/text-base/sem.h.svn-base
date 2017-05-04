/* Copyright (C) 2005, 2006, 2009 Free Software Foundation, Inc.
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

/* This is the default POSIX 1003.1b implementation of a semaphore
   synchronization mechanism for libgomp.  This type is private to
   the library.

   This is a bit heavy weight for what we need, in that we're not
   interested in sem_wait as a cancelation point, but it's not too
   bad for a default.  */

#ifndef GOMP_SEM_H
#define GOMP_SEM_H 1

#include <semaphore.h>

typedef sem_t gomp_sem_t;

static inline void gomp_sem_init (gomp_sem_t *sem, omp_int_t value)
{
  sem_init (sem, 0, value);
}

static inline void gomp_sem_post (gomp_sem_t *sem)
{
  sem_post (sem);
}

static inline void gomp_sem_destroy (gomp_sem_t *sem)
{
  sem_destroy (sem);
}

static inline void gomp_sem_wait (gomp_sem_t *sem)
{
#if 0
  /* With POSIX, the wait can be canceled by signals.  We don't want that.
     It is expected that the return value here is -1 and errno is EINTR.  */
  while (sem_wait (sem) != 0)
    continue;
#else
  sem_wait(sem);
#endif
}


#endif /* GOMP_SEM_H  */

