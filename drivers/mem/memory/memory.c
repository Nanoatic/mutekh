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

#include <device/mem.h>
#include <device/device.h>
#include <device/driver.h>

#include "memory.h"

DEVMEM_GET_INFO(memory_get_info)
{
	uint32_t flags = (uint32_t)dev->drv_pv;

	info->base = dev->addr[DEV_MEMORY_ADDR_BEGIN];
	info->size = dev->addr[DEV_MEMORY_ADDR_END] - dev->addr[DEV_MEMORY_ADDR_BEGIN];
	info->flags = flags;
}

static const struct driver_param_binder_s memory_param_binder[] =
{
	PARAM_BIND(struct dev_memory_param_s, cached, PARAM_DATATYPE_BOOL),
	PARAM_BIND(struct dev_memory_param_s, coherent, PARAM_DATATYPE_BOOL),
	{ 0 }
};

static const struct devenum_ident_s	memory_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("memory", sizeof(struct dev_memory_param_s), memory_param_binder),
	{ 0 }
};

const struct driver_s   memory_drv =
{
    .class      = device_class_mem,
	.id_table	= memory_ids,
    .f_init     = memory_init,
    .f_cleanup  = memory_cleanup,
	.f.mem = {
		.f_get_info = memory_get_info,
	},
};

REGISTER_DRIVER(memory_drv);

DEV_INIT(memory_init)
{
	struct dev_memory_param_s *param = params;

	dev->drv = &memory_drv;

	uint32_t flags = 0;
	if ( param->coherent )
		flags |= DEV_MEM_CACHED|DEV_MEM_COHERENT;
	if ( param->cached )
		flags |= DEV_MEM_CACHED;

	dev->drv_pv = (void*)flags;

	return 0;
}

DEV_CLEANUP(memory_cleanup)
{
}

