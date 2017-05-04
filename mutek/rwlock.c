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

#include <mutek/rwlock.h>

error_t
rwlock_destroy(struct rwlock_s *rwlock)
{
  sched_queue_destroy(&rwlock->wait_rd);
  sched_queue_destroy(&rwlock->wait_wr);

  return 0;
}

error_t
rwlock_init(struct rwlock_s *rwlock)
{
  rwlock->count = 0;

  if (!sched_queue_init(&rwlock->wait_rd))
    {
      if (!sched_queue_init(&rwlock->wait_wr))
	return 0;

      sched_queue_destroy(&rwlock->wait_rd);      
    }

  return -ENOMEM;
}

error_t
rwlock_rdlock(struct rwlock_s *rwlock)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&rwlock->wait_wr);

  /* check write locked or write lock pending */
  if (rwlock->count < 0 || !sched_queue_isempty(&rwlock->wait_wr))
    {
      sched_queue_wrlock(&rwlock->wait_rd);
      sched_queue_unlock(&rwlock->wait_wr);
      /* add current thread in read wait queue */
      sched_wait_unlock(&rwlock->wait_rd);
    }
  else
    {
      /* mark rwlock as used */
      rwlock->count++;
      sched_queue_unlock(&rwlock->wait_wr);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
rwlock_tryrdlock(struct rwlock_s *rwlock)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&rwlock->wait_wr);

  /* check write locked or write lock pending */
  if (rwlock->count < 0 || !sched_queue_isempty(&rwlock->wait_wr))
    res = -EBUSY;
  else
    rwlock->count++;

  sched_queue_unlock(&rwlock->wait_wr);
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

error_t
rwlock_wrlock(struct rwlock_s *rwlock)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&rwlock->wait_wr);

  /* check locked */
  if (rwlock->count != 0)
    {
      /* add current thread in write wait queue */
      sched_wait_unlock(&rwlock->wait_wr);
    }
  else
    {
      /* mark rwlock as write locked */
      rwlock->count--;
      sched_queue_unlock(&rwlock->wait_wr);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
rwlock_trywrlock(struct rwlock_s *rwlock)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&rwlock->wait_wr);

  /* check locked */
  if (rwlock->count != 0)
    res = -EBUSY;
  else
    rwlock->count--;

  sched_queue_unlock(&rwlock->wait_wr);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
rwlock_unlock(struct rwlock_s *rwlock)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&rwlock->wait_wr);

  switch (rwlock->count)
    {
      /* read locked once */
    case 1:
      /* write locked */
    case -1:

      rwlock->count = 0;
      /* we are unlocked here */

      /* try to wake 1 pending write thread */
      if (sched_wake(&rwlock->wait_wr))
	{
	  rwlock->count--;
	  break;
	}

      /* wake all pending read threads */
      sched_queue_wrlock(&rwlock->wait_rd);
      while (sched_wake(&rwlock->wait_rd))
	rwlock->count++;
      sched_queue_unlock(&rwlock->wait_rd);

      break;

#ifdef CONFIG_DEBUG
      /* already unlocked */
    case 0:
      res = -EPERM;
      break;
#endif

      /* read locked mutiple times */
    default:
      rwlock->count--;
      break;
    }

  sched_queue_unlock(&rwlock->wait_wr);
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

