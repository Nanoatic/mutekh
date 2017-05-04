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

#include <assert.h>
#include <mutek/mem_alloc.h>
#include <mutek/mem_region.h>
#include <mutek/memory_allocator.h>
#include <hexo/segment.h>
#include <hexo/lock.h>
#include <hexo/endian.h>


#ifndef CONFIG_ARCH_IBMPC_MEMORY

static void *
mem_ibmpc_memsize_probe(void *start)
{
  volatile uint8_t	*x = ALIGN_ADDRESS_UP(start, 4096);
  size_t		step = 4096;

  while (1) {
    x += step;
    *x = 0x5a;
    *x = ~*x;

    if (*x == 0xa5)
      continue;

    x -= step;

    if (step == 1)
      break;

    step /= 2;
  }

  return (void*)x;
}

#endif

void mem_init(void)
{
  void	*mem_start = (uint8_t*)&__system_heap_start;
#ifdef CONFIG_ARCH_IBMPC_MEMORY
  void	*mem_end = (uint8_t*)CONFIG_ARCH_IBMPC_MEMORY;
#else
  void	*mem_end = mem_ibmpc_memsize_probe(&__system_heap_start);
#endif

  mem_end = ALIGN_ADDRESS_LOW(mem_end, CONFIG_MUTEK_MEMALLOC_ALIGN);
  mem_start = ALIGN_ADDRESS_UP(mem_start, CONFIG_MUTEK_MEMALLOC_ALIGN);

  assert(mem_end > mem_start);

#ifdef CONFIG_HEXO_MMU
  default_region = memory_allocator_init(NULL,
					 mem_start,
					 CONFIG_HEXO_MMU_INITIAL_END);
#else
  default_region = memory_allocator_init(NULL,
					 mem_start,
					 mem_end);
#endif

  printk("Ram memory: %p-%p (%i bytes)\n", mem_start, mem_end,
         (uint8_t*)mem_end - (uint8_t*)mem_start);
}

void mem_region_init(void)
{
#if defined(CONFIG_MUTEK_MEM_REGION)
  cpu_id_t cpu;
  uint_fast16_t i;
  
  for (cpu=0; cpu<arch_get_cpu_count(); cpu++)
    {
      mem_region_id_init(cpu);
      
      for (i=0; i<mem_scope_e_count; i++)
	{
	  if (i == mem_scope_sys)
	    mem_region_id_add(cpu, i, default_region, 0);
	  else
	    mem_region_id_add(cpu, i, default_region, 200);
	}
    }

  default_region = NULL;
#endif
}
