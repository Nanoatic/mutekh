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

#include "timer-8253-private.h"

#include "timer-8253.h"

/*
 * timer device callback setup
 */

DEVTIMER_SETCALLBACK(timer_8253_setcallback)
{
  struct timer_8253_context_s	*pv = dev->drv_pv;

  if (callback)
    {
      pv->cb[id] = callback;
      pv->pv[id] = priv;
    }

  return 0;
}

/*
 * timer device period setup
 */

DEVTIMER_SETPERIOD(timer_8253_setperiod)
{
  if (period)
    {
      /* control register */
      cpu_io_write_8(dev->addr[0] + 3, TIMER_8253_CHANID(id) | TIMER_8253_CTRL_LOADMODE_HILO | TIMER_8253_CTRL_CNTMODE_SQWRGEN);

      /* period lsb */
      cpu_io_write_8(dev->addr[0] + id, period & 0xff);

      /* period msb */
      cpu_io_write_8(dev->addr[0] + id, period >> 8);
    }

  return 0;
}

/*
 * timer device value change
 */

DEVTIMER_SETVALUE(timer_8253_setvalue)
{
  return -ENOTSUP;
}

/*
 * timer device period setup
 */

DEVTIMER_GETVALUE(timer_8253_getvalue)
{
  cpu_io_write_8(dev->addr[0] + 3, TIMER_8253_CHANID(id) | TIMER_8253_CTRL_LOADMODE_READ | TIMER_8253_CTRL_CNTMODE_SQWRGEN);

  return cpu_io_read_16(dev->addr[0] + id);
}

/*
 * device irq
 */

DEV_IRQ(timer_8253_irq)
{
  struct timer_8253_context_s	*pv = dev->drv_pv;
  uint_fast8_t			id = 0;

  /* invoke timer callback */
  if (pv->cb[id])
    pv->cb[id](pv->pv[id]);

  return 1;
}

/* 
 * device close operation
 */

DEV_CLEANUP(timer_8253_cleanup)
{
  struct timer_8253_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, timer_8253_irq);

  mem_free(pv);
}

/* 
 * device open operation
 */

const struct driver_s	timer_8253_drv =
{
  .class		= device_class_timer,
  .f_init		= timer_8253_init,
  .f_cleanup		= timer_8253_cleanup,
  .f_irq		= timer_8253_irq,
  .f.timer = {
    .f_setcallback	= timer_8253_setcallback,
    .f_setperiod	= timer_8253_setperiod,
    .f_setvalue		= timer_8253_setvalue,
    .f_getvalue		= timer_8253_getvalue,
  }
};

DEV_INIT(timer_8253_init)
{
  struct timer_8253_context_s	*pv;

  dev->drv = &timer_8253_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  memset(pv, 0, sizeof(*pv));

  dev->drv_pv = pv;

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, timer_8253_irq);

  return 0;
}

