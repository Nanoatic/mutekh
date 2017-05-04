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

#include <pthread.h>

/************************************************************************
		PTHREAD_MUTEX_NORMAL
************************************************************************/

error_t
__pthread_mutex_normal_lock(pthread_mutex_t *mutex)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  /* check current mutex state */
  if (mutex->count)
    {
      /* add current thread in mutex wait queue */
      sched_wait_unlock(&mutex->wait);
    }
  else
    {
      /* mark mutex as used */
      mutex->count++;
      sched_queue_unlock(&mutex->wait);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

error_t
__pthread_mutex_normal_trylock(pthread_mutex_t *mutex)
{
  error_t	res = EBUSY;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  /* check current mutex state */
  if (!mutex->count)
    {
      mutex->count++;
      res = 0;
    }

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

error_t
__pthread_mutex_normal_unlock(pthread_mutex_t *mutex)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  if (!sched_wake(&mutex->wait))
    mutex->count--;

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

#ifdef CONFIG_PTHREAD_MUTEX_ATTR

pthread_mutexattr_t __pthread_mutex_attr_normal =
  {
    .type =
    {
      .mutex_lock	= __pthread_mutex_normal_lock,
      .mutex_trylock	= __pthread_mutex_normal_trylock,
      .mutex_unlock	= __pthread_mutex_normal_unlock,
    }
  };

#endif



/************************************************************************
		PTHREAD_MUTEX_ERRORCHECK
************************************************************************/

#ifdef CONFIG_PTHREAD_MUTEX_ATTR

static error_t
__pthread_mutex_errorcheck_lock(pthread_mutex_t *mutex)
{
  error_t		res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  /* check current mutex state */
  if (mutex->count)
    {
      if (mutex->owner == sched_get_current())
	{
	  /* dead lock condition detected */
	  res = EDEADLK;
	  sched_queue_unlock(&mutex->wait);
	}
      else
	{
	  /* add current thread in mutex wait queue */
	  sched_wait_unlock(&mutex->wait);
	}
    }
  else
    {
      /* mark mutex as used */
      mutex->owner = sched_get_current();
      mutex->count++;
      sched_queue_unlock(&mutex->wait);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static error_t
__pthread_mutex_errorcheck_trylock(pthread_mutex_t *mutex)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  /* check current mutex state */
  if (mutex->count)
    {
      if (mutex->owner == sched_get_current())
	/* dead lock condition detected */
	res = EDEADLK;
      else
	res = EBUSY;
    }
  else
    mutex->count++;

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static error_t
__pthread_mutex_errorcheck_unlock(pthread_mutex_t *mutex)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  if (mutex->count)
    {
      if (mutex->owner == sched_get_current())
	{
	  mutex->owner = sched_wake(&mutex->wait);

          if (mutex->owner == NULL)
            mutex->count--;
	}
      else
	res = EPERM;
    }
  else
    res = EBUSY;

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

pthread_mutexattr_t __pthread_mutex_attr_errorcheck =
  {
    .type =
    {
      .mutex_lock	= __pthread_mutex_errorcheck_lock,
      .mutex_trylock	= __pthread_mutex_errorcheck_trylock,
      .mutex_unlock	= __pthread_mutex_errorcheck_unlock,
    }
  };

#endif



/************************************************************************
		PTHREAD_MUTEX_RECURSIVE
************************************************************************/

#ifdef CONFIG_PTHREAD_MUTEX_ATTR

static error_t
__pthread_mutex_recursive_lock(pthread_mutex_t *mutex)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  /* check current mutex state */
  if (mutex->count && (mutex->owner != sched_get_current()))
    {
      /* add current thread in mutex wait queue */
      sched_wait_unlock(&mutex->wait);
    }
  else
    {
      /* mark mutex as used */
      mutex->owner = sched_get_current();
      mutex->count++;
      sched_queue_unlock(&mutex->wait);
    }

  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

static error_t
__pthread_mutex_recursive_trylock(pthread_mutex_t *mutex)
{
  error_t	res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  if (mutex->count && (mutex->owner != sched_get_current()))
    res = EBUSY;
  else
    mutex->count++;

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static error_t
__pthread_mutex_recursive_unlock(pthread_mutex_t *mutex)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_queue_wrlock(&mutex->wait);

  if (--mutex->count == 0)
    {
      mutex->owner = sched_wake(&mutex->wait);

      if (mutex->owner != NULL)
        mutex->count++;
    }

  sched_queue_unlock(&mutex->wait);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

pthread_mutexattr_t __pthread_mutex_attr_recursive =
  {
    .type =
    {
      .mutex_lock	= __pthread_mutex_recursive_lock,
      .mutex_trylock	= __pthread_mutex_recursive_trylock,
      .mutex_unlock	= __pthread_mutex_recursive_unlock,
    }
  };

#endif



/************************************************************************/



error_t
pthread_mutex_init(pthread_mutex_t *mutex,
		   const pthread_mutexattr_t *attr)
{
  mutex->count = 0;

#ifdef CONFIG_PTHREAD_MUTEX_ATTR
  /* default mutex attribute */
  if (!attr)
    attr = &__pthread_mutex_attr_normal;

  mutex->attr = attr;
#endif

  return sched_queue_init(&mutex->wait);
}

error_t
pthread_mutex_destroy(pthread_mutex_t *mutex)
{
  sched_queue_destroy(&mutex->wait);
  return 0;
}

#ifdef CONFIG_PTHREAD_MUTEX_ATTR

error_t
pthread_mutexattr_settype(pthread_mutexattr_t *attr, int_fast8_t type)
{
  switch (type)
    {
    case PTHREAD_MUTEX_DEFAULT:
    case PTHREAD_MUTEX_NORMAL:
      attr->type = __pthread_mutex_attr_normal.type;
      return 0;

    case PTHREAD_MUTEX_ERRORCHECK:
      attr->type = __pthread_mutex_attr_errorcheck.type;
      return 0;

    case PTHREAD_MUTEX_RECURSIVE:
      attr->type = __pthread_mutex_attr_recursive.type;
      return 0;
    }

  return EINVAL;
}

#endif

