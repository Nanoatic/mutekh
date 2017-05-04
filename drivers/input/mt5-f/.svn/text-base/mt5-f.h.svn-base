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

#ifndef _MT5F_H_
#define _MT5F_H_

#include <device/gpio.h>
#include <device/input.h>
#include <device/device.h>

#define MT5F_LEFT 0
#define MT5F_DOWN 1
#define MT5F_UP 2
#define MT5F_RIGHT 3
#define MT5F_BUTTON 4

DEVINPUT_INFO(dev_mt5f_info);
DEVINPUT_READ(dev_mt5f_read);
DEVINPUT_WRITE(dev_mt5f_write);
DEVINPUT_SETCALLBACK(dev_mt5f_setcallback);

struct dev_mt5f_param_s
{
	struct device_s *gpio_dev;
	devgpio_id_t a;
	devgpio_id_t b;
	devgpio_id_t c;
	devgpio_id_t d;
	devgpio_id_t common;
};

DEV_INIT(dev_mt5f_init);
DEV_CLEANUP(dev_mt5f_cleanup);

#endif

