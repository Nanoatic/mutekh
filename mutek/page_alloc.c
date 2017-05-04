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

#include <mutek/page_alloc.h>

#include <hexo/endian.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>

#define VMEM_PPAGE_ISFREE(x) ((x) & 0x80000000)
#define VMEM_PPAGE_VALUE(x) ((x) & 0x7fffffff)
#define VMEM_PPAGE_SET(isfree, value) ((isfree << 31) | (value))

struct vmem_page_region_s initial_region;

error_t ppage_region_init(struct vmem_page_region_s *r,
			       paddr_t paddr, paddr_t paddr_end)
{
  uint_fast32_t i;

  paddr = ALIGN_VALUE_UP(paddr, MMU_PAGESIZE);
  paddr_end = ALIGN_VALUE_LOW(paddr_end, MMU_PAGESIZE);

  assert(paddr_end > paddr);

  if (lock_init(&r->lock))
    goto err;

  r->paddr = paddr;
  r->size = paddr_end - paddr;
  r->free_count = r->count = r->size / MMU_PAGESIZE;
  r->free_head = 0;
  r->table = mem_alloc(r->count * sizeof (uint_fast32_t), (mem_scope_sys));

  if (!r->table)
    goto err_lock;

  for (i = 0; i < r->count; i++)
    r->table[i] = VMEM_PPAGE_SET(1, i + 1);

  r->table[r->count - 1] = VMEM_PPAGE_SET(1, 0);

  return 0;

 err_lock:
  lock_destroy(&r->lock);
 err:
  return -ENOMEM;
}


void ppage_region_destroy(struct vmem_page_region_s *r)
{
  mem_free(r->table);
  lock_destroy(&r->lock);  
}

error_t ppage_alloc(struct vmem_page_region_s *r, paddr_t *paddr)
{
  uint_fast32_t *t;
  error_t res = -ENOMEM;

  LOCK_SPIN_IRQ(&r->lock);  

  if (r->free_count > 0)
    {
      t = r->table + r->free_head;
      *paddr = r->paddr + r->free_head * MMU_PAGESIZE;
      r->free_head = VMEM_PPAGE_VALUE(*t);
      *t = VMEM_PPAGE_SET(0, 1);		/* intial refcount is 1 */
      r->free_count--;
      res = 0;
    }

  LOCK_RELEASE_IRQ(&r->lock);  

  return res;
}

error_t ppage_contiguous_alloc(struct vmem_page_region_s *r, paddr_t *paddr, size_t size)
{
    if (size > r->free_count)
        goto err;

    LOCK_SPIN_IRQ(&r->lock);

    uint_fast32_t i, first;
    /* begin to search from index 0:
     * cannot use free_head as start index since the freelist is not sorted.
     */
    first = 0;
beg:
    for (i = first; i < first + size; i++)
        if (!VMEM_PPAGE_ISFREE(r->table[i]))
        {
            /* no more place after index i? */
            if ((i+size) > r->count)
                goto err;
            /* search again from the page after */
            first = i + 1;
            goto beg;
        }

    uint_fast32_t *n, c = 0;
    for (n = &r->free_head; c < size; )
    {
        uint_fast32_t i = VMEM_PPAGE_VALUE(*n);

        if (i >= first && i < first + size)
        {
            *n = VMEM_PPAGE_VALUE(r->table[i]);
            r->table[i] = VMEM_PPAGE_SET(0, 1);
            c++;
        }
        else
        {
            n = &r->table[i];
        }
    }

    r->free_count -= size;

    LOCK_RELEASE_IRQ(&r->lock);

   *paddr = r->paddr + first * MMU_PAGESIZE;

   return 0;
 err:
   return -ENOMEM;
}

bool_t ppage_inrange(struct vmem_page_region_s *r, paddr_t paddr)
{
  //  assert(paddr % MMU_PAGESIZE == 0);

  return ((paddr >= r->paddr) &&
	  (paddr < r->paddr + r->size));
}

error_t ppage_reserve(paddr_t paddr, paddr_t paddr_end)
{
  uint_fast32_t i, p;
  size_t size;
  error_t res = 0;
  struct vmem_page_region_s *r;
    
  paddr = ALIGN_VALUE_UP(paddr, MMU_PAGESIZE);
  paddr_end = ALIGN_VALUE_LOW(paddr_end, MMU_PAGESIZE);

  assert(paddr_end > paddr);

  r = ppage_to_region(paddr);

  size = (paddr_end - paddr) / MMU_PAGESIZE;

  LOCK_SPIN_IRQ(&r->lock);

  p = (paddr - r->paddr) / MMU_PAGESIZE;

  /* check if all region is free */
  for (i = p; i < p + size; i++)
    if (!VMEM_PPAGE_ISFREE(r->table[i]))
      {
	res = -ENOMEM;
	break;
      }

  if (!res)
    {
      uint_fast32_t *n, c = 0;

      for (n = &r->free_head; c < size; )
	{
	  uint_fast32_t i = VMEM_PPAGE_VALUE(*n);

	  if (i >= p && i < p + size)
	    {
	      *n = VMEM_PPAGE_VALUE(r->table[i]);
	      r->table[i] = VMEM_PPAGE_SET(0, 1);
	      c++;
	    }
	  else
	    {
	      n = &r->table[i];
	    }
	}

      r->free_count -= size;
      }

  LOCK_RELEASE_IRQ(&r->lock);  

  return res;
}

paddr_t ppage_refnew(paddr_t paddr)
{
  uint_fast32_t *t;
  uint_fast32_t p;

  struct vmem_page_region_s *r;

  r = ppage_to_region(paddr);    
  assert(ppage_inrange(r,paddr));

  p = (paddr - r->paddr) / MMU_PAGESIZE;
  t = r->table + p;

  LOCK_SPIN_IRQ(&r->lock);
  assert(!VMEM_PPAGE_ISFREE(*t));
  (*t)++;
  LOCK_RELEASE_IRQ(&r->lock);
  
  return paddr;
}

void ppage_refdrop(paddr_t paddr)
{
  uint_fast32_t *t;
  uint_fast32_t p;
  struct vmem_page_region_s *r;

  r = ppage_to_region(paddr);  
  assert(ppage_inrange(r,paddr));
  
  p = (paddr - r->paddr) / MMU_PAGESIZE;
  t = r->table + p;

  LOCK_SPIN_IRQ(&r->lock);
  assert(!VMEM_PPAGE_ISFREE(*t));
  assert(*t > 0);

  if (--(*t) == 0)
    {
      *t = VMEM_PPAGE_SET(1, r->free_head);
      r->free_head = p;
      r->free_count++;
    }

  LOCK_RELEASE_IRQ(&r->lock);  
}

struct vmem_page_region_s *ppage_to_region(paddr_t paddr)
{
  return &initial_region;
}


struct vmem_page_region_s *ppage_initial_region_get() 
{
  return &initial_region;
}

