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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net> 2009

*/


#include <hexo/types.h>

#include <device/gpio.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/endian.h>

#include <assert.h>

#include "gpio-sam7.h"
#include "gpio-sam7-private.h"

#include "arch/sam7/at91sam7x256.h"

DEVGPIO_SET_WAY(gpio_sam7_set_way)
{
	AT91PS_PIO registers = (void*)dev->addr[0];
	
	switch (way) {
	case GPIO_WAY_INPUT:
		registers->PIO_ODR = 1 << gpio;
		registers->PIO_IFDR = 1 << gpio;
		break;
	case GPIO_WAY_OUTPUT:
		registers->PIO_OER = 1 << gpio;
		break;
	case GPIO_WAY_INPUT_FILTERED:
		registers->PIO_ODR = 1 << gpio;
		registers->PIO_IFER = 1 << gpio;
		break;
	case GPIO_WAY_OPENDRAIN:
		registers->PIO_OER = 1 << gpio;
		registers->PIO_MDER = 1 << gpio;
		break;
	}
	return 0;
}


DEVGPIO_SET_VALUE(gpio_sam7_set_value)
{
	AT91PS_PIO registers = (void*)dev->addr[0];

	if (value)
		registers->PIO_SODR = 1 << gpio;
	else
		registers->PIO_CODR = 1 << gpio;

	return 0;
}


DEVGPIO_SET_PULLUP(gpio_sam7_set_pullup)
{
	AT91PS_PIO registers = (void*)dev->addr[0];

	if (pullup)
		registers->PIO_PPUER = 1 << gpio;
	else
		registers->PIO_PPUDR = 1 << gpio;

	return 0;
}


DEVGPIO_ASSIGN_TO_PERIPHERAL(gpio_sam7_assign_to_peripheral)
{
	AT91PS_PIO registers = (void*)dev->addr[0];

	switch ( device_id ) {
	case 0:
		registers->PIO_PER = 1 << gpio;
		break;

	case 1:
		registers->PIO_PDR = 1 << gpio;
		registers->PIO_ASR = 1 << gpio;
		break;

	case 2:
		registers->PIO_PDR = 1 << gpio;
		registers->PIO_BSR = 1 << gpio;
		break;

	case 3:
		return EINVAL;
	}

	return 0;
}


DEVGPIO_GET_VALUE(gpio_sam7_get_value)
{
	AT91PS_PIO registers = (void*)dev->addr[0];

	return (registers->PIO_PDSR >> gpio) & 1;
}


DEVGPIO_REGISTER_IRQ(gpio_sam7_register_irq)
{
    struct gpio_sam7_context_s *pv = dev->drv_pv;
	AT91PS_PIO registers = (void*)dev->addr[0];

	switch ( event ) {
	case GPIO_NONE:
		registers->PIO_IDR = 1<<gpio;
		pv->down_mask &= ~(1<<gpio);
		pv->up_mask &= ~(1<<gpio);
		return 0;

	case GPIO_VALUE_UP:
	case GPIO_EDGE_RAISING:
		pv->up_mask |= 1<<gpio;
		goto enable;

	case GPIO_VALUE_DOWN:
	case GPIO_EDGE_FALLING:
		pv->down_mask |= 1<<gpio;
		goto enable;

	case GPIO_EVENT_ALL:
		pv->up_mask |= 1<<gpio;
		pv->down_mask |= 1<<gpio;
		goto enable;

	enable:
		pv->handler[gpio].func = callback;
		pv->handler[gpio].priv = private_data;
		registers->PIO_IER = 1<<gpio;
		return 0;
	}

	return EINVAL;
}


DEV_IRQ(gpio_sam7_irq)
{
    struct gpio_sam7_context_s *pv = dev->drv_pv;
	AT91PS_PIO registers = (void*)dev->addr[0];

	uint32_t state = registers->PIO_ISR;
	uint32_t value = registers->PIO_PDSR;
	uint32_t mask = pv->down_mask | pv->up_mask;

	if ( !state )
		return 0;

	while (state & mask) {
		devgpio_id_t gpio = __builtin_ctz(state & mask);
		uint32_t mgpio = 1<<gpio;
		struct gpio_sam7_handler_s *h = &pv->handler[gpio];

		h->func(dev,
				gpio,
				(value & mgpio) ? GPIO_VALUE_UP : GPIO_VALUE_DOWN,
				h->priv);

		state &= ~mgpio;
	}

	return 1;
}


#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct devenum_ident_s	gpio_sam7_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("sam7:gpio", 0, 0),
	{ 0 }
};
#endif

const struct driver_s   gpio_sam7_drv =
{
    .class      = device_class_gpio,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = gpio_sam7_ids,
#endif
    .f_init     = gpio_sam7_init,
    .f_cleanup  = gpio_sam7_cleanup,
    .f_irq      = gpio_sam7_irq,
	.f.gpio = {
		.f_set_way = gpio_sam7_set_way,
		.f_set_value = gpio_sam7_set_value,
		.f_set_pullup = gpio_sam7_set_pullup,
		.f_assign_to_peripheral = gpio_sam7_assign_to_peripheral,
		.f_get_value = gpio_sam7_get_value,
		.f_register_irq = gpio_sam7_register_irq,
	},
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(gpio_sam7_drv);
#endif

DEV_INIT(gpio_sam7_init)
{
	struct gpio_sam7_context_s *pv;
	AT91PS_PIO registers = (void*)dev->addr[0];

	dev->drv = &gpio_sam7_drv;

	AT91C_BASE_PMC->PMC_PCER = 1 << dev->irq;

	registers->PIO_IDR = ~0;

	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

	memset(pv, 0, sizeof(*pv));

	if (!pv)
		return -1;

	dev->drv_pv = pv;

	dev_icu_sethndl(dev->icudev, dev->irq, gpio_sam7_irq, dev);
	dev_icu_enable(dev->icudev, dev->irq, 1, AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL | 0x2);

	return 0;
}

DEV_CLEANUP(gpio_sam7_cleanup)
{
    struct gpio_sam7_context_s *pv = dev->drv_pv;

    DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, gpio_sam7_irq);

    mem_free(pv);
}

