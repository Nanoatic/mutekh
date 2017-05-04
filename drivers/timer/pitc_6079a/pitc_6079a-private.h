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

    Copyright (c) 2009, Nicolas Pouillon, <nipo@ssji.net>
*/

#ifndef __PITC_6079A_PRIVATE_H_
#define __PITC_6079A_PRIVATE_H_

#include <device/timer.h>
#include <device/device.h>

#define PITC_PITEN     (1<<24)
#define PITC_PITIEN    (1<<25)
#define PITC_CPIV_MASK (0xfffff)

struct pitc_6079a_context_s
{
	devtimer_callback_t	*cb;
	void			*pv;
};

#define REG_MR 0x0
#define REG_SR 0x4
#define REG_VR 0x8
#define REG_IR 0xc

#endif
