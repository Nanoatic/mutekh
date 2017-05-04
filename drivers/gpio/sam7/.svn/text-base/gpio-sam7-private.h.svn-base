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

#ifndef GPIO_SAM7_PRIVATE_H_
#define GPIO_SAM7_PRIVATE_H_

#include <hexo/types.h>

struct gpio_sam7_handler_s
{
	devgpio_irq_t *func;
	void *priv;
};

struct gpio_sam7_context_s
{
	struct gpio_sam7_handler_s handler[32];
	uint32_t up_mask;
	uint32_t down_mask;
};

#endif
