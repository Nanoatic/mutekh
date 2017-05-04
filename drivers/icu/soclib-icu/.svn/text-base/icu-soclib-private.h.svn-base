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

#ifndef __ICU_soclib_PRIVATE_H_
#define __ICU_soclib_PRIVATE_H_

#include <device/icu.h>
#include <device/device.h>

struct icu_soclib_handler_s
{
  dev_irq_t		*hndl;
  void			*data;
};

#define ICU_SOCLIB_MAX_VECTOR	32

#define ICU_SOCLIB_REG_ISR	0
#define ICU_SOCLIB_REG_IER_RO	4
#define ICU_SOCLIB_REG_IER_SET	8
#define ICU_SOCLIB_REG_IER_CLR	12
#define ICU_SOCLIB_REG_VECTOR	16

struct icu_soclib_private_s
{
  struct icu_soclib_handler_s	table[ICU_SOCLIB_MAX_VECTOR];
  struct device_s		*dev;
};

#endif

