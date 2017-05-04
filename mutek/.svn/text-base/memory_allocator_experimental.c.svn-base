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
              Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2009
*/

#include <mutek/memory_allocator.h>
#include <string.h>
#include <hexo/types.h>
#include <hexo/lock.h>
#include <hexo/endian.h>
#include <mutek/printk.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

#ifdef CONFIG_MUTEK_MEMALLOC_CRC
#include <crypto/crc32.h>
#endif

#ifdef CONFIG_HEXO_MMU
#include <hexo/mmu.h>
#endif

#ifdef CONFIG_SOCLIB_MEMCHECK
#include <arch/mem_checker.h>
#endif

/********************************************************/
/*********** Structure and global declaration ***********/

/** memory block header */
struct memory_allocator_header_s
{
  union
  {
    CONTAINER_ENTRY_TYPE(CLIST)	free_entry;
    struct
    {
      struct memory_allocator_region_s	*region;    
      void *free_null_marker;
    };
  };

  CONTAINER_ENTRY_TYPE(CLIST)	block_entry;

#ifdef CONFIG_MUTEK_MEMALLOC_CRC
  uint32_t crc;
#endif
};

static const size_t mem_hdr_size = sizeof (struct memory_allocator_header_s);

#ifdef CONFIG_MUTEK_MEMALLOC_CRC
static const size_t mem_hdr_size_no_crc = sizeof (struct memory_allocator_header_s) - sizeof (uint32_t);
#else
static const size_t mem_hdr_size_no_crc = sizeof (struct memory_allocator_header_s);
#endif

static const size_t	mem_hdr_size_align = ALIGN_VALUE_UP(sizeof (struct memory_allocator_header_s),
						      CONFIG_MUTEK_MEMALLOC_ALIGN);

CONTAINER_TYPE(free_list, CLIST, struct memory_allocator_header_s, free_entry);
CONTAINER_TYPE(block_list, CLIST, struct memory_allocator_header_s, block_entry);

#define MEMALLOC_SPLIT_SIZE	( mem_hdr_size_align + 16 + CONFIG_MUTEK_MEMALLOC_GUARD_SIZE * 2 )

/*************************/

/** memory region handler */
struct memory_allocator_region_s
{
  lock_t lock;
  free_list_root_t free_root;
  block_list_root_t block_root;
  size_t size;
#ifdef CONFIG_MUTEK_MEMALLOC_STATS
  size_t alloc_blocks;
  size_t free_size;
  size_t free_blocks;
#endif
};

static const size_t region_hdr_size = ALIGN_VALUE_UP ( sizeof( struct memory_allocator_region_s ),
						       CONFIG_MUTEK_MEMALLOC_ALIGN);

struct memory_allocator_region_s *default_region;

/********************************************************/
/******************* GPCT function **********************/

CONTAINER_FUNC(block_list, CLIST, static inline, block_list, block_entry);
CONTAINER_FUNC(free_list, CLIST, static inline, free_list, free_entry);

/********************************************************/

static inline
bool_t header_is_alloc(struct memory_allocator_header_s *hdr);

static inline
bool_t header_is_endblock(struct memory_allocator_header_s *hdr);


/********************************************************/
/*************** size and address translator ******************/

/*@this return the memory block header corresponding to the given user
  memory address*/

static inline
void *mem2hdr(void *mem)
{
  return (void*)((uintptr_t)mem - mem_hdr_size_align
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
		 - CONFIG_MUTEK_MEMALLOC_GUARD_SIZE
#endif
		 );
}

/*@this return the useable memory address corresponding to given
  memory block header*/

static inline
void *hdr2mem(void *hdr)
{
  return (void *)((uintptr_t)hdr + mem_hdr_size_align
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
		  + CONFIG_MUTEK_MEMALLOC_GUARD_SIZE
#endif  
		  );
}

/*@this converts usable memory block size to actual block size*/

static inline
size_t size_alloc2real(size_t size)
{
  return mem_hdr_size_align
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
    + CONFIG_MUTEK_MEMALLOC_GUARD_SIZE * 2
#endif
    + ALIGN_VALUE_UP(size, CONFIG_MUTEK_MEMALLOC_ALIGN);
}

/*@this converts actual block size to usable memory block size*/

static inline
size_t size_real2alloc(size_t size)
{
  return size
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
    - CONFIG_MUTEK_MEMALLOC_GUARD_SIZE * 2
#endif
    - mem_hdr_size_align;
}

/*@this return the real size of a memory block*/

static inline
size_t header_get_size( block_list_root_t *root, struct memory_allocator_header_s *hdr)
{
  assert( !header_is_endblock(hdr) );
  struct memory_allocator_header_s *next = block_list_next(root,hdr);
  return (size_t)( ( uintptr_t ) next - ( uintptr_t ) hdr);
}

/********************************************************/
/*************** Memory check function ******************/

/*@this set the header's crc field*/

static inline
void memory_allocator_crc_set(struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_CRC

  struct crypto_crc32_ctx_s crc;

  crypto_crc32_init(&crc);
  crypto_crc32_update(&crc, (uint8_t*)hdr, mem_hdr_size_no_crc );
  crypto_crc32_get(&crc, (uint8_t*)&hdr->crc);

#endif
}

/*@this check the header's crc field*/

static inline
void memory_allocator_crc_check(struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_CRC

  struct crypto_crc32_ctx_s crc;
  uint32_t result;

  crypto_crc32_init(&crc);
  crypto_crc32_update(&crc, (uint8_t*)hdr, mem_hdr_size_no_crc );
  crypto_crc32_get(&crc, (uint8_t*)&result);

  if ( memcmp(&hdr->crc, &result, 4) )
    {
      printk("Memory allocator error: Header crc check failed at %p\n", hdr);
      abort();
    }
#endif
}

/*@this apply function to a list, and update previous and next header's crc  */

#if defined (CONFIG_MUTEK_MEMALLOC_CRC)

 #define MEM_LIST_FUNCTION_REM(function, list_name, hdr) do{		\
  struct memory_allocator_header_s *_prev, *_next;			\
  _prev = list_name##_list_prev(&region->list_name##_root, hdr);	\
  _next = list_name##_list_next(&region->list_name##_root, hdr);	\
  list_name##_list_##function(&region->list_name##_root, hdr);	\
  if (_prev)memory_allocator_crc_set(_prev);					\
  if (_next)memory_allocator_crc_set(_next);				\
 }while(0)

 #define MEM_LIST_FUNCTION_INS(function, list_name, hdr, other) do{		\
  struct memory_allocator_header_s *_prev, *_next;			\
  list_name##_list_##function(&region->list_name##_root, other, hdr); \
  _prev = list_name##_list_prev(&region->list_name##_root, hdr);	\
  _next = list_name##_list_next(&region->list_name##_root, hdr);	\
  if (_prev)memory_allocator_crc_set(_prev);				\
  if (_next)memory_allocator_crc_set(_next);				\
 }while(0)

 #define MEM_LIST_FUNCTION_PUSH(function, list_name, hdr) do{		\
  struct memory_allocator_header_s *_prev, *_next;			\
  list_name##_list_##function(&region->list_name##_root, hdr);	\
  _prev = list_name##_list_prev(&region->list_name##_root, hdr);	\
  _next = list_name##_list_next(&region->list_name##_root, hdr);	\
  if (_prev)memory_allocator_crc_set(_prev);				\
  if (_next)memory_allocator_crc_set(_next);				\
 }while(0)


#else

 #define MEM_LIST_FUNCTION_REM(function, list_name, hdr) do{		\
  list_name##_list_##function(&region->list_name##_root, hdr);	\
 }while(0)

 #define MEM_LIST_FUNCTION_INS(function, list_name, hdr, other) do{	\
  list_name##_list_##function(&region->list_name##_root, other, hdr); \
 }while(0) 

 #define MEM_LIST_FUNCTION_PUSH(function, list_name, hdr) do{		\
  list_name##_list_##function(&region->list_name##_root, hdr);	\
 }while(0)

#endif


/*@this set two guard zone, at start and end point, in the given memory block*/

static inline
void memory_allocator_guard_set(size_t size, struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
  memset( (void*)( (uintptr_t) hdr + mem_hdr_size_align ),
	  0x55,
	  CONFIG_MUTEK_MEMALLOC_GUARD_SIZE );
  memset( (void*)( (uintptr_t)hdr + size - CONFIG_MUTEK_MEMALLOC_GUARD_SIZE),
	  0xaa,
	  CONFIG_MUTEK_MEMALLOC_GUARD_SIZE );
#endif
}

/*@this check the memory block's guard zone*/

static inline
void memory_allocator_guard_check(size_t size, struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_GUARD
  uint8_t r;

  r = memcstcmp( (void*)( (uintptr_t)hdr + mem_hdr_size_align),
		 0x55,
		CONFIG_MUTEK_MEMALLOC_GUARD_SIZE );
  if ( r )
    {
      printk("Memory allocator error: Header guard head zone check failed at %p\n", hdr);
      abort();
    }

  r = memcstcmp( (void*)( (uintptr_t)hdr + size - CONFIG_MUTEK_MEMALLOC_GUARD_SIZE),
		 0xaa,
		CONFIG_MUTEK_MEMALLOC_GUARD_SIZE );
  if ( r )
    {
      printk("Memory allocator error: Header guard tail zone check failed at %p\n", hdr);
      abort();
    }

#endif  
}

/*@this set the whole memory block with a special value*/

static inline
void memory_allocator_scramble_set(size_t size, struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_SCRAMBLE
  if (!header_is_alloc(hdr))
    {
      memset( hdr2mem(hdr), 0xa5 , size_real2alloc(size) );
    }
  else
    {
      memset( hdr2mem(hdr), 0x5a , size_real2alloc(size) );
    }

#endif
}

/*@this check if the memory block isn't corrupt*/

static inline
void memory_allocator_scramble_check(size_t size, struct memory_allocator_header_s *hdr)
{
#ifdef CONFIG_MUTEK_MEMALLOC_SCRAMBLE
  int_fast8_t res = 0;

  if (!header_is_alloc(hdr))
    {
      res = memcstcmp( hdr2mem(hdr), 0xa5 , size_real2alloc(size) );
      if (res)
	{
	  printk("Memory allocator error: scramble check failed at %p\n", hdr);
	  abort();
	}
    }
#endif
}

static inline
void disable_memchecker()
{
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_disable(SOCLIB_MC_CHECK_REGIONS);
#endif
}

static inline
void enable_memchecker()
{
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_enable(SOCLIB_MC_CHECK_REGIONS);
#endif
}

static inline
void memchecker_set_alloc(size_t size, void *hdr)
{
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_region_status(hdr2mem(hdr),
				 size_real2alloc(size),
				 SOCLIB_MC_REGION_ALLOC);
#endif
}

static inline
void memchecker_set_free(size_t size, void *hdr)
{
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_region_status(hdr2mem(hdr),
				 size_real2alloc(size),
				 SOCLIB_MC_REGION_FREE);
#endif
}

static inline
bool_t header_is_alloc(struct memory_allocator_header_s *hdr)
{
  return ( hdr->free_null_marker == NULL );
}

static inline
void header_set_alloc(struct memory_allocator_header_s *hdr, struct memory_allocator_region_s *region )
{
  hdr->free_null_marker = NULL;
  hdr->region = region;
}

static inline
bool_t header_is_endblock(struct memory_allocator_header_s *hdr)
{
  return ( hdr->region == NULL );
}

static inline
void header_set_endblock(struct memory_allocator_header_s *hdr)
{
  hdr->region = NULL;
  hdr->free_null_marker = NULL;
}

/********************************************************/
/********* Non-algorithmic internal function ************/

static inline
void update_region_stats(struct memory_allocator_region_s *region,
		  ssize_t size, uint32_t free_block, uint32_t alloc_block)
{
#ifdef CONFIG_MUTEK_MEMALLOC_STATS
  region->free_size += size;
  region->free_blocks += free_block;
  region->alloc_blocks += alloc_block;
#endif
} 

static inline
void init_region_stats(struct memory_allocator_region_s *region,
		  ssize_t size, uint32_t free_block, uint32_t alloc_block)
{
#ifdef CONFIG_MUTEK_MEMALLOC_STATS
  region->free_size = size;
  region->free_blocks = free_block;
  region->alloc_blocks = alloc_block;
#endif
} 

static inline
struct memory_allocator_header_s *
memory_allocator_nolock_extend(struct memory_allocator_region_s *region, void *start, size_t size)
{
  struct memory_allocator_header_s *hdr = start;
  struct memory_allocator_header_s *hdr_end = start + size - mem_hdr_size_align;
  size_t hdr_size = (void*)hdr - (void*)hdr_end;

  assert( hdr != NULL );
  
  MEM_LIST_FUNCTION_PUSH(pushback, block, hdr);
  MEM_LIST_FUNCTION_PUSH(push, free, hdr);
  
  header_set_endblock(hdr_end);
  MEM_LIST_FUNCTION_PUSH(pushback, block, hdr_end);

  memory_allocator_scramble_set(hdr_size, hdr);
  memory_allocator_guard_set(hdr_size, hdr);

  memory_allocator_crc_set(hdr);
  memory_allocator_crc_set(hdr_end);

  region->size += size;

  update_region_stats(region, size, 1, 0);

  return hdr;
}

# ifdef CONFIG_HEXO_MMU
static inline struct memory_allocator_header_s *
mmu_region_nolock_extend(struct memory_allocator_region_s *region, size_t size)
{
  return memory_allocator_nolock_extend(region, vmem_ops.vpage_alloc(initial_ppage_region, size), size * CONFIG_HEXO_MMU_PAGESIZE);
}
# endif


/********************************************************/
/*********** algorithmic internal function **************/

#if defined(CONFIG_MUTEK_MEMALLOC_ALGO_FIRSTFIT)

/* FIRST FIT allocation algorithm */

static inline struct memory_allocator_header_s *
memory_allocator_candidate(struct memory_allocator_region_s *region, size_t size)
{
  CONTAINER_FOREACH(free_list, CLIST, &region->free_root,
  {
    if ( header_get_size(&region->block_root, item) >= size)
      return item;
  });

  return NULL;
}

#elif defined(CONFIG_MUTEK_MEMALLOC_ALGO_BESTFIT)

/* BEST FIT allocation algorithm */

static inline struct memory_allocator_header_s *
memory_allocator_candidate(struct memory_allocator_region_s *region, size_t size)
{
  struct memory_allocator_header_s	*best = NULL;
  size_t item_size, best_size;
  CONTAINER_FOREACH(free_list, CLIST, &region->free_root,
  {
    item_size = header_get_size(&region->block_root, item);
    if ( item_size >= size &&
	((best == NULL) || (best_size > item_size)))
      {
	best = item;
	best_size = item_size;
      }
  });

  return best;
}

#endif

static inline 
struct memory_allocator_header_s *get_hdr_for_rsv(struct memory_allocator_region_s *region, void *start, size_t size)
{
  CONTAINER_FOREACH(free_list, CLIST, &region->free_root,
  {
    if (((void *)(item + 1) <= start ) &&
	( ((void*)item + header_get_size(&region->block_root, item)) >= (start + size) ))
      return item;
  });

  return NULL;
}

/********************************************************/
/******************** API function **********************/

struct memory_allocator_header_s *
memory_allocator_extend(struct memory_allocator_region_s *region, void *start, size_t size)
{
    struct memory_allocator_header_s *h;

    CPU_INTERRUPT_SAVESTATE_DISABLE;
    lock_spin(&region->lock);
    h = memory_allocator_nolock_extend(region, start, size);
    lock_release(&region->lock);
    CPU_INTERRUPT_RESTORESTATE;

    return h;
}

void *memory_allocator_resize(void *address, size_t size)
{
  struct memory_allocator_header_s *hdr = mem2hdr(address);

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  disable_memchecker();

  struct memory_allocator_region_s *region = hdr->region;

  lock_spin(&region->lock);
  
  assert( header_is_alloc(hdr) && !header_is_endblock(hdr) );

  size = size_alloc2real(size);
  
  size_t hdr_size = header_get_size(&region->block_root, hdr);

  ssize_t diff = (size - hdr_size);
  
  if (! diff )
    goto done;
  
  struct memory_allocator_header_s *next = block_list_next(&region->block_root, hdr);
  
  if ( next && !header_is_alloc(next) )
    {
      size_t next_size = header_get_size(&region->block_root, next);
      
      if (next_size < diff)
	{
	  address = NULL;
	}
      else
	{
	  MEM_LIST_FUNCTION_REM(remove, free, next);
	  MEM_LIST_FUNCTION_REM(remove, block, next);
	  
	  memchecker_set_free(hdr_size, hdr);

	  if (next_size >= diff + MEMALLOC_SPLIT_SIZE)
	    {

	      memory_allocator_guard_set(size, hdr);

	      next = (void*)((uintptr_t)hdr + size);
	      next_size -= diff;

	      MEM_LIST_FUNCTION_INS(insert_post, block, next, hdr);
	      MEM_LIST_FUNCTION_PUSH(push, free, next);

	      memory_allocator_crc_set(next);
	      memory_allocator_guard_set(next_size, next);
	      memory_allocator_scramble_set(next_size, next);

	      update_region_stats(region, -diff, 1, 0);
	      memchecker_set_alloc(size, hdr);
	    }
	  else
	    {
	      memchecker_set_alloc( hdr_size + next_size, hdr);
	    }
	  update_region_stats(region, -next_size, -1, 0);

	  memory_allocator_crc_set(hdr);
	}
    }
  else
    {
      if (size >= hdr_size)
	{
	  address = NULL;
	}
      else
	{
	  if (size + MEMALLOC_SPLIT_SIZE <= hdr_size)
	    {
	      memory_allocator_guard_set(size, hdr);

	      next = (void*)((uintptr_t)hdr + size);
	      size_t next_size = -diff;
	      MEM_LIST_FUNCTION_INS(insert_post, block, next, hdr);
	      MEM_LIST_FUNCTION_PUSH(push, free, next);

	      memory_allocator_crc_set(next);
	      memory_allocator_guard_set(next_size, next);
	      memory_allocator_scramble_set(next_size, next);

	      update_region_stats(region, -diff, 1, 0);
	      memchecker_set_free(next_size, next);
	    }
	}
    }

 done:
  enable_memchecker();
  lock_release(&region->lock);
  CPU_INTERRUPT_RESTORESTATE;

  return address;
}

void *memory_allocator_pop(struct memory_allocator_region_s *region, size_t size)
{
  struct memory_allocator_header_s	*hdr;

  size = size_alloc2real(size);

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  lock_spin(&region->lock);

  disable_memchecker();

  /* find suitable free block */
  if ((hdr = memory_allocator_candidate(region, size)) 
#ifdef CONFIG_HEXO_MMU
      || (hdr = mmu_region_nolock_extend(region, (size / CONFIG_HEXO_MMU_PAGESIZE) + 1))
#endif
      )
    {
      assert ( !header_is_alloc(hdr) );
      size_t hdr_size = header_get_size(&region->block_root, hdr);

      assert ( size <= hdr_size );

      memory_allocator_scramble_check(hdr_size, hdr);
      memory_allocator_guard_check(hdr_size, hdr);
      memory_allocator_crc_check(hdr);
      
      MEM_LIST_FUNCTION_REM(remove, free, hdr);
      header_set_alloc(hdr, region);
      
      /* check if split is needed */
      if (hdr_size >= size + MEMALLOC_SPLIT_SIZE)
	{
	  struct memory_allocator_header_s	*next = (void*)((uintptr_t)hdr + size);

	  MEM_LIST_FUNCTION_INS(insert_post, block, next, hdr);
	  MEM_LIST_FUNCTION_PUSH(push, free, next);
	    
	  memory_allocator_crc_set(next);
	  
	  size_t next_size = header_get_size(&region->block_root, next); 
	  memory_allocator_scramble_set(next_size, next);
	  memory_allocator_guard_set(next_size, next);

	  memory_allocator_guard_set(size, hdr);
          update_region_stats(region, 0, 1, 0);
	}

      update_region_stats(region, 0, -1, 1);

      memory_allocator_crc_set(hdr);
      memory_allocator_scramble_set(size, hdr);

      memchecker_set_alloc(size, hdr);
    }

  enable_memchecker();

  lock_release(&region->lock);
  CPU_INTERRUPT_RESTORESTATE;

  return hdr != NULL ? (uint8_t*)hdr2mem(hdr) : NULL;
}

void memory_allocator_push(void *address)
{
  struct memory_allocator_header_s	*next, *prev, *hdr = mem2hdr(address);

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  disable_memchecker();

  struct memory_allocator_region_s	*region = hdr->region;

  lock_spin(&region->lock);

  assert( header_is_alloc(hdr) && !header_is_endblock(hdr) );

  size_t size = header_get_size(&region->block_root, hdr);

  memory_allocator_guard_check(size, hdr);
  memory_allocator_crc_check(hdr);

  next = block_list_next(&region->block_root, hdr);
  if (next)
    memory_allocator_crc_check(next);

  prev = block_list_prev(&region->block_root, hdr);
  if (prev)  
    memory_allocator_crc_check(prev);

  memchecker_set_free(size, hdr);

  if ( next && ! header_is_alloc(next) )
    {
      MEM_LIST_FUNCTION_REM(remove, free, next);
      MEM_LIST_FUNCTION_REM(remove, block, next);
      update_region_stats(region, 0, -1, 0);
    }

  if ( prev && ! header_is_alloc(prev) )
    {
      MEM_LIST_FUNCTION_REM(remove, block, hdr);
      memory_allocator_crc_set(prev);
      hdr = prev;
    }
  else
    {
      MEM_LIST_FUNCTION_PUSH(push, free, hdr);
      memory_allocator_crc_set(hdr);
      update_region_stats(region, 0, 1, 0);
    }

  size = header_get_size(&region->block_root, hdr);
  memory_allocator_scramble_set(size, hdr);

  update_region_stats(region, 0, 0, -1);

  enable_memchecker();
  lock_release(&region->lock);
  CPU_INTERRUPT_RESTORESTATE;
}

size_t memory_allocator_getsize(void *ptr)
{
  size_t result;

  struct memory_allocator_header_s *hdr = mem2hdr(ptr);

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  disable_memchecker();
  lock_spin(& (hdr->region->lock) );

  assert( header_is_alloc(hdr) && !header_is_endblock(hdr) );

  size_t size = header_get_size(&hdr->region->block_root, hdr);

  memory_allocator_guard_check(size, hdr);
  memory_allocator_crc_check(hdr);

  result = size_real2alloc(size);

  lock_release(& (hdr->region->lock) );
  enable_memchecker();
  CPU_INTERRUPT_RESTORESTATE;

  return result;
}

void *memory_allocator_reserve(struct memory_allocator_region_s *region, void *start, size_t size)
{
  struct memory_allocator_header_s	*hdr;

  size = ALIGN_VALUE_UP(size, CONFIG_MUTEK_MEMALLOC_ALIGN);

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  lock_spin(&region->lock);

  disable_memchecker();

  /* test if the reserve memory space is not already used and return the header which contains the reserve space*/
  if ((hdr = get_hdr_for_rsv(region, start, size)))
    {
      assert( !header_is_alloc(hdr) );

      size_t hdr_size = header_get_size(&region->block_root, hdr);

      assert( hdr_size >= size );

      memory_allocator_scramble_check(hdr_size, hdr);
      memory_allocator_guard_check(hdr_size, hdr);
      memory_allocator_crc_check(hdr);
      
      /* check if split is needed FIXME: split after with no split before*/
      if (MEMALLOC_SPLIT_SIZE < ( (uintptr_t)start - (uintptr_t)hdr) )
	{
	  struct memory_allocator_header_s  *prev = hdr;

	  hdr = mem2hdr(start);

	  size_t prev_size = hdr - prev;

	  header_set_alloc(hdr, region);

	  MEM_LIST_FUNCTION_INS(insert_post, block, hdr, prev);

	  /*check is split is needed after the reserve space*/
	  if( MEMALLOC_SPLIT_SIZE <= ( hdr_size - size - prev_size ) )
	    {
	      struct memory_allocator_header_s *next = ((void*)hdr + size);
	      
	      MEM_LIST_FUNCTION_INS(insert_post, block, next, hdr);
	      MEM_LIST_FUNCTION_PUSH(push, free, next); 
	      
	      size_t next_size = hdr_size - size - prev_size;

	      memory_allocator_crc_set(next);
	      memory_allocator_guard_set(next_size, next);

	      update_region_stats(region, 0, 1, 0);
	    }

	  memory_allocator_crc_set(prev);
	  memory_allocator_guard_set(prev_size, prev);

	  memory_allocator_crc_set(hdr);
	  memory_allocator_scramble_set(size, hdr);
	  memory_allocator_guard_set(size, hdr);

	}
      else
	{
	  MEM_LIST_FUNCTION_REM(remove, free, hdr);

	  memory_allocator_crc_set(hdr);      
	  memory_allocator_scramble_set(hdr_size, hdr);
	  update_region_stats(region, 0, -1, 0);
	}
      
      hdr_size = header_get_size(&region->block_root, hdr);
      update_region_stats(region, hdr_size, 0, 1);

      memchecker_set_alloc(hdr_size, hdr);
    }

  enable_memchecker();

  lock_release(&region->lock);
  CPU_INTERRUPT_RESTORESTATE;

  return hdr;
}

struct memory_allocator_region_s *
memory_allocator_init(struct memory_allocator_region_s *container_region,
			   void *start, void *end)
{
  struct memory_allocator_region_s *region;
  struct memory_allocator_header_s	*hdr, *hdr_end;

    start = ALIGN_ADDRESS_UP(start, CONFIG_MUTEK_MEMALLOC_ALIGN);
    end = ALIGN_ADDRESS_LOW(end, CONFIG_MUTEK_MEMALLOC_ALIGN);

  if (container_region == NULL)
    {
      region = start;
      hdr = start + region_hdr_size;
    }
  else
    {
      region = memory_allocator_pop(container_region, sizeof (struct memory_allocator_region_s));
      hdr = start;
    }

  hdr_end = end - mem_hdr_size_align;

  size_t size = (uintptr_t)hdr_end - (uintptr_t)hdr;
  
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  disable_memchecker();


  /* init region struct */

  lock_init(&region->lock);
  region->size = size;
  block_list_init(&region->block_root);
  free_list_init(&region->free_root);
  
  init_region_stats(region, size, 1, 0);
  
  /* push initials blocks */
  
  assert(size > mem_hdr_size_align);
  
  header_set_endblock(hdr_end);

  block_list_push(&region->block_root, hdr_end);  

  block_list_push(&region->block_root, hdr);
  free_list_push(&region->free_root, hdr);

  memory_allocator_crc_set(hdr_end);  
  memory_allocator_crc_set(hdr);

  size_t size = header_get_size(&region->block_root, hdr);
  memory_allocator_scramble_set(size, hdr);
  memory_allocator_guard_set(size, hdr);

  memchecker_set_free(size, hdr);
  enable_memchecker();
  CPU_INTERRUPT_RESTORESTATE;
  
  return region;
}

void memory_allocator_region_check(struct memory_allocator_region_s *region)
{
  size_t header_size = 0;
  size_t header_count = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  lock_spin(&region->lock);
  disable_memchecker();

  CONTAINER_FOREACH(block_list, CLIST, &region->block_root,
  {
    if (! header_is_endblock(item) )
      {
	size_t hdr_size = header_get_size(&region->block_root, item);
	header_size += hdr_size;
	header_count++;

	memory_allocator_guard_check(hdr_size, item);
	memory_allocator_scramble_check(hdr_size, item);
      }
    memory_allocator_crc_check(item);
  });

  printk("Memory allocator: Check done on %d headers, with %d total size\n", header_count, header_size);

  enable_memchecker();
  lock_release(&region->lock);
  CPU_INTERRUPT_RESTORESTATE;
}

error_t memory_allocator_stats(struct memory_allocator_region_s *region,
			size_t *alloc_blocks,
			size_t *free_size,
			size_t *free_blocks)
{
#ifdef CONFIG_MUTEK_MEMALLOC_STATS

  if (alloc_blocks)
      *alloc_blocks = region->alloc_blocks;

  if (free_size)
      *free_size = region->free_size;

  if (free_blocks)
      *free_blocks = region->free_blocks;

  return 0;
#else
  return -ENOTSUP;
#endif
}
