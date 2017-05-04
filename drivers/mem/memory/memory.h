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

#ifndef DRIVER_MEMORY_H_
#define DRIVER_MEMORY_H_

#include <device/mem.h>
#include <device/device.h>

DEV_INIT(memory_init);
DEV_CLEANUP(memory_cleanup);
DEV_IRQ(memory_irq);
DEVMEM_GET_INFO(memory_get_info);

struct dev_memory_param_s
{
	bool_t coherent;
	bool_t cached;
};

#define DEV_MEMORY_ADDR_BEGIN 0
#define DEV_MEMORY_ADDR_END 1

#endif

