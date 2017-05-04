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

#ifndef DRIVER_PITC_6079A_H_
#define DRIVER_PITC_6079A_H_

#include <device/timer.h>
#include <device/device.h>

/* timer device functions */

DEVTIMER_SETCALLBACK(pitc_6079a_setcallback);
DEVTIMER_SETPERIOD(pitc_6079a_setperiod);
DEVTIMER_SETVALUE(pitc_6079a_setvalue);
DEVTIMER_GETVALUE(pitc_6079a_getvalue);
DEV_IRQ(pitc_6079a_irq);
DEV_CLEANUP(pitc_6079a_cleanup);
DEV_INIT(pitc_6079a_init);

#endif

