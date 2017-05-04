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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net>, 2009

*/

#ifndef __ICU_ARM_PRIVATE_H_
#define __ICU_ARM_PRIVATE_H_

#include <device/icu.h>
#include <device/device.h>

struct icu_arm_handler_s
{
  dev_irq_t		*hndl;
  void			*data;
};

#define ICU_ARM_MAX_VECTOR	1

struct icu_arm_private_s
{
  struct icu_arm_handler_s	table[ICU_ARM_MAX_VECTOR];
};

#endif

