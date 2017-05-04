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

#ifndef __ENUM_FDT_PRIVATE_H_
#define __ENUM_FDT_PRIVATE_H_

#if 0
#include <mutek/printk.h>
#define dprintk(x...) do{ printk(x); }while(0)
#else
#define dprintk(x...) do{}while(0)
#endif

#define ENUM_FDT_PATH_MAXLEN 32

CONTAINER_TYPE(fdt_node, CLIST,
struct enum_pv_fdt_s
{
	fdt_node_entry_t list_entry;
	struct device_s *dev;
	const char *device_type;
	char device_path[ENUM_FDT_PATH_MAXLEN];
	uint32_t offset;
	uint32_t phandle;
	union {
		struct {
			uint32_t cpuid;
		};
	};
	uint8_t addr_cells;
	uint8_t size_cells;
}
, list_entry)

CONTAINER_FUNC(fdt_node, CLIST, static inline, fdt_node, list_entry);

struct enum_fdt_context_s
{
	void *blob;
	const char *console_path;
	fdt_node_root_t devices;
};

void enum_fdt_create_children(struct device_s *dev);
error_t enum_fdt_register_one(struct device_s *dev, struct device_s *item);

struct device_s *enum_fdt_get_at_offset(struct device_s *dev, uint32_t offset);
struct device_s *enum_fdt_lookup(struct device_s *dev, const char *path);
error_t enum_fdt_use_drv(
	struct device_s *enum_dev,
	struct device_s *dev,
	const struct driver_s *drv);



#endif

