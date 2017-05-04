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


#ifndef LCD_S1D15G00_PRIVATE_H_
#define LCD_S1D15G00_PRIVATE_H_

#include <hexo/types.h>
#include <device/driver.h>
#include <hexo/lock.h>
#include <hexo/error.h>
#include "s1d15g00.h"

#define S1D15G00_DISON         0xAF
#define S1D15G00_DISOFF        0xAE
#define S1D15G00_DISNOR        0xA6
#define S1D15G00_DISINV        0xA7
#define S1D15G00_COMSCN        0xBB // 1B ARG: WAY 
#define S1D15G00_DISCTL        0xCA // 4B: 00 20 00
#define S1D15G00_SLPIN         0x95
#define S1D15G00_SLPOUT        0x94
#define S1D15G00_PASET         0x75 // 2B min/max
#define S1D15G00_CASET         0x15 // 2B min/max
#define S1D15G00_DATCTL        0xBC // 3B: flip+scan, RGB/BGR, Gray type
// Palette 8bit -> rgb
#define S1D15G00_RGBSET8       0xCE // palette 332: R*8, G*8, B*4
#define S1D15G00_RAMWR         0x5C // Data
#define S1D15G00_RAMRD         0x5D
// Partial display
#define S1D15G00_PTLIN         0xA8 // 2B: min/max (in 4-line blocks)
#define S1D15G00_PTLOUT        0xA9
#define S1D15G00_RMWIN         0xE0
#define S1D15G00_RMWOUT        0xEE
// Scroll
#define S1D15G00_ASCSET        0xAA // 4B: top, bottom, count, mode (in 4-line blocks)
#define S1D15G00_SCSTART       0xAB // 1B: 
#define S1D15G00_OSCON         0xD1
#define S1D15G00_OSCOFF        0xD2
#define S1D15G00_PWRCTR        0x20 // 1B: below
#define S1D15G00_VOLCTR        0x81 // 2B: volume a, resistor ratio r
#define S1D15G00_VOLUP         0xD6
#define S1D15G00_VOLDOWN       0xD7
#define S1D15G00_TMPGRD        0x82 // 1B: (2bits) gradient in -.05 %/deg
#define S1D15G00_EPCTIN        0xCD // 1B: eeprom read/write
#define S1D15G00_EPCOUT        0xCC
#define S1D15G00_EPMWR         0xFC
#define S1D15G00_EPMRD         0xFD
#define S1D15G00_EPSRRD1       0x7C
#define S1D15G00_EPSRRD2       0x7D
#define S1D15G00_NOP           0x25

#define S1D15G00_DATCTL0_MIRROR_X  0x01
#define S1D15G00_DATCTL0_MIRROR_Y  0x02
#define S1D15G00_DATCTL0_SCAN_PAGE 0x04
#define S1D15G00_DATCTL1_RGB       0x00
#define S1D15G00_DATCTL1_BGR       0x01
#define S1D15G00_DATCTL2_GRAY8     0x01
#define S1D15G00_DATCTL2_GRAY16A   0x02
#define S1D15G00_DATCTL2_GRAY16B   0x04

#define S1D15G00_COMSCN_1_68 0
#define S1D15G00_COMSCN_68_1 2
#define S1D15G00_COMSCN_69_132 0
#define S1D15G00_COMSCN_132_69 1

#define S1D15G00_ASCSET_CENTER 0x00
#define S1D15G00_ASCSET_TOP	   0x01
#define S1D15G00_ASCSET_BOTTOM 0x02
#define S1D15G00_ASCSET_WHOLE  0x03

#define S1D15G00_PWRCTR_VGEN_ON      0x03
#define S1D15G00_PWRCTR_BOOST_SEC_ON 0x04
#define S1D15G00_PWRCTR_BOOST_PRI_ON 0x08
#define S1D15G00_PWRCTR_EXTERNAL_RES 0x10

#define S1D15G00_EPCTIN_READ  0x00
#define S1D15G00_EPCTIN_WRITE 0x20

struct s1d15g00_context_s
{
#ifndef NO_QUEUE
	dev_lcd_queue_root_t queue;
#endif
	struct device_s *spi;
	struct lcd_info_s info;
	struct device_s *set_reset_gpio_dev;
	uint_fast8_t set_reset_gpio_id;
	uint_fast8_t spi_lun;
	uint8_t xmin;
	uint8_t xmax;
	uint8_t ymin;
	uint8_t ymax;
	uint16_t cmd[8];
	uint8_t addend[8];
	struct dev_spi_rq_cmd_s spi_commands[5];
	struct dev_spi_rq_s spi_request;
};

#endif

