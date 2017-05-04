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

#include "pthread_pv.h"
#include <stdio.h>

#include <hexo/error.h>
#include <mutek/mem_alloc.h>
#include <hexo/local.h>
#include <hexo/types.h>
#include <mutek/scheduler.h>
#include <hexo/segment.h>

/** pointer to current thread */
CONTEXT_LOCAL pthread_t __pthread_current = NULL;

/** switch to next thread */
void
__pthread_switch(void)
{
#ifdef CONFIG_HEXO_IRQ
  assert(cpu_is_interruptible());
#endif

#ifdef CONFIG_PTHREAD_CANCEL
  pthread_testcancel();
#endif

  cpu_interrupt_disable();
  sched_context_switch();
  cpu_interrupt_enable();
}

void
__pthread_cleanup(void)
{
  struct pthread_s *thread = pthread_self();

  /* cleanup current context */
  arch_contextstack_free(context_destroy(&thread->sched_ctx.context));

  lock_destroy(&thread->lock);

  /* free thread structure */
  mem_free(thread);

  /* schduler context switch without saving */
  sched_context_exit();
}

/** end pthread execution */
void
pthread_exit(void *retval)
{
  struct pthread_s	*this = pthread_self();

#ifdef CONFIG_PTHREAD_KEYS
  _pthread_keys_cleanup(this);
#endif

  atomic_bit_set(&this->state, _PTHREAD_STATE_CANCELED);

  /* remove thread from runnable list */
  cpu_interrupt_disable();
  lock_spin(&this->lock);

#ifdef CONFIG_PTHREAD_JOIN
  if (!atomic_bit_test(&this->state, _PTHREAD_STATE_DETACHED))
    {
      pthread_t joined_thread = this->joined;

      if(joined_thread == NULL)
      /* thread not joined yet */
	{
	  /* mark thread as joinable */
	  atomic_bit_set(&this->state, _PTHREAD_STATE_JOINABLE);

	  this->joined_retval = retval;

	  /* stop thread, waiting for pthread_join or pthread_detach */
	  sched_context_stop_unlock(&this->lock);
	}
      else
	/* thread already joined */
	{
	  joined_thread->joined_retval = retval;

	  /* wake up joined thread */
	  sched_context_start(&joined_thread->sched_ctx);
	  lock_release(&this->lock);
	}
    }
#endif /* CONFIG_PTHREAD_JOIN */

  sched_lock();

  /* setup temp stack memory and jump to __pthread_cleanup() */
  cpu_context_set(sched_tmp_stack(), CONFIG_MUTEK_SCHEDULER_TMP_STACK_SIZE,
		  __pthread_cleanup);
}



#ifdef CONFIG_PTHREAD_JOIN

/** wait for thread termination */
error_t
pthread_join(pthread_t thread, void **value_ptr)
{
  error_t	res = 0;

#ifdef CONFIG_PTHREAD_CANCEL
  pthread_testcancel();
#endif

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  lock_spin(&thread->lock);

# ifdef CONFIG_PTHREAD_CHECK
  if (atomic_bit_test(&thread->state, _PTHREAD_STATE_DETACHED))
    {
      lock_release(&thread->lock);
      res = EINVAL;
    }
  else
# endif
    {
      if (atomic_bit_test(&thread->state, _PTHREAD_STATE_JOINABLE))
        {
          if (value_ptr)
            *value_ptr = thread->joined_retval;

          /* wake up terminated thread waiting for join */
          sched_context_start(&thread->sched_ctx);

          lock_release(&thread->lock);
        }
      else
        {
          if (thread->joined)
            {
              lock_release(&thread->lock);
              res = EINVAL;
            }
          else
            {
              /* register current thread into target thread's descriptor */
              thread->joined = pthread_self();
              assert(thread->joined && "Can not call pthread_join from non pthread context");

              /* wait for thread termination */
              sched_context_stop_unlock(&thread->lock);

              /* get joined thread's exit value */
              if (value_ptr)
                *value_ptr = pthread_self()->joined_retval;
            }
        }
    }

  CPU_INTERRUPT_RESTORESTATE;

  return res;
}



/** detach pthread */
error_t
pthread_detach(pthread_t thread)
{
  error_t	res = 0;

  LOCK_SPIN_IRQ(&thread->lock);

  if (!atomic_bit_testset(&thread->state, _PTHREAD_STATE_DETACHED))
    {
      if (atomic_bit_test(&thread->state, _PTHREAD_STATE_JOINABLE))
	sched_context_start(&thread->sched_ctx);
    }
  else
    res = EINVAL;

  LOCK_RELEASE_IRQ(&thread->lock);
  return res;
}

#endif /* CONFIG_PTHREAD_JOIN */


/** thread context entry point */

static CONTEXT_ENTRY(pthread_context_entry)
{
  struct pthread_s	*thread = param;

  CONTEXT_LOCAL_SET(__pthread_current, thread);

  /* release lock acquired in previous sched_context_switch() call */
  sched_unlock();

  /* enable interrupts for current thread */
  cpu_interrupt_enable();	/* FIXME should reflect state at thread creation time ? */

  /* call pthread_exit with return value if thread main functions
     returns */
  pthread_exit(thread->start_routine(thread->arg));
}


/** create a new pthread  */

error_t
pthread_create(pthread_t *thread_, const pthread_attr_t *attr,
	       pthread_start_routine_t *start_routine, void *arg)
{
  struct pthread_s	*thread;
  error_t		res;
  uint8_t		*stack = NULL;
  size_t		stack_size = CONFIG_PTHREAD_STACK_SIZE;

#ifdef CONFIG_PTHREAD_ATTRIBUTES
  if (attr && attr->flags & _PTHREAD_ATTRFLAG_AFFINITY)
    {
      thread = mem_alloc_cpu(sizeof (struct pthread_s), (mem_scope_cpu), attr->cpulist[0]);
    } 
  else
#endif
    {
      thread = mem_alloc(sizeof (struct pthread_s), (mem_scope_sys));
    }

  if (!thread)
    return ENOMEM;

  if (lock_init(&thread->lock))
    {
      mem_free(thread);
      return ENOMEM;
    }

  /* find a stack buffer */
#ifdef CONFIG_PTHREAD_ATTRIBUTES
  if ( attr )
    {
      if(attr->flags & _PTHREAD_ATTRFLAG_STACK)
        {
          stack_size = attr->stack_size;
          stack = attr->stack_buf;
        } else if (attr->flags & _PTHREAD_ATTRFLAG_AFFINITY)
        {
          stack = mem_alloc_cpu(stack_size, mem_scope_cpu, attr->cpulist[0]);
        }
    }
  if (stack == NULL)
#endif
    {
      stack = arch_contextstack_alloc(stack_size);

      if (stack == NULL)
        {
          mem_free(thread);
          return ENOMEM;
        }
    }

  assert(stack_size % sizeof(reg_t) == 0);

  /* setup context for new thread */
  res = context_init(&thread->sched_ctx.context, stack,
		     stack + stack_size, pthread_context_entry, thread);

  if (res)
    {
      mem_free(thread);
      arch_contextstack_free(stack);
      return res;
    }

  sched_context_init(&thread->sched_ctx);
  thread->sched_ctx.priv = thread;

  thread->start_routine = start_routine;
  thread->arg = arg;
  thread->joined = NULL;
  atomic_set(&thread->state, 0);

#ifdef CONFIG_PTHREAD_ATTRIBUTES
  if (attr && attr->flags & _PTHREAD_ATTRFLAG_DETACHED)
    atomic_bit_set(&thread->state, _PTHREAD_STATE_DETACHED);

  /* add cpu affinity */
  if (attr && attr->flags & _PTHREAD_ATTRFLAG_AFFINITY)
    {
      sched_affinity_single(&thread->sched_ctx, attr->cpulist[0]);
      
#ifdef CONFIG_MUTEK_SCHEDULER_MIGRATION
      cpu_id_t i;
      for (i = 1; i < attr->cpucount; i++)
	sched_affinity_add(&thread->sched_ctx, attr->cpulist[i]);
#endif

    }
#endif

#ifdef CONFIG_PTHREAD_KEYS
  _pthread_keys_init(thread);
#endif

  *thread_ = thread;

  /* add new thread runnable threads list */
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_context_start(&thread->sched_ctx);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}

#ifdef CONFIG_PTHREAD_ATTRIBUTES
error_t pthread_attr_affinity(pthread_attr_t *attr, cpu_id_t cpu)
{
  if (!(attr->flags & _PTHREAD_ATTRFLAG_AFFINITY))
    {
      attr->flags |= _PTHREAD_ATTRFLAG_AFFINITY;
      attr->cpucount = 0;
    }

  if (attr->cpucount >= CONFIG_CPU_MAXCOUNT)
    return ENOMEM;

#ifdef CONFIG_MUTEK_SCHEDULER_MIGRATION
  attr->cpulist[attr->cpucount++] = cpu;
#endif

#ifdef CONFIG_MUTEK_SCHEDULER_STATIC
  attr->cpulist[0] = cpu;
#endif

  return 0;
}

error_t pthread_attr_destroy(pthread_attr_t *attr)
{
  return 0;
}

error_t pthread_attr_init(pthread_attr_t *attr)
{
  attr->flags = 0;
  return 0;
}

error_t pthread_attr_setstack(pthread_attr_t *attr, void *stack_buf, size_t stack_size)
{
  attr->flags |= _PTHREAD_ATTRFLAG_STACK;
  attr->stack_buf = stack_buf;
  attr->stack_size = stack_size;
  /* FIXME enforce stack alignment here */
  return 0;
}

error_t pthread_attr_setstacksize(pthread_attr_t *attr, size_t stack_size)
{
  attr->flags |= _PTHREAD_ATTRFLAG_STACK;
  attr->stack_buf = NULL;
  attr->stack_size = stack_size;
  /* FIXME enforce stack alignment here */
  return 0;
}

error_t pthread_attr_setdetachstate(pthread_attr_t *attr, uint8_t state)
{
  if (state == PTHREAD_CREATE_DETACHED)
    attr->flags |= _PTHREAD_ATTRFLAG_DETACHED;
  else
    attr->flags &= ~_PTHREAD_ATTRFLAG_DETACHED;    

  return 0;
}

#endif

