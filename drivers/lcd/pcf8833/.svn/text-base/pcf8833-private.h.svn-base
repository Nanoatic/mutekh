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


#ifndef LCD_PCF8833_PRIVATE_H_
#define LCD_PCF8833_PRIVATE_H_

#include <hexo/types.h>
#include <device/driver.h>
#include <hexo/lock.h>
#include <hexo/error.h>
#include "pcf8833.h"

#define PCF8833_SLEEPOUT 0x11
#define PCF8833_INVON    0x20
#define PCF8833_COLMOD   0x3a
#define PCF8833_MADCTL   0x36
#define PCF8833_SETCON   0x25
#define PCF8833_DISPON   0x29
#define PCF8833_DISPOFF  0x28
#define PCF8833_CASET    0x2a
#define PCF8833_PASET    0x2b
#define PCF8833_WRITE    0x2c
#define PCF8833_RGBSET   0x2d

#define PCF8833_COLOR_8BPP  0x2
#define PCF8833_COLOR_12BPP 0x3
#define PCF8833_COLOR_16BPP 0x5

#define PCF8833_MIRROR_X 0x40
#define PCF8833_MIRROR_Y 0x80
#define PCF8833_INVERT   0x08

struct pcf8833_context_s
{
	struct device_s *spi;
	struct lcd_info_s info;
	pcf8833_adhoc_func_t *set_reset;
	uint_fast8_t spi_lun;
	uint8_t xmin;
	uint8_t xmax;
	uint8_t ymin;
	uint8_t ymax;
	struct dev_spi_rq_cmd_s spi_commands[12];
	struct dev_spi_rq_s spi_request;
};

#endif

