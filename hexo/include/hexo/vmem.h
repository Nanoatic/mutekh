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

/**
 * @file
 * @module{Hexo}
 * @short Memory Management Unit and memory contexts stuff
 */

#ifndef VMEM_H_
#define VMEM_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#if defined(CONFIG_VMEM)

struct vmem_page_region_s;

/* Virtual memory allocator function prototype*/
#define VMEM_VPAGE_ALLOCATOR(n) void    *(n)(struct vmem_page_region_s *, size_t)
#define VMEM_VPAGE_FREE(n) void (n)(void *, size_t )
#define VMEM_VPAGE_INIT(n) void (n)(void);

typedef VMEM_VPAGE_ALLOCATOR(vmem_vpage_allocator_t);
typedef VMEM_VPAGE_FREE(vmem_vpage_free_t);
typedef VMEM_VPAGE_INIT(vmem_vpage_init_t);


/* Physical page allocator function prototype*/
#define VMEM_PPAGE_ALLOCATOR(n) error_t (n)(struct vmem_page_region_s *, paddr_t *)
#define VMEM_PPAGE_TO_REGION(n) struct vmem_page_region_s *(n)(paddr_t)
#define VMEM_PPAGE_REFDROP(n) void (n)(paddr_t )
#define VMEM_PPAGE_REGION_INIT(n) error_t (n)(struct vmem_page_region_s *, paddr_t, paddr_t);
#define VMEM_PPAGE_INITIAL_REGION_GET(n) struct vmem_page_region_s *(n)(void);

typedef VMEM_PPAGE_ALLOCATOR(vmem_ppage_allocator_t);
typedef VMEM_PPAGE_TO_REGION(vmem_ppage_to_region_t);
typedef VMEM_PPAGE_REFDROP(vmem_ppage_refdrop_t);
typedef VMEM_PPAGE_REGION_INIT(vmem_ppage_region_init_t);
typedef VMEM_PPAGE_INITIAL_REGION_GET(vmem_ppage_initial_region_get_t);


struct vmem_ops_s
{
  vmem_vpage_allocator_t *vpage_alloc;
  vmem_vpage_free_t *vpage_free;
  vmem_vpage_init_t *vpage_init;

  vmem_ppage_allocator_t *ppage_alloc;
  vmem_ppage_to_region_t *ppage_to_region;
  vmem_ppage_refdrop_t *ppage_refdrop;
  vmem_ppage_region_init_t *ppage_region_init;
  vmem_ppage_initial_region_get_t *ppage_initial_region_get;
};


#ifdef CONFIG_VMEM_PHYS_ALLOC
VMEM_PPAGE_ALLOCATOR(ppage_alloc);
VMEM_PPAGE_TO_REGION(ppage_to_region);
VMEM_PPAGE_REFDROP(ppage_refdrop);
VMEM_PPAGE_REGION_INIT(ppage_region_init);
VMEM_PPAGE_INITIAL_REGION_GET(ppage_initial_region_get);
#else
# error Add physical page allocator here
#endif

#ifdef CONFIG_VMEM_KERNEL_ALLOC
VMEM_VPAGE_ALLOCATOR(vpage_kalloc);
VMEM_VPAGE_FREE(vpage_kfree);
VMEM_VPAGE_INIT(vpage_init);
#else 
# error Add kernel virtual memory allocator here 
#endif

static inline
void vmem_ppage_ops_init(struct vmem_ops_s *vmem_ops)
{
  vmem_ops->ppage_alloc = &ppage_alloc;
  vmem_ops->ppage_to_region = &ppage_to_region;
  vmem_ops->ppage_refdrop = &ppage_refdrop;
  vmem_ops->ppage_region_init = &ppage_region_init;
  vmem_ops->ppage_initial_region_get = &ppage_initial_region_get;
}

static inline
void vmem_vpage_ops_init(struct vmem_ops_s *vmem_ops)
{
  vmem_ops->vpage_alloc = &vpage_kalloc;
  vmem_ops->vpage_free = &vpage_kfree;
  vmem_ops->vpage_init = &vpage_init;
}

#endif

C_HEADER_END

#endif
