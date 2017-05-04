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

#include <hexo/init.h>
#include <hexo/types.h>
#include <hexo/endian.h>
#include <hexo/segment.h>

#include <string.h>
#include <stdio.h>

#include <srl.h>
#include <srl_private_types.h>

#if CONFIG_SOCLIB_MEMCHECKER
# include "arch/mem_checker.h"
#endif

extern const srl_appdesc_s app_desc;
lock_t fault_lock;

static CPU_EXCEPTION_HANDLER(fault_handler)
{
  int_fast8_t		i;
#if defined(CPU_FAULT_COUNT) && defined(CPU_FAULT_NAMES)
  static const char *const fault_names[] = CPU_FAULT_NAMES;
  const char *fault_name;
  if ( type >= CPU_FAULT_COUNT )
	  fault_name = "Unknown";
  else
	  fault_name = fault_names[type];
#endif
#ifdef CPU_GPREG_NAMES
  static const char		*reg_names[] = CPU_GPREG_NAMES;
#endif

  cpu_interrupt_enable();
  lock_spin(&fault_lock);

#if defined(CPU_FAULT_COUNT) && defined(CPU_FAULT_NAMES)
  _cpu_printf("CPU Fault: cpuid(%u) faultid(%s)\n", cpu_id(), fault_name);
#else
  _cpu_printf("CPU Fault: cpuid(%u) faultid(%u)\n", cpu_id(), type);
#endif
  _cpu_printf("Execution pointer: %p, Bad address (if any): %p\n", execptr, dataptr);
  _cpu_printf("Registers:");

  for (i = 0; i < CPU_GPREG_COUNT; i++)
#ifdef CPU_GPREG_NAMES
    _cpu_printf("%s=%p%c", reg_names[i], regtable[i], (i + 1) % 4 ? ' ' : '\n');
#else
    _cpu_printf("%p%c", regtable[i], (i + 1) % 4 ? ' ' : '\n');
#endif

  _cpu_printf("Stack top:");

  for (i = 0; i < 8; i++)
	  _cpu_printf("%p%c", ((uint32_t*)stackptr)[i], (i + 1) % 4 ? ' ' : '\n');

  lock_release(&fault_lock);
  cpu_interrupt_disable();

  while (1);
}

static void srl_task_run(srl_task_s *task)
{
	if ( task->bootstrap ) {
		srl_log_printf(NONE, "Bootstrapping %s on cpu %d\n", task->name, cpu_id());
		task->bootstrap(task->args);
	}

	srl_barrier_wait(app_desc.start);
	srl_log_printf(NONE, "Running %s\n", task->name);
	if ( task->func )
		for (;;) {
			task->func( task->args );
		}
}

void user_hw_init()
{
#ifdef CONFIG_ARCH_SMP
	const srl_cpudesc_s * const *cpu;
	uint_fast16_t cpuid;
	
	for ( cpu = app_desc.cpu, cpuid = 0;
		  *cpu;
		  ++cpu, ++cpuid ) {
		cpu_local_storage[cpuid] = arch_cpudata_alloc();
	}
#endif

	hw_init();
}

#ifdef CONFIG_PTHREAD

static void *srl_run_task( void* param )
{
	srl_task_s *task = param;
	srl_console_init_task(task->tty_addr);
	srl_task_run(task);
	return NULL;
}

static void srl_task_init(srl_task_s *task)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_affinity(&attr, cpu_id());
	pthread_attr_setstack(&attr, task->stack, task->stack_size);
	pthread_create( &task->pthread, &attr, srl_run_task, task );
	pthread_attr_destroy(&attr);
}

#else

static CONTEXT_ENTRY(srl_run_task)
{
	srl_task_s *task = param;
	srl_console_init_task(task->tty_addr);
	sched_unlock();
	cpu_interrupt_enable();
	srl_task_run(task);
	cpu_interrupt_disable();
	sched_lock();
	sched_context_exit();
}

static void srl_task_init(srl_task_s *task)
{
	CPU_INTERRUPT_SAVESTATE_DISABLE;
	context_init( &task->context.context,
				  task->stack, (uint8_t*)task->stack + task->stack_size,
				  srl_run_task, task );
	sched_context_init( &task->context );
	sched_affinity_single( &task->context, cpu_id() );
	sched_context_start( &task->context );
	CPU_INTERRUPT_RESTORESTATE;
}

#endif

static void srl_cpu_init(const srl_cpudesc_s *cpu)
{
	size_t i;
	cpu_interrupt_enable();
	srl_console_init_cpu(cpu->tty_addr);

/* 	cpu_printf("CPU %i is up and running\n" */
/* 			   "DCache: %d bytes/line\n", */
/* 			   cpu_id(), cpu_dcache_line_size()); */
//	cpu_printf("Bootstrapping cpu %d: %d tasks\n", cpu_id(), cpu->ntasks);
	for ( i=0; i<cpu->ntasks; ++i )
		srl_task_init(cpu->task_list[i]);
}

#if !defined(CONFIG_MUTEK_MAIN)

void srl_console_init_cpu(void *addr);
void srl_console_init_task(void *addr);
void srl_console_init(void *addr);

void mutek_start_smp(void)
{
	cpu_exception_sethandler(fault_handler);
	srl_cpu_init(app_desc.cpu[cpu_id()]);
	
	cpu_interrupt_disable();
	sched_lock();
#if CONFIG_SOCLIB_MEMCHECKER
	soclib_mem_check_delete_ctx(cpu_id())
#endif
	sched_context_exit();
}

int_fast8_t mutek_start(int_fast8_t argc, char **argv)
{
	srl_console_init(app_desc.tty_addr);

	lock_init(&fault_lock);
#ifndef CONFIG_PTHREAD
	lock_init(&app_desc.start->lock);
#endif

	arch_start_other_cpu();
	mutek_start_smp();
	return 0;
}

#else /* has mutek_start() */

void app_start()
{
	lock_init(&fault_lock);
#ifndef CONFIG_PTHREAD
	lock_init(&app_desc.start->lock);

/* 	{ */
/* 		srl_mwmr_t *mwmr = app_desc.mwmr; */
/* 		while (*mwmr ) { */
/* 			mwmr_init(*mwmr); */
/* 			mwmr++; */
/* 		} */
/* 	} */

#endif

	cpu_exception_sethandler(fault_handler);
	srl_cpu_init(app_desc.cpu[0]);

	cpu_interrupt_disable();
	sched_lock();
#if CONFIG_SOCLIB_MEMCHECKER
	soclib_mem_check_delete_ctx(cpu_id())
#endif
	sched_context_exit();
}

#endif
