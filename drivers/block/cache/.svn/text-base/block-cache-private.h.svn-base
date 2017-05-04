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

#ifndef BLOCK_CACHE_PRIVATE_H_
#define BLOCK_CACHE_PRIVATE_H_

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_blist.h>

#include <hexo/types.h>
#include <hexo/lock.h>

struct cache_entry_s
{
  CONTAINER_ENTRY_TYPE(BLIST) list_entry_lba;
  CONTAINER_ENTRY_TYPE(BLIST) list_entry_age;

  /** lba of first cached block */
  dev_block_lba_t lba;
  /** number of cached blocks, may be 0 if entry has been disabled */
  size_t count;
  /** associated data */
  uint8_t *data;

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
  /** hit count */
  size_t hit_cnt;
#endif
};

CONTAINER_TYPE(blk_cache_lba, BLIST, struct cache_entry_s, list_entry_lba);
CONTAINER_KEY_TYPE(blk_cache_lba, PTR, SCALAR, lba);

CONTAINER_TYPE(blk_cache_age, BLIST, struct cache_entry_s, list_entry_age);

struct block_cache_context_s
{
  struct device_s dev;

  const struct dev_block_params_s *bp;
  size_t parent_rq_size;

  /** cache entries sorted by lba */
  blk_cache_lba_root_t lba_list;
  /** cache entries sorted by age */
  blk_cache_age_root_t age_list;

#ifdef CONFIG_DRIVER_BLOCK_CACHE_STATS
  /** requests and hits stats */
  size_t rq_cnt;
  size_t nocache_rq_cnt;
  size_t miss_cnt;
  size_t hit_cnt;
#endif

  /** maximum number of cached blocks */
  size_t blk_max;
  /** total cached blocks count */
  size_t blk_count;

  /** maximum request size considered for caching */
  size_t max_rq_size;

  /** use delyed write */
  bool_t sync;
};

struct block_cache_rq_s
{
  struct block_cache_context_s  *pv;
  devblock_callback_t		*old_callback;
  bool_t			update_only;
};

#endif

