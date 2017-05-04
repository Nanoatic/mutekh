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
    Copyright (c) 2009, Nicolas Pouillon <nipo@ssji.net>
*/


#ifndef DRIVER_XICU_soclib_H_
#define DRIVER_XICU_soclib_H_

#include <device/timer.h>
#include <device/icu.h>
#include <device/device.h>

/*
  Xicu is a tricky device. It is a composite device with
  * programmable timers
  * IPI endpoints
  * Hardware irq lines
  * A multi-output ICU concentrating all these

  In MutekH, devices must have only one output IRQ line, and devices
  can only be one class at a time.  Hopefully, ICU class handles IPIs,
  and there is actually one ICU per output line. So we'll have
  (n_output_lines + 1) devices for handling the Xicu.
  
  There will be:
  * 1 device in the timer class, handled by the code in xicu-soclib.c,
    this device is the root of the Xicu code, and actually handles
    all the IRQ routing;
  * n_output_lines "filter" devices in the icu class handling the IRQ
    routing for each output line, these devices actually rely on the
    root.

  First device to initialize is the "root" (timer) one. It only needs
  the following attributes:
  * count of hardware irq lines
  * count of timers
  * count of IPI endpoints
  * address of device
  * (no ICU device is needed)

  Then, initialize the "filter" devices. The following attributes are
  needed:
  * output line number in the xicu
  * parent "root" device reference
  * a valid icu device and irq
  * (no address is needed)
 */

/* root dev */

DEV_CLEANUP(xicu_root_cleanup);
DEV_INIT(xicu_root_init);

struct xicu_root_param_s
{
	size_t input_lines;
	size_t ipis;
	size_t timers;
};

/* timer device functions */

DEVTIMER_SETCALLBACK(xicu_timer_setcallback);
DEVTIMER_SETPERIOD(xicu_timer_setperiod);
DEVTIMER_SETVALUE(xicu_timer_setvalue);
DEVTIMER_GETVALUE(xicu_timer_getvalue);

/* icu (filter) device functions */

DEV_INIT(xicu_filter_init);
DEV_CLEANUP(xicu_filter_cleanup);

DEVICU_ENABLE(xicu_filter_enable);
DEVICU_SETHNDL(xicu_filter_sethndl);
DEVICU_DELHNDL(xicu_filter_delhndl);
DEVICU_SENDIPI(xicu_filter_sendipi);
DEVICU_SETUP_IPI_EP(xicu_filter_setup_ipi_ep);

struct xicu_filter_param_s
{
	struct device_s *parent;
	size_t output_line;
};

#endif

