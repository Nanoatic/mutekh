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
#include <hexo/error.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>
#include <mutek/timer.h>

#include <mutek/printk.h>

#include "block-ata.h"

#include "block-ata-private.h"

/**************************************************************/

/* add a new request in queue and start if idle */
static void drive_ata_rq_start(struct device_s *dev,
			       struct dev_block_rq_s *rq)
{
  struct drive_ata_context_s *dpv = dev->drv_pv;
  bool_t idle = dev_blk_queue_isempty(&dpv->queue);

  dev_blk_queue_pushback(&dpv->queue, rq);

  if (idle)
    {
      struct drive_ata_rq_s *arq = (void*)(rq + 1);
      arq->start(dev, rq);
    }
}

/* drop current request and start next in queue if any */
static void drive_ata_rq_end(struct device_s *dev)
{
  struct drive_ata_context_s *dpv = dev->drv_pv;
  struct dev_block_rq_s *rq;

  dev_blk_queue_pop(&dpv->queue);

  if ((rq = dev_blk_queue_head(&dpv->queue)) != NULL)
    {
      struct drive_ata_rq_s *arq = (void*)(rq + 1);
      arq->start(dev, rq);
    }
  else
    {
      /* drive goes idle, select other drive to give a chance to interrupt */
      controller_ata_reg_w8(dev->parent, ATA_REG_DRVHEAD,
			    (dpv->devhead_reg ^ ATA_DRVHEAD_SLAVE) & ATA_DRVHEAD_SLAVE);
    }
}

/* try irq */
bool_t drive_ata_try_irq(struct device_s *dev)
{
  struct drive_ata_context_s *dpv = dev->drv_pv;
  struct dev_block_rq_s *rq;

  if ((rq = dev_blk_queue_head(&dpv->queue)) != NULL)
    {
      struct drive_ata_rq_s *arq = (void*)(rq + 1);
      return arq->irq(dev, rq);
    }

  return 0;
}

/* 
 * device read operation
 */

static DRIVE_ATA_START_FUNC(drive_ata_read_start)
{
  struct device_s *parent = dev->parent;
  struct drive_ata_context_s *dpv = dev->drv_pv;
  uint32_t lba = (rq->lba + rq->progress) & 0x0fffffff;

  dpv->ata_sec_count = __MIN(256, rq->count - rq->progress);

  controller_ata_reg_w8(parent, ATA_REG_DRVHEAD, dpv->devhead_reg | (lba >> 24));

  controller_ata_reg_w8(parent, ATA_REG_CYLINDER_HIGH, lba >> 16);
  controller_ata_reg_w8(parent, ATA_REG_CYLINDER_LOW, lba >> 8);
  controller_ata_reg_w8(parent, ATA_REG_SECTOR_NUMBER, lba);
  controller_ata_reg_w8(parent, ATA_REG_SECTOR_COUNT, dpv->ata_sec_count);

  controller_ata_reg_w8(parent, ATA_REG_COMMAND, ATA_CMD_READ_SECTORS);
}

static DRIVE_ATA_IRQ_FUNC(drive_ata_read_irq)
{
  struct drive_ata_context_s	*dpv = dev->drv_pv;
  struct drive_ata_rq_s *arq = (void*)(rq + 1);
  uint8_t status;

  controller_ata_reg_w8(dev->parent, ATA_REG_DRVHEAD, dpv->devhead_reg);

  status = controller_ata_reg_r8(dev->parent, ATA_REG_STATUS);

  if (status & ATA_STATUS_ERROR)
    {
      rq->progress = -EIO;
      rq->callback(rq, 0, arq + 1);

      drive_ata_rq_end(dev);
      return 1;
    }

  if (status & ATA_STATUS_DATA_RQ)
    {
      controller_ata_data_read16(dev->parent, rq->data[rq->progress]);

      rq->progress++;
      dpv->ata_sec_count--;

      rq->callback(rq, 1, arq + 1);

      if (rq->count >= rq->progress)
	{
	  drive_ata_rq_end(dev);
	}
      else
	{
	  if (dpv->ata_sec_count == 0)
	    drive_ata_read_start(dev, rq);
	}

      return 1;
    }

  return 0;
}

/* 
 * device write operation
 */

static DRIVE_ATA_START_FUNC(drive_ata_write_start)
{
  struct device_s *parent = dev->parent;
  struct drive_ata_context_s *dpv = dev->drv_pv;
  uint32_t lba = (rq->lba + rq->progress) & 0x0fffffff;

  dpv->ata_sec_count = __MIN(256, rq->count - rq->progress);

  controller_ata_reg_w8(parent, ATA_REG_DRVHEAD, dpv->devhead_reg | (lba >> 24));
  controller_ata_reg_w8(parent, ATA_REG_CYLINDER_HIGH, lba >> 16);
  controller_ata_reg_w8(parent, ATA_REG_CYLINDER_LOW, lba >> 8);
  controller_ata_reg_w8(parent, ATA_REG_SECTOR_NUMBER, lba);
  controller_ata_reg_w8(parent, ATA_REG_SECTOR_COUNT, dpv->ata_sec_count);

  controller_ata_reg_w8(parent, ATA_REG_COMMAND, ATA_CMD_WRITE_SECTORS);

  while (controller_ata_reg_r8(parent, ATA_REG_STATUS) & ATA_STATUS_BUSY)
    ;

  controller_ata_data_write16(parent, rq->data[rq->progress]);
}

static DRIVE_ATA_IRQ_FUNC(drive_ata_write_irq)
{
  struct drive_ata_context_s	*dpv = dev->drv_pv;
  struct drive_ata_rq_s *arq = (void*)(rq + 1);
  uint8_t status;

  controller_ata_reg_w8(dev->parent, ATA_REG_DRVHEAD, dpv->devhead_reg);

  status = controller_ata_reg_r8(dev->parent, ATA_REG_STATUS);

  if (!(status & ATA_STATUS_BUSY))
    {
      if (status & ATA_STATUS_ERROR)
	{
	  rq->progress = -EIO;
	  rq->callback(rq, 0, arq + 1);

	  drive_ata_rq_end(dev);
	}
      else
	{
	  rq->progress++;
	  dpv->ata_sec_count--;

	  rq->callback(rq, 1, arq + 1);

	  if (rq->count >= rq->progress)
	    {
	      drive_ata_rq_end(dev);
	    }
	  else
	    {
	      if (dpv->ata_sec_count > 0)
		controller_ata_data_write16(dev->parent, rq->data[rq->progress]);
	      else
		drive_ata_write_start(dev, rq);
	    }
	}

      return 1;
    }

  return 0;
}

DEVBLOCK_REQUEST(drive_ata_request)
{
//  struct controller_ata_context_s *cpv = dev->parent->drv_pv;
  struct drive_ata_context_s *dpv = dev->drv_pv;
  struct drive_ata_rq_s *arq = (void*)(rq + 1);

  LOCK_SPIN_IRQ(&dev->parent->lock);

  if (rq->lba + rq->count > dpv->drv_params.blk_count)
    {
      rq->progress = -ERANGE;
      rq->callback(rq, 0, arq + 1);
    }
  else
    {
      switch (rq->type & DEV_BLOCK_OPMASK)
	{
	case DEV_BLOCK_READ:
	  arq->irq = drive_ata_write_irq;
	  arq->start = drive_ata_write_start;
	  drive_ata_rq_start(dev, rq);
	  break;

	case DEV_BLOCK_WRITE:
	  arq->irq = drive_ata_read_irq;
	  arq->start = drive_ata_read_start;
	  drive_ata_rq_start(dev, rq);
	  break;

	default:
	  rq->progress = -ENOTSUP;
	  rq->callback(rq, 0, arq + 1);
	  break;
	}
    }

  LOCK_RELEASE_IRQ(&dev->parent->lock);
}

DEVBLOCK_GETPARAMS(drive_ata_getparams)
{
  struct drive_ata_context_s	*pv = dev->drv_pv;

  return &pv->drv_params;
}

DEVBLOCK_GETRQSIZE(block_soclib_getrqsize)
{
  return sizeof(struct dev_block_rq_s) + sizeof(struct block_ata_rq_s);
}

DEV_CLEANUP(drive_ata_cleanup)
{
  struct drive_ata_context_s	*pv = dev->drv_pv;

  mem_free(pv);
}

const struct driver_s	drive_ata_drv =
{
  .class		= device_class_block,
  .f_cleanup		= drive_ata_cleanup,
  .f.blk = {
    .f_request		= drive_ata_request,
    .f_getparams	= drive_ata_getparams,
  }
};

error_t drive_ata_init(struct device_s *dev, bool_t slave)
{
  struct drive_ata_context_s	*pv;

  assert(sizeof(struct ata_indent_s) == 512);

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  dev->drv_pv = pv;
  pv->devhead_reg = ATA_DRVHEAD_RESERVED_HIGH | (slave ? ATA_DRVHEAD_SLAVE : 0);

  controller_ata_reg_w8(dev->parent, ATA_REG_DRVHEAD, pv->devhead_reg);

  /* read identification data */
  controller_ata_reg_w8(dev->parent, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_DRIVE);

  if (controller_ata_waitbusy(dev->parent))
    return 1;

  controller_ata_data_swapread16(dev->parent, &pv->ident);

  /* does not support CHS mode yet */
  if (pv->ident.lba_supported)
    return 1;

  pv->devhead_reg |= ATA_DRVHEAD_LBA;

  pv->drv_params.blk_size = 512;
  pv->drv_params.blk_count =
    endian_be16(pv->ident.lba_count_low)
    | (endian_be16(pv->ident.lba_count_high) << 16);

  printk("ATA drive : %u sectors : %.40s\n",
	 pv->drv_params.blk_count, pv->ident.model_name);

  dev_blk_queue_init(&pv->queue);

  /* enable interrupts for this drive */
  controller_ata_rega_w8(dev->parent, ATA_REG_DEVICE_CONTROL,
			 ATA_DEVCTRL_RESERVED_HIGH);

  dev->drv = &drive_ata_drv;

  return 0;
}

