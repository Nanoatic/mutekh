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

    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009
*/

/**
 * @file
 * @module{Device drivers}
 * @short Memory device driver API
 */

#ifndef __DEVICE_MEM_H__
#define __DEVICE_MEM_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>

struct device_s;
struct driver_s;

#define DEV_MEM_CACHED   1
#define DEV_MEM_COHERENT 2

struct dev_mem_info_s
{
	paddr_t base;
	paddr_t size;
	uint16_t flags;
};


/** Mem device class get_info() function tempate. */
#define DEVMEM_GET_INFO(n)	void  (n) (struct device_s *dev, struct dev_mem_info_s *info)

/** Mem device class request() methode shortcut */
#define dev_mem_get_info(dev, ...) (dev)->drv->f.mem.f_get_info(dev, __VA_ARGS__ )

/**
   Mem device class get_info() function type. Get information about a ram device.

   @param dev pointer to device descriptor
   @param info pointer to a yet-to-fill info structure
*/
typedef DEVMEM_GET_INFO(devmem_get_info_t);


/** Mem device class methodes */
struct dev_class_mem_s
{
  devmem_get_info_t		*f_get_info;
};

#endif

