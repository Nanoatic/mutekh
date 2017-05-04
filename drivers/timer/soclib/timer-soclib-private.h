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

#ifndef __TIMER_SOCLIB_PRIVATE_H_
#define __TIMER_SOCLIB_PRIVATE_H_

#include <device/timer.h>
#include <device/device.h>

/* timer device registers addresses */
#define TIMER_SOCLIB_REG_VALUE		0
#define TIMER_SOCLIB_REG_MODE		4
# define TIMER_SOCLIB_REG_MODE_EN	0x01
# define TIMER_SOCLIB_REG_MODE_IRQEN	0x02
#define TIMER_SOCLIB_REG_PERIOD		8
#define TIMER_SOCLIB_REG_IRQ		12
#define TIMER_SOCLIB_REGSPACE_SIZE	16

/* timer private context */
#define TIMER_SOCLIB_IDCOUNT	256

struct			timer_soclib_context_s
{
  devtimer_callback_t	*cb[TIMER_SOCLIB_IDCOUNT];
  void			*pv[TIMER_SOCLIB_IDCOUNT];
};

#endif

