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

#ifndef DRIVER_GPIO_SAM7_H_
#define DRIVER_GPIO_SAM7_H_

#include <device/gpio.h>

struct gpio_sam7_param_s
{
	uint_fast8_t lun_count;
};

DEVGPIO_SET_WAY(gpio_sam7_set_way);
DEVGPIO_SET_VALUE(gpio_sam7_set_value);
DEVGPIO_SET_PULLUP(gpio_sam7_set_pullup);
DEVGPIO_ASSIGN_TO_PERIPHERAL(gpio_sam7_assign_to_peripheral);
DEVGPIO_GET_VALUE(gpio_sam7_get_value);
DEVGPIO_REGISTER_IRQ(gpio_sam7_register_irq);
DEV_INIT(gpio_sam7_init);
DEV_CLEANUP(gpio_sam7_cleanup);

#endif

