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

    Copyright (c) 2009, Nicolas Pouillon, <nipo@ssji.net>
*/


#include <hexo/types.h>

#include <device/timer.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include "pitc_6079a-private.h"

#include "pitc_6079a.h"

/*
 * timer device callback setup
 */

DEVTIMER_SETCALLBACK(pitc_6079a_setcallback)
{
	struct pitc_6079a_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	if ( id )
		return ENOTSUP;

	if (callback) {
		pv->cb = callback;
		pv->pv = priv;

        cpu_mem_mask_set_32(registers + REG_MR, PITC_PITIEN);
    } else {
        cpu_mem_mask_clear_32(registers + REG_MR, PITC_PITIEN);
    }

	return 0;
}

/*
 * timer device period setup
 */

DEVTIMER_SETPERIOD(pitc_6079a_setperiod)
{
	uintptr_t registers = (uintptr_t)dev->addr[0];

	if (period & ~PITC_CPIV_MASK)
		return ENOTSUP;

	if (period) {
        cpu_mem_write_32(registers + REG_MR, 0
            | (cpu_mem_read_32(registers + REG_MR) & PITC_PITIEN)
			| (period | PITC_PITEN)
			);
	} else {
        cpu_mem_mask_clear_32(registers + REG_MR, PITC_PITEN);
    }

	return 0;
}

/*
 * timer device value change
 */

DEVTIMER_SETVALUE(pitc_6079a_setvalue)
{
	return ENOTSUP;
}

/*
 * timer device period setup
 */

DEVTIMER_GETVALUE(pitc_6079a_getvalue)
{
	uintptr_t registers = (uintptr_t)dev->addr[0];

    return cpu_mem_read_32(registers + REG_IR) & PITC_CPIV_MASK;
}

/*
 * device irq
 */

DEV_IRQ(pitc_6079a_irq)
{
	struct pitc_6079a_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

    uint32_t count = cpu_mem_read_32(registers + REG_VR) >> 20;
	
	if ( !count )
		return 0;

	while ( count-- )
		if (pv->cb)
			pv->cb(pv->pv);

	return 1;
}

/* 
 * device close operation
 */

DEV_CLEANUP(pitc_6079a_cleanup)
{
	struct pitc_6079a_context_s	*pv = dev->drv_pv;

	DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, pitc_6079a_irq);

	mem_free(pv);
}

#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct devenum_ident_s	pitc_6079a_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("pitc6079a", 0, 0),
	{ 0 }
};
#endif

const struct driver_s	pitc_6079a_drv =
{
    .class      = device_class_timer,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = pitc_6079a_ids,
#endif
    .f_init     = pitc_6079a_init,
    .f_cleanup      = pitc_6079a_cleanup,
    .f_irq      = pitc_6079a_irq,
    .f.timer = {
        .f_setcallback  = pitc_6079a_setcallback,
        .f_setperiod    = pitc_6079a_setperiod,
        .f_setvalue     = pitc_6079a_setvalue,
        .f_getvalue     = pitc_6079a_getvalue,
    }
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(pitc_6079a_drv);
#endif

/* 
 * device open operation
 */

DEV_INIT(pitc_6079a_init)
{
	struct pitc_6079a_context_s *pv;
	dev->drv = &pitc_6079a_drv;

	/* allocate private driver data */
	pv = mem_alloc(sizeof(*pv), mem_scope_sys);

	if (!pv)
		return -1;

	memset(pv, 0, sizeof(*pv));

	dev->drv_pv = pv;

	dev_icu_sethndl(dev->icudev, dev->irq, pitc_6079a_irq, dev);
	dev_icu_enable(dev->icudev, dev->irq, 1, 0x2);

	return 0;
}

