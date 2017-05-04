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

#include "block-ata.h"
#include "block-ata-private.h"

#include <device/enum.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <mutek/timer.h>

#include <mutek/printk.h>


/**************************************************************/

/* 
 * device close operation
 */

DEV_CLEANUP(controller_ata_cleanup)
{
  struct controller_ata_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, controller_ata_irq);

  mem_free(pv);
}

/*
 * IRQ handler
 */

DEV_IRQ(controller_ata_irq)
{
  struct controller_ata_context_s *pv = dev->drv_pv;
  bool_t res;

  lock_spin(&dev->lock);

  //  do
    {
      res = 0;

      if (pv->drive[0] != NULL)
	res |= drive_ata_try_irq(pv->drive[0]);

      if (pv->drive[1] != NULL)
	res |= drive_ata_try_irq(pv->drive[1]);
    }
  //  while (res);

  lock_release(&dev->lock);

  return res;
}

/*
 * PCI identifiers of compatible cards.
 */

static const struct devenum_ident_s	controller_ata_ids[] =
  {
	  DEVENUM_PCI_ENTRY(-1, -1, 0x0101), /* PCI IDE controller */
	  { 0 },
  };

const struct driver_s	controller_ata_drv =
{
  .class		= device_class_enum,
  .id_table		= controller_ata_ids,
  .f_irq		= controller_ata_irq,
  .f_init		= controller_ata_init,
  .f_cleanup		= controller_ata_cleanup,
};

//REGISTER_DRIVER(controller_ata_drv);

bool_t controller_ata_waitbusy(struct device_s *dev)
{
  return TIMER_BUSY_WAIT(&timer_ms, 1000,
			 cpu_io_read_8(dev->addr[0] + ATA_REG_STATUS) & ATA_STATUS_BUSY);
}

void controller_ata_new_atadrv(struct device_s *dev, bool_t slave)
{
  struct controller_ata_context_s	*pv = dev->drv_pv;
  struct device_s *new;

  if ((new = device_obj_new(NULL)))
    {
      if (!device_register(new, dev, pv))
	{
	  if (!drive_ata_init(new, slave))
	    {
	      pv->drive[slave] = new;
	    }
	}
      device_obj_refdrop(new);  
    }
}

void controller_ata_detect(struct device_s *dev, bool_t slave)
{
//  struct controller_ata_context_s	*pv = dev->drv_pv;

  cpu_io_write_8(dev->addr[0] + ATA_REG_DRVHEAD,
		 ATA_DRVHEAD_RESERVED_HIGH | (slave ? ATA_DRVHEAD_SLAVE : 0));

  if (!controller_ata_waitbusy(dev))
    {
      if (cpu_io_read_8(dev->addr[0] + ATA_REG_STATUS) & ATA_STATUS_READY)
	return controller_ata_new_atadrv(dev, slave);

      if (cpu_io_read_8(dev->addr[0] + ATA_REG_CYLINDER_LOW) == ATA_ATAPI_LOW_SIGN &&
	  cpu_io_read_8(dev->addr[0] + ATA_REG_CYLINDER_HIGH) == ATA_ATAPI_HIGH_SIGN)
	{
	  printk ("ATAPI drive found\n");
	  return;
	}
    }
}

DEV_INIT(controller_ata_init)
{
  struct controller_ata_context_s	*pv;

  dev->drv = &controller_ata_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  pv->drive[0] = pv->drive[1] = NULL;
  dev->drv_pv = pv;

  /* reset master and slave drives */

  cpu_io_write_8(dev->addr[1] + ATA_REG_DEVICE_CONTROL,
		 ATA_DEVCTRL_RESERVED_HIGH | ATA_DEVCTRL_RESET);

  cpu_io_write_8(dev->addr[1] + ATA_REG_DEVICE_CONTROL,
		 ATA_DEVCTRL_RESERVED_HIGH | ATA_DEVCTRL_DISABLE_IRQ);

  /* try to detect master */
  controller_ata_detect(dev, 0);
  /* try to detect slave */
  controller_ata_detect(dev, 1);

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, controller_ata_irq);

  return 0;
}

