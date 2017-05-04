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

#include <hexo/endian.h>

#include <hexo/types.h>

#include <device/block.h>
#include <device/device.h>
#include <device/icu.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>

#include <stdlib.h>
#include <string.h>

#include "block-soclib.h"
#include "block-soclib-private.h"

/**************************************************************/

static void block_soclib_op_start(struct device_s *dev,
				  struct dev_block_rq_s *rq)
{
  struct block_soclib_rq_s *srq = (void*)(rq + 1);

  cpu_mem_write_32(dev->addr[0] + BLOCK_SOCLIB_BUFFER,
                   endian_le32((uint32_t)rq->data[rq->progress]));
  cpu_mem_write_32(dev->addr[0] + BLOCK_SOCLIB_LBA,
                   endian_le32(rq->lba + rq->progress));
  cpu_mem_write_32(dev->addr[0] + BLOCK_SOCLIB_COUNT,
                   endian_le32(1));
  cpu_mem_write_32(dev->addr[0] + BLOCK_SOCLIB_OP,
                   endian_le32((uint32_t)srq->rq_code));
}

/* add a new request in queue and start if idle */
static void block_soclib_rq_start(struct device_s *dev,
				  struct dev_block_rq_s *rq)
{
  struct block_soclib_context_s *pv = dev->drv_pv;
  bool_t idle = dev_blk_queue_isempty(&pv->queue);

  dev_blk_queue_pushback(&pv->queue, rq);

  if (idle)
    block_soclib_op_start(dev, rq);
}

/* drop current request and start next in queue if any */
static void block_soclib_rq_end(struct device_s *dev)
{
  struct block_soclib_context_s *pv = dev->drv_pv;
  struct dev_block_rq_s *rq;

  dev_blk_queue_pop(&pv->queue);

  if ((rq = dev_blk_queue_head(&pv->queue)) != NULL)
    block_soclib_op_start(dev, rq);
}

/* 
 * device read operation
 */

DEVBLOCK_REQUEST(block_soclib_request)
{
  struct block_soclib_context_s *pv = dev->drv_pv;
  struct dev_block_params_s *p = &pv->params;
  struct block_soclib_rq_s *srq = (void*)(rq + 1);

  LOCK_SPIN_IRQ(&dev->lock);

  if (rq->lba + rq->count <= p->blk_count)
    {
      switch (rq->type & DEV_BLOCK_OPMASK)
	{
	case DEV_BLOCK_READ:
	  srq->rq_code = BLOCK_SOCLIB_OP_READ;
	  block_soclib_rq_start(dev, rq);
	  break;

	case DEV_BLOCK_WRITE:
	  srq->rq_code = BLOCK_SOCLIB_OP_WRITE;
	  block_soclib_rq_start(dev, rq);
	  break;

	default:
	  rq->progress = -ENOTSUP;
	  rq->callback(rq, 0, srq + 1);
	  break;
	}
    }
  else
    {
      rq->progress = -ERANGE;
      rq->callback(rq, 0, srq + 1);
    }

  LOCK_RELEASE_IRQ(&dev->lock);
}

/* 
 * device params
 */

DEVBLOCK_GETPARAMS(block_soclib_getparams)
{
  struct block_soclib_context_s *pv = dev->drv_pv;
  return &pv->params;
}

/* 
 * device close operation
 */

DEV_CLEANUP(block_soclib_cleanup)
{
  struct block_soclib_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, block_soclib_irq);

  mem_free(pv);
}

/*
 * IRQ handler
 */

DEV_IRQ(block_soclib_irq)
{
  struct block_soclib_context_s	*pv = dev->drv_pv;

  lock_spin(&dev->lock);

  struct dev_block_rq_s *rq = dev_blk_queue_head(&pv->queue);
  struct block_soclib_rq_s *srq = (void*)(rq + 1);

  uint32_t st = endian_le32(
      cpu_mem_read_32(dev->addr[0] + BLOCK_SOCLIB_STATUS));

/*   printk("block dev irq %p, st: %x\n", dev, st); */

  switch (st)
    {
    case BLOCK_SOCLIB_STATUS_READ_SUCCESS:
    case BLOCK_SOCLIB_STATUS_WRITE_SUCCESS:
      assert(rq != NULL);

      rq->progress++;
      rq->callback(rq, 1, srq + 1);

      if (rq->count >= rq->progress)
	block_soclib_rq_end(dev);
      else
	block_soclib_op_start(dev, rq);

      lock_release(&dev->lock);
      return 1;

    case BLOCK_SOCLIB_STATUS_ERROR:
    case BLOCK_SOCLIB_STATUS_READ_ERROR:
    case BLOCK_SOCLIB_STATUS_WRITE_ERROR:
      assert(rq != NULL);

      rq->progress = -EIO;
      rq->callback(rq, 0, srq + 1);
      block_soclib_rq_end(dev);

      lock_release(&dev->lock);
      return 1;

    default:
      lock_release(&dev->lock);
      return 0;
    }

}

DEVBLOCK_GETRQSIZE(block_soclib_getrqsize)
{
  return sizeof(struct dev_block_rq_s) + sizeof(struct block_soclib_rq_s);
}

static const struct devenum_ident_s	block_soclib_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:block_device", 0, 0),
	{ 0 }
};

const struct driver_s	block_soclib_drv =
{
  .class		= device_class_block,
  .id_table		= block_soclib_ids,
  .f_init		= block_soclib_init,
  .f_cleanup		= block_soclib_cleanup,
  .f_irq		= block_soclib_irq,
  .f.blk = {
    .f_request		= block_soclib_request,
    .f_getparams	= block_soclib_getparams,
    .f_getrqsize	= block_soclib_getrqsize,
  }
};

REGISTER_DRIVER(block_soclib_drv);

DEV_INIT(block_soclib_init)
{
  struct block_soclib_context_s	*pv;

  dev->drv = &block_soclib_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    goto err;

  pv->params.blk_size = endian_le32(
      cpu_mem_read_32(dev->addr[0] + BLOCK_SOCLIB_BLOCK_SIZE));
  pv->params.blk_count = endian_le32(
      cpu_mem_read_32(dev->addr[0] + BLOCK_SOCLIB_SIZE));

  dev_blk_queue_init(&pv->queue);

  cpu_mem_write_32(dev->addr[0] + BLOCK_SOCLIB_IRQ_ENABLE,
                   endian_le32(1));

  printk("Soclib block device : %u sectors %u bytes per block\n",
	 pv->params.blk_count, pv->params.blk_size);

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, block_soclib_irq);

  dev->drv_pv = pv;
  return 0;

#if 0
 err_pv:
  mem_free(pv);
#endif
 err:
  return -1;
}

