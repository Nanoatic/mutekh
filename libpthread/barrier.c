/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <mutek/scheduler.h>
#include <hexo/error.h>
#include <pthread.h>
#include <assert.h>

error_t pthread_barrier_destroy(pthread_barrier_t *barrier)
{
  sched_queue_destroy(&barrier->wait);

  return 0;
}

error_t pthread_barrier_init(pthread_barrier_t *barrier,
			     const pthread_barrierattr_t *attr,
			     unsigned count)
{
  assert(attr == NULL);
  barrier->count = count;
  return sched_queue_init(&barrier->wait);
}

error_t pthread_barrier_wait(pthread_barrier_t *barrier)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&barrier->wait);

  assert(barrier->count >= 1);

  if (barrier->count == 1)
    {
      while (sched_wake(&barrier->wait) != NULL)
	barrier->count++;
      sched_queue_unlock(&barrier->wait);
      res = PTHREAD_BARRIER_SERIAL_THREAD;
    }
  else
    {
      barrier->count--;
      sched_wait_unlock(&barrier->wait);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

