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

#include <hexo/types.h>

#include <device/gpio.h>
#include <device/input.h>
#include <device/device.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>

#include "mt5-f.h"
#include "mt5-f-private.h"

DEVINPUT_INFO(dev_mt5f_info)
{
	info->name = "mt5f";
	info->ctrl_button_count = 5;
}

static inline uint_fast8_t mt5f_get_state(struct device_s *dev)
{
	struct mt5f_context_s *pv = dev->drv_pv;
	return 0
		| (dev_gpio_get_value(pv->gpio_dev, pv->a) << MT5F_LEFT)
		| (dev_gpio_get_value(pv->gpio_dev, pv->b) << MT5F_DOWN)
		| (dev_gpio_get_value(pv->gpio_dev, pv->c) << MT5F_UP)
		| (dev_gpio_get_value(pv->gpio_dev, pv->d) << MT5F_RIGHT)
		| (dev_gpio_get_value(pv->gpio_dev, pv->common) << MT5F_BUTTON)
		;
}

DEVINPUT_READ(dev_mt5f_read)
{
	return 1 & (mt5f_get_state(dev) >> id);
}

DEVINPUT_WRITE(dev_mt5f_write)
{
	return ENOTSUP;
}

DEVINPUT_SETCALLBACK(dev_mt5f_setcallback)
{
	struct mt5f_context_s *pv = dev->drv_pv;

	pv->callback = callback;
	pv->priv = priv;

	return 0;
}

static DEVGPIO_IRQ(mt5f_state_changed)
{
	struct device_s *me = priv;
	struct mt5f_context_s *pv = me->drv_pv;
	
	uint_fast8_t new_state = mt5f_get_state(me);
	uint_fast8_t diff = new_state ^ pv->last_sate;
	pv->last_sate = new_state;

	if ( ! diff || !pv->callback )
		return;

	while ( diff ) {
		uint_fast8_t id = __builtin_ctz(diff);

		pv->callback(id, !!(new_state & (1<<id)), pv->priv);

		diff &= ~(1<<id);
	}
}

#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct driver_param_binder_s dev_mt5f_binder[] =
{
	PARAM_BIND(struct dev_mt5f_param_s, gpio_dev, PARAM_DATATYPE_DEVICE_PTR),
	PARAM_BIND(struct dev_mt5f_param_s, a, PARAM_DATATYPE_INT),
	PARAM_BIND(struct dev_mt5f_param_s, b, PARAM_DATATYPE_INT),
	PARAM_BIND(struct dev_mt5f_param_s, c, PARAM_DATATYPE_INT),
	PARAM_BIND(struct dev_mt5f_param_s, d, PARAM_DATATYPE_INT),
	PARAM_BIND(struct dev_mt5f_param_s, common, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	dev_mt5f_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("mt5f", sizeof(struct dev_mt5f_param_s), dev_mt5f_binder),
	{ 0 }
};
#endif

const struct driver_s   mt5f_drv =
{
    .class      = device_class_input,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = dev_mt5f_ids,
#endif
    .f_init     = dev_mt5f_init,
    .f_cleanup  = dev_mt5f_cleanup,
	.f.input = {
		.f_info = dev_mt5f_info,
		.f_read = dev_mt5f_read,
		.f_write = dev_mt5f_write,
		.f_setcallback = dev_mt5f_setcallback,
	},
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(mt5f_drv);
#endif

DEV_INIT(dev_mt5f_init)
{
	struct mt5f_context_s *pv;
	struct dev_mt5f_param_s *param = params;

	dev->drv = &mt5f_drv;

	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

	if (!pv)
		return -1;

	dev->drv_pv = pv;

	pv->gpio_dev = param->gpio_dev;
	pv->a = param->a;
	pv->b = param->b;
	pv->c = param->c;
	pv->d = param->d;
	pv->common = param->common;
	pv->callback = NULL;
	pv->priv = NULL;

	device_obj_refnew(pv->gpio_dev);

	dev_gpio_set_way(pv->gpio_dev, pv->a, GPIO_WAY_INPUT_FILTERED);
	dev_gpio_set_way(pv->gpio_dev, pv->b, GPIO_WAY_INPUT_FILTERED);
	dev_gpio_set_way(pv->gpio_dev, pv->c, GPIO_WAY_INPUT_FILTERED);
	dev_gpio_set_way(pv->gpio_dev, pv->d, GPIO_WAY_INPUT_FILTERED);
	dev_gpio_set_way(pv->gpio_dev, pv->common, GPIO_WAY_INPUT_FILTERED);

	dev_gpio_assign_to_peripheral(pv->gpio_dev, pv->a, 0);
	dev_gpio_assign_to_peripheral(pv->gpio_dev, pv->b, 0);
	dev_gpio_assign_to_peripheral(pv->gpio_dev, pv->c, 0);
	dev_gpio_assign_to_peripheral(pv->gpio_dev, pv->d, 0);
	dev_gpio_assign_to_peripheral(pv->gpio_dev, pv->common, 0);

	dev_gpio_register_irq(pv->gpio_dev, pv->a, GPIO_EVENT_ALL, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->b, GPIO_EVENT_ALL, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->c, GPIO_EVENT_ALL, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->d, GPIO_EVENT_ALL, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->common, GPIO_EVENT_ALL, mt5f_state_changed, dev);

	pv->last_sate = mt5f_get_state(dev);

	return 0;
}

DEV_CLEANUP(dev_mt5f_cleanup)
{
    struct mt5f_context_s *pv = dev->drv_pv;

	dev_gpio_register_irq(pv->gpio_dev, pv->a, GPIO_NONE, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->b, GPIO_NONE, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->c, GPIO_NONE, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->d, GPIO_NONE, mt5f_state_changed, dev);
	dev_gpio_register_irq(pv->gpio_dev, pv->common, GPIO_NONE, mt5f_state_changed, dev);

	device_obj_refdrop(pv->gpio_dev);

    mem_free(pv);
}
