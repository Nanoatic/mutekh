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

#include <device/enum.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>
#include <stdlib.h>

#include "enum-pci.h"
#include "enum-pci-private.h"

/**************************************************************/

static void enum_pci_register_all(struct device_s *dev)
{
//  struct enum_pci_context_s	*pv = dev->drv_pv;

  /* walk through all devices */
  CONTAINER_FOREACH(device_list, CLIST, &dev->children,
  {
    struct enum_pv_pci_s		*enum_pv = item->enum_pv;
//    uint_fast8_t			i;

    /* ignore already configured devices */
    if (item->drv != NULL)
      continue;

	const struct driver_s *drv = driver_get_matching_pci(
		enum_pv->vendor, enum_pv->devid, enum_pv->class);

	if ( !drv )
		CONTAINER_FOREACH_CONTINUE;

	item->icudev = dev->icudev;

	printk("device: %p driver: %p init: %p\n", item, drv, drv->f_init);

	/* call driver device init function, use same icu as PCI
	   enumerator parent device */
	drv->f_init(item, NULL);
  });
}

/*
 * device close operation
 */

DEV_CLEANUP(enum_pci_cleanup)
{
  struct enum_pci_context_s	*pv = dev->drv_pv;

  //  lock_destroy(&pv->lock);

  mem_free(pv);
}

static error_t
pci_enum_dev_probe(struct device_s *dev, uint8_t bus,
		   uint8_t dv, uint8_t fn)
{
  error_t			res = -ENOMEM;
  struct device_s		*new;
  struct enum_pv_pci_s		*enum_pv;
  uint16_t			vendor;
  uint8_t			htype;

  /* get pci vendor id */
  vendor = pci_confreg_read(bus, dv, fn, PCI_CONFREG_VENDOR);

  if (!vendor || vendor == 0xffff)
    return -ENOENT;

  if ((new = device_obj_new(NULL)))
    {
      if ((enum_pv = mem_alloc(sizeof (*enum_pv), (mem_scope_sys))))
	{
	  uint_fast8_t		regaddr;
	  uint_fast8_t		index;

	  enum_pv->vendor = vendor;
	  enum_pv->devid = pci_confreg_read(bus, dv, fn, PCI_CONFREG_DEVID);
	  enum_pv->class = (pci_confreg_read(bus, dv, fn, PCI_CONFREG_CLASS) & 0x00ffff00) >> 8;

	  printk("PCI device %04x:%04x class %06x, device %p\n",
		 vendor, enum_pv->devid, enum_pv->class, new);

	  for ((index = 0, regaddr = PCI_CONFREG_ADDRESS_0);
	       index < __MIN(PCI_CONFREG_ADDRESS_COUNT, DEVICE_MAX_ADDRSLOT);
	       (index++, regaddr += 4))
	    {
	      uint32_t		reg;

	      reg = pci_confreg_read(bus, dv, fn, regaddr);

	      /* test if register is implemented */
	      pci_confreg_write(bus, dv, fn, regaddr, 0xffffffff);
	      if (pci_confreg_read(bus, dv, fn, regaddr) == 0)
		break;
	      pci_confreg_write(bus, dv, fn, regaddr, reg);

	      if (PCI_CONFREG_ADDRESS_IS_IO(reg))
		/* get IO base address */
		{
		  new->addr[index] = PCI_CONFREG_ADDRESS_IO(reg);
		  printk("  PCI IO base    : 0x%04x\n", new->addr[index]);
		}
	      else if (PCI_CONFREG_ADDRESS_IS_MEM32(reg))
		/* get memory 32 base address */
		{
		  new->addr[index] = PCI_CONFREG_ADDRESS_MEM32(reg);
		  printk("  PCI mem32 base : 0x%p\n", new->addr[index]);
		}
	      else if (PCI_CONFREG_ADDRESS_IS_MEM64(reg))
		/* get memory 64 base address */
		{
		  uint32_t	high;

		  regaddr += 4;
		  assert(regaddr <= PCI_CONFREG_ADDRESS_5);
		  high = pci_confreg_read(bus, dv, fn, regaddr);
		  new->addr[index] = PCI_CONFREG_ADDRESS_MEM64(reg, high);
		  printk("  PCI mem64 base : 0x%p\n", new->addr[index]);
		}
	    }

	  /* get irq line */
	  {
	    uint8_t		reg;

	    reg = pci_confreg_read(bus, dv, fn, PCI_CONFREG_IRQLINE);

	    if (reg != 0xff)
	      {
		printk("  PCI irq : %u\n", reg);
		new->irq = reg;
	      }
	    else
	      {
		new->irq = DEVICE_IRQ_INVALID;
	      }
	  }

	  /* the device is not bound to any driver */
	  new->drv = NULL;

	  device_register(new, dev, enum_pv);

	  /* device with multiple functions ? */
	  htype = pci_confreg_read(bus, dv, fn, PCI_CONFREG_HTYPE);
	  res = (htype & PCI_CONFREG_HTYPE_MULTI ? 1 : 0);
	}

      device_obj_refdrop(new);
    }

  return res;
}

static error_t
pci_enum_probe(struct device_s *dev)
{
  uint_fast8_t	bus, dv, fn;

  for (bus = 0; bus < PCI_CONF_MAXBUS; bus++)
    for (dv = 0; dv < PCI_CONF_MAXDEVICE; dv++)
      {
	for (fn = 0; fn < PCI_CONF_MAXFCN; fn++)
	  if (pci_enum_dev_probe(dev, bus, dv, fn) <= 0)
	    break;
      }

  return 0;
}

/*
 * device open operation
 */

const struct driver_s	enum_pci_drv =
{
  .class		= device_class_enum,
  .f_init		= enum_pci_init,
  .f_cleanup		= enum_pci_cleanup,
  .f.denum = {
    .f_lookup		= enum_pci_lookup,
  }
};

DEV_INIT(enum_pci_init)
{
  struct enum_pci_context_s	*pv;

  dev->drv = &enum_pci_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  lock_init(&pv->lock);

  dev->drv_pv = pv;

  pci_enum_probe(dev);
  enum_pci_register_all(dev);

  return 0;
}

DEVENUM_LOOKUP(enum_pci_lookup)
{
	return NULL;
}
