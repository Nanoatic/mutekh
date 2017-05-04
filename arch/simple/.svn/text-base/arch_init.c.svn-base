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

#include <hexo/types.h>
#include <hexo/init.h>
#include <hexo/cpu.h>
#include <hexo/lock.h>
#include <mutek/mem_alloc.h>

#if defined(CONFIG_MUTEK_SCHEDULER)
#include <mutek/scheduler.h>
#endif

#include <string.h>

#ifdef CONFIG_DATA_FROM_ROM
extern __ldscript_symbol_t __bss_start;
extern __ldscript_symbol_t __bss_end;
extern __ldscript_symbol_t __data_start;
extern __ldscript_symbol_t __data_load_start;
extern __ldscript_symbol_t __data_load_end;
#endif

void arch_specific_init();

/* architecture specific init function */
void arch_init() 
{
#ifdef CONFIG_DATA_FROM_ROM
	memcpy_from_code((uint8_t*)&__data_start, (uint8_t*)&__data_load_start, (uint8_t*)&__data_load_end-(uint8_t*)&__data_load_start);
	memset((uint8_t*)&__bss_start, 0, (uint8_t*)&__bss_end-(uint8_t*)&__bss_start);
#endif

  /* Configure arch-specific things */
  arch_specific_init();

  /* configure system wide cpu data */
  cpu_global_init();

  mem_init();

  hexo_global_init();

  /* configure first CPU */
  cpu_init();

#if defined(CONFIG_MUTEK_SCHEDULER)
  sched_global_init();
  sched_cpu_init();
#endif

#if defined(CONFIG_ARCH_HW_INIT_USER)
  user_hw_init();
#elif defined(CONFIG_ARCH_HW_INIT)
  arch_hw_init();
#else
# error No supported hardware initialization
#endif

  /* run mutek_start() */
  mutek_start(0, 0);

  while (1)
    ;
}

void arch_start_other_cpu(void)
{
}

inline cpu_id_t arch_get_cpu_count(void)
{
  return 1;
}

