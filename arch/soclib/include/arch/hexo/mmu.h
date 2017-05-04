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

  Copyright Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2006

*/

#if !defined(MMU_H_) || defined(ARCH_SOCLIB_MMU_H_)
#error This file can not be included directly
#else

#define ARCH_SOCLIB_MMU_H_
#include <hexo/endian.h>
#include <hexo/types.h>
#include <hexo/cpu.h>

#if defined( CONFIG_ARCH_SOCLIB_VCACHE_32 )
 #include <arch/hexo/vcache32.h>
#elif defined( CONFIG_ARCH_SOCLIB_VCACHE_40 )
#include <arch/hexo/vcache40.h>
#else
 #error No vcache type specified
#endif

#define MMU_SOCLIB_ALIGN __attribute__ ((aligned (MMU_PAGESIZE * MMU_DIRECTORY_PAGE_SIZE)))//

union cpu_vcache_directory_entry_s
{
  struct cpu_vcache_pagetable_entry_s pte;
  struct cpu_vcache_page2m_entry_s p2m;
  uint32_t val;
};

union cpu_vcache_table_entry_s
{
  struct cpu_vcache_page4k_entry_s p4k;
  uint64_t val;
};

typedef union cpu_vcache_directory_entry_s cpu_vcache_directory_entry_t;
typedef union cpu_vcache_table_entry_s cpu_vcache_table_entry_t;

struct mmu_context_s
{
  cpu_vcache_directory_entry_t	*pagedir; /* page directory */
  cpu_vcache_table_entry_t	*mirror; /* page table mirroring page directory */
  paddr_t		pagedir_paddr; /* page directory physical address */
  uint_fast32_t		k_count; /* kernel entries count */
  struct vmem_page_region_s *ppage_region; /* physical page region to allocate ppage */
};

static inline error_t mmu_get_data_error_type(void)
{
  return mmu_vcache_get_dexcep_type();
}

static inline error_t mmu_get_ins_error_type(void)
{
  return mmu_vcache_get_iexcep_type();
}

static inline uintptr_t mmu_get_data_bad_address(void)
{
  return mmu_vcache_get_data_bad_vaddr();
}

static inline uintptr_t mmu_get_ins_bad_address(void)
{
  return mmu_vcache_get_ins_bad_vaddr();
}

#endif

