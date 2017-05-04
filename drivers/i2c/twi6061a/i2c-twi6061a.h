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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net> 2009

*/

#ifndef DRIVER_I2C_TWI6061A_H_
#define DRIVER_I2C_TWI6061A_H_

#include <device/i2c.h>
#include <device/device.h>

DEV_INIT(i2c_twi6061a_init);
DEV_CLEANUP(i2c_twi6061a_cleanup);
DEV_IRQ(i2c_twi6061a_irq);
DEVI2C_SET_BAUDRATE(i2c_twi6061a_set_baudrate);
DEVI2C_REQUEST(i2c_twi6061a_request);

#endif

