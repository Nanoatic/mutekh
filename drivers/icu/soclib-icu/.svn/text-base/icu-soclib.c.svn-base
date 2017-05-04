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

#include "icu-soclib.h"

#include "icu-soclib-private.h"

#include <string.h>

#include <hexo/types.h>
#include <hexo/endian.h>
#include <device/device.h>
#include <device/driver.h>
#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/interrupt.h>

DEVICU_ENABLE(icu_soclib_enable)
{
  if (enable)
	  cpu_mem_write_32(dev->addr[0] + ICU_SOCLIB_REG_IER_SET, endian_le32(1 << irq));
  else
	  cpu_mem_write_32(dev->addr[0] + ICU_SOCLIB_REG_IER_CLR, endian_le32(1 << irq));

  return 0;
}

DEVICU_SETHNDL(icu_soclib_sethndl)
{
  struct icu_soclib_private_s	*pv = dev->drv_pv;
  struct icu_soclib_handler_s	*h = pv->table + irq;

  h->hndl = hndl;
  h->data = data;

  return 0;
}

DEVICU_DELHNDL(icu_soclib_delhndl)
{
  struct icu_soclib_private_s	*pv = dev->drv_pv;
  struct icu_soclib_handler_s	*h = pv->table + irq;

  h->hndl = NULL;
  h->data = NULL;

  return 0;
}

DEV_IRQ(icu_soclib_handler)
{
  struct icu_soclib_handler_s	*h;
  struct icu_soclib_private_s	*pv = dev->drv_pv;

  uint32_t idx = cpu_mem_read_32(dev->addr[0] + ICU_SOCLIB_REG_VECTOR);
  h = pv->table + endian_le32(idx);

  /* call interrupt handler */
  return h->hndl(h->data);
}

DEV_CLEANUP(icu_soclib_cleanup)
{
  struct icu_soclib_private_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, icu_soclib_handler);

  mem_free(pv);
}

static const struct devenum_ident_s	icu_soclib_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:icu", 0, 0),
	{ 0 }
};

const struct driver_s	icu_soclib_drv =
{
  .class		= device_class_icu,
  .id_table		= icu_soclib_ids,
  .f_init		= icu_soclib_init,
  .f_cleanup		= icu_soclib_cleanup,
  .f_irq            = icu_soclib_handler,
  .f.icu = {
    .f_enable		= icu_soclib_enable,
    .f_sethndl		= icu_soclib_sethndl,
    .f_delhndl		= icu_soclib_delhndl,
  }
};

REGISTER_DRIVER(icu_soclib_drv);

DEV_INIT(icu_soclib_init)
{
  struct icu_soclib_private_s	*pv;
  device_mem_map( dev , ( 1 << ICU_ADDR_MASTER ) );
  dev->drv = &icu_soclib_drv;

  if ((pv = mem_alloc(sizeof (*pv), (mem_scope_sys)))) /* FIXME allocation scope ? */
    {
      dev->drv_pv = pv;
      pv->dev = dev;

      cpu_mem_write_32(dev->addr[0] + ICU_SOCLIB_REG_IER_CLR, -1);

	  DEV_ICU_BIND(dev->icudev, dev, dev->irq, icu_soclib_handler);
  
      return 0;
    }

  return -ENOMEM;
}

