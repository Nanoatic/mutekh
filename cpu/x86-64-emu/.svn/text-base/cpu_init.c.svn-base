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


#include <mutek/mem_alloc.h>
#include <string.h>
#include <stdlib.h>
#include <hexo/interrupt.h>
#include <hexo/init.h>
#include <hexo/iospace.h>
#include <hexo/lock.h>
#include <hexo/segment.h>

/** pointer to cpu local storage itself */
CPU_LOCAL void *__cpu_data_base;
/** pointer to context local storage in cpu local storage */
CPU_LOCAL void *__context_data_base;

/* cpu interrupts state */
volatile CPU_LOCAL bool_t cpu_irq_state = 0;

#ifdef CONFIG_ARCH_SMP
void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];
CPU_LOCAL cpu_id_t _cpu_id;     /* use cpu_id() to access */
#endif

void cpu_trap()
{
#ifdef CONFIG_ARCH_EMU_TRAP_KILL
  /* kill process group */
  if (cpu_pids[0] > 1)
    emu_do_syscall(EMU_SYSCALL_KILL, 2, -cpu_pids[0], EMU_SIG_TERM);
  emu_do_syscall(EMU_SYSCALL_EXIT, 1, 0);
#else
  asm volatile ("int3");
#endif
}

error_t
cpu_global_init(void)
{
  return 0;
}

void cpu_init(void)
{
#ifdef CONFIG_ARCH_SMP
  void			*cls;

  if(!(cls = arch_cpudata_alloc()))
      abort();

  /* setup cpu local storage */
  cpu_local_storage[cpu_id()] = cls;

  /* we use this variable in non shared page as a cls register
   * that's why we do not use CPU_LOCAL_SET here. */
  __cpu_data_base = cls;
#endif

#if defined(CONFIG_CPU_X86_ALIGNCHECK)
   /* enable alignment check */
    asm volatile("	pushf						\n"
                 "	orl	$0x40000, (%rsp)			\n"
                 "	popf						\n");
#endif

}

