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

#ifndef VMEM_PALLOC_H_
#define VMEM_PALLOC_H_

/**
 * @file
 * @module{Mutek}
 * @short Physical pages allocator algorithms
 */

# ifndef CONFIG_VMEM_PHYS_ALLOC
#  warning CONFIG_VMEM_PHYS_ALLOC is disabled !!!
# else

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/lock.h>

/*
 *  physical page allocator
 */

struct vmem_page_region_s
{
  /** physical address of memory region */
  paddr_t			paddr;
  /** memory region size */
  size_t			size;
  /** memory region page count */
  size_t			count;
  /** memory region free page count */
  size_t			free_count;
  /** first free page */
  uint_fast32_t			free_head;
  /** page allocation table */
  uint_fast32_t			*table;

  lock_t			lock;
};

extern struct vmem_page_region_s initial_region;

/** Init a physical pages memory allocator region. */
error_t ppage_region_init(struct vmem_page_region_s *r, paddr_t paddr, paddr_t paddr_end);

/** Destroy a physical pages memory allocator region. */
void ppage_region_destroy(struct vmem_page_region_s *r);

/** Check if a physical address is in region range. */
bool_t ppage_inrange(struct vmem_page_region_s *r, paddr_t paddr);

/** Allocate a free physical page in region and set paddr value. */
error_t ppage_alloc(struct vmem_page_region_s *r, paddr_t *paddr);

/** Allocate contiguous free physical pages and set paddr value to the first one */
error_t ppage_contiguous_alloc(struct vmem_page_region_s *r, paddr_t *paddr, size_t size);

/** Try to reserve all pages in pysical address range. All pages must be free. */
error_t ppage_reserve(paddr_t paddr, paddr_t paddr_end);

/** Get a new reference to an already allocated physical page. */
paddr_t ppage_refnew( paddr_t paddr);

/** Drop a reference to an allocated physical page, page is marked as free if counter reach 0. */
void ppage_refdrop( paddr_t paddr);

/** Return paddr's region*/
struct vmem_page_region_s *ppage_to_region(paddr_t paddr);

/** Return the physical page allocator's initial region*/
struct vmem_page_region_s *ppage_initial_region_get();

# endif

#endif
