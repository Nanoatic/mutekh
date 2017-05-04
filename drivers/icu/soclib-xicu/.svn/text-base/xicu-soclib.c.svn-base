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
#include <mutek/printk.h>
#include <hexo/iospace.h>
#include <hexo/endian.h>
#include <hexo/ipi.h>
#include <device/driver.h>

DEVTIMER_SETCALLBACK(xicu_timer_setcallback)
{
	struct xicu_root_private_s *pv = dev->drv_pv;
	struct timer_handler_s *handler = &pv->timer_handlers[id];

	handler->hndl = callback;
	handler->data = priv;
	return 0;
}

DEVTIMER_SETPERIOD(xicu_timer_setperiod)
{
	cpu_mem_write_32(
		XICU_REG_ADDR(dev->addr[0],
					  XICU_PTI_PER,
					  id),
		endian_le32(period));

	cpu_mem_write_32(
		XICU_REG_ADDR(dev->addr[0],
					  XICU_MSK_PTI_ENABLE,
					  id),
		endian_le32(1<<id));
	return 0;
}

DEVTIMER_SETVALUE(xicu_timer_setvalue)
{
	cpu_mem_write_32(
		XICU_REG_ADDR(dev->addr[0],
					  XICU_PTI_VAL,
					  id),
		endian_le32(value));

	return 0;
}

DEVTIMER_GETVALUE(xicu_timer_getvalue)
{
	return endian_le32(
		cpu_mem_read_32(
			XICU_REG_ADDR(dev->addr[0],
						  XICU_PTI_VAL,
						  id)));
}


/* private functions */

void xicu_root_enable_hwi(struct device_s *dev,
						  uint_fast8_t input_line,
						  uint_fast8_t output_line,
						  bool_t enable)
{
	cpu_mem_write_32(
		XICU_REG_ADDR(dev->addr[0],
					  enable ? XICU_MSK_HWI_ENABLE : XICU_MSK_HWI_DISABLE,
					  output_line),
		endian_le32(1 << input_line));
}


#ifdef CONFIG_HEXO_IPI
void xicu_root_enable_ipi(struct device_s *dev,
						  uint_fast8_t input_line,
						  uint_fast8_t output_line,
						  bool_t enable)
{
	cpu_mem_write_32(
		XICU_REG_ADDR(dev->addr[0],
					  enable ? XICU_MSK_WTI_ENABLE : XICU_MSK_WTI_DISABLE,
					  output_line),
		endian_le32(1 << input_line));
}
#endif

error_t xicu_root_set_hwi_handler(struct device_s *dev,
								  uint_fast8_t input_line,
								  dev_irq_t *hndl,
								  void *data)
{
	struct xicu_root_private_s	*pv = dev->drv_pv;

	if ( input_line >= pv->input_lines )
		return -EINVAL;

	pv->hwi_handlers[input_line].hndl = hndl;
	pv->hwi_handlers[input_line].data = data;
	return 0;
}

#ifdef CONFIG_HEXO_IPI
error_t xicu_root_set_ipi_handler(struct device_s *dev,
								  uint_fast8_t input_line,
								  dev_irq_t *hndl,
								  void *data)
{
	struct xicu_root_private_s	*pv = dev->drv_pv;

	if ( input_line >= pv->ipis )
		return -EINVAL;

	pv->ipi_handlers[input_line].hndl = hndl;
	pv->ipi_handlers[input_line].data = data;
	return 0;
}

bool_t xicu_root_handle_ipi(struct device_s *dev, uint_fast8_t ipi)
{
	struct xicu_root_private_s *pv = dev->drv_pv;
	struct xicu_handler_s *handler = &pv->ipi_handlers[ipi];

	cpu_mem_read_32(XICU_REG_ADDR(dev->addr[0],
								  XICU_WTI_REG,
								  ipi));

	if (handler->hndl) {
	    handler->hndl(handler->data);
		return 0;
	}
	printk("XICU ipi lost irq\n");
	return 0;
}
#endif

bool_t xicu_root_handle_hwi(struct device_s *dev, uint_fast8_t id)
{
	struct xicu_root_private_s *pv = dev->drv_pv;
	struct xicu_handler_s *handler = &pv->hwi_handlers[id];

/* 	cpu_mem_read_32(XICU_REG_ADDR(timer_dev->addr[0], */
/* 								  XICU_HWI_ACK, */
/* 								  id)); */
/*     printk("xicu %p %d hwi: %p %p\n", dev, id, */
/*            handler->hndl, handler->data); */

	if (handler->hndl) {
	    handler->hndl(handler->data);
		return 0;
	}
	printk("XICU hwi lost irq\n");
	return 0;
}

bool_t xicu_root_handle_timer(
	struct device_s *dev, int_fast8_t id)
{
	struct xicu_root_private_s *pv = dev->drv_pv;
	struct timer_handler_s *handler = &pv->timer_handlers[id];

	cpu_mem_read_32(XICU_REG_ADDR(dev->addr[0],
								  XICU_PTI_ACK,
								  id));

	if (handler->hndl) {
	    handler->hndl(handler->data);
		return 0;
	}
	printk("XICU timer lost interrupt\n");
	return 0;
}

DEV_CLEANUP(xicu_root_cleanup)
{
	struct xicu_root_private_s	*pv = dev->drv_pv;

	mem_free(pv);
}

static const struct driver_param_binder_s xicu_root_binder[] =
{
	PARAM_BIND(struct xicu_root_param_s, input_lines, PARAM_DATATYPE_INT),
	PARAM_BIND(struct xicu_root_param_s, ipis, PARAM_DATATYPE_INT),
	PARAM_BIND(struct xicu_root_param_s, timers, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	xicu_root_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:xicu:root", sizeof(struct xicu_root_param_s), xicu_root_binder),
	{ 0 }
};

const struct driver_s	xicu_root_drv =
{
	.class           = device_class_timer,
	.id_table        = xicu_root_ids,
	.f_init          = xicu_root_init,
	.f_cleanup       = xicu_root_cleanup,
	.f.timer = {
		.f_setcallback	= xicu_timer_setcallback,
		.f_setperiod	= xicu_timer_setperiod,
		.f_setvalue		= xicu_timer_setvalue,
		.f_getvalue		= xicu_timer_getvalue,
	}
};

REGISTER_DRIVER(xicu_root_drv);

DEV_INIT(xicu_root_init)
{
	struct xicu_root_param_s *param = params;
	size_t priv_size = sizeof(struct xicu_root_private_s);
	priv_size += (param->input_lines + param->ipis + param->timers)
		* sizeof(struct xicu_handler_s);

	device_mem_map( dev , 1 );
	dev->drv = &xicu_root_drv;

	struct xicu_root_private_s *pv = mem_alloc(priv_size, mem_scope_sys);

	if ( !pv )
		return -ENOMEM;

	memset(pv, 0, priv_size);

	dev->drv_pv = pv;

	pv->input_lines = param->input_lines;
	pv->ipis = param->ipis;
	pv->timers = param->timers;

	pv->hwi_handlers = (struct xicu_handler_s*)(pv+1);
	pv->ipi_handlers = pv->hwi_handlers + pv->input_lines;
	pv->timer_handlers = (struct timer_handler_s*)(pv->ipi_handlers + pv->ipis);

	return 0;
}

