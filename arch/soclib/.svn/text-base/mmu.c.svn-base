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

#include <hexo/mmu.h>
#include <hexo/local.h>
#include <hexo/endian.h>
#include <string.h>
#include <assert.h>

#ifdef CONFIG_ARCH_SOCLIB
#include <soclib_addresses.h>
#endif


extern __ldscript_symbol_t      __segment_excep_start, __segment_excep_end;
extern __ldscript_symbol_t  	__segment_text_start , __segment_text_end;
extern __ldscript_symbol_t  	__segment_data_uncached_start, __system_uncached_heap_start;

MMU_SOCLIB_ALIGN cpu_vcache_directory_entry_t mmu_k_pagedir[2048];
MMU_SOCLIB_ALIGN cpu_vcache_table_entry_t mmu_k_mirror[2048];
struct mmu_context_s mmu_k_context;

CPU_LOCAL struct mmu_context_s *mmu_context_cur = &mmu_k_context;

struct vmem_page_region_s *initial_ppage_region;

struct vmem_ops_s vmem_ops;

inline bool_t mmu_is_user_vaddr(uintptr_t vaddr)
{
  return ( ( vaddr >= MMU_USER_START_ADDR ) && ( vaddr < MMU_USER_END_ADDR ) );
}

uint_fast32_t mmu_global_init()
{
  uint_fast16_t	excep_pde,text_pde,uncached_pde;
  uintptr_t t;

  mmu_k_context.pagedir = mmu_k_pagedir;
  mmu_k_context.mirror = mmu_k_mirror;
  mmu_k_context.pagedir_paddr = (paddr_t)(uintptr_t)mmu_k_pagedir;

  memset(mmu_k_pagedir, 0, sizeof (mmu_k_pagedir));
  memset(mmu_k_mirror, 0, sizeof (mmu_k_mirror));

  /* identity map memory space with 2Mb pages *///FIXME: 4k page for identity mapping

  t = (uintptr_t)(&__segment_text_start);
  text_pde = MMU_VADDR_TO_PDE( t );
  t = (uintptr_t)(&__segment_data_uncached_start);
  uncached_pde = MMU_VADDR_TO_PDE( t );
  t = (uintptr_t)(&__segment_excep_start);
  excep_pde = MMU_VADDR_TO_PDE( t );


  mmu_k_pagedir[text_pde].p2m.valid = 1;
  mmu_k_pagedir[text_pde].p2m.type = 0;
  mmu_k_pagedir[text_pde].p2m.local_access = 0;
  mmu_k_pagedir[text_pde].p2m.remote_access = 0;
  mmu_k_pagedir[text_pde].p2m.address = MMU_PADDR_TO_PPN1( MMU_PDE_TO_VADDR( text_pde ) );
  mmu_k_pagedir[text_pde].p2m.cacheable = 1;
  mmu_k_pagedir[text_pde].p2m.writable = 0;
  mmu_k_pagedir[text_pde].p2m.executable = 1;
  mmu_k_pagedir[text_pde].p2m.user = 0;
  mmu_k_pagedir[text_pde].p2m.global = 1;
  mmu_k_pagedir[text_pde].p2m.dirty = 0;

  if (text_pde != uncached_pde)
    {
      mmu_k_pagedir[uncached_pde].p2m.valid = 1;
      mmu_k_pagedir[uncached_pde].p2m.type = 0;
      mmu_k_pagedir[uncached_pde].p2m.local_access = 0;
      mmu_k_pagedir[uncached_pde].p2m.remote_access = 0;
      mmu_k_pagedir[uncached_pde].p2m.address = MMU_PADDR_TO_PPN1( MMU_PDE_TO_VADDR( uncached_pde ) );
#ifdef CONFIG_ARCH_SMP
      mmu_k_pagedir[uncached_pde].p2m.cacheable = 1;//fixme
#else
      mmu_k_pagedir[uncached_pde].p2m.cacheable = 1;
#endif
      mmu_k_pagedir[uncached_pde].p2m.writable = 1;
      mmu_k_pagedir[uncached_pde].p2m.executable = 0;
      mmu_k_pagedir[uncached_pde].p2m.user = 0;
      mmu_k_pagedir[uncached_pde].p2m.global = 1;
      mmu_k_pagedir[uncached_pde].p2m.dirty = 0;
    }
  else
    {
#ifdef CONFIG_ARCH_SMP
      mmu_k_pagedir[text_pde].p2m.cacheable = 1;//FIXME
#endif      
      mmu_k_pagedir[text_pde].p2m.writable = 1;
    }

  mmu_k_context.k_count = uncached_pde - MMU_KERNEL_START_PDE + 1;

  uint_fast16_t i = 0;
  for( i = 0 ; i < MMU_MIRROR_PAGE_SIZE ; i++)
    {
      mmu_k_pagedir[MMU_MIRROR_PDE + i].pte.valid = 1;
      mmu_k_pagedir[MMU_MIRROR_PDE + i].pte.type = 1;
      mmu_k_pagedir[MMU_MIRROR_PDE + i].pte.address = MMU_PADDR_TO_PTBA( (paddr_t)(uintptr_t)mmu_k_mirror ) + i;	

      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.valid = 1;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.type = 0;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.cacheable = 1;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.writable = 1;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.executable = 0;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.user = 0;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.global = 1;
      mmu_k_mirror[MMU_MIRROR_PDE + i].p4k.address = MMU_PADDR_TO_PPN2( (paddr_t)(uintptr_t)mmu_k_mirror ) + i ;
    }

  mmu_k_context.ppage_region = initial_ppage_region;

  return mmu_k_context.k_count;
}

void mmu_cpu_init()
{
  mmu_vcache_set_pagedir(mmu_k_context.pagedir_paddr);
  mmu_vcache_set_tlb_mode(MMU_VCACHE_TLB_ON);
}

/* Context function OK*/

struct mmu_context_s * mmu_get_kernel_context()
{
  return &mmu_k_context;
}

static void mmu_update_k_context(struct mmu_context_s *ctx)
{
  uint_fast32_t diff;

  if ((diff = mmu_k_context.k_count - ctx->k_count))
    {
      /* copy kernel part pagedir to context pagedir */
      memcpy(ctx->pagedir + ctx->k_count + MMU_KERNEL_START_PDE,
	     mmu_k_context.pagedir + ctx->k_count + MMU_KERNEL_START_PDE,
	     diff * sizeof(cpu_vcache_directory_entry_t));

      memcpy(ctx->mirror + ctx->k_count + MMU_KERNEL_START_PDE,
	     mmu_k_context.mirror + ctx->k_count + MMU_KERNEL_START_PDE,
	     diff * sizeof(cpu_vcache_table_entry_t));

      ctx->k_count += diff;
    }
}

error_t mmu_context_init(struct mmu_context_s *ctx, struct vmem_page_region_s *ppage_region)
{
  cpu_vcache_directory_entry_t *pagedir; 
  cpu_vcache_table_entry_t *mirror;

  if ((pagedir = vmem_ops.vpage_alloc(ppage_region, MMU_DIRECTORY_PAGE_SIZE)) == NULL)
    goto err;

  if ((mirror = vmem_ops.vpage_alloc(ppage_region, MMU_MIRROR_PAGE_SIZE)) == NULL)
    goto err2;

  memset(pagedir, 0, MMU_DIRECTORY_PAGE_SIZE * CONFIG_HEXO_MMU_PAGESIZE);
  memset(mirror, 0, MMU_MIRROR_PAGE_SIZE * CONFIG_HEXO_MMU_PAGESIZE);

  paddr_t mirror_paddr = mmu_vpage_get_paddr((uintptr_t)mirror);
  paddr_t pagedir_paddr = mmu_vpage_get_paddr((uintptr_t)pagedir);

  ctx->pagedir = pagedir;
  ctx->mirror = mirror;
  ctx->pagedir_paddr = pagedir_paddr;
  ctx->k_count = 0;
  ctx->ppage_region = ppage_region;

  /* setup page directory mirror */

  uint_fast16_t i = 0;
  for( i = 0 ; i < MMU_MIRROR_PAGE_SIZE ; i++)
    {
      pagedir[MMU_MIRROR_PDE + i].pte.valid = 1;
      pagedir[MMU_MIRROR_PDE + i].pte.type = 1;
      pagedir[MMU_MIRROR_PDE + i].pte.address = MMU_PADDR_TO_PTBA( mirror_paddr ) + i;	

      mirror[MMU_MIRROR_PDE + i].p4k.valid = 1;
      mirror[MMU_MIRROR_PDE + i].p4k.type = 0;
      mirror[MMU_MIRROR_PDE + i].p4k.cacheable = 1;
      mirror[MMU_MIRROR_PDE + i].p4k.writable = 1;
      mirror[MMU_MIRROR_PDE + i].p4k.executable = 0;
      mirror[MMU_MIRROR_PDE + i].p4k.user = 0;
      mirror[MMU_MIRROR_PDE + i].p4k.global = 0;
      mirror[MMU_MIRROR_PDE + i].p4k.address = MMU_PADDR_TO_PPN2( mirror_paddr ) + i;
    }
  mmu_update_k_context(ctx);

  return 0;

 err2:
  vmem_ops.vpage_free(pagedir,MMU_DIRECTORY_PAGE_SIZE);
 err:
  return -ENOMEM;
}

void mmu_context_destroy(void)
{
  struct mmu_context_s *ctx = mmu_context_get();
  uint_fast8_t i, j;

  /* refrdop all physical pages mapped in context */
  for (i = MMU_USER_START_PDE; i <= MMU_USER_END_PDE; i++)
    if (ctx->pagedir[i].pte.type && ctx->pagedir[i].pte.valid)
      {
	for (j = 0; j < MMU_TABLE_ENTRY; j++)
	  {
	    struct cpu_vcache_page4k_entry_s *e = mmu_vcache_get_vpage_entry(i, j);
	    
	    if (e->valid)
	      vmem_ops.ppage_refdrop( MMU_PPN2_TO_PADDR( e->address ) );
	  }
	vmem_ops.ppage_refdrop( MMU_PTBA_TO_PADDR( ctx->pagedir[i].pte.address ) );
      }
  
  /* switch to kernel context */
  mmu_context_switch_to(mmu_get_kernel_context());

  vmem_ops.vpage_free(ctx->pagedir, MMU_DIRECTORY_PAGE_SIZE);
  vmem_ops.vpage_free(ctx->mirror, MMU_MIRROR_PAGE_SIZE);
}

void mmu_context_switch_to(struct mmu_context_s *ctx)
{
  if(ctx!=mmu_context_get()){	
    if(ctx!=&mmu_k_context)
      mmu_update_k_context(ctx);
    
    mmu_vcache_set_pagedir(ctx->pagedir_paddr);
    
    CPU_LOCAL_SET(mmu_context_cur, ctx);
  }
}

/* Entry page attribute function*/

static error_t
mmu_vcache_alloc_pagetable(uintptr_t vaddr)
{
  struct mmu_context_s *ctx = mmu_context_get();
  struct cpu_vcache_pagetable_entry_s *pte;
  struct cpu_vcache_page4k_entry_s *p4k;
  paddr_t paddr;
  uint_fast16_t i = MMU_VADDR_TO_PDE( vaddr );


  /* allocate a new physical page for page table */
  if (vmem_ops.ppage_alloc(ctx->ppage_region, &paddr))
    return -ENOMEM;

  pte = (void*)(ctx->pagedir + i);
  p4k = (void*)(ctx->mirror + i);

  assert(!pte->valid);

  pte->valid=1;
  pte->type =1;
  pte->address = MMU_PADDR_TO_PTBA( paddr );
  
  p4k->valid=1;
  p4k->type=0;
  p4k->executable=0;
  p4k->user= 0;
  p4k->writable = 1;
  p4k->cacheable = 1;
  p4k->address = MMU_PADDR_TO_PPN2( paddr );

  /* clear page table */
  memset(mmu_vcache_get_vpage_entry(i, 0), 0, CONFIG_HEXO_MMU_PAGESIZE);

  if (! mmu_is_user_vaddr( vaddr ) )
    {
      if (ctx->k_count <= (i - MMU_KERNEL_START_PDE) )
	ctx->k_count = i - MMU_KERNEL_START_PDE + 1;

      if (ctx != &mmu_k_context)
	{
	  /* copy to real kernel context */
	  mmu_k_pagedir[i].pte = *pte;
	  mmu_k_mirror[i].p4k = *p4k;
	  mmu_k_context.k_count = ctx->k_count;
	}
    }

  return 0;
}

static inline struct cpu_vcache_page4k_entry_s *
mmu_vcache_get_vpage(uintptr_t vaddr)
{
  union cpu_vcache_directory_entry_s *pd;

  /* get pointer to appropiate pagedir. We must point to the real up
     to date kernel page directory here as we want to test the present
     bit ourself, we can't rely on update on exception mechanism. */
  if (vaddr >= MMU_KERNEL_START_ADDR)
    pd = mmu_k_pagedir;
  else
    pd = mmu_context_get()->pagedir;

  pd += ( MMU_VADDR_TO_PDE( vaddr ) );

  if (!pd->pte.valid)
    return NULL;

  /* return pointer to page entry mapped through mirror page table */
  return (void*) mmu_vcache_get_vpage_entry_vaddr( vaddr );
}

static inline struct cpu_vcache_page4k_entry_s *
mmu_vcache_alloc_vpage(uintptr_t vaddr)
{
  struct cpu_vcache_page4k_entry_s *e;

  e = mmu_vcache_get_vpage( vaddr );
  if( ( e == NULL ) && mmu_vcache_alloc_pagetable( vaddr ) )
    return NULL;
  
  return (void*) mmu_vcache_get_vpage_entry_vaddr( vaddr );
}

mmu_pageattr_t mmu_vpage_get_attr(uintptr_t vaddr)
{
  mmu_pageattr_t attr = 0;
  struct cpu_vcache_page4k_entry_s *e = mmu_vcache_get_vpage(vaddr);

  if (e != NULL)
    {
      if (e->valid)
	attr |= MMU_PAGE_ATTR_PRESENT;

      if (e->executable)
	attr |= MMU_PAGE_ATTR_X;
      
      if (e->writable)
	attr |= MMU_PAGE_ATTR_W;

      if (e->user)
	attr |= MMU_PAGE_ATTR_USERLEVEL;

      if (e->dirty)
	attr |= MMU_PAGE_ATTR_DIRTY;

      if (e->local_access || e->remote_access)
	attr |= MMU_PAGE_ATTR_ACCESSED;

      if (!e->cacheable)
	attr |= MMU_PAGE_ATTR_NOCACHE;
    }

  return attr;
}

error_t mmu_vpage_set(uintptr_t vaddr, paddr_t paddr, mmu_pageattr_t attr)
{
  struct cpu_vcache_page4k_entry_s *e = mmu_vcache_alloc_vpage(vaddr);

  if (e == NULL)
    return -ENOMEM;

  assert((paddr & 0xFFF) == 0);
  e->address = MMU_PADDR_TO_PPN2( paddr );
  
  e->valid = (attr & MMU_PAGE_ATTR_PRESENT) ? 1 : 0;
  e->executable = (attr & MMU_PAGE_ATTR_X) ? 1 : 0;
  e->writable = (attr & MMU_PAGE_ATTR_W) ? 1 : 0;
  e->user = (attr & MMU_PAGE_ATTR_USERLEVEL) ? 1 : 0;
  e->dirty = (attr & MMU_PAGE_ATTR_DIRTY) ? 1 : 0;
  e->local_access = (attr & MMU_PAGE_ATTR_ACCESSED) ? 1 : 0;
  e->cacheable = (attr & MMU_PAGE_ATTR_NOCACHE) ? 0 : 1;

  return 0;
}

/* set (logical or) and clear (logical nand) page attributes, may flush tlb */
void mmu_vpage_mask_attr(uintptr_t vaddr, mmu_pageattr_t setmask, mmu_pageattr_t clrmask)
{
  struct cpu_vcache_page4k_entry_s *e = mmu_vcache_get_vpage(vaddr);

  assert(e != NULL);
  assert((setmask & clrmask) == 0);

  if (setmask & MMU_PAGE_ATTR_PRESENT)
    e->valid = 1;
  if (clrmask & MMU_PAGE_ATTR_PRESENT)
    e->valid = 0;
	
  if (setmask & MMU_PAGE_ATTR_X)
    e->executable = 1;
  if (clrmask & MMU_PAGE_ATTR_X)
    e->executable = 0;
  
  if (setmask & MMU_PAGE_ATTR_W)
    e->writable = 1;
  if (clrmask & MMU_PAGE_ATTR_W)
    e->writable = 0;

  if (setmask & MMU_PAGE_ATTR_USERLEVEL)
    e->user = 1;
  if (clrmask & MMU_PAGE_ATTR_USERLEVEL)
    e->user = 0;

  if (setmask & MMU_PAGE_ATTR_DIRTY)
    e->dirty = 1;
  if (clrmask & MMU_PAGE_ATTR_DIRTY)
    e->dirty = 0;

  if (setmask & MMU_PAGE_ATTR_ACCESSED)
    e->local_access = 1;
  if (clrmask & MMU_PAGE_ATTR_ACCESSED)
    e->local_access = 0;

  if (setmask & MMU_PAGE_ATTR_NOCACHE)
    e->cacheable = 0;
  if (clrmask & MMU_PAGE_ATTR_NOCACHE)
    e->cacheable = 1;
}

paddr_t mmu_vpage_get_paddr(uintptr_t vaddr)
{
  struct cpu_vcache_page4k_entry_s *e = mmu_vcache_get_vpage(vaddr);

  assert(e != NULL);

  return MMU_PPN2_TO_PADDR( e->address );
}

