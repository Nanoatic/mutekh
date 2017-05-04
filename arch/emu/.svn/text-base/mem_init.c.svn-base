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
              Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2009
*/

#include <mutek/mem_alloc.h>
#include <mutek/memory_allocator.h>

#if defined(CONFIG_MUTEK_MEM_REGION)
#include <mutek/mem_region.h>
#endif

#include <hexo/segment.h>
#include <hexo/lock.h>
#include <hexo/endian.h>

#include <arch/hexo/emu_syscalls.h>

void mem_init(void)
{
  void	*mem_start;
  void	*mem_end;


  mem_start = (void*)emu_do_syscall(EMU_SYSCALL_MMAP, 6, NULL, 
				    CONFIG_ARCH_EMU_MEMORY,
				    EMU_PROT_READ | EMU_PROT_WRITE | EMU_PROT_EXEC,
				    EMU_MAP_SHARED | EMU_MAP_ANONYMOUS, -1, 0);

  if (mem_start == EMU_MAP_FAILED)
    emu_do_syscall(EMU_SYSCALL_EXIT, 1);

  mem_end = (uint8_t*)mem_start + CONFIG_ARCH_EMU_MEMORY;

  mem_end = ALIGN_ADDRESS_LOW(mem_end, CONFIG_MUTEK_MEMALLOC_ALIGN);
  mem_start = ALIGN_ADDRESS_UP(mem_start, CONFIG_MUTEK_MEMALLOC_ALIGN);

  default_region = memory_allocator_init( NULL,
				       mem_start,
				       mem_end);


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
