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

    Copyright Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2009
*/

#ifndef VMEM_KALLOC_H_
#define VMEM_KALLOC_H_

/**
 * @file
 * @module{Mutek}
 * @short Virtual memory kernel land page allocator API
 */

#include <hexo/types.h>
#include <hexo/mmu.h>
#include <mutek/page_alloc.h>

# ifndef CONFIG_VMEM_KERNEL_ALLOC
#  warning CONFIG_VMEM_KERNEL_ALLOC is disabled !!!
# else

/*
 * Kernel virtual space management
 */

/* initialisation of virtual memory kernel allocator */
void vpage_init();

/* map a physical address range somewhere in kernel space and return
   its address or 0 on error, may flush tlb  */
uintptr_t vpage_io_map(paddr_t paddr, size_t byte_size);

/* allocate virtual page memory in kernel page space, may flush tlb */
void * vpage_kalloc(struct vmem_page_region_s *r, size_t count);

/* free a kernel virtual page */
void vpage_kfree(void *vaddr, size_t count);

# endif

#endif

