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

#include <device/timer.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include "timer-soclib-private.h"

#include "timer-soclib.h"

/*
 * timer device callback setup
 */

DEVTIMER_SETCALLBACK(timer_soclib_setcallback)
{
  struct timer_soclib_context_s	*pv = dev->drv_pv;
  uintptr_t			base = dev->addr[0] + id * TIMER_SOCLIB_REGSPACE_SIZE;
  uint32_t			mode = cpu_mem_read_32(base + TIMER_SOCLIB_REG_MODE);

  if (callback)
    {
      pv->cb[id] = callback;
      pv->pv[id] = priv;

      cpu_mem_write_32(base + TIMER_SOCLIB_REG_MODE, mode | TIMER_SOCLIB_REG_MODE_IRQEN);
    }
  else
    {
      cpu_mem_write_32(base + TIMER_SOCLIB_REG_MODE, mode & ~TIMER_SOCLIB_REG_MODE_IRQEN);
    }

  return 0;
}

/*
 * timer device period setup
 */

DEVTIMER_SETPERIOD(timer_soclib_setperiod)
{
  uintptr_t	base = dev->addr[0] + id * TIMER_SOCLIB_REGSPACE_SIZE;
  uint32_t	mode = cpu_mem_read_32(base + TIMER_SOCLIB_REG_MODE);

  if (period)
    {
      cpu_mem_write_32(base + TIMER_SOCLIB_REG_MODE, mode | TIMER_SOCLIB_REG_MODE_EN);
      cpu_mem_write_32(base + TIMER_SOCLIB_REG_PERIOD, period);
    }
  else
    {
      cpu_mem_write_32(base + TIMER_SOCLIB_REG_MODE, mode & ~TIMER_SOCLIB_REG_MODE_EN);
    }

  return 0;
}

/*
 * timer device value change
 */

DEVTIMER_SETVALUE(timer_soclib_setvalue)
{
  uintptr_t	base = dev->addr[0] + id * TIMER_SOCLIB_REGSPACE_SIZE;

  cpu_mem_write_32(base + TIMER_SOCLIB_REG_VALUE, value);

  return 0;
}

/*
 * timer device period setup
 */

DEVTIMER_GETVALUE(timer_soclib_getvalue)
{
  uintptr_t	base = dev->addr[0] + id * TIMER_SOCLIB_REGSPACE_SIZE;

  return cpu_mem_read_32(base + TIMER_SOCLIB_REG_VALUE);
}

/*
 * device irq
 */

DEV_IRQ(timer_soclib_irq)
{
  struct timer_soclib_context_s	*pv = dev->drv_pv;
  uint_fast16_t	id;

  /* check irq for each timer */
  for (id = 0; id < TIMER_SOCLIB_IDCOUNT; id++)
    {
      uintptr_t	base = dev->addr[0] + id * TIMER_SOCLIB_REGSPACE_SIZE;

      if (cpu_mem_read_32(base + TIMER_SOCLIB_REG_IRQ))
	{
	  /* ack irq for this timer */
	  cpu_mem_write_32(base + TIMER_SOCLIB_REG_IRQ, 0);

	  /* invoke timer callback if any */
	  if (pv->cb[id])
	    pv->cb[id](pv->pv[id]);

	  return 1;
	}
    }

  return 0;
}

/* 
 * device close operation
 */

DEV_CLEANUP(timer_soclib_cleanup)
{
  struct timer_soclib_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, timer_soclib_irq);

  mem_free(pv);
}

static const struct devenum_ident_s	timer_soclib_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:timer", 0, 0),
	{ 0 }
};

const struct driver_s	timer_soclib_drv =
{
  .class		= device_class_timer,
  .id_table		= timer_soclib_ids,
  .f_init		= timer_soclib_init,
  .f_cleanup		= timer_soclib_cleanup,
  .f_irq		= timer_soclib_irq,
  .f.timer = {
    .f_setcallback	= timer_soclib_setcallback,
    .f_setperiod	= timer_soclib_setperiod,
    .f_setvalue		= timer_soclib_setvalue,
    .f_getvalue		= timer_soclib_getvalue,
  }
};

REGISTER_DRIVER(timer_soclib_drv);

/* 
 * device open operation
 */

DEV_INIT(timer_soclib_init)
{
  struct timer_soclib_context_s	*pv;
  device_mem_map( dev , 1 << 0 );
  dev->drv = &timer_soclib_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  memset(pv, 0, sizeof(*pv));

  dev->drv_pv = pv;

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, timer_soclib_irq);

  return 0;
}

