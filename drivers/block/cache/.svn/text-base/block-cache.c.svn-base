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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2009

*/

#include <hexo/types.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>
#include <hexo/lock.h>

#include <stdlib.h>
#include <string.h>

#include "block-cache.h"
#include "block-cache-private.h"

CONTAINER_FUNC(blk_cache_lba, BLIST, static inline, lba);
CONTAINER_KEY_FUNC(blk_cache_lba, BLIST, static inline, lba, lba);

CONTAINER_FUNC(blk_cache_age, BLIST, static inline, age);

/**************************************************************/

static void
block_cache_delent(struct block_cache_context_s *pv,
		   struct cache_entry_s *ent)
{
  lba_remove(&pv->lba_list, ent);
  age_remove(&pv->age_list, ent);

  assert(pv->blk_count > ent->count);
  pv->blk_count -= ent->count;

  free(ent->data);
  free(ent);
}

static void
block_cache_newent(struct block_cache_context_s *pv,
		   dev_block_lba_t lba,
		   size_t count, uint8_t **data)
{
  struct cache_entry_s *ent = malloc(sizeof(struct cache_entry_s));
  size_t i;

  if (!ent)
    return;

  ent->lba = lba;
  ent->count = count;
  ent->data = malloc(count * bs);

  if (!ent->data)
    {
      free(ent->data);
      return;
    }

  pv->blk_count += count;

  for (i = 0; i < count; i++)
    memcpy(ent->data + bs * i, data[i], bs);

  lba_insert_ascend(&pv->lba_list, ent);

  while (pv->blk_count > pv->blk_max)
    block_cache_delent(pv, lba_tail(&pv->lba_list));
}

/*
 * add new cache entries for data
 */

static DEVBLOCK_CALLBACK(block_cache_update)
{
  struct block_cache_rq_s *crq = rq_extra;
  struct block_cache_context_s *pv = crq->pv;
  size_t bs = pv->bp->blk_size;

  if (count > 0 && rq->progress >= 0)
    {
      dev_block_lba_t lba = rq->lba + rq->progress;
      uint8_t **data = rq->data + rq->progress;
      size_t i;

      LOCK_SPIN_IRQ(&dev->lock);

      struct block_cache_context_s *ent, *next;

      for (ent = lba_lookup_lowereq(&pv->lba_list, lba);
	   ent && ent->lba < lba + count; ent = next)
	{
	  size_t size;

	  next = lba_next(&pv->lba_list, ent);

	  if (ent->lba + ent->count < lba)
	    {
	      // rq              [------]
	      // ent   [------] 
	      continue;
	    }

	  if (ent->lba < lba)
	    {
	      // rq         [------]
	      // ent   [-----XX]

	      /* update cache entry for X */
	      size_t offset = lba - ent->lba;
	      size = __MIN(ent->count - offset, count);

	      for (i = 0; i < size; i++)
		memcpy(ent->data + bs * offset, data[i], bs);
      	    }

	  else
	    {
	      // rq         [-------]
	      // ent            [XXX----]
	      // new        [YYY]

	      if (ent->lba > lba)
		{
		  /* add new entry for Y */
		  size = ent->lba - lba;

		  if (!crq->update_only)
		    block_cache_newent(pv, lba, size, data);
		}

	      else
		{
		  /* update cache entry for X */
		  size_t offset = ent->lba - lba;
		  size = __MIN(count - offset, ent->count);

		  for (i = 0; i < size; i++)
		    memcpy(ent->data, data[offset + i], bs);
		}
	    }

	  count -= size;
	  lba += size;
	  data += size;
	}

      LOCK_RELEASE_IRQ(&dev->lock);
    }

  return crq->callback(rq, count, crq + 1);
}

DEVBLOCK_REQUEST(block_cache_request)
{
  struct block_cache_context_s	*pv = dev->drv_pv;
  struct block_cache_rq_s *crq = (void*)((uint8_t*)rq + pv->parent_rq_size);

  dev_block_lba_t lba = rq->lba + rq->progress;
  uint8_t **data = rq->data + rq->progress;
  size_t count = rq->count - rq->progress;

  assert(count > 0);

  /* check error here to avoid caching out of range blocks */
  if (lba + count > pv->bp->blk_count)
    {
      rq->progress = -ERANGE;
      rq->callback(rq, 0, crq + 1);
    }

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
  pv->rq_cnt++;
#endif

  crq->old_callback = rq->callback;
  crq->pv = pv;

  if ((rq->type & DEV_BLOCK_NOCACHE) || /* no-cache flaged request */
      (count > pv->max_rq_size) ||  /* request too big */
      (pv->sync && (rq->type & DEV_BLOCK_WRITE))) /* delayed write disabled */
    {
      /* do NOT cache this request */

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
      pv->nocacherq_cnt++;
#endif

      if (rq->type & DEV_BLOCK_WRITE)
	{
	  crq->update_only = 1;
	  rq->callback = block_cache_update;
	}

      return dev_block_request(dev->parent, rq);
    }

  LOCK_SPIN_IRQ(&dev->lock);

  if (rq->type & DEV_BLOCK_READ)
    {
      struct block_cache_context_s *ent_low =  lba_lookup_lowereq(&pv->lba_list, lba);
      struct block_cache_context_s *ent_high = lba_lookup_upper(&pv->lba_list, lba);
      assert(ent_low != ent_high);

      size_t i, bs = pv->bp->blk_size;

      if (ent_low && ent_low->lba + ent->count <= lba)
	ent_low = NULL;

      if (ent_high && ent_high->lba >= lba + count)
	ent_high = NULL;

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
      if (ent_low || ent_high)
	pv->hit_cnt++;
      else
	pv->miss_cnt++;
#endif

      /* fetch lower overlapping data from cache */
      if (ent_low)
	{
	  // rq               [-----
	  // low   [--------------

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
	  ent_low->hit_cnt++;
#endif
	  age_remove(&pv->age_list, ent_low);
	  age_push(&pv->age_list, ent_low);

	  size_t offset = lba - ent_low->lba;
	  size_t low_count = __MIN(ent_low->count - offset, count);

	  for (i = 0; i < low_count; i++)
	    memcpy(data[i], ent_low->data + (i + offset) * bs, bs);

	  /* strip request start */
	  rq->progress += low_count;
	}

      /* fetch upper overlapping data from cache */
      if (ent_high)
	{
	  // rq       [--------]
	  // high          [-----------

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
	  ent_high->hit_cnt++;
#endif
	  age_remove(&pv->age_list, ent_high);
	  age_push(&pv->age_list, ent_high);

	  size_t offset = ent_high->lba - lba;
	  size_t high_count = __MIN(count - offset, ent_high->count);

	  for (i = 0; i < high_count; i++)
	    memcpy(data[i + offset], ent_high->data + i * bs, bs);

	  /* strip request end */
	  rq->count -=  high_count;
	}
    }

  LOCK_RELEASE_IRQ(&dev->lock);

  if (rq->count > rq->progress)
    {
      /* read cache miss, read partial hit or write */
      crq->update_only = !!(rq->type & DEV_BLOCK_WRITE);
      rq->callback = block_cache_update;

      dev_block_request(dev->parent, rq);
    }
  else
    {
      rq->callback(rq, count, crq + 1);
    }
}

DEVBLOCK_GETRQSIZE(block_cache_getrqsize)
{
  struct block_cache_context_s	*pv = dev->drv_pv;

  return pv->parent_rq_size + sizeof(struct block_cache_rq_s);
}

DEVBLOCK_GETPARAMS(block_cache_getparams)
{
  struct block_cache_context_s	*pv = dev->drv_pv;

  return pv->bp;
}

DEV_CLEANUP(block_cache_cleanup)
{
  struct block_cache_context_s	*pv = dev->drv_pv;
  /* FIXME free context and device */

  while (pv->blk_count)
    block_cache_delent(pv, lba_tail(&pv->lba_list));
}

const struct driver_s	block_cache_drv;

DEV_CREATE(block_cache_create)
{
  size_t count;
  const struct dev_block_params_s *bp;
  struct block_partition_context_s *pv;

  /* check parent */
  if (parent == NULL)
    return -EINVAL;
  if (parent->drv == NULL || parent->drv->class != device_class_block)
    return -EINVAL;

  /* allocate device node and private data */
  if (!(pv = mem_alloc(sizeof(*pv), (mem_scope_sys))))
    return -ENOMEM;

  /* create and register device */
  struct device_s *dev = device_obj_new(&pv->dev);
  device_register(dev, parent, NULL);

  dev->drv_pv = pv;
  dev->irq = DEVICE_IRQ_INVALID;
  dev->drv = &block_cache_drv;

  pv->bp = dev_block_getparams(parent);
  pv->parent_rq_size = dev_block_getrqsize(dev->parent);

  blk_cache_lba_init(&pv->lba_list);
  blk_cache_age_init(&pv->age_list);

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
  pv->rq_cnt = 0;
  pv->nocache_rq_cnt = 0;
  pv->hit_cnt = 0;
  pv->miss_cnt = 0;
#endif

  pv->blk_max = CONFIG_DRIVER_BLOCK_CACHE_SIZE;
  pv->blk_count = 0;

  pv->max_rq_size = pv->blk_max / 8;

  pv->sync = 1;			/* FIXME */

  return 1;
}


const struct driver_s block_cache_drv =
{
  .class		= device_class_block,
  .f_create		= block_cache_create,
  .f_cleanup		= block_cache_cleanup,
  .f.blk = {
    .f_request		= block_cache_request,
    .f_getparams	= block_cache_getparams,
    .f_getrqsize	= block_cache_getrqsize,
  }
};

