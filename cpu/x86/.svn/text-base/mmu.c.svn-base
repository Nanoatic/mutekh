
#include <hexo/mmu.h>
#include <hexo/local.h>
#include <hexo/endian.h>
#include <string.h>
#include <assert.h>

MMU_X86_ALIGN cpu_x86_page_entry_t mmu_k_pagedir[1024];
MMU_X86_ALIGN cpu_x86_page_entry_t mmu_k_mirror[1024];
MMU_X86_ALIGN struct mmu_context_s mmu_k_context;

CPU_LOCAL struct mmu_context_s *mmu_context_cur = &mmu_k_context;

struct vmem_page_region_s *initial_ppage_region;

struct mmu_vmem_ops_s vmem_ops;

inline bool_t mmu_is_user_vaddr(uintptr_t vaddr)
{
  return (vaddr >= CONFIG_HEXO_MMU_USER_START && vaddr < CONFIG_HEXO_MMU_USER_END);
}

void mmu_global_init()
{
  uint_fast16_t	i;

  mmu_k_context.k_count = MMU_INITIAL_PDE;
  mmu_k_context.pagedir = mmu_k_pagedir;
  mmu_k_context.mirror = mmu_k_mirror;
  mmu_k_context.pagedir_paddr = (uintptr_t)mmu_k_pagedir;

  memset(mmu_k_pagedir, 0, sizeof (mmu_k_pagedir));
  memset(mmu_k_mirror, 0, sizeof (mmu_k_mirror));

  /* identity map memory space with 4Mb pages */
  for (i = 0; i < MMU_INITIAL_PDE; i++)
    {
      mmu_k_pagedir[i].p4m.present = 1;
      mmu_k_pagedir[i].p4m.writable = 1;
      mmu_k_pagedir[i].p4m.pagsize4m = 1;
      mmu_k_pagedir[i].p4m.global = 1;
      mmu_k_pagedir[i].p4m.address = i;
    }

  mmu_k_pagedir[MMU_MIRROR_PDE].pte.present = 1;
  mmu_k_pagedir[MMU_MIRROR_PDE].pte.writable = 1;
  mmu_k_pagedir[MMU_MIRROR_PDE].pte.address = ((uintptr_t)mmu_k_mirror) >> 12;

  mmu_k_mirror[MMU_MIRROR_PDE].p4k.present = 1;
  mmu_k_mirror[MMU_MIRROR_PDE].p4k.writable = 1;
  mmu_k_mirror[MMU_MIRROR_PDE].p4k.address = ((uintptr_t)mmu_k_mirror) >> 12;
}

void mmu_cpu_init()
{
  uint32_t tmp;

  mmu_x86_set_pagedir(mmu_k_context.pagedir_paddr);

  asm volatile (
		"mov %%cr4, %0		\n"
		"orl  $0x00000090, %0	\n" /* set PSE and PGE */
		"andl $0xffffffdf, %0	\n" /* clear PAE */
		"mov %0, %%cr4		\n"

		"mov %%cr0, %0		\n"
		"orl  $0x80000000, %0	\n" /* enable paging */
		"mov %0, %%cr0		\n"
		: "=r" (tmp)
		);
}

struct mmu_context_s * mmu_get_kernel_context()
{
  return &mmu_k_context;
}

static void mmu_x86_update_k_context(struct mmu_context_s *ctx)
{
  uint_fast16_t diff;

  if ((diff = mmu_k_context.k_count - ctx->k_count))
    {
      /* copy kernel part pagedir to context pagedir */
      memcpy(ctx->pagedir + ctx->k_count,
	     mmu_k_context.pagedir + ctx->k_count,
	     diff * sizeof(struct cpu_x86_pagetable_entry_s));

      memcpy(ctx->mirror + ctx->k_count,
	     mmu_k_context.mirror + ctx->k_count,
	     diff * sizeof(struct cpu_x86_page4k_entry_s));

      ctx->k_count += diff;
    }
}

error_t mmu_context_init(struct mmu_context_s *ctx)
{
  cpu_x86_page_entry_t	*pagedir, *mirror;

  if ((pagedir = vmem_ops.vpage_alloc(initial_ppage_region, 1)) == NULL)
    goto err;

  if ((mirror = vmem_ops.vpage_alloc(initial_ppage_region, 1)) == NULL)
    goto err2;

  memset(pagedir, 0, CONFIG_HEXO_MMU_PAGESIZE);
  memset(mirror, 0, CONFIG_HEXO_MMU_PAGESIZE);

  uint32_t mirror_paddr = mmu_vpage_get_paddr((uintptr_t)mirror);
  uint32_t pagedir_paddr = mmu_vpage_get_paddr((uintptr_t)pagedir);

  ctx->pagedir = pagedir;
  ctx->mirror = mirror;
  ctx->pagedir_paddr = pagedir_paddr;
  ctx->k_count = 0;

  /* setup page directory mirror */
  pagedir[MMU_MIRROR_PDE].pte.present = 1;
  pagedir[MMU_MIRROR_PDE].pte.writable = 1;
  pagedir[MMU_MIRROR_PDE].pte.address = mirror_paddr >> 12;

  mirror[MMU_MIRROR_PDE].p4k.present = 1;
  mirror[MMU_MIRROR_PDE].p4k.writable = 1;
  mirror[MMU_MIRROR_PDE].p4k.address = mirror_paddr >> 12;

  mmu_x86_update_k_context(ctx);

  return 0;

 err2:
  vmem_ops.vpage_free(pagedir, 1);
 err:
  return -ENOMEM;
}

static inline struct cpu_x86_page4k_entry_s *
mmu_x86_get_vpage_entry(uint_fast16_t pde, uint_fast16_t pte)
{
  return (void*)(uintptr_t)(MMU_MIRROR_ADDR | (pde << 12) | (pte << 2));
}

void mmu_context_destroy(void)
{
  struct mmu_context_s *ctx = mmu_context_get();
  uint_fast8_t i, j;

  /* refrdop all physical pages mapped in context */
  for (i = MMU_USER_START_PDE; i <= MMU_USER_END_PDE; i++)
    if (ctx->pagedir[i].pte.present)
      for (j = 0; j < 1024; j++)
	{
	  struct cpu_x86_page4k_entry_s *e = mmu_x86_get_vpage_entry(i, j);

	  if (e->present)
	    vmem_ops.ppage_refdrop(e->address << 12);
	}

  /* switch to kernel context */
  mmu_x86_set_pagedir(mmu_k_context.pagedir_paddr);

  vmem_ops.vpage_free(ctx->pagedir, 1);
  vmem_ops.vpage_free(ctx->mirror, 1);
}

void mmu_context_switch_to(struct mmu_context_s *ctx)
{
  if(ctx!=mmu_context_get()){	
    if(ctx!=&mmu_k_context)
      mmu_x86_update_k_context(ctx);
    
    mmu_x86_set_pagedir(ctx->pagedir_paddr);
    
    CPU_LOCAL_SET(mmu_context_cur, ctx);
  }
}

static error_t
mmu_x86_alloc_pagetable(uintptr_t vaddr)
{
  struct mmu_context_s *ctx;
  struct cpu_x86_pagetable_entry_s *pte;
  struct cpu_x86_page4k_entry_s *p4k;
  uintptr_t paddr;
  uint_fast16_t i = vaddr >> 22;

  assert(i <= MMU_USER_END_PDE);
  assert(i >= MMU_INITIAL_PDE);

  /* allocate a new physical page for page table */
  if (vmem_ops.ppage_alloc(initial_ppage_region, &paddr))
    return -ENOMEM;

  ctx = mmu_context_get();
  pte = (void*)(ctx->pagedir + i);
  p4k = (void*)(ctx->mirror + i);

  assert(!pte->present);

  pte->present = 1;
  pte->writable = 1;
  pte->userlevel = mmu_is_user_vaddr(vaddr);
  pte->address = paddr >> 12;

  p4k->present = 1;
  p4k->writable = 1;
  p4k->address = paddr >> 12;

  /* clear page table */
  memset(mmu_x86_get_vpage_entry(i, 0), 0, CONFIG_HEXO_MMU_PAGESIZE);

  if (!mmu_is_user_vaddr(vaddr))
    {
      if (ctx->k_count <= i)
	ctx->k_count = i + 1;

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

static struct cpu_x86_page4k_entry_s *
mmu_x86_get_vpage(uintptr_t vaddr)
{
  union cpu_x86_page_entry_s *pd;

  /* get pointer to appropiate pagedir. We must point to the real up
     to date kernel page directory here as we want to test the present
     bit ourself, we can't rely on update on exception mechanism. */
  if (mmu_is_user_vaddr(vaddr))
    pd = mmu_context_get()->pagedir;
  else
    pd = mmu_k_pagedir;

  pd += (vaddr >> 22);

  if (!pd->pte.present)
    return NULL;

  /* return pointer to page entry mapped through mirror page table */
  return (void*)(uintptr_t)(MMU_MIRROR_ADDR | ((vaddr & 0xfffffc00) >> 10));
}

static struct cpu_x86_page4k_entry_s *
mmu_x86_alloc_vpage(uintptr_t vaddr)
{
  union cpu_x86_page_entry_s *pd;

  if (mmu_is_user_vaddr(vaddr))
    pd = mmu_context_get()->pagedir;
  else
    pd = mmu_k_pagedir;

  pd += (vaddr >> 22);

  if (!pd->pte.present && mmu_x86_alloc_pagetable(vaddr))
    return NULL;

  return (void*)(uintptr_t)(MMU_MIRROR_ADDR | ((vaddr & 0xfffffc00) >> 10));
}

mmu_pageattr_t mmu_vpage_get_attr(uintptr_t vaddr)
{
  mmu_pageattr_t attr = 0;
  struct cpu_x86_page4k_entry_s *e = mmu_x86_get_vpage(vaddr);

  if (e != NULL)
    {
      if (e->present)
	attr |= MMU_PAGE_ATTR_PRESENT | MMU_PAGE_ATTR_R | MMU_PAGE_ATTR_X;

      if (e->writable)
	attr |= MMU_PAGE_ATTR_W;

      if (e->userlevel)
	attr |= MMU_PAGE_ATTR_USERLEVEL;

      if (e->dirty)
	attr |= MMU_PAGE_ATTR_DIRTY;

      if (e->accessed)
	attr |= MMU_PAGE_ATTR_ACCESSED;

      if (e->nocache)
	attr |= MMU_PAGE_ATTR_NOCACHE;
    }

  return attr;
}

error_t mmu_vpage_set(uintptr_t vaddr, uintptr_t paddr, mmu_pageattr_t attr)
{
  struct cpu_x86_page4k_entry_s *e = mmu_x86_alloc_vpage(vaddr);

  if (e == NULL)
    return -ENOMEM;

  assert((paddr & 0x3ff) == 0);
  e->address = paddr >> 12;

  e->present = (attr & MMU_PAGE_ATTR_PRESENT) ? 1 : 0;
  e->writable = (attr & MMU_PAGE_ATTR_W) ? 1 : 0;
  e->userlevel = (attr & MMU_PAGE_ATTR_USERLEVEL) ? 1 : 0;
  e->dirty = (attr & MMU_PAGE_ATTR_DIRTY) ? 1 : 0;
  e->accessed = (attr & MMU_PAGE_ATTR_ACCESSED) ? 1 : 0;
  e->nocache = (attr & MMU_PAGE_ATTR_NOCACHE) ? 1 : 0;

  return 0;
}

/* set (logical or) and clear (logical nand) page attributes, may flush tlb */
void mmu_vpage_mask_attr(uintptr_t vaddr, mmu_pageattr_t setmask, mmu_pageattr_t clrmask)
{
  struct cpu_x86_page4k_entry_s *e = mmu_x86_get_vpage(vaddr);

  assert(e != NULL);
  assert((setmask & clrmask) == 0);

  if (setmask & MMU_PAGE_ATTR_PRESENT)
    e->present = 1;
  if (clrmask & MMU_PAGE_ATTR_PRESENT)
    e->present = 0;

  if (setmask & MMU_PAGE_ATTR_W)
    e->writable = 1;
  if (clrmask & MMU_PAGE_ATTR_W)
    e->writable = 0;

  if (setmask & MMU_PAGE_ATTR_USERLEVEL)
    e->userlevel = 1;
  if (clrmask & MMU_PAGE_ATTR_USERLEVEL)
    e->userlevel = 0;

  if (setmask & MMU_PAGE_ATTR_DIRTY)
    e->dirty = 1;
  if (clrmask & MMU_PAGE_ATTR_DIRTY)
    e->dirty = 0;

  if (setmask & MMU_PAGE_ATTR_ACCESSED)
    e->accessed = 1;
  if (clrmask & MMU_PAGE_ATTR_ACCESSED)
    e->accessed = 0;

  if (setmask & MMU_PAGE_ATTR_NOCACHE)
    e->nocache = 1;
  if (clrmask & MMU_PAGE_ATTR_NOCACHE)
    e->nocache = 0;
}

uintptr_t mmu_vpage_get_paddr(uintptr_t vaddr)
{
  struct cpu_x86_page4k_entry_s *e = mmu_x86_get_vpage(vaddr);

  assert(e != NULL);

  return e->address << 12;
}


