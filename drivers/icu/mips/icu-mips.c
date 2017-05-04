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

#include "icu-mips.h"

#include "icu-mips-private.h"

#include <string.h>
#include <stdio.h>

#include <hexo/types.h>
#include <device/device.h>
#include <device/driver.h>
#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/interrupt.h>

#include <mutek/printk.h>

void icu_mips_update(struct device_s *dev)
{
	/*
	 * Prevent the Mips-specific code to be compiled on heterogeneous
	 * builds.
	 */
#if defined(__mips)
	struct icu_mips_private_s	*pv = dev->drv_pv;

//	printk("ICU mips mask %x for cpu %d\n", pv->mask, cpu_id());

	reg_t status = cpu_mips_mfc0(12, 0);
	status = (status & 0xffff00ff) | ((uint32_t)pv->mask << 8);
	cpu_mips_mtc0(12, 0, status);

    pv->must_update = 0;
#endif
}

DEVICU_ENABLE(icu_mips_enable)
{
	struct icu_mips_private_s	*pv = dev->drv_pv;

	reg_t mask = 1 << (irq + 2);
	if (enable)
		pv->mask |= mask;
	else
		pv->mask &= ~mask;

    pv->must_update = 1;

    return 0;
}

DEVICU_SETHNDL(icu_mips_sethndl)
{
	struct icu_mips_private_s	*pv = dev->drv_pv;
	struct icu_mips_handler_s	*h = pv->table + irq;

	h->hndl = hndl;
	h->data = data;

	return 0;
}

DEVICU_DELHNDL(icu_mips_delhndl)
{
	struct icu_mips_private_s	*pv = dev->drv_pv;
	struct icu_mips_handler_s	*h = pv->table + irq;

	reg_t mask = 1 << (irq + 2);
	assert( (mask & pv->mask) == 0 && "You should have disabled this interrupt already" );
	(void)(mask&pv->mask);

	h->hndl = NULL;
	h->data = NULL;

	return 0;
}

static CPU_INTERRUPT_HANDLER(icu_mips_handler)
{
	struct device_s *dev = CPU_LOCAL_GET(cpu_interrupt_handler_dev);
	struct icu_mips_private_s	*pv = dev->drv_pv;
	struct icu_mips_handler_s	*h;
	if ( !irq )
		return;
	uint32_t irq_no = __builtin_ctz(irq);

	if ( irq_no >= ICU_MIPS_MAX_VECTOR ) {
		printk("Mips %d got spurious interrupt %i\n", cpu_id(), irq_no);
		return;
	}

	h = pv->table + irq_no;
	
	if (h->hndl)
		h->hndl(h->data);
	else
		printk("Mips %d lost interrupt %i\n", cpu_id(), irq_no);

    if ( pv->must_update )
        icu_mips_update(dev);
}

DEV_CLEANUP(icu_mips_cleanup)
{
	struct icu_mips_private_s *pv = dev->drv_pv;

    mem_free(pv);
}

static const struct devenum_ident_s	icu_mips_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("cpu:mips", 0, 0),
	{ 0 }
};

const struct driver_s	icu_mips_drv =
{
	.class		= device_class_icu,
    .id_table   = icu_mips_ids,
	.f_init		= icu_mips_init,
	.f_irq      = (dev_irq_t *)icu_mips_handler,
	.f_cleanup		= icu_mips_cleanup,
	.f.icu = {
		.f_enable		= icu_mips_enable,
		.f_sethndl		= icu_mips_sethndl,
		.f_delhndl		= icu_mips_delhndl,
	}
};

REGISTER_DRIVER(icu_mips_drv);

DEV_INIT(icu_mips_init)
{
	struct icu_mips_private_s	*pv;

	dev->drv = &icu_mips_drv;

	/* FIXME allocation scope ? */
	pv = mem_alloc(sizeof (*pv), (mem_scope_sys));

	if ( pv == NULL )
		goto memerr;

	dev->drv_pv = pv;

	memset(pv, 0, sizeof(*pv));

    pv->must_update = 1;

	return 0;

  memerr:
	return -ENOMEM;
}
