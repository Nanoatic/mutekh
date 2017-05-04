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

    Copyright (c) 2009, Nicolas Pouillon <nipo@ssji.net>
*/


#include <hexo/types.h>

#include <device/enum.h>
#include <device/device.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>

#include <string.h>

#include "enum-root.h"
//#include "enum-root-private.h"


/*
 * device open operation
 */

const struct driver_s	enum_root_drv =
{
	.class		= device_class_enum,
	.f_init		= enum_root_init,
	.f_cleanup		= enum_root_cleanup,
	.f.denum = {
		.f_lookup = enum_root_lookup,
	}
};

DEV_INIT(enum_root_init)
{
/* 	struct enum_root_context_s *pv; */

	dev->drv = &enum_root_drv;

	/* allocate private driver data */
/* 	pv = mem_alloc(sizeof(*pv), (mem_scope_sys)); */

/* 	if (!pv) */
/* 		return -1; */

/* 	dev->drv_pv = pv; */

	return 0;
}


/*
 * device close operation
 */

DEV_CLEANUP(enum_root_cleanup)
{
/* 	struct enum_root_context_s	*pv = dev->drv_pv; */

/* 	mem_free(pv); */
}

DEVENUM_LOOKUP(enum_root_lookup)
{
	return device_get_child(dev, ato_intl16(path));
}
