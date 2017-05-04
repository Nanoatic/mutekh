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

#ifndef __TIMER_8253_PRIVATE_H_
#define __TIMER_8253_PRIVATE_H_

#include <device/timer.h>

/* timer device registers addresses */
#define TIMER_8253_CHANID(n)		((n) << 6)
#define TIMER_8253_CTRL_LOADMODE_READ	0x00
#define TIMER_8253_CTRL_LOADMODE_HIGH	0x10
#define TIMER_8253_CTRL_LOADMODE_LOW	0x20
#define TIMER_8253_CTRL_LOADMODE_HILO	0x30
#define TIMER_8253_CTRL_CNTMODE_IRQTERM	0x00
#define TIMER_8253_CTRL_CNTMODE_ONESHOT	0x02
#define TIMER_8253_CTRL_CNTMODE_RATEGEN	0x04
#define TIMER_8253_CTRL_CNTMODE_SQWRGEN	0x06
#define TIMER_8253_CTRL_CNTMODE_SOFTSTR	0x08
#define TIMER_8253_CTRL_CNTMODE_HARDSTR	0x0a
#define TIMER_8253_CTRL_BCD		0x01

/* timer private context */
#define TIMER_8253_IDCOUNT	256

struct			timer_8253_context_s
{
  devtimer_callback_t	*cb[TIMER_8253_IDCOUNT];
  void			*pv[TIMER_8253_IDCOUNT];
};

#endif

