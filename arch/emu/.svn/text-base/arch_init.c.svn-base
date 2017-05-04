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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/


#include <hexo/types.h>
#include <hexo/init.h>
#include <hexo/cpu.h>
#include <hexo/lock.h>
#include <hexo/endian.h>
#include <mutek/scheduler.h>
#include <mutek/printk.h>

#include <arch/hexo/emu_syscalls.h>

#ifdef CONFIG_ARCH_SMP
static size_t cpu_count = CONFIG_CPU_MAXCOUNT;
static bool_t cpu_init_flag = 0;

extern __ldscript_symbol_t __data_start, __data_end;
extern __ldscript_symbol_t __bss_start, __bss_end;

static void emu_data_remap(void)
{
    uint8_t *data_start = ALIGN_ADDRESS_LOW(&__data_start, CONFIG_ARCH_EMU_PAGESIZE);
    uint8_t *bss_end = ALIGN_ADDRESS_UP(&__bss_end, CONFIG_ARCH_EMU_PAGESIZE);
    size_t size = bss_end - data_start;

    uint8_t copy[size];
    memcpy(copy, data_start, size);

    if ((void*)emu_do_syscall(EMU_SYSCALL_MMAP, 6, data_start, size,
            EMU_PROT_READ | EMU_PROT_WRITE,
            EMU_MAP_FIXED | EMU_MAP_SHARED | EMU_MAP_ANONYMOUS, -1, 0) == EMU_MAP_FAILED)
        emu_do_syscall(EMU_SYSCALL_EXIT, 1, 42);  
    
    memcpy(data_start, copy, size);
}
#endif

#ifdef CONFIG_EMU_EARLY_CONSOLE
PRINTF_OUTPUT_FUNC(early_console_fd1)
{
  emu_do_syscall(EMU_SYSCALL_WRITE, 3, 1, str, len);  
}
#endif

__compiler_sint_t cpu_pids[CONFIG_CPU_MAXCOUNT];

/* architecture specific init function */
void arch_init()
{
#ifdef CONFIG_EMU_EARLY_CONSOLE
  printk_set_output(early_console_fd1, NULL);
#endif

#if defined(CONFIG_ARCH_SMP)
    /* remap data+bss segment with SHARED attribute */
    emu_data_remap();
#endif

    mem_init();

    hexo_global_init();

    cpu_global_init();

    cpu_pids[0] = emu_do_syscall(EMU_SYSCALL_GETPID, 0);

#if defined(CONFIG_ARCH_SMP)
    /* now everything is shared except the stack (the current unix stack) */
    size_t i;
    for (i=1; i<CONFIG_CPU_MAXCOUNT; i++)
    {
      __compiler_sint_t pid;
      
      pid = emu_do_syscall(EMU_SYSCALL_FORK, 0);

      if (pid == 0)
	{
	  _cpu_id = i;
	  goto other_cpu;
        }

      cpu_pids[i] = pid;
    }
    _cpu_id = 0;
#endif

    /* configure first CPU */
    cpu_init();

#ifdef CONFIG_HEXO_IRQ
    emu_interrupts_init();
#endif

#if defined(CONFIG_MUTEK_SCHEDULER)
    sched_global_init();
    sched_cpu_init();
#endif

    arch_hw_init();
    mem_region_init();

#if defined(CONFIG_ARCH_SMP)
    cpu_init_flag = 1;
#endif

    /* run mutek_start() */
    mutek_start(0, 0);

    emu_do_syscall(EMU_SYSCALL_EXIT, 1, 1);  

#ifdef CONFIG_ARCH_SMP
other_cpu:
    /* configure other CPUs */

    while (cpu_init_flag != 1)
        order_compiler_mem();

    cpu_init();

#if defined(CONFIG_MUTEK_SCHEDULER)
    sched_cpu_init();
#endif

    mutek_start_smp();

    emu_do_syscall(EMU_SYSCALL_EXIT, 1, 0);  
#endif
}

void arch_start_other_cpu(void)
{
}

cpu_id_t arch_get_cpu_count(void)
{
#ifdef CONFIG_ARCH_SMP
  return cpu_count;
#else
  return 1;
#endif
}

