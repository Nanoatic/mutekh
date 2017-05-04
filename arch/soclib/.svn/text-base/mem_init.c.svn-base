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
              Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2009
*/


#include <mutek/mem_alloc.h>
#include <mutek/mem_region.h>
#include <mutek/memory_allocator.h>

extern __ldscript_symbol_t __system_uncached_heap_start, __system_uncached_heap_end;

void mem_init(void)
{
#ifdef CONFIG_HEXO_MMU

  uint32_t t = (uint32_t)(&__system_uncached_heap_start);

  /* default_region is defined in mutek/include/mutek/mem_alloc.h */
  default_region = memory_allocator_init(NULL,
				      t,
				      t+CONFIG_SOCLIB_VMEM_MALLOC_REGION_SIZE);

#else

  default_region = memory_allocator_init(NULL, 
				      &__system_uncached_heap_start, 
					 (void*)((uintptr_t)&__system_uncached_heap_end -
                                                 (1 << CONFIG_HEXO_RESET_STACK_SIZE) * CONFIG_CPU_MAXCOUNT));

#endif
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
