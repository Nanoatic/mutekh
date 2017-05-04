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

#if !defined( ARCH_SOCLIB_MMU_H_ ) || defined( ARCH_SOCLIB_VCACHE40_H_ )
#error This file can not be included directly
#else

#define ARCH_SOCLIB_VCACHE40_H_
#include <hexo/endian.h>
#include <hexo/types.h>
#include <hexo/cpu.h>

#include <cpu/soclib/mmu_access.h>

//#define MMU_USERLIMIT_PDE ( CONFIG_HEXO_MMU_USER_START >> 22 )
//#define MMU_INITIAL_PDE ( CONFIG_HEXO_MMU_INITIAL >> 22 )


#define MMU_DIRECTORY_ENTRY 2048ul
#define MMU_TABLE_ENTRY 512ul

#define MMU_KERNEL_START_PDE 1024ul
#define MMU_KERNEL_END_PDE 2044ul

#define MMU_KERNEL_START_ADDR ( MMU_KERNEL_START_PDE * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY )
#define MMU_KERNEL_END_ADDR ( MMU_KERNEL_END_PDE * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY )

#define MMU_USER_START_PDE 0ul
#define MMU_USER_END_PDE 1024ul 

#define MMU_USER_START_ADDR ( MMU_USER_START_PDE * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY )
#define MMU_USER_END_ADDR ( MMU_USER_END_PDE * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY )

#define MMU_MIRROR_PDE 2044ul
#define MMU_MIRROR_ADDR ( MMU_MIRROR_PDE * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY )

#define MMU_VCACHE_TLB_OFF 3
#define MMU_VCACHE_TLB_ON 15

#define MMU_DIRECTORY_PAGE_SIZE 2
#define MMU_MIRROR_PAGE_SIZE 4

#define MMU_PTBA_SIZE 28
#define MMU_PPN1_SIZE 19
#define MMU_PPN2_SIZE 28

#define MMU_PDE_SIZE 11
#define MMU_PTE_SIZE 9
#define MMU_OFFSET_SIZE (CONFIG_HEXO_MMU_VADDR - MMU_PDE_SIZE - MMU_PTE_SIZE)

#define MMU_PDE_MASK 0xffe00000
#define MMU_PTE_MASK 0x001ff000

#define MMU_PDR_SIZE 27

#define MMU_PTBA_TO_PADDR(x) (paddr_t)( x << ( CONFIG_HEXO_MMU_PADDR - MMU_PTBA_SIZE ) )
#define MMU_PADDR_TO_PTBA(x) (uint32_t)( x >> ( CONFIG_HEXO_MMU_PADDR - MMU_PTBA_SIZE ) )

#define MMU_PPN1_TO_PADDR(x) (paddr_t)( x << ( CONFIG_HEXO_MMU_PADDR - MMU_PPN1_SIZE ) )
#define MMU_PADDR_TO_PPN1(x) (uint32_t)( x >> ( CONFIG_HEXO_MMU_PADDR - MMU_PPN1_SIZE ) )

#define MMU_PPN2_TO_PADDR(x) (paddr_t)( x << ( CONFIG_HEXO_MMU_PADDR - MMU_PPN2_SIZE ) )
#define MMU_PADDR_TO_PPN2(x) (uint32_t)( x >> ( CONFIG_HEXO_MMU_PADDR - MMU_PPN2_SIZE ) )

#define MMU_VADDR_TO_PDE(x) (uint_fast16_t)( ( x & MMU_PDE_MASK) >> ( CONFIG_HEXO_MMU_VADDR - MMU_PDE_SIZE ) )
#define MMU_PDE_TO_VADDR(x) (uintptr_t)( ( x ) << ( CONFIG_HEXO_MMU_VADDR - MMU_PDE_SIZE ) )

#define MMU_VADDR_TO_PTE(x) (uint_fast16_t)( ( x & MMU_PTE_MASK) >> ( CONFIG_HEXO_MMU_VADDR - MMU_PTE_SIZE ) )
#define MMU_PTE_TO_VADDR(x) (uintptr_t)( ( x ) << ( CONFIG_HEXO_MMU_VADDR - MMU_PTE_SIZE ) )

#define MMU_PDR_TO_PADDR(x) (paddr_t)( x << ( CONFIG_HEXO_MMU_PADDR - MMU_PDR_SIZE ) )
#define MMU_PADDR_TO_PDR(x) (uint32_t)( x >> ( CONFIG_HEXO_MMU_PADDR - MMU_PDR_SIZE ) )

/* page table in directory */

struct cpu_vcache_pagetable_entry_s
{
  ENDIAN_BITFIELD(	uint32_t  valid:1,
			uint32_t  type:1,
			uint32_t  reserved:2,
			uint32_t  address:28
			);
};

/* 2Mb page in directory */
struct cpu_vcache_page2m_entry_s
{
  ENDIAN_BITFIELD(	uint32_t  valid:1,
			uint32_t  type:1,
			uint32_t  local_access:1,
			uint32_t  remote_access:1,
			uint32_t  cacheable:1,
			uint32_t  writable:1,
			uint32_t  executable:1,
			uint32_t  user:1,
			uint32_t  global:1,
			uint32_t  dirty:1,
			uint32_t  reserved:3,
			uint32_t  address:19,
			);
};

/* 4k page in page table */

struct cpu_vcache_page4k_entry_s
{
  struct {
  ENDIAN_BITFIELD(
		  uint32_t  valid:1,
		  uint32_t  type:1,
		  uint32_t  local_access:1,
		  uint32_t  remote_access:1,
		  uint32_t  cacheable:1,
		  uint32_t  writable:1,
		  uint32_t  executable:1,
		  uint32_t  user:1,
		  uint32_t  global:1,
		  uint32_t  dirty:1,
		  uint32_t  reserved0:14,
		  uint32_t  soft:8,
		  );
  } __attribute__((packed));
  struct {
  ENDIAN_BITFIELD(
		  uint32_t  reserved1:4,
		  uint32_t  address:28,
		  );
  } __attribute__((packed));
};

static inline struct cpu_vcache_page4k_entry_s *
mmu_vcache_get_vpage_entry( uint_fast16_t pde, uint_fast16_t pte )
{
  return ( void* )( uintptr_t )( MMU_MIRROR_ADDR | ( pde << 12 ) | ( pte << 3 ) );
}

static inline struct cpu_vcache_page4k_entry_s *
mmu_vcache_get_vpage_entry_vaddr( uintptr_t vaddr )
{
  return ( void* )( uintptr_t )( MMU_MIRROR_ADDR | ( ( vaddr >> ( MMU_OFFSET_SIZE ) ) << 3 ) );
}


/*Read*/

static inline paddr_t
mmu_vcache_get_pagedir( void )
{
  paddr_t pd = cpu_soclib_mmu_get_register( 0 );
  return MMU_PDR_TO_PADDR( pd );
}

static inline uint32_t
mmu_vcache_get_tlb_mode( void )
{
  uint32_t mode = cpu_soclib_mmu_get_register( 1 );
  return mode;
}

static inline uint32_t
mmu_vcache_get_iexcep_type( void )
{
  uint32_t iexcep = cpu_soclib_mmu_get_register( 11 );
  return iexcep;
}

static inline uint32_t
mmu_vcache_get_dexcep_type( void )
{
  uint32_t dexcep = cpu_soclib_mmu_get_register( 12 );
  return dexcep;
}

static inline uint32_t
mmu_vcache_get_ins_bad_vaddr( void )
{
  uint32_t bvaddr = cpu_soclib_mmu_get_register( 13 );
  return bvaddr;
}

static inline uint32_t
mmu_vcache_get_data_bad_vaddr( void )
{
  uint32_t bvaddr = cpu_soclib_mmu_get_register( 14 );
  return bvaddr;
}

static inline uint32_t
mmu_vcache_get_mmu_param( void )
{
  uint32_t param = cpu_soclib_mmu_get_register( 15 );
  return param;
}

static inline uint32_t
mmu_vcache_get_mmu_version( void )
{
  uint32_t version = cpu_soclib_mmu_get_register( 16 );
  return version;
}

/*Write*/

static inline void
mmu_vcache_set_pagedir( paddr_t paddr )
{
  cpu_soclib_mmu_set_register( 0, MMU_PADDR_TO_PDR( paddr ) );
}

static inline void
mmu_vcache_set_tlb_mode( uint32_t mode )
{
  cpu_soclib_mmu_set_register( 1, mode );
}

static inline void
mmu_vcache_icache_flush( )
{
 cpu_soclib_mmu_set_register( 2, 1 );
}

static inline void
mmu_vcache_dcache_flush( )
{
 cpu_soclib_mmu_set_register( 3,1 );
}

static inline void
mmu_vcache_invalidate_instruction_page( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 4,vaddr );
}

static inline void
mmu_vcache_invalidate_data_page( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 5,vaddr );
}

static inline void
mmu_vcache_invalidate_instruction_line( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 6,vaddr );
}

static inline void
mmu_vcache_invalidate_data_line( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 7,vaddr );
}

static inline void
mmu_vcache_prefetch_instruction_line( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 8,vaddr );
}

static inline void
mmu_vcache_prefetch_data_line( uintptr_t vaddr )
{
  cpu_soclib_mmu_set_register( 9,vaddr );
}

static inline void
mmu_vcache_sync( )
{
  cpu_soclib_mmu_set_register( 10,1 );
}

#endif

