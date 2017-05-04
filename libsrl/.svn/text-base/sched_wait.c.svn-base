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

#include <stddef.h>
#include <hexo/endian.h>
#include <hexo/iospace.h>
#include <mutek/scheduler.h>
#include <srl_private_types.h>
#include <srl/srl_sched_wait.h>
#include <srl/srl_hw_helpers.h>

static inline srl_task_s *context_to_srl_task( struct sched_context_s *ctx )
{
    uintptr_t p = (uintptr_t)ctx;
    p -= offsetof(srl_task_s, context);
    return (srl_task_s *)p;
}

#define endian_cpu32(x) (x)

#define DECLARE_WAIT(endianness, name, cmp)                                              \
                                                                                         \
    static SCHED_CANDIDATE_FCN(wait_##name##endianness##_f)                              \
    {                                                                                    \
        srl_task_s *task = context_to_srl_task(sched_ctx);                               \
                                                                                         \
        cpu_dcache_invld((void*)task->wait_addr);                                        \
        return (endian_##endianness##32(cpu_mem_read_32((uintptr_t)task->wait_addr)) cmp task->wait_val); \
    }                                                                                    \
                                                                                         \
    void srl_sched_wait_##name##_##endianness( void *addr, int32_t val )        \
    {                                                                                    \
        cpu_dcache_invld((void*)addr);                                                   \
        if ( endian_##endianness##32(cpu_mem_read_32((uintptr_t)addr)) cmp val ) \
        return;                                                                          \
        srl_task_s *current = context_to_srl_task(sched_get_current());                  \
        current->wait_val = val;                                                         \
        current->wait_addr = addr;                                                       \
        do {                                                                             \
            cpu_interrupt_disable();                                                     \
            sched_context_candidate_fcn(&current->context, wait_##name##endianness##_f); \
            sched_context_switch();                                                      \
            sched_context_candidate_fcn(&current->context, NULL);                        \
            cpu_interrupt_enable();                                                      \
        } while (!wait_##name##endianness##_f(&current->context));                       \
    }

static SCHED_CANDIDATE_FCN(wait_priv)
{
	srl_task_s *task = context_to_srl_task(sched_ctx);
	srl_callback_t *cb = (srl_callback_t*)task->wait_addr;

	return cb((uint32_t)task->wait_val);
}

void srl_sched_wait_priv( srl_callback_t *cb, uint32_t val )
{
	srl_task_s *current = context_to_srl_task(sched_get_current());
	current->wait_val = (int32_t)val;
	current->wait_addr = cb;
	cpu_interrupt_disable();
	sched_context_candidate_fcn(&current->context, wait_priv);
	sched_context_switch();
	sched_context_candidate_fcn(&current->context, NULL);
	cpu_interrupt_enable();
}

DECLARE_WAIT(le, eq, ==)
DECLARE_WAIT(le, ne, !=)
DECLARE_WAIT(le, le, <=)
DECLARE_WAIT(le, ge, >=)
DECLARE_WAIT(le, lt, <)
DECLARE_WAIT(le, gt, >)

DECLARE_WAIT(be, eq, ==)
DECLARE_WAIT(be, ne, !=)
DECLARE_WAIT(be, le, <=)
DECLARE_WAIT(be, ge, >=)
DECLARE_WAIT(be, lt, <)
DECLARE_WAIT(be, gt, >)

DECLARE_WAIT(cpu, eq, ==)
DECLARE_WAIT(cpu, ne, !=)
DECLARE_WAIT(cpu, le, <=)
DECLARE_WAIT(cpu, ge, >=)
DECLARE_WAIT(cpu, lt, <)
DECLARE_WAIT(cpu, gt, >)

static int next_run( uint32_t date )
{
	return srl_cycle_count() > date;
}

void srl_sleep_cycles( uint32_t n )
{
	uint32_t next_run_to = srl_cycle_count()+n;

	while(srl_cycle_count()<next_run_to)
		srl_sched_wait_priv((void*)next_run, next_run_to);
}

