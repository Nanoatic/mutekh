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

#ifndef DRIVER_SD_MMC_H_
#define DRIVER_SD_MMC_H_

#include <device/block.h>
#include <device/device.h>

struct sd_mmc_param_s
{
	struct device_s *spi;
	uint_fast8_t spi_lun;
};

DEV_INIT(sd_mmc_init);
DEV_CLEANUP(sd_mmc_cleanup);
DEVBLOCK_REQUEST(sd_mmc_request);
DEVBLOCK_GETPARAMS(sd_mmc_get_params);
error_t sd_mmc_rehash(struct device_s *dev);

#endif

