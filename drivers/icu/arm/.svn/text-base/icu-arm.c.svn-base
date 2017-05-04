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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net>, 2009

*/

#include "icu-arm.h"

#include "icu-arm-private.h"

#include <string.h>

#include <hexo/types.h>
#include <device/device.h>
#include <device/driver.h>
#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/interrupt.h>

#include <mutek/printk.h>

DEVICU_ENABLE(icu_arm_enable)
{
	return 0;
}

DEVICU_SETHNDL(icu_arm_sethndl)
{
	assert(irq == 0 && "Only one irq line is available on ARM");
	struct icu_arm_private_s	*pv = dev->drv_pv;
	struct icu_arm_handler_s	*h = pv->table;

	h->hndl = hndl;
	h->data = data;

	return 0;
}

DEVICU_DELHNDL(icu_arm_delhndl)
{
	struct icu_arm_private_s	*pv = dev->drv_pv;
	struct icu_arm_handler_s	*h = pv->table;

	h->hndl = NULL;
	h->data = NULL;

	return 0;
}

static CPU_INTERRUPT_HANDLER(icu_arm_handler)
{
	struct device_s *dev = CPU_LOCAL_GET(cpu_interrupt_handler_dev);
	struct icu_arm_private_s	*pv = dev->drv_pv;
	struct icu_arm_handler_s	*h = pv->table;
	
	if (h->hndl)
		h->hndl(h->data);
	else
		printk("ARM %d lost interrupt %i\n", cpu_id(), irq);
}

DEV_CLEANUP(icu_arm_cleanup)
{
}

static const struct devenum_ident_s	icu_arm_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("cpu:arm", 0, 0),
	{ 0 }
};

const struct driver_s	icu_arm_drv =
{
	.class		= device_class_icu,
    .id_table   = icu_arm_ids,
	.f_init		= icu_arm_init,
	.f_irq      = (dev_irq_t *)icu_arm_handler,
	.f_cleanup		= icu_arm_cleanup,
	.f.icu = {
		.f_enable		= icu_arm_enable,
		.f_sethndl		= icu_arm_sethndl,
		.f_delhndl		= icu_arm_delhndl,
	}
};

REGISTER_DRIVER(icu_arm_drv);

DEV_INIT(icu_arm_init)
{
	struct icu_arm_private_s	*pv;

	dev->drv = &icu_arm_drv;

	/* FIXME allocation scope ? */
	pv = mem_alloc(sizeof (*pv), (mem_scope_sys));

	if ( pv == NULL )
		goto memerr;

	dev->drv_pv = pv;

	return 0;

  memerr:
	return -ENOMEM;
}
