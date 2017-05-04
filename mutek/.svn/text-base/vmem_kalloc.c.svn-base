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

#include <assert.h>
#include <hexo/mmu.h>
#include <hexo/endian.h>
#include <mutek/vmem_kalloc.h>
#include <mutek/page_alloc.h>


static uintptr_t next_v_page;

void vpage_init()
{
  next_v_page = mmu_global_init() * CONFIG_HEXO_MMU_PAGESIZE * MMU_TABLE_ENTRY + MMU_KERNEL_START_ADDR;
}

void * vpage_kalloc(struct vmem_page_region_s *r, size_t count)
{
  paddr_t paddr;
  uintptr_t vaddr;
  uint_fast16_t i, j;
  
  printk("VKalloc with size=%d\n",count);

  vaddr = next_v_page;
  assert( count <= ( MMU_KERNEL_END_ADDR - vaddr ) / CONFIG_HEXO_MMU_PAGESIZE );

  if( count == 1 )
    {
      if( ppage_alloc(r, &paddr))
	return NULL;
    }
  else
    {
      if( ppage_contiguous_alloc( r, &paddr, count ) )
	return NULL;
    }  
  
  for( i = 0 ; i < count ; i++ )
    {
      if (mmu_vpage_set(vaddr + i * CONFIG_HEXO_MMU_PAGESIZE, paddr + i * CONFIG_HEXO_MMU_PAGESIZE, MMU_PAGE_ATTR_RWX | MMU_PAGE_ATTR_PRESENT ) )
	{
	  for( j = 0 ; j < count ; j ++ )
	    ppage_refdrop(paddr + j * CONFIG_HEXO_MMU_PAGESIZE );
	  return NULL;
	}
    }

  next_v_page += CONFIG_HEXO_MMU_PAGESIZE * count;
  
  return (void*)vaddr;
}

void vpage_kfree(void *vaddr, size_t count)
{
  paddr_t paddr = mmu_vpage_get_paddr((uintptr_t)vaddr);
  uint_fast16_t i;

  for( i = 0 ; i < count ; i++ )
    {
      ppage_refdrop( paddr + i * CONFIG_HEXO_MMU_PAGESIZE );
      mmu_vpage_mask_attr( ( uintptr_t )vaddr + i * CONFIG_HEXO_MMU_PAGESIZE, 0, MMU_PAGE_ATTR_PRESENT );
    }
}

uintptr_t vpage_io_map(paddr_t paddr, size_t size)
{
  uintptr_t vaddr, res;

  size = ALIGN_VALUE_UP(size, CONFIG_HEXO_MMU_PAGESIZE);
  
  assert( size <= MMU_KERNEL_END_ADDR - next_v_page );

  res = vaddr = next_v_page;
  next_v_page += size;

  while (size)
    {
      mmu_vpage_set(vaddr, paddr, MMU_PAGE_ATTR_RW | MMU_PAGE_ATTR_PRESENT | MMU_PAGE_ATTR_NOCACHE );

      paddr += CONFIG_HEXO_MMU_PAGESIZE;
      vaddr += CONFIG_HEXO_MMU_PAGESIZE;
      size -= CONFIG_HEXO_MMU_PAGESIZE;
    }

  return res;
}


