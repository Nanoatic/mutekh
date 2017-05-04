/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 */

#ifndef SRL_PRIVATE_TYPES_H
#define SRL_PRIVATE_TYPES_H

#include <mutek/scheduler.h>
#include <hexo/types.h>
#include <hexo/error.h>
#include <srl/srl_public_types.h>
#include <stdint.h>

#include <hexo/lock.h>

#include <srl/srl_lock.h>
#include <hexo/context.h>

#include <mwmr/mwmr.h>

typedef struct mwmr_s srl_mwmr_s;
typedef struct mwmr_status_s srl_mwmr_status_s;

#define SRL_MWMR_INITIALIZER MWMR_INITIALIZER
#define SRL_MWMR_LOCK_INITIALIZER MWMR_LOCK_INITIALIZER
#define SRL_MWMR_STATUS_INITIALIZER MWMR_STATUS_INITIALIZER

#define SRL_CONST_INITIALIZER(x) x

#if defined(MWMR_USE_SEPARATE_LOCKS)
#define SRL_MWMR_USE_SEPARATE_LOCKS
#endif

#ifdef CONFIG_PTHREAD

#include <pthread.h>

#define SRL_LOCK_INITIALIZER() PTHREAD_MUTEX_INITIALIZER

typedef pthread_barrier_t srl_barrier_s;
# define SRL_BARRIER_INITIALIZER PTHREAD_BARRIER_INITIALIZER

#else /* not CONFIG_PTHREAD */

#define SRL_LOCK_INITIALIZER() LOCK_INITIALIZER

typedef struct srl_barrier_s
{
	const int_fast8_t max;
	int_fast8_t count;
#if 0
  /** blocked threads waiting for read */
	sched_queue_root_t wait;
#else
	lock_t lock;
	uint32_t serial;
	
#endif
} srl_barrier_s;

#if 0
# define SRL_BARRIER_INITIALIZER(n)										\
	{																	\
		.wait = CONTAINER_ROOT_INITIALIZER(sched_queue, CLIST),	\
		.count = (n),												\
		.max = (n),												\
	}
#else
# define SRL_BARRIER_INITIALIZER(n)										\
	{																	\
		.serial = 0, \
		.count = (n),												\
		.max = (n),												\
	}
#endif

#endif /* CONFIG_PTHREAD */

typedef void srl_task_func_t( void* );
typedef struct srl_abstract_task_s {
	srl_task_func_t *bootstrap;
	srl_task_func_t *func;
	void *args;
	void *stack;
	size_t stack_size;
	const char *name;
#ifdef CONFIG_PTHREAD
	pthread_t pthread;
#else /* not CONFIG_PTHREAD */
	struct sched_context_s context;
	int32_t wait_val;
	void *wait_addr;
#endif
	void *tty_addr;
} srl_task_s;

#define SRL_TASK_INITIALIZER(b, f, ss, s, a, n, ttya, ttyn)	\
	{														\
		.bootstrap = (srl_task_func_t *)b,					\
		.func = (srl_task_func_t *)f,						\
		.args = (void*)a,									\
		.stack = (void*)s,									\
		.stack_size = ss,									\
		.name = n,											\
		.tty_addr = (uint32_t*)ttya+4*ttyn,			\
	}

#define SRL_MEMSPACE_INITIALIZER( b, s ) \
{\
	.buffer = b,\
		 .size = s,\
		 }

typedef struct srl_abstract_cpudesc_s srl_cpudesc_s;
struct srl_abstract_cpudesc_s {
	const size_t ntasks;
	const srl_task_s * const *task_list;
	void *tty_addr;
};

#define SRL_CPUDESC_INITIALIZER(nt, tl, ttya, ttyn)	\
	{												\
		.ntasks = nt,								\
		.task_list = tl,							\
		.tty_addr = (uint32_t*)ttya+4*ttyn,	\
	}

typedef struct srl_abstract_appdesc_s srl_appdesc_s;
struct srl_abstract_appdesc_s {
	const size_t ntasks;
	srl_barrier_s *start;
	const srl_mwmr_s * const *mwmr;
	const srl_cpudesc_s * const *cpu;
	const srl_task_s * const *task;
	void *tty_addr;
};

#define SRL_APPDESC_INITIALIZER(nt, cl, ml, tl, sb, ttya, ttyn) \
	{														   \
		.ntasks = nt,										   \
		.cpu = cl,										       \
		.mwmr = ml,										       \
		.task = tl,										       \
		.start = sb,										   \
		.tty_addr = (uint32_t*)ttya+4*ttyn,			   \
	}

#endif
