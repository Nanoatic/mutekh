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


#ifndef LCD_PCF8833_H_
#define LCD_PCF8833_H_

#include <hexo/types.h>
#include <device/driver.h>

DEVLCD_REQUEST(pcf8833_request);
DEVLCD_GETINFO(pcf8833_getinfo);
DEV_CLEANUP(pcf8833_cleanup);

#define PCF8833_ADHOC_FUNC(x) void (x)(bool_t value)
typedef PCF8833_ADHOC_FUNC(pcf8833_adhoc_func_t);

struct pcf8833_param_s
{
	struct device_s *spi;
	uint_fast8_t spi_lun;
	pcf8833_adhoc_func_t *set_reset;
};

DEV_INIT(pcf8833_init);

#endif

