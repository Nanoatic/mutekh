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

    Copyright (c), Nicolas Pouillon <nipo@ssji.net>, 2009

*/

#include <hexo/types.h>


#include <device/lcd.h>
#include <device/spi.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include "pcf8833.h"
#include "pcf8833-private.h"

static DEVSPI_CALLBACK(__pcf8833_cmd_done)
{
	bool_t *done = priv;

	*done = 1;
}

#define TO_SPI(x) pv->spi_commands[cmdc++] = ({ struct dev_spi_rq_cmd_s __s = x; __s; })

static
void __pcf8833_send_cmd(struct device_s *dev,
						uint8_t cmd,
						const uint8_t *data, size_t dlen,
						devspi_callback_t *cb, void *cb_data)
{
	struct pcf8833_context_s *pv = dev->drv_pv;

	size_t cmdc = 0;

	TO_SPI(SPIRQ_PAD(cmd, 1));
	TO_SPI(SPIRQ_SET_CONSTANT(0x100));
	if ( dlen )
		TO_SPI(SPIRQ_W_8(data, dlen, 1));

	pv->spi_request.command_count = cmdc;
	pv->spi_request.callback = cb;
	pv->spi_request.pvdata = cb_data;
	pv->spi_request.command = pv->spi_commands;
	pv->spi_request.device_id = pv->spi_lun;

	dev_spi_request(pv->spi, &pv->spi_request);
}

static inline
void __pcf8833_send_cmd_sync(struct device_s *dev,
							 uint8_t cmd,
							 const uint8_t *data, size_t dlen)
{
	bool_t done;
	
//	printk("PCF8833 sending cmd %x... ", cmd);

	__pcf8833_send_cmd(dev, cmd, data, dlen, __pcf8833_cmd_done, (void*)&done);
	while (!done)
		order_compiler_mem();

//	printk("Done\n");
}

static
void __pcf8833_set_mode(struct device_s *dev)
{
	struct pcf8833_context_s *pv = dev->drv_pv;
	
	uint8_t colmode = 0, flags = 0;
	switch (pv->info.packing) {
	case LCD_PACK_RGB:
		switch (pv->info.r_bpp+pv->info.g_bpp+pv->info.b_bpp) {
		case 8:
			colmode = PCF8833_COLOR_8BPP;
			break;
		case 12:
			colmode = PCF8833_COLOR_12BPP;
			break;
		case 16:
			colmode = PCF8833_COLOR_16BPP;
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	if ( pv->info.flags & LCD_FLIP_HORIZ )
		flags |= PCF8833_MIRROR_X;

	if ( pv->info.flags & LCD_FLIP_VERT )
		flags |= PCF8833_MIRROR_Y;

	if ( ! (pv->info.flags & LCD_INVERT) )
		flags |= PCF8833_INVERT;

	__pcf8833_send_cmd_sync(dev, PCF8833_COLMOD, &colmode, 1);
	__pcf8833_send_cmd_sync(dev, PCF8833_MADCTL, &flags, 1);
}



static
void __pcf8833_init_lcd(struct device_s *dev)
{
	struct pcf8833_context_s *pv = dev->drv_pv;
	uint_fast16_t i;

	pv->info.packing = LCD_PACK_RGB;
	pv->info.r_bpp = 4;
	pv->info.g_bpp = 4;
	pv->info.b_bpp = 4;
	pv->info.width = 131;
	pv->info.height = 131;
	pv->info.flags = LCD_FLIP_HORIZ | LCD_FLIP_VERT;

	pv->set_reset(1);
	for (i=0; i<100000; ++i) asm volatile("nop");
	pv->set_reset(0);
	for (i=0; i<100000; ++i) asm volatile("nop");
	__pcf8833_send_cmd_sync(dev, PCF8833_SLEEPOUT, 0, 0);
	__pcf8833_send_cmd_sync(dev, PCF8833_INVON, 0, 0);
	__pcf8833_set_mode(dev);
	uint8_t contrast = 0x30;
	__pcf8833_send_cmd_sync(dev, PCF8833_SETCON, &contrast, 1);
	__pcf8833_send_cmd_sync(dev, PCF8833_DISPON, 0, 0);
}

static DEVSPI_CALLBACK(__pcf8833_req_done)
{
	struct lcd_req_s *req = priv;

	req->callback(req->callback_data, req);
}

DEVLCD_REQUEST(pcf8833_request)
{
	struct pcf8833_context_s *pv = dev->drv_pv;

	size_t cmdc = 0;
	switch (req->type) {
	case LCD_REQ_BLIT: {
		size_t size = (req->blit.xmax-req->blit.xmin) * (req->blit.ymax-req->blit.ymin);

		TO_SPI(SPIRQ_PAD(PCF8833_CASET, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(&req->blit.xmin, 2, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x000));

		TO_SPI(SPIRQ_PAD(PCF8833_PASET, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(&req->blit.ymin, 2, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x000));

		TO_SPI(SPIRQ_PAD(PCF8833_WRITE, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(req->blit.src, size, 1));
		break;
	}
	case LCD_REQ_SET_PALETTE: {
		size_t stride = (size_t)&req->palette.pal[1].r - (size_t)&req->palette.pal[0].r;

		TO_SPI(SPIRQ_PAD(PCF8833_RGBSET, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].r, req->palette.count, stride));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].g, req->palette.count, stride));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].b, req->palette.count, stride));

		break;
	}
	case LCD_REQ_SET_MODE: {
		pv->info.flags = req->mode.flags;
	
		uint8_t colmode = 0, flags = 0;

		switch (req->mode.bpp) {
		case 8:
			pv->info.r_bpp = 3;
			pv->info.g_bpp = 3;
			pv->info.b_bpp = 2;
			colmode = PCF8833_COLOR_8BPP;
			break;
		case 12:
			pv->info.r_bpp = 4;
			pv->info.g_bpp = 4;
			pv->info.b_bpp = 4;
			colmode = PCF8833_COLOR_12BPP;
			break;
		case 16:
			pv->info.r_bpp = 5;
			pv->info.g_bpp = 6;
			pv->info.b_bpp = 5;
			colmode = PCF8833_COLOR_16BPP;
			break;
		default:
			return ERANGE;
		}

		if ( pv->info.flags & LCD_FLIP_HORIZ )
			flags |= PCF8833_MIRROR_X;

		if ( pv->info.flags & LCD_FLIP_VERT )
			flags |= PCF8833_MIRROR_Y;

		if ( ! (pv->info.flags & LCD_INVERT) )
			flags |= PCF8833_INVERT;

		TO_SPI(SPIRQ_PAD(PCF8833_COLMOD, 1));
		TO_SPI(SPIRQ_PAD(0x100|colmode, 1));
		TO_SPI(SPIRQ_PAD(PCF8833_MADCTL, 1));
		TO_SPI(SPIRQ_PAD(0x100|flags, 1));
		break;
	}
	case LCD_REQ_SET_CONTRAST: {
		TO_SPI(SPIRQ_PAD(PCF8833_SETCON, 1));
		TO_SPI(SPIRQ_PAD(0x100|req->contrast.value, 1));

		break;
	}
	default:
		return ENOTSUP;
	}

	pv->spi_request.command_count = cmdc;
	pv->spi_request.callback = __pcf8833_req_done;
	pv->spi_request.pvdata = req;
	pv->spi_request.command = pv->spi_commands;
	pv->spi_request.device_id = pv->spi_lun;
	dev_spi_request(pv->spi, &pv->spi_request);

	return 0;
}

DEVLCD_GETINFO(pcf8833_getinfo)
{
	struct pcf8833_context_s *pv = dev->drv_pv;
	return &pv->info;
}

/* 
 * device open operation
 */

const struct driver_s	pcf8833_drv =
{
	.class		= device_class_lcd,
	.f_init		= pcf8833_init,
	.f_cleanup		= pcf8833_cleanup,
	.f.lcd = {
		.f_request      = pcf8833_request,
		.f_getinfo      = pcf8833_getinfo,
	}
};

DEV_INIT(pcf8833_init)
{
	struct pcf8833_context_s	*pv;
	struct pcf8833_param_s *param = params;
	
	dev->drv = &pcf8833_drv;

	/* alocate private driver data */
	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));
	
	if (!pv)
		return -1;

	pv->spi = param->spi;
	pv->spi_lun = param->spi_lun;
	pv->set_reset = param->set_reset;

	device_obj_refnew(pv->spi);

	dev->drv_pv = pv;

	dev_spi_set_data_format(pv->spi, pv->spi_lun, 9, SPI_MODE_3, 0);
	dev_spi_set_baudrate(pv->spi, pv->spi_lun, 660000, 3, 10);

	__pcf8833_init_lcd(dev);

	return 0;
}

DEV_CLEANUP(pcf8833_cleanup)
{
	struct pcf8833_context_s *pv = dev->drv_pv;

	device_obj_refdrop(pv->spi);

	mem_free(pv);
}
