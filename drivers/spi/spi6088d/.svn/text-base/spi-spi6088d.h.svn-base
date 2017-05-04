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

#ifndef DRIVER_SPI_SPI6088D_H_
#define DRIVER_SPI_SPI6088D_H_

#include <device/spi.h>
#include <device/device.h>

struct spi_spi6088d_param_s
{
	uint_fast8_t lun_count;
};

DEV_INIT(spi_spi6088d_init);
DEV_CLEANUP(spi_spi6088d_cleanup);
DEV_IRQ(spi_spi6088d_irq);
DEVSPI_SET_BAUDRATE(spi_spi6088d_set_baudrate);
DEVSPI_SET_DATA_FORMAT(spi_spi6088d_set_data_format);
DEVSPI_REQUEST(spi_spi6088d_request);

#endif

