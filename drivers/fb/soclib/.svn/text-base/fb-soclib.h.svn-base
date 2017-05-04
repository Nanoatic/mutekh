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

    This driver is based on ssocliblib code (http://www.ssocliblib.org/)

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef DRIVER_FB_SOCLIBFB_H_
#define DRIVER_FB_SOCLIBFB_H_

#include <device/fb.h>
#include <device/device.h>

/* fb device functions */

//DEV_IRQ(fb_soclib_irq);
DEV_INIT(fb_soclib_init);
DEV_CLEANUP(fb_soclib_cleanup);
DEVFB_SETMODE(fb_soclib_setmode);
DEVFB_GETBUFFER(fb_soclib_getbuffer);
DEVFB_FLIPPAGE(fb_soclib_flippage);
DEVFB_SETPALETTE(fb_soclib_setpalette);

#endif

