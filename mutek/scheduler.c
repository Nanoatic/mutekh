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
#include <gpct/cont_slist.h>

#include <hexo/init.h>
#include <hexo/local.h>
#include <hexo/cpu.h>
#include <hexo/segment.h>
#include <hexo/ipi.h>

/* processor current scheduler context */
CONTEXT_LOCAL struct sched_context_s *sched_cur = NULL;

/* processor idle context */
CPU_LOCAL struct sched_context_s sched_idle;

/***********************************************************************
 *      Scheduler base operations
 */

/************************** return next scheduler candidate except idle */

static struct sched_context_s *
__sched_candidate_noidle(sched_queue_root_t *root)
{
#ifdef CONFIG_MUTEK_SCHEDULER_CANDIDATE_FCN
  struct sched_context_s *c = NULL;

  CONTAINER_FOREACH_NOLOCK(sched_queue, DLIST, root, {
    if (item->is_candidate == NULL || item->is_candidate(item))
      {
	sched_queue_nolock_remove(root, item);
	c = item;
	CONTAINER_FOREACH_BREAK;
      }
  });

  return c;
#else
  return sched_queue_nolock_pop(root);
#endif
}

/************************** return next scheduler candidate */

static inline struct sched_context_s *
__sched_candidate(sched_queue_root_t *root)
{
  struct sched_context_s	*next;

  if ((next = __sched_candidate_noidle(root)) == NULL)
    next = CPU_LOCAL_ADDR(sched_idle);

  return next;
}

/************************** scheduler idle processors queue */

#if defined(CONFIG_HEXO_IPI)
# define CONTAINER_ORPHAN_CHK_idle_cpu_queue
/* We use a singly linked list here as idle cpu pick up order doesn't
   matter. No lock is needed as we only access this list when the
   running queue lock is held. */
CONTAINER_TYPE(idle_cpu_queue, SLIST, struct ipi_endpoint_s, idle_cpu_queue_list_entry);
CONTAINER_FUNC(idle_cpu_queue, SLIST, static inline, idle_cpu_queue, list_entry);
#endif

/************************** scheduler running contexts queue */

struct scheduler_s
{
    sched_queue_root_t root;
#if defined(CONFIG_HEXO_IPI)
    idle_cpu_queue_root_t idle_cpu;
#endif
    reg_t tmp_stack[CONFIG_MUTEK_SCHEDULER_TMP_STACK_SIZE / sizeof(reg_t)];
};

#if defined (CONFIG_MUTEK_SCHEDULER_MIGRATION)

/* scheduler root */
static struct scheduler_s CPU_NAME_DECL(scheduler);

/* return scheduler root */
static inline struct scheduler_s *
__scheduler_get(void)
{
  return & CPU_NAME_DECL(scheduler);
}

#elif defined (CONFIG_MUTEK_SCHEDULER_STATIC)

/* scheduler root */
static CPU_LOCAL struct scheduler_s	scheduler;

/* return scheduler root */
static inline struct scheduler_s *
__scheduler_get(void)
{
  return CPU_LOCAL_ADDR(scheduler);
}

#endif

/************************** scheduler context wake */

static inline
void __sched_context_push(struct sched_context_s *sched_ctx)
{
    struct scheduler_s *sched = sched_ctx->scheduler;

    sched_queue_wrlock(&sched->root);
    sched_queue_nolock_pushback(&sched->root, sched_ctx);

#if defined(CONFIG_HEXO_IPI)
    struct ipi_endpoint_s *idle = idle_cpu_queue_pop(&sched->idle_cpu);

    sched_queue_unlock(&sched->root);

    if ( idle )
      ipi_post(idle);
#else

    sched_queue_unlock(&sched->root);
#endif
}

/***********************************************************************
 *      Scheduler idle context
 */

struct sched_context_s main_ctx;

/* idle context runtime */
static CONTEXT_ENTRY(sched_context_idle)
{
  struct scheduler_s *sched = __scheduler_get();

  /* destroy app_start context */
  if (cpu_isbootstrap())
    context_destroy(&main_ctx.context);

#ifdef CONFIG_HEXO_IPI
  /* Get scheduler IPI endpoint for this processor  */
  struct ipi_endpoint_s *ipi_e = CPU_LOCAL_ADDR(ipi_endpoint);
  assert( ipi_endpoint_isvalid(ipi_e) );
#endif

  /* Scheduler running queue lock is held here */

  while (1)
    {
      struct sched_context_s	*next;

      cpu_interrupt_disable();

      /* Try to get a runnable context from running queue */
      if ((next = __sched_candidate_noidle(&sched->root)) != NULL)
	context_switch_to(&next->context);

  /************************** single processor case */

#if !defined(CONFIG_ARCH_SMP)
      /* Unlock running queue before going to sleep */
      sched_queue_unlock(&sched->root);

# ifdef CONFIG_CPU_WAIT_IRQ
      /* CPU sleep waiting for device IRQ */
      cpu_interrupt_wait();
      cpu_interrupt_disable();
# endif

  /************************** SMP case */

#else /* CONFIG_ARCH_SMP */

      /* Do not always make CPU sleep if SMP because context may be put
	 in running queue by an other cpu with no signalling. IPI is the
	 only way to solve this issue and wake a lazy processor. */

# if defined(CONFIG_HEXO_IPI)

      /* Declare processor as idle before unlocking scheduler */
      idle_cpu_queue_push(&sched->idle_cpu, ipi_e);

    still_idle:
      sched_queue_unlock(&sched->root);

      /* CPU sleep waiting for device IRQ or IPI */
      cpu_interrupt_wait();
      cpu_interrupt_disable();

# else  /* !CONFIG_HEXO_IPI */

      /* We are obliged to actively poll the running queue on SMP
	 systems without IPI support, Ugh. */

      sched_queue_unlock(&sched->root);

# endif
#endif

  /***************************/

      /* Let enough time for pending interrupts to execute and assume
	 memory is clobbered to force scheduler root queue
	 reloading after interrupts execution. */
      cpu_interrupt_process();

      /* WARNING: cpu_interrupt_wait and cpu_interrupt_process may
	 reenable interrupts. We must disable interrupts again before
	 taking the scheduler lock. */
      cpu_interrupt_disable();

      sched_queue_wrlock(&sched->root);

#if defined(CONFIG_HEXO_IPI)
      /* Do not even try to poll the running queue if always marked as
	 idle. Doing this also save us from removing the us from the
	 idle cpu list after cpu_interrupt_wait() call in case of IRQ and
	 therefor allow use of a singly linked and non-locked list. */
      if (!idle_cpu_queue_isorphan(ipi_e))
	goto still_idle;
#endif
    }
}

/***********************************************************************
 *      Scheduler primitives
 */

/* Switch to next context available in the root queue. This function
   returns if no other context is available, control is passed
   back to current context rather than Idle context. Must be called
   with interrupts disabled */
void sched_context_switch(void)
{
  struct scheduler_s *sched = __scheduler_get();
  struct sched_context_s *next;

  assert(!cpu_is_interruptible());

  sched_queue_wrlock(&sched->root);

  if ((next = __sched_candidate_noidle(&sched->root)))
    {
      /* push context back in running queue */
      sched_queue_nolock_pushback(&sched->root, CONTEXT_LOCAL_GET(sched_cur));
      context_switch_to(&next->context);
    }

  sched_queue_unlock(&sched->root);
}

/* Must be called with interrupts disabled and sched locked */
void sched_context_exit(void)
{
  assert(!cpu_is_interruptible());

  struct sched_context_s	*next;

  /* get next running context */
  next = __sched_candidate(&__scheduler_get()->root);
  context_jump_to(&next->context);
}

void sched_lock(void)
{
  assert(!cpu_is_interruptible());

  sched_queue_wrlock(&__scheduler_get()->root);
}

void sched_unlock(void)
{
  assert(!cpu_is_interruptible());

  sched_queue_unlock(&__scheduler_get()->root);
}

uintptr_t sched_tmp_stack(void)
{
  return (uintptr_t)__scheduler_get()->tmp_stack;
}

void sched_context_init(struct sched_context_s *sched_ctx)
{
  /* set sched_cur context local variable */
  CONTEXT_LOCAL_TLS_SET(sched_ctx->context.tls,
			sched_cur, sched_ctx);

  sched_ctx->priv = NULL;
  sched_ctx->scheduler = __scheduler_get();

#ifdef CONFIG_MUTEK_SCHEDULER_CANDIDATE_FCN
  sched_ctx->is_candidate = NULL;
#endif

}

/* Must be called with interrupts disabled */
void sched_context_start(struct sched_context_s *sched_ctx)
{
  assert(!cpu_is_interruptible());

  __sched_context_push(sched_ctx);
}

void sched_wait_callback(sched_queue_root_t *queue,
		         void (*callback)(void *ctx), void *ctx)
{
  struct scheduler_s *sched = __scheduler_get();
  struct sched_context_s *next;

  assert(!cpu_is_interruptible());

  /* add current context to queue, assume dont need lock */
  sched_queue_nolock_pushback(queue, CONTEXT_LOCAL_GET(sched_cur));

  /* lock scheduler before callback so that current
     context can not be woken up in the mean time. */
  sched_queue_wrlock(&sched->root);
  callback(ctx);

  /* get next running context */
  next = __sched_candidate(&sched->root);
  context_switch_to(&next->context);
  sched_queue_unlock(&sched->root);
}

/* push current context in the 'queue', unlock it and switch to next
   context available in the 'root' queue. Must be called with
   interrupts disabled */
void sched_wait_unlock(sched_queue_root_t *queue)
{
  struct scheduler_s *sched = __scheduler_get();
  struct sched_context_s *next;

  assert(!cpu_is_interruptible());

  /* add current context to queue, assume queue is already locked */
  sched_queue_nolock_pushback(queue, CONTEXT_LOCAL_GET(sched_cur));

  /* lock scheduler before unlocking queue so that current
     context can not be woken up in the mean time. */
  sched_queue_wrlock(&sched->root);
  sched_queue_unlock(queue);

  /* get next running context */
  next = __sched_candidate(&sched->root);
  context_switch_to(&next->context);
  sched_queue_unlock(&sched->root);
}

/* Switch to next context available in the 'root' queue, do not put
   current context in any queue. Idle context may be selected if no
   other contexts are available. Must be called with interrupts
   disabled */
void sched_context_stop(void)
{
  struct scheduler_s *sched = __scheduler_get();
  struct sched_context_s *next;

  assert(!cpu_is_interruptible());

  /* get next running context */
  sched_queue_wrlock(&sched->root);
  next = __sched_candidate(&sched->root);
  context_switch_to(&next->context);
  sched_queue_unlock(&sched->root);
}

/* Same as sched_context_stop but unlock given spinlock before switching */
void sched_context_stop_unlock(lock_t *lock)
{
  struct scheduler_s *sched = __scheduler_get();
  struct sched_context_s *next;

  assert(!cpu_is_interruptible());

  /* get next running context */
  sched_queue_wrlock(&sched->root);
  lock_release(lock);
  next = __sched_candidate(&sched->root);
  context_switch_to(&next->context);
  sched_queue_unlock(&sched->root);
}

/* Must be called with interrupts disabled and queue locked */
struct sched_context_s *sched_wake(sched_queue_root_t *queue)
{
  struct sched_context_s	*sched_ctx;

  assert(!cpu_is_interruptible());

  if ((sched_ctx = sched_queue_nolock_pop(queue)))
	  __sched_context_push(sched_ctx);

  return sched_ctx;
}

void sched_global_init(void)
{
#if defined(CONFIG_MUTEK_SCHEDULER_MIGRATION)
    struct scheduler_s *sched = __scheduler_get();

    sched_queue_init(&sched->root);
# if defined(CONFIG_HEXO_IPI)
    idle_cpu_queue_init(&sched->idle_cpu);
# endif
#endif
}

void sched_cpu_init(void)
{
  struct sched_context_s *idle = CPU_LOCAL_ADDR(sched_idle);
  uint8_t *stack;
  error_t err;

  assert(CONFIG_MUTEK_SCHEDULER_IDLE_STACK_SIZE % sizeof(reg_t) == 0);
  stack = arch_contextstack_alloc(CONFIG_MUTEK_SCHEDULER_IDLE_STACK_SIZE);

  assert(stack != NULL);

  err = context_init(&idle->context, stack,
		     stack + CONFIG_MUTEK_SCHEDULER_IDLE_STACK_SIZE,
		     sched_context_idle, 0);

  assert(err == 0);

#if defined(CONFIG_MUTEK_SCHEDULER_STATIC)
    struct scheduler_s *sched = __scheduler_get();

    sched_queue_init(&sched->root);
# if defined(CONFIG_HEXO_IPI)
    idle_cpu_queue_init(&sched->idle_cpu);
# endif
#endif
}

#ifdef CONFIG_MUTEK_SCHEDULER_MIGRATION

void sched_affinity_add(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
# ifndef CONFIG_MUTEK_SCHEDULER_MIGRATION_AFFINITY
# endif
}

void sched_affinity_remove(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
# ifndef CONFIG_MUTEK_SCHEDULER_MIGRATION_AFFINITY
# endif
}

void sched_affinity_single(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
# ifndef CONFIG_MUTEK_SCHEDULER_MIGRATION_AFFINITY
# endif
}

void sched_affinity_all(struct sched_context_s *sched_ctx)
{
# ifndef CONFIG_MUTEK_SCHEDULER_MIGRATION_AFFINITY
# endif
}

void sched_affinity_clear(struct sched_context_s *sched_ctx)
{
# ifndef CONFIG_MUTEK_SCHEDULER_MIGRATION_AFFINITY
# endif
}

#endif

#ifdef CONFIG_MUTEK_SCHEDULER_STATIC

void sched_affinity_add(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
#if defined(CONFIG_ARCH_SMP)
  void *cls = CPU_GET_CLS_ID(cpu);
  sched_ctx->scheduler = CPU_LOCAL_CLS_ADDR(cls, scheduler);
#endif
}

void sched_affinity_remove(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
}

void sched_affinity_single(struct sched_context_s *sched_ctx, cpu_id_t cpu)
{
#if defined(CONFIG_ARCH_SMP)
  sched_affinity_add(sched_ctx, cpu);
#endif
}

void sched_affinity_all(struct sched_context_s *sched_ctx)
{
}

void sched_affinity_clear(struct sched_context_s *sched_ctx)
{
}

#endif

void sched_context_candidate_fcn(struct sched_context_s *sched_ctx,
				 sched_candidate_fcn_t *fcn)
{
#ifdef CONFIG_MUTEK_SCHEDULER_CANDIDATE_FCN
  sched_ctx->is_candidate = fcn;
#else
  abort();
#endif
}

