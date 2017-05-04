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

    Copyright (c) UPMC, Lip6
         Alexandre Becoulet <alexandre.becoulet@lip6.fr>, 2006-2009
         Nicolas Pouillon <nipo@ssji.net>, 2009

*/

#include "xicu-soclib.h"
#include "xicu-soclib-private.h"

#include <mutek/mem_alloc.h>
#include <hexo/iospace.h>
#include <hexo/endian.h>
#include <hexo/ipi.h>
#include <device/driver.h>
#include <device/device.h>

#define PARENT(dev) (((struct xicu_filter_private_s *)(dev->drv_pv))->parent)

DEVICU_ENABLE(xicu_filter_enable)
{
    struct xicu_filter_private_s *pv = dev->drv_pv;

/*     printk("xicu %p %d enable %d %d\n", dev, pv->output, irq, enable); */
    xicu_root_enable_hwi(PARENT(dev), irq&0x1f, pv->output, enable);

    return 0;
}

DEVICU_SETHNDL(xicu_filter_sethndl)
{
/*     struct xicu_filter_private_s *pv = dev->drv_pv; */

/*     printk("xicu %p %d sethndl %d -> %p %p\n", dev, pv->output, irq, hndl, data); */
    return xicu_root_set_hwi_handler(PARENT(dev), irq&0x1f, hndl, data);
}

DEVICU_DELHNDL(xicu_filter_delhndl)
{
    return xicu_root_set_hwi_handler(PARENT(dev), irq&0x1f, NULL, NULL);
}

DEV_IRQ(xicu_filter_handler)
{
//    struct xicu_soclib_handler_s *h;
    struct xicu_filter_private_s *pv = dev->drv_pv;

    uint32_t prio = endian_le32(cpu_mem_read_32(XICU_REG_ADDR(
			dev->addr[0], XICU_PRIO, pv->output)));

/*     printk("xicu %p %d prio %x\n", dev, pv->output, prio); */
/*     printk("xicu %p %d wti: %d %x\n", dev, pv->output, */
/*            !!XICU_PRIO_HAS_WTI(prio), XICU_PRIO_WTI(prio)); */
/*     printk("xicu %p %d hwi: %d %x\n", dev, pv->output, */
/*            !!XICU_PRIO_HAS_HWI(prio), XICU_PRIO_HWI(prio)); */
/*     printk("xicu %p %d pti: %d %x\n", dev, pv->output, */
/*            !!XICU_PRIO_HAS_PTI(prio), XICU_PRIO_PTI(prio)); */

#ifdef CONFIG_HEXO_IPI
	if ( XICU_PRIO_HAS_WTI(prio) )
		return xicu_root_handle_ipi(PARENT(dev), XICU_PRIO_WTI(prio));
#endif

	if ( XICU_PRIO_HAS_HWI(prio) )
		return xicu_root_handle_hwi(PARENT(dev), XICU_PRIO_HWI(prio));

	if ( XICU_PRIO_HAS_PTI(prio) )
		return xicu_root_handle_timer(PARENT(dev), XICU_PRIO_PTI(prio));

	return 0;
}

#ifdef CONFIG_HEXO_IPI
DEVICU_SETUP_IPI_EP(xicu_filter_setup_ipi_ep)
{
	struct xicu_filter_private_s *pv = dev->drv_pv;

	endpoint->icu_dev = dev;
	xicu_root_set_ipi_handler(PARENT(dev), ipi_no&0x1f, (dev_irq_t*)ipi_process_rq, NULL);
	xicu_root_enable_ipi(PARENT(dev), ipi_no&0x1f, pv->output, 1);

	endpoint->priv = (void*)(uintptr_t)ipi_no;
	return 0;
}

DEVICU_SENDIPI(xicu_filter_sendipi)
{
	uint32_t cpu_ident = (uint32_t)endpoint->priv;
/* 	printk("xicu send ipi to %x\n", cpu_icu_identifier); */

	cpu_mem_write_32(XICU_REG_ADDR(endpoint->icu_dev->addr[0],
				       XICU_WTI_REG,
				       cpu_ident & 0x1f),
			 endian_le32(cpu_ident));
	return 0;
}
#endif

static const struct driver_param_binder_s xicufilter_param_binder[] =
{
	PARAM_BIND(struct xicu_filter_param_s, parent, PARAM_DATATYPE_DEVICE_PTR),
	PARAM_BIND(struct xicu_filter_param_s, output_line, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	xicu_filter_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:xicu:filter", sizeof(struct xicu_filter_param_s), xicufilter_param_binder),
	{ 0 }
};

const struct driver_s	xicu_filter_drv =
{
	.class           = device_class_icu,
	.id_table        = xicu_filter_ids,
	.f_irq           = xicu_filter_handler,
	.f_init          = xicu_filter_init,
	.f_cleanup       = xicu_filter_cleanup,
	.f.icu = {
		.f_enable    = xicu_filter_enable,
		.f_sethndl   = xicu_filter_sethndl,
		.f_delhndl   = xicu_filter_delhndl,
#ifdef CONFIG_HEXO_IPI
		.f_sendipi   = xicu_filter_sendipi,
		.f_setup_ipi_ep  = xicu_filter_setup_ipi_ep,
#endif
	}
};

REGISTER_DRIVER(xicu_filter_drv);

DEV_CLEANUP(xicu_filter_cleanup)
{
	struct xicu_filter_private_s	*pv = dev->drv_pv;

	mem_free(pv);
}

DEV_INIT(xicu_filter_init)
{
	struct xicu_filter_param_s *param = params;

/* 	printk("Creating an XICU filter device. Parent %p [%p] output %d @%p; icu %p/%d\n", */
/* 		   param->parent, param->parent->drv, */
/* 		   param->output_line, param->parent->addr[0], */
/* 		   dev->icudev, dev->irq); */

	struct xicu_filter_private_s *pv = mem_alloc(sizeof(*pv), mem_scope_sys);

	if ( !pv )
		return -ENOMEM;

	device_mem_map( dev , 1 );
	dev->drv = &xicu_filter_drv;
	dev->drv_pv = pv;

	pv->parent = param->parent;

	dev->addr[0] = PARENT(dev)->addr[0];

	pv->output = param->output_line;

	DEV_ICU_BIND(dev->icudev, dev, dev->irq, xicu_filter_handler);

	return 0;
}
