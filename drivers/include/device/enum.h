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

/**
 * @file
 * @module{Device drivers}
 * @short Enumerator driver API
 */                                                                 

#ifndef __DEVICE_ENUM_H__
#define __DEVICE_ENUM_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>

/**
   Lookup function prototype macro
 */
#define DEVENUM_LOOKUP(x) struct device_s *(x)(struct device_s *dev, const char *path)

/**
   Lookup function prototype. Lookup a device inside an enumerated
   device. The path parameter depends on the type of enumerator.

   @param dev The device to lookup from
   @param path The device to lookup for in dev
   @return a pointer to the found device, or NULL
 */
typedef DEVENUM_LOOKUP(devenum_lookup_t);

/**
   Lookup function shortcut
 */
#define dev_enum_lookup(dev, ...) (dev)->drv->f.denum.f_lookup(dev, __VA_ARGS__)

#define DEV_ENUM_MAX_PATH_LEN 32

struct dev_enum_info_s
{
    char path[DEV_ENUM_MAX_PATH_LEN];
};


/**
   Info function prototype macro
 */
#define DEVENUM_INFO(x) error_t (x)(struct device_s *dev,              \
                                    struct device_s *child,            \
                                    struct dev_enum_info_s *info)

/**
   Info function prototype. Queries information about a child of a
   given device.

   @param dev The device to info from
   @param child The child of @tt dev to query information about
   @param info The caller-allocated information structure to return
   information in
   @return 0 if done, or an error.
 */
typedef DEVENUM_INFO(devenum_info_t);

/**
   Info function shortcut
 */
#define dev_enum_info(dev, ...) (dev)->drv->f.denum.f_info(dev, __VA_ARGS__)

struct dev_class_enum_s
{
	devenum_lookup_t *f_lookup;
	devenum_info_t *f_info;
};

#endif
