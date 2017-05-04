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
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include "timer-emu-private.h"

#include "timer-emu.h"

/*
 * timer device callback setup
 */

DEVTIMER_SETCALLBACK(timer_emu_setcallback)
{
  struct timer_emu_context_s	*pv = dev->drv_pv;

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

DEVTIMER_SETPERIOD(timer_emu_setperiod)
{
  return 0;
}

/*
 * timer device value change
 */

DEVTIMER_SETVALUE(timer_emu_setvalue)
{
  return -ENOTSUP;
}

/*
 * timer device period setup
 */

DEVTIMER_GETVALUE(timer_emu_getvalue)
{
  return -ENOTSUP;
}

/*
 * device close operation
 */

DEV_CLEANUP(timer_emu_cleanup)
{
  struct timer_emu_context_s	*pv = dev->drv_pv;

  mem_free(pv);
}

/*
 * device open operation
 */

const struct driver_s	timer_emu_drv =
{
  .class		= device_class_timer,
  .f_init		= timer_emu_init,
  .f_cleanup		= timer_emu_cleanup,
  .f_irq		= NULL,
  .f.timer = {
    .f_setcallback	= timer_emu_setcallback,
    .f_setperiod	= timer_emu_setperiod,
    .f_setvalue		= timer_emu_setvalue,
    .f_getvalue		= timer_emu_getvalue,
  }
};

DEV_INIT(timer_emu_init)
{
  struct timer_emu_context_s	*pv;

  dev->drv = &timer_emu_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  memset(pv, 0, sizeof(*pv));

  dev->drv_pv = pv;

  return 0;
}

