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

#ifndef __ENUM_FDT_H_
#define __ENUM_FDT_H_

#include <device/enum.h>
#include <device/device.h>

#include <stdint.h>

DEV_CLEANUP(enum_fdt_cleanup);
DEV_INIT(enum_fdt_init);
DEVENUM_LOOKUP(enum_fdt_lookup);
struct device_s *enum_fdt_icudev_for_cpuid(struct device_s *dev, cpu_id_t id);
struct device_s *enum_fdt_get_phandle(struct device_s *dev, uint32_t phandle);

#endif

