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

*/

#include <hexo/types.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_array.h>

#include <stdlib.h>
#include <string.h>

#include "block-partition.h"
#include "block-partition-private.h"

/**************************************************************/

DEVBLOCK_REQUEST(block_partition_request)
{
  struct block_partition_context_s	*pv = dev->drv_pv;

  if (rq->lba + rq->count > pv->drv_params.blk_count)
    {
      rq->progress = -ERANGE;
      rq->callback(rq, 0, rq + 1);
    }
  else
    {
      rq->lba += pv->first;
      dev_block_request(dev->parent, rq);
    }
}

/* 
 * device params
 */

DEVBLOCK_GETPARAMS(block_partition_getparams)
{
  struct block_partition_context_s	*pv = dev->drv_pv;

  return &pv->drv_params;
}

DEV_CLEANUP(block_partition_cleanup)
{
  struct block_partition_context_s	*pv = dev->drv_pv;

  if (pv != NULL)
    mem_free(pv);
}

const struct driver_s	block_partition_drv;

void block_partition_new(struct device_s *parent,
			 uint8_t type,
			 dev_block_lba_t lba,
			 dev_block_lba_t size)
{
  struct device_s *new = device_obj_new(NULL);

  if (new != NULL)
    {
      struct block_partition_context_s *pv;

      pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

      if (pv != NULL)
	{
	  printk("Partition (%02x) start: %u size: %u\n", type, lba, size);
	  new->drv_pv = pv;
	  new->irq = DEVICE_IRQ_INVALID;
	  new->drv = &block_partition_drv;
	  pv->first = lba;

	  pv->drv_params.blk_size = 512;
	  pv->drv_params.blk_count = size;

	  device_register(new, parent, NULL);
	}

      device_obj_refdrop(new);
    }
}

struct block_partition_list_s
{
  dev_block_lba_t	lba;
  dev_block_lba_t	size;
  uint8_t		type;
};

CONTAINER_TYPE(block_partition_list, ARRAY, struct block_partition_list_s, CONFIG_DRIVER_BLOCK_PARTITION_MAXCOUNT);
CONTAINER_FUNC(block_partition_list, ARRAY, static inline, block_partition_list);

static void block_partition_parse_extended(struct device_s *parent,
					   dev_block_lba_t last,
					   block_partition_list_root_t *list,
					   dev_block_lba_t lba)
{
  struct partition_table_s t;
  dev_block_lba_t next_lba = lba;
  uint8_t *data[1];

  while (1)
    {
      data[0] = (uint8_t*)&t;

      if (dev_block_spin_read(parent, data, next_lba, 1))
	return;

      if (endian_le16_na_load(&t.signature) != 0xaa55)
	return;

      block_partition_list_item_t e =
	{
	  .lba = next_lba + endian_le32_na_load(&t.part[0].offset),
	  .size = endian_le32_na_load(&t.part[0].size),
	  .type = t.part[0].type
	};

      if (!block_partition_list_pushback(list, e))
	return;			/* too many partitions */

      if (!t.part[1].type)
	return;			/* no more logical partitions */

      next_lba = lba + endian_le32_na_load(&t.part[1].offset);

      if (next_lba >= last)
	return;
    }
}

static void block_partition_parse(struct device_s *parent,
				  dev_block_lba_t last,
				  block_partition_list_root_t *list)
{
  struct dev_block_rq_s rq;
  struct partition_table_s t;
  uint8_t *data[1];
  uint_fast8_t i;

  data[0] = (uint8_t*)&t;

  if (dev_block_spin_read(parent, data, 0, 1))
    {
      printk("partition table read error\n");
      return;
    }

  if (endian_le16_na_load(&t.signature) != 0xaa55)
    {
      printk("bad partition table signature\n");
      return;
    }

  for (i = 0; i < 4; i++)
    {
      struct partition_table_entry_s *p = t.part + i;
      dev_block_lba_t size = endian_le32_na_load(&p->size);
      dev_block_lba_t lba = endian_le32_na_load(&p->offset);

      switch (p->type)
	{
	case PARTITION_TYPE_EMPTY:
	  break;

	case PARTITION_TYPE_EXTENDED:
	case PARTITION_TYPE_EXTENDED_LBA:
	  if (lba < last)
	    block_partition_parse_extended(parent, last, list, lba);
	  break;

	default: {
	  block_partition_list_item_t e =
	    {
	      .lba = lba,
	      .size = size,
	      .type = p->type
	    };

	  block_partition_list_pushback(list, e);
	} break;

	}
    }
}


DEV_CREATE(block_partition_create)
{
  size_t count;
  const struct dev_block_params_s *bp;

  if (parent == NULL)
    return 0;

  if (parent->drv == NULL || parent->drv->class != device_class_block)
    return 0;

  bp = dev_block_getparams(parent);

  if (bp->blk_size != 512)
    {
      printk("unable to read partition table, block size != 512\n");
      return 0;
    }

  block_partition_list_root_t list;

  block_partition_list_init(&list);

  block_partition_parse(parent, bp->blk_count, &list);

  CONTAINER_FOREACH(block_partition_list, ARRAY, &list, 
  {
    /* FIXME check overlap and size here */
    block_partition_new(parent, item.type, item.lba, item.size);
  });

  count = block_partition_list_count(&list);

  block_partition_list_destroy(&list);

  return count;
}

DEVBLOCK_GETRQSIZE(block_partition_getrqsize)
{
  return dev_block_getrqsize(dev->parent);
}

const struct driver_s block_partition_drv =
{
  .class		= device_class_block,
  .f_create		= block_partition_create,
  .f_cleanup		= block_partition_cleanup,
  .f.blk = {
    .f_request		= block_partition_request,
    .f_getparams	= block_partition_getparams,
    .f_getrqsize	= block_partition_getrqsize,
  }
};

