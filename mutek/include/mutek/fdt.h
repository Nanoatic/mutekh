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

    Copyright Nicolas Pouillon, <nipo@ssji.net>, 2010
*/

#ifndef MUTEK_FDT_H_
#define MUTEK_FDT_H_

/**
 * @file
 * @module{Mutek}
 * @short FDT consumer API
 */

#include <hexo/types.h>
#include <hexo/error.h>
#include <device/device.h>

/**
   @this parses the device-tree pointed by dt. It uses the device-tree
   to create CPU local storage areas, logically connects IPI devices,
   and retrieves "/chosen" nodes to take references to correct device
   structures.

   @param dt Pointer to a device tree
   @param enum_dev enum-fdt device enumerator already initialized from
   the same device-tree
 */
void mutek_parse_fdt(void *dt, struct device_s *enum_dev);

#endif
