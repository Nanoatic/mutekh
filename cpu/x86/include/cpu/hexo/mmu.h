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

#if !defined(MMU_H_) || defined(CPU_X86_MMU_H_)
#error This file can not be included directly
#else

#define CPU_X86_MMU_H_

#include <hexo/types.h>

#define MMU_USERLIMIT_PDE (CONFIG_HEXO_MMU_USER_START >> 22)
#define MMU_INITIAL_PDE (CONFIG_HEXO_MMU_INITIAL_END >> 22)

#define MMU_KERNEL_START_PDE 0
#define MMU_KERNEL_END_PDE (MMU_USERLIMIT_PDE - 1)

#define MMU_KERNEL_START_ADDR (MMU_KERNEL_START_PDE * 0x400000)
#define MMU_KERNEL_END_ADDR (MMU_KERNEL_END_PDE * 0x400000)

#define MMU_USER_START_PDE ((uintptr_t)MMU_USERLIMIT_PDE)
#define MMU_USER_END_PDE 1022

#define MMU_USER_START_ADDR (MMU_USER_START_PDE * 0x400000)
#define MMU_USER_END_ADDR 0xffbfffff

#define MMU_MIRROR_PDE 1023
#define MMU_MIRROR_ADDR 0xffc00000


#define MMU_X86_ALIGN __attribute__ ((aligned (CONFIG_HEXO_MMU_PAGESIZE)))

/* page table in directory */

struct cpu_x86_pagetable_entry_s
{
    uint32_t
		present:1,
		writable:1,
		userlevel:1,
		write_through:1,
		nocache:1,
		accessed:1,
		zero:2,
		global:1,
		unused:3,
		address:20;
};

/* 4Mb page in directory */

struct cpu_x86_page4m_entry_s
{
    uint32_t
		present:1,
		writable:1,
		userlevel:1,
		write_through:1,
		nocache:1,
		accessed:1,
		dirty:1,
		pagsize4m:1,
		global:1,
		unused:3,
		patindex:1,
		reserved:9,
		address:10;
};

/* 4k page in page table */

struct cpu_x86_page4k_entry_s
{
    uint32_t
		present:1,
		writable:1,
		userlevel:1,
		write_through:1,
		nocache:1,
		accessed:1,
		dirty:1,
		pat_index:1,
		global:1,
		unused:3,
		address:20;
};


union cpu_x86_page_entry_s
{
  struct cpu_x86_pagetable_entry_s pte;
  struct cpu_x86_page4m_entry_s p4m;
  struct cpu_x86_page4k_entry_s p4k;
  uint32_t val;
};

typedef union cpu_x86_page_entry_s cpu_x86_page_entry_t;

struct mmu_context_s
{
  cpu_x86_page_entry_t	*pagedir; /* page directory */
  cpu_x86_page_entry_t	*mirror; /* page table mirroring page directory */
  uintptr_t		pagedir_paddr; /* page directory physical address */
  uint_fast16_t		k_count; /* kernel entries count */
};

static inline void
mmu_x86_set_pagedir(uintptr_t paddr)
{
  reg_t tmp;

  asm volatile ("	orl	$0x08, %0	\n" /* pagedir caching is write through */
		"	movl	%%cr3, %1	\n"
		"	cmpl	%0, %1		\n" /* avoid useless TLB flush */
		"	je	1f		\n"
		"	movl	%0, %%cr3	\n"
		"1:				\n"
		: "=r" (paddr)
		, "=r" (tmp)
		: "0" (paddr)
		);
}

static inline union cpu_x86_page_entry_s *
mmu_x86_get_pagedir(void)
{
  void *pd;

  asm volatile ("movl %%cr3, %0		\n"
		"andl $0xfffffc00, %0	\n"
		: "=r" (pd)
		);

  return pd;
}

static inline void
mmu_x86_invalidate_page(uintptr_t vaddr)
{
  asm volatile("invlpg (%0)	\n"
	       :
	       : "r" (vaddr));
}

#endif

