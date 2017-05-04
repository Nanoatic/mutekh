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
#include <device/gpio.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include <mutek/printk.h>

#include "s1d15g00.h"

//#define NO_QUEUE


#include "s1d15g00-private.h"

static DEVSPI_CALLBACK(__s1d15g00_cmd_done)
{
	bool_t *done = priv;

	*done = 1;
}

#define TO_SPI(x) pv->spi_commands[cmdc++] = ({ struct dev_spi_rq_cmd_s __s = x; __s; })

static
void __s1d15g00_send_cmd(struct device_s *dev,
						 uint8_t cmd,
						 const uint8_t *data, size_t dlen,
						 devspi_callback_t *cb, void *cb_data)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;

	size_t cmdc = 0;

	TO_SPI(SPIRQ_DESELECT());
	TO_SPI(SPIRQ_PAD(cmd, 1));
	if ( dlen ) {
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(data, dlen, 1));
	}
	TO_SPI(SPIRQ_DESELECT());

	pv->spi_request.command_count = cmdc;
	pv->spi_request.callback = cb;
	pv->spi_request.pvdata = cb_data;
	pv->spi_request.command = pv->spi_commands;
	pv->spi_request.device_id = pv->spi_lun;

	dev_spi_request(pv->spi, &pv->spi_request);
}

static inline
void __s1d15g00_send_cmd_sync(struct device_s *dev,
							  uint8_t cmd,
							  const uint8_t *data, size_t dlen)
{
	bool_t done;
	
//	printk("S1D15G00 sending cmd %x... ", cmd);

	__s1d15g00_send_cmd(dev, cmd, data, dlen, __s1d15g00_cmd_done, (void*)&done);
	while (!done)
		order_compiler_mem();

//	printk("Done\n");
}


static
void __s1d15g00_set_mode(struct device_s *dev)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;
	
	uint8_t datctl_arg[] = {0, 0, 0};
	switch (pv->info.packing) {
	case LCD_PACK_RGB:
		switch (pv->info.r_bpp+pv->info.g_bpp+pv->info.b_bpp) {
		case 8:
			datctl_arg[2] = S1D15G00_DATCTL2_GRAY8;
			break;
		case 12:
			datctl_arg[2] = S1D15G00_DATCTL2_GRAY16A;
			break;
		default:
			return;
		}
		break;
	default:
		return;
	}

	if ( !(pv->info.flags & LCD_FLIP_HORIZ) )
		datctl_arg[0] |= S1D15G00_DATCTL0_MIRROR_X;

	if ( !(pv->info.flags & LCD_FLIP_VERT) )
		datctl_arg[0] |= S1D15G00_DATCTL0_MIRROR_Y;

//	printk("Setting DATCTL: %x %x %x\n", datctl_arg[0], datctl_arg[1], datctl_arg[2]);

	__s1d15g00_send_cmd_sync(dev, S1D15G00_DATCTL, datctl_arg, 3);
	__s1d15g00_send_cmd_sync(dev, (pv->info.flags & LCD_INVERT)
							 ? S1D15G00_DISNOR
							 : S1D15G00_DISINV,
							 0, 0);
}


static
void __s1d15g00_init_lcd(struct device_s *dev)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;
	uint_fast16_t i;

	pv->info.packing = LCD_PACK_RGB;
	pv->info.r_bpp = 4;
	pv->info.g_bpp = 4;
	pv->info.b_bpp = 4;
	pv->info.width = 131;
	pv->info.height = 131;
	pv->info.flags = LCD_FLIP_HORIZ | LCD_FLIP_VERT;

	dev_gpio_set_value(pv->set_reset_gpio_dev, pv->set_reset_gpio_id, 0);
	for (i=0; i<1000; ++i) asm volatile("nop");
	dev_gpio_set_value(pv->set_reset_gpio_dev, pv->set_reset_gpio_id, 1);
	for (i=0; i<100; ++i) asm volatile("nop");
//	const uint8_t disctl_arg[] = { 0, 0x20, 0xa };
	const uint8_t disctl_arg[] = { 0, 0x20, 0x0 };
	const uint8_t comscn_arg[] = {
//		S1D15G00_COMSCN_132_69
//		0
		1,
	};
	const uint8_t pwrctrl_arg[] = { S1D15G00_PWRCTR_VGEN_ON
									| S1D15G00_PWRCTR_BOOST_SEC_ON
									| S1D15G00_PWRCTR_BOOST_PRI_ON };
	const uint8_t volctr_arg[] = { 37, 3 };
	__s1d15g00_send_cmd_sync(dev, S1D15G00_DISCTL, disctl_arg, 3);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_COMSCN, comscn_arg, 1);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_OSCON, 0, 0);
//	for (i=0; i<100000; ++i) asm volatile("nop");
	__s1d15g00_send_cmd_sync(dev, S1D15G00_SLPOUT, 0, 0);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_VOLCTR, volctr_arg, 2);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_TMPGRD, disctl_arg, 1);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_PWRCTR, pwrctrl_arg, 1);
	__s1d15g00_set_mode(dev);
//	for (i=0; i<100000; ++i) asm volatile("nop");
	__s1d15g00_send_cmd_sync(dev, S1D15G00_PTLOUT, 0, 0);
	__s1d15g00_send_cmd_sync(dev, S1D15G00_DISON, 0, 0);
}


static DEVSPI_CALLBACK(__s1d15g00_req_done);

static error_t __s1d15g00_handle_one_req(struct device_s *dev, struct lcd_req_s *req)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;

	size_t cmdc = 0;

	switch (req->type) {
	case LCD_REQ_BLIT: {
		size_t size = (req->blit.xmax-req->blit.xmin) * (req->blit.ymax-req->blit.ymin);
		size_t real_size = size;

		if (pv->info.r_bpp+pv->info.g_bpp+pv->info.b_bpp == 12) {
			size = size/2*3-1;
			real_size = (real_size*3+1)/2-1;
			if ( size != real_size ) {
				pv->addend[0] = req->blit.src[size];
				pv->addend[1] = ((req->blit.src[size+1]&0xf)<<4) | (req->blit.src[0] >> 4);
				pv->addend[2] = ((req->blit.src[0] & 0xf)<<4) | (req->blit.src[1] >> 4);
				pv->addend[3] = ((req->blit.src[1] & 0xf)<<4) | (req->blit.src[2] >> 4);
				pv->addend[4] = ((req->blit.src[2] & 0xf)<<4) | (req->blit.src[3] >> 4);
				pv->addend[5] = ((req->blit.src[3] & 0xf)<<4) | (req->blit.src[4] >> 4);
			}
		}

/* 		printk("Writing %d bytes for %dx%d=%d pixels\n", real_size, */
/* 			   (req->blit.xmax-req->blit.xmin), (req->blit.ymax-req->blit.ymin), */
/* 			   (req->blit.xmax-req->blit.xmin) * (req->blit.ymax-req->blit.ymin)); */

		size_t c = 0;
		pv->cmd[c++] = S1D15G00_NOP;
		pv->cmd[c++] = S1D15G00_CASET;
		pv->cmd[c++] = 0x100 | req->blit.xmin;
		pv->cmd[c++] = 0x100 | (req->blit.xmax-1);
		pv->cmd[c++] = S1D15G00_PASET;
		pv->cmd[c++] = 0x100 | req->blit.ymin;
		pv->cmd[c++] = 0x100 | (req->blit.ymax-1);
		pv->cmd[c++] = S1D15G00_RAMWR;
		
		TO_SPI(SPIRQ_W_16(pv->cmd, c, 2));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(req->blit.src, size, 1));
		if ( size < real_size ) {
/* 			printk("Adding %d bytes\n", real_size-size); */
			TO_SPI(SPIRQ_W_8(pv->addend, real_size-size, 1));
		}
		break;
	}
	case LCD_REQ_SET_PALETTE: {
		size_t stride = (size_t)&req->palette.pal[1].r - (size_t)&req->palette.pal[0].r;

		TO_SPI(SPIRQ_PAD(S1D15G00_RGBSET8, 1));
		TO_SPI(SPIRQ_SET_CONSTANT(0x100));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].r, req->palette.count, stride));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].g, req->palette.count, stride));
		TO_SPI(SPIRQ_W_8(&req->palette.pal[0].b, req->palette.count, stride));
		break;
	}
	case LCD_REQ_SET_MODE: {

		pv->info.flags = req->mode.flags;
	
		pv->cmd[0] = S1D15G00_DATCTL;
		pv->cmd[1] = 0x100;
		pv->cmd[2] = 0x100;
		pv->cmd[3] = 0x100;

		switch (req->mode.bpp) {
		case 8:
			pv->info.r_bpp = 3;
			pv->info.g_bpp = 3;
			pv->info.b_bpp = 2;
			pv->cmd[3] |= S1D15G00_DATCTL2_GRAY8;
			break;
		case 12:
			pv->info.r_bpp = 4;
			pv->info.g_bpp = 4;
			pv->info.b_bpp = 4;
			pv->cmd[3] |= S1D15G00_DATCTL2_GRAY16A;
			break;
		default:
			return ERANGE;
		}

		if ( !(pv->info.flags & LCD_FLIP_HORIZ) )
			pv->cmd[1] |= S1D15G00_DATCTL0_MIRROR_X;
		
		if ( !(pv->info.flags & LCD_FLIP_VERT) )
			pv->cmd[1] |= S1D15G00_DATCTL0_MIRROR_Y;

//		printk("Setting DATCTL: %x %x %x\n", pv->cmd[1], pv->cmd[2], pv->cmd[3]);

		pv->cmd[4] = (pv->info.flags & LCD_INVERT)
			? S1D15G00_DISNOR
			: S1D15G00_DISINV;
		
		TO_SPI(SPIRQ_W_16(pv->cmd, 5, 2));
		break;
	}
	case LCD_REQ_SET_CONTRAST:
		TO_SPI(SPIRQ_PAD(S1D15G00_VOLCTR, 1));
		TO_SPI(SPIRQ_PAD(0x100|req->contrast.value, 1));
		TO_SPI(SPIRQ_PAD(0x100, 1));
		break;
	default:
		return ENOTSUP;
	}
	pv->spi_request.command_count = cmdc;
	pv->spi_request.callback = __s1d15g00_req_done;
#ifdef NO_QUEUE
	pv->spi_request.pvdata = req;
#else
	pv->spi_request.pvdata = dev;
#endif
	pv->spi_request.command = pv->spi_commands;
	pv->spi_request.device_id = pv->spi_lun;

	dev_spi_request(pv->spi, &pv->spi_request);

	return 0;
}



static DEVSPI_CALLBACK(__s1d15g00_req_done)
{
#ifdef NO_QUEUE
	struct lcd_req_s *req = priv;
	req->callback(req->callback_data, req);
#else
	struct device_s *dev = priv;
	struct s1d15g00_context_s *pv = dev->drv_pv;
	struct lcd_req_s *req;

	lock_spin(&dev->lock);
	req = dev_lcd_queue_head(&pv->queue);
	lock_release(&dev->lock);

	assert(req);

	req->callback(req->callback_data, req);

	lock_spin(&dev->lock);
	dev_lcd_queue_remove(&pv->queue, req);
	req = dev_lcd_queue_head(&pv->queue);
	lock_release(&dev->lock);

	if (req)
		__s1d15g00_handle_one_req(dev, req);
#endif
}

DEVLCD_REQUEST(s1d15g00_request)
{
#ifdef NO_QUEUE
	return __s1d15g00_handle_one_req(dev, req);
#else
	struct s1d15g00_context_s *pv = dev->drv_pv;
	bool_t empty = 0;

	lock_spin(&dev->lock);
	empty = ! dev_lcd_queue_head(&pv->queue);
	dev_lcd_queue_pushback(&pv->queue, req);
	lock_release(&dev->lock);

	if ( empty )
		return __s1d15g00_handle_one_req(dev, req);
	return 0;
#endif
}

DEVLCD_GETINFO(s1d15g00_getinfo)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;
	return &pv->info;
}

/* 
 * device open operation
 */

#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct driver_param_binder_s s1d15g00_binder[] =
{
	PARAM_BIND(struct s1d15g00_param_s, spi_dev, PARAM_DATATYPE_DEVICE_PTR),
	PARAM_BIND(struct s1d15g00_param_s, spi_lun, PARAM_DATATYPE_INT),
	PARAM_BIND(struct s1d15g00_param_s, set_reset_gpio_dev, PARAM_DATATYPE_DEVICE_PTR),
	PARAM_BIND(struct s1d15g00_param_s, set_reset_gpio_id, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	s1d15g00_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("s1d15g00", sizeof(struct s1d15g00_param_s), s1d15g00_binder),
	{ 0 }
};
#endif

const struct driver_s	s1d15g00_drv =
{
	.class		= device_class_lcd,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = s1d15g00_ids,
#endif
	.f_init		= s1d15g00_init,
	.f_cleanup		= s1d15g00_cleanup,
	.f.lcd = {
		.f_request      = s1d15g00_request,
		.f_getinfo      = s1d15g00_getinfo,
	}
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(s1d15g00_drv);
#endif


DEV_INIT(s1d15g00_init)
{
	struct s1d15g00_context_s	*pv;
	struct s1d15g00_param_s *param = params;
	
	dev->drv = &s1d15g00_drv;

	/* alocate private driver data */
	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));
	
	if (!pv)
		return -1;

	pv->spi = param->spi;
	pv->spi_lun = param->spi_lun;
	pv->set_reset_gpio_id = param->set_reset_gpio_id;
	pv->set_reset_gpio_dev = param->set_reset_gpio_dev;

	device_obj_refnew(pv->spi);

	dev->drv_pv = pv;

	dev_spi_set_data_format(pv->spi, pv->spi_lun, 9, SPI_MODE_3, 0);
	dev_spi_set_baudrate(pv->spi, pv->spi_lun, 600000*8, 0, 0);

#ifndef NO_QUEUE
	dev_lcd_queue_init(&pv->queue);
#endif

	__s1d15g00_init_lcd(dev);

	return 0;
}

DEV_CLEANUP(s1d15g00_cleanup)
{
	struct s1d15g00_context_s *pv = dev->drv_pv;

#ifndef NO_QUEUE
	dev_lcd_queue_destroy(&pv->queue);
#endif

	device_obj_refdrop(pv->spi);

	mem_free(pv);
}
