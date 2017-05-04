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

error_t
pthread_cond_init(pthread_cond_t *cond,
		  const pthread_condattr_t *attr)
{
  return sched_queue_init(&cond->wait);
}

error_t
pthread_cond_destroy(pthread_cond_t *cond)
{
  sched_queue_destroy(&cond->wait);

  return 0;
}

error_t
pthread_cond_signal(pthread_cond_t *cond)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&cond->wait);

  sched_wake(&cond->wait);

  sched_queue_unlock(&cond->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
pthread_cond_broadcast(pthread_cond_t *cond)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&cond->wait);

  while (sched_wake(&cond->wait));

  sched_queue_unlock(&cond->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  error_t	res = 0;

#ifdef CONFIG_PTHREAD_CANCEL
  pthread_testcancel();
#endif

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&cond->wait);

  if (!pthread_mutex_unlock(mutex))
    {
      sched_wait_unlock(&cond->wait);

      pthread_mutex_lock(mutex);
    }
#ifdef CONFIG_PTHREAD_CHECK
  else
    {
      sched_queue_unlock(&cond->wait);
      res = EINVAL;
    }
#endif

  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

error_t
pthread_cond_timedwait(pthread_cond_t *cond, 
		       pthread_mutex_t *mutex,
		       const struct timespec *delay)
{
#ifdef CONFIG_PTHREAD_CANCEL
  pthread_testcancel();
#endif

  /* FIXME */
  return pthread_cond_wait(cond, mutex);
}

