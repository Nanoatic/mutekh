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


#ifndef DRIVER_ICU_apic_H_
#define DRIVER_ICU_apic_H_

#include <device/icu.h>
#include <device/device.h>

#include <hexo/local.h>

extern CPU_LOCAL struct device_s apic_dev;

/* icu device functions */

DEV_INIT(icu_apic_init);
DEVICU_ENABLE(icu_apic_enable);
DEVICU_SETHNDL(icu_apic_sethndl);
DEVICU_DELHNDL(icu_apic_delhndl);
DEV_CLEANUP(icu_apic_cleanup);

#endif

