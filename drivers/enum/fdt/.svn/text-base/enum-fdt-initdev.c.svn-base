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

#include <fdt/reader.h>

#include <mutek/printk.h>

#include "enum-fdt.h"
#include "enum-fdt-private.h"

struct initdev_state_s
{
	struct device_s *enum_dev;
	struct device_s *dev;
	const struct devenum_ident_s *ident;
	uint8_t *param;
	error_t err;
};


static void parse_icudev( struct device_s *enum_dev,
					   struct device_s *dev,
					   const uint32_t *data,
					   size_t datalen )
{
	if ( datalen < 4 )
		return;

	dprintk("Getting icudev with path %s... ", data);
    struct device_s *icu = enum_fdt_lookup(enum_dev, (const char*)data);
	dprintk("got %p\n", icu);

    if ( icu->drv != NULL && icu->drv->class != device_class_icu )
        printk("Warning: %s is not an ICU, expect crashes\n", (const char *)data);
	dev->icudev = icu;
}

static void parse_reg( struct device_s *dev, const void *data, size_t datalen )
{
	struct enum_pv_fdt_s *pv = dev->enum_pv;
	uint_fast8_t i;
	const void *ptr = data;

	for ( i=0; i<DEVICE_MAX_ADDRSLOT; ++i ) {
		if ( ptr > (void*)((uintptr_t)data+datalen) )
			break;
		ptr = fdt_parse_sized( pv->addr_cells, ptr,
						   sizeof(dev->addr[i]), &dev->addr[i] );
		ptr = fdt_parse_sized( pv->size_cells, ptr,
						   0, NULL );
	}
}

static void parse_reg_size( struct device_s *dev, const void *data, size_t datalen )
{
	struct enum_pv_fdt_s *pv = dev->enum_pv;
	uint_fast8_t i;
	const void *ptr = data;

	for ( i=0; i<DEVICE_MAX_ADDRSLOT; i+=2 ) {
		if ( ptr > (void*)((uintptr_t)data+datalen) )
			break;
		ptr = fdt_parse_sized( pv->addr_cells, ptr,
						   sizeof(*dev->addr), &dev->addr[i] );
		ptr = fdt_parse_sized( pv->size_cells, ptr,
						   sizeof(*dev->addr), &dev->addr[i+1] );
		dev->addr[i+1] += dev->addr[i];
	}
}

static FDT_ON_NODE_ENTRY_FUNC(initdev_node_entry)
{
	struct initdev_state_s *pv = priv;
	struct enum_pv_fdt_s *enum_pv = pv->dev->enum_pv;
	const struct driver_param_binder_s *binder = pv->ident->fdtname.binder;
	
	{
		const void *value = NULL;
		size_t len;

		if ( fdt_reader_has_prop(state, "icudev", &value, &len ) ) {
			printk("Warning: icudev/irq couple got deprecated in favor of irq = <&{/dev} irq_no>\n");
			parse_icudev( pv->enum_dev, pv->dev, value, len );
		} else
			pv->dev->icudev = NULL;

		if ( fdt_reader_has_prop(state, "irq", &value, &len ) ) {
			if ( len > 4 ) {
				// New mode = <phandle irq>
				uint32_t phandle = endian_be32(*(uint32_t*)value);
				pv->dev->irq = endian_be32(*((uint32_t*)value + 1));
				fdt_parse_sized( 1, value, sizeof(phandle), &phandle );
				pv->dev->icudev = enum_fdt_get_phandle(pv->enum_dev, phandle);
			} else {
				printk("Warning: icudev/irq couple got deprecated in favor of irq = <&{/dev} irq_no>\n");
				pv->dev->irq = endian_be32(*(uint32_t*)value);
			}
		} else
			pv->dev->irq = -1;

		if ( fdt_reader_has_prop(state, "reg", &value, &len ) ) {
			if ( !strcmp(enum_pv->device_type, "memory") )
				parse_reg_size( pv->dev, value, len );
			else
				parse_reg( pv->dev, value, len );
		}
	}

	/*
	  If the icu controller is not initialized yet, try to do it now
	 */
	if ( pv->dev->icudev && !pv->dev->icudev->drv ) {
		error_t err = 
			enum_fdt_register_one(pv->enum_dev, pv->dev->icudev);
		if (err) {
			printk("lzkjeflzkjf %d\n", err);
			pv->err = err;
			return 0;
		}
	}

	if ( binder ) {
		dprintk("  has a binder\n");
		for ( ; binder->param_name; binder++ ) {
			const void *value = NULL;
			size_t len;
			dprintk("   considering parameter %s, type=%d, off=%d, size=%d... ",
					binder->param_name, binder->datatype,
					binder->struct_offset, binder->datalen);
			if ( fdt_reader_has_prop(state, binder->param_name, &value, &len ) ) {
				dprintk("%P\n", value, len);
				switch (binder->datatype) {
				case PARAM_DATATYPE_BOOL:
					*(bool_t*)(pv->param + binder->struct_offset) = 1;
					break;
				case PARAM_DATATYPE_INT:
					fdt_parse_sized( 1, value,
								 binder->datalen, pv->param + binder->struct_offset );
					break;
				case PARAM_DATATYPE_DEVICE_PTR:
				{
					struct device_s *ref = enum_fdt_lookup(pv->enum_dev, value);
					if ( ! ref->drv )
						enum_fdt_register_one(pv->enum_dev, ref);
					*(struct device_s **)(pv->param + binder->struct_offset) =
						ref;
					break;
				}
				case PARAM_DATATYPE_ADDR:
					fdt_parse_sized( enum_pv->addr_cells, value,
								 binder->datalen, pv->param + binder->struct_offset );
					break;
				}
			} else {
				dprintk("no value\n");
				switch (binder->datatype) {
				case PARAM_DATATYPE_BOOL:
					*(bool_t*)(pv->param + binder->struct_offset) = 0;
					break;
				default:
#if 1
					printk("  Warning: parameter %s not found for device %s, trying without\n", binder->param_name, enum_pv->device_path);
#else
					pv->err = ENOENT;
					return 0;
#endif
				}
			}
		}
	}

	return 0;
}

error_t enum_fdt_use_drv(
	struct device_s *enum_dev,
	struct device_s *dev,
	const struct driver_s *drv)
{
	struct enum_pv_fdt_s *enum_pv = dev->enum_pv;
	struct enum_fdt_context_s *pv = enum_dev->drv_pv;
	const struct devenum_ident_s *ident = drv->id_table;

	assert(ident);

	dprintk(" Considering usage of driver %p for %s\n",
		   drv,
		   enum_pv->device_path);


	for ( ; ident->type != 0; ident++ ) {
		if ( (ident->type == DEVENUM_TYPE_FDTNAME)
			 && !strcmp(ident->fdtname.name, enum_pv->device_type) )
			break;
	}

	assert( !strcmp(ident->fdtname.name, enum_pv->device_type) );

	uint8_t param[ident->fdtname.param_size];

	dprintk("  param size: %d\n", ident->fdtname.param_size);

	struct initdev_state_s priv = {
		.enum_dev = enum_dev,
		.dev = dev,
		.ident = ident,
		.param = param,
		.err = 0,
	};

	struct fdt_walker_s walker = {
		.priv = &priv,
		.on_node_entry = initdev_node_entry,
		.on_node_leave = NULL,
		.on_node_prop = NULL,
		.on_mem_reserve = NULL,
	};

	const char *reason = "walking";
	error_t err = fdt_walk_blob_from(pv->blob, &walker, enum_pv->offset);

//	printk("end walk\n");

	if ( !err ) {
		reason = "inside";
		err = priv.err;
	}

	if ( !err ) {
		dprintk("Initializing device %s with driver %p, icu %p, irq: %d\n",
				enum_pv->device_path,
				drv, dev->icudev, dev->irq);
		reason = "driver";
		err = drv->f_init(dev, param);
	}
	if ( err )
		printk("Initializing device %s with driver %p, icu %p, irq: %d failed in %s: %d\n",
			   enum_pv->device_path,
			   drv, dev->icudev, dev->irq,
			   reason, err);
	return err;
}

