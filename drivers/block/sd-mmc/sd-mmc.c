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

    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009

*/


#include <hexo/types.h>

#include <device/spi.h>
#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <hexo/ordering.h>
#include <mutek/mem_alloc.h>
#include <hexo/endian.h>
#include <mutek/printk.h>

#include "sd-mmc.h"
#include "sd-mmc-private.h"

#include "arch/sam7/at91sam7x256.h"

#define DBGU_DEBUG(x)													\
	do {																\
		while ( ! (AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_COMM_TX) );		\
		asm volatile ("MCR	p14, 0, %0, c1, c0, 0" :: "r" (x));			\
	} while(0)

#define DEBUG(x) //DBGU_DEBUG(x)
#define DEBUG_HEX(x) DEBUG(((x) > 9 ? ((x)+'a'-10) : ((x)+'0')))
#define DEBUG_HEX_CHR(x) DEBUG_HEX(((x)>>4)&0xf); DEBUG_HEX((x)&0xf)

static const uint8_t xfer_delay = 30;
static const uint8_t cs_delay = 30;
static const uint32_t base_br = 300000;
static const bool_t cs_bet = 0;
static const uint8_t sd_mode = 0;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

static void __sd_mmc_read(struct device_s *dev, struct dev_block_rq_s *rq);
static void __sd_mmc_write(struct device_s *dev, struct dev_block_rq_s *rq);
static void sd_mmc_handle_next_req(struct device_s *dev);

static inline uint8_t crc7(uint8_t * data, size_t size)
{
	uint8_t crc = 0;
	size_t i;

	for ( i=0; i<size; ++i ) {
		size_t n;
		uint8_t d = data[i];

		for (n = 0; n < 8; n++) {
			crc <<= 1;
			if ((d & 0x80) ^ (crc & 0x80))
				crc ^= 0x09;
			d <<= 1;
		}
	}
	return (crc << 1) | 1;
}

static DEVSPI_WAIT_VALUE_CALLBACK(check_cmd_accepted)
{
	struct cmd_state_s *state = rq->pvdata;

//	DEBUG_HEX_CHR(value);
	state->data = value;
	if ( !(value & 0x80) ) {
		if ( value & 0x7c )
			return DEV_SPI_VALUE_FAIL;
		else
			return DEV_SPI_VALUE_FOUND;
	}
	return DEV_SPI_VALUE_RETRY;
}

static DEVSPI_WAIT_VALUE_CALLBACK(wait_token_data_start)
{
	if ( value == 0xff )
		return DEV_SPI_VALUE_RETRY;
	if ( value == 0xfe )
		return DEV_SPI_VALUE_FOUND;
	return DEV_SPI_VALUE_FAIL;
}

static DEVSPI_WAIT_VALUE_CALLBACK(get_data_response)
{
	if ( (value & 0x11) != 0x1 )
		return DEV_SPI_VALUE_RETRY;
	if ( value & 0x8 )
		return DEV_SPI_VALUE_FAIL;
	return DEV_SPI_VALUE_FOUND;
}

static DEVSPI_WAIT_VALUE_CALLBACK(wait_not_busy)
{
	if ( value == 0x00 )
		return DEV_SPI_VALUE_RETRY;
	return DEV_SPI_VALUE_FOUND;
}

static DEVSPI_CALLBACK(sd_mmc_read_done)
{
	struct rw_cmd_state_s *state = priv;
	struct device_s *dev = state->dev;
    struct sd_mmc_context_s *pv = dev->drv_pv;
	struct dev_block_rq_s *bdrq = state->blockdev_rq;

	state->state.done = 1;

	if ( error ) {
		lock_spin(&dev->lock);

		state->state.error = 1;
		bdrq->progress = -EIO;
		lock_release(&dev->lock);

		bdrq->callback(bdrq, 0, bdrq + 1);

		dev_blk_queue_remove(&pv->queue, bdrq);

		sd_mmc_handle_next_req(dev);
	} else {
		bdrq->progress++;
		bdrq->callback(bdrq, 1, bdrq + 1);
		if ( bdrq->count > bdrq->progress ) {
		  switch (bdrq->type & DEV_BLOCK_OPMASK) {
			case DEV_BLOCK_READ:
				__sd_mmc_read(dev, bdrq);
				break;
			case DEV_BLOCK_WRITE:
				__sd_mmc_write(dev, bdrq);
				break;
			}
		} else {
			dev_blk_queue_remove(&pv->queue, bdrq);
			sd_mmc_handle_next_req(dev);
		}
	}
}

static DEVSPI_CALLBACK(sd_mmc_cmd_done)
{
	struct cmd_state_s *state = rq->pvdata;

	state->error = error;
	state->done = 1;
}

static inline
void __sd_mmc_stuff_bytes(
	struct device_s *dev, size_t n)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	struct cmd_state_s state = {0,0,0};
	struct dev_spi_rq_cmd_s spi_commands[] = {
		SPIRQ_PAD_UNSELECTED(0xff, n),
	};
	struct dev_spi_rq_s spi_request = {
		.callback = sd_mmc_cmd_done,
		.pvdata = &state,
		.command = spi_commands,
		.command_count = ARRAY_SIZE(spi_commands),
		.device_id = pv->spi_lun,
	};
		
	dev_spi_request(pv->spi, &spi_request);
	
	while ( !state.done )
		order_compiler_mem();
}

static inline
uint8_t __sd_mmc_to_spi_mode(
	struct device_s *dev)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	struct cmd_state_s state = {0,0,0};
	uint8_t command_value [6] = { 0x40, 0, 0, 0, 0, 0x95 };
//	uint8_t command_value [6] = { 0x40, 0x41, 0x42, 0x43, 0x44, 0x40 };
	struct dev_spi_rq_cmd_s spi_commands[] = {
		SPIRQ_PAD_UNSELECTED(0xff, 15),
		SPIRQ_W_8(command_value, ARRAY_SIZE(command_value), 1),
		SPIRQ_WAIT_VALUE(0xff, 16, check_cmd_accepted),
		SPIRQ_PAD(0xff, 2),
	};
	struct dev_spi_rq_s spi_request = {
		.callback = sd_mmc_cmd_done,
		.pvdata = &state,
		.command = spi_commands,
		.command_count = ARRAY_SIZE(spi_commands),
		.device_id = pv->spi_lun,
	};
		
	dev_spi_request(pv->spi, &spi_request);
	
	while ( !state.done )
		order_compiler_mem();
/* 	printk("to_spi -> %x %s\n", */
/* 		   state.data, */
/* 		   (state.error ? "error" : "ok")); */
	return state.data;
}

static inline
uint8_t __sd_mmc_send_command(
	struct device_s *dev,
	uint8_t cmd,
	uint32_t arg)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	struct cmd_state_s state = {0,0,0};
	uint8_t command_value [6] = {
		0x40 | cmd,
		(arg >> 24) & 0xff,
		(arg >> 16) & 0xff,
		(arg >> 8) & 0xff,
		(arg) & 0xff,
	};
	command_value[5] = crc7(command_value, 5);
	struct dev_spi_rq_cmd_s spi_commands[] = {
//		SPIRQ_RESELECT(),
		SPIRQ_PAD_UNSELECTED(0xff, 1),
		SPIRQ_W_8(command_value, 6, 1),
		SPIRQ_WAIT_VALUE(0xff, 16, check_cmd_accepted),
	};
	struct dev_spi_rq_s spi_request = {
		.callback = sd_mmc_cmd_done,
		.pvdata = &state,
		.command = spi_commands,
		.command_count = ARRAY_SIZE(spi_commands),
		.device_id = pv->spi_lun,
	};
		
	dev_spi_request(pv->spi, &spi_request);
	
	while ( !state.done )
		order_compiler_mem();
/* 	printk("cmd(%d, %p) (%x) -> %x %s\n", */
/* 		   cmd, (void*)arg, command_value[5], */
/* 		   state.data, */
/* 		   (state.error ? "error" : "ok")); */
	return state.data;
}

static inline
uint8_t __sd_mmc_send_acmd(
	struct device_s *dev,
	uint8_t cmd,
	uint32_t arg)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	struct cmd_state_s state = {0,0,0};
	uint8_t a_command_value [6] = {
		0x77, 0, 0, 0, 0 };
	uint8_t command_value [6] = {
		0x40 | cmd,
		(arg >> 24) & 0xff,
		(arg >> 16) & 0xff,
		(arg >> 8) & 0xff,
		(arg) & 0xff,
	};
	a_command_value[5] = crc7(a_command_value, 5);
	command_value[5] = crc7(command_value, 5);
	struct dev_spi_rq_cmd_s spi_commands[] = {
//		SPIRQ_RESELECT(),
		SPIRQ_PAD_UNSELECTED(0xff, 1),
		SPIRQ_W_8(a_command_value, 6, 1),
		SPIRQ_WAIT_VALUE(0xff, 16, check_cmd_accepted),
//		SPIRQ_RESELECT(),
		SPIRQ_W_8(command_value, 6, 1),
		SPIRQ_WAIT_VALUE(0xff, 16, check_cmd_accepted),
	};
	struct dev_spi_rq_s spi_request = {
		.callback = sd_mmc_cmd_done,
		.pvdata = &state,
		.command = spi_commands,
		.command_count = ARRAY_SIZE(spi_commands),
		.device_id = pv->spi_lun,
	};
		
	dev_spi_request(pv->spi, &spi_request);
	
	while ( !state.done )
		order_compiler_mem();
/* 	printk("cmd(%d, %p) (%x) -> %x %s\n", */
/* 		   cmd, (void*)arg, command_value[5], */
/* 		   state.data, */
/* 		   (state.error ? "error" : "ok")); */
	return state.data;
}

static inline
bool_t __sd_mmc_get_block(
	struct device_s *dev,
	void *data,
	size_t len )
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	uint8_t crc[2];

	struct cmd_state_s state = {0,0,0};
	struct dev_spi_rq_cmd_s spi_commands[] = {
		SPIRQ_WAIT_VALUE(0xff, 16, wait_token_data_start),
		SPIRQ_R_8(data, len, 0xff, 1),
		SPIRQ_R_8(crc, 2, 0xff, 1),
	};
	struct dev_spi_rq_s spi_request = {
		.callback = sd_mmc_cmd_done,
		.pvdata = &state,
		.command = spi_commands,
		.command_count = ARRAY_SIZE(spi_commands),
		.device_id = pv->spi_lun,
	};
	dev_spi_request(pv->spi, &spi_request);
	while ( !state.done )
        order_compiler_mem();
	return state.error;
}

#define TO_SPI(x) buf->spi_commands[cmdc++] = ({ struct dev_spi_rq_cmd_s __s = x; __s; })

void sdmmc_rw_command_buffer_init(
	struct device_s *dev,
	struct dev_block_rq_s *bdrq,
	uint8_t cmd,
	uint32_t byte)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;
	struct sd_mmc_rw_command_buffer_s *buf = &pv->rw_cmd_buffer;
    struct dev_block_params_s *p = &pv->params;
	size_t cmdc = 0;

	memset(buf, 0, sizeof(*buf));
	buf->state.blockdev_rq = bdrq;
	buf->state.dev = dev;
	buf->command_value[0] = 0x40|cmd;
	buf->command_value[1] = (byte >> 24) & 0xff;
	buf->command_value[2] = (byte >> 16) & 0xff;
	buf->command_value[3] = (byte >> 8) & 0xff;
	buf->command_value[4] = (byte) & 0xff;
	buf->command_value[5] = crc7(buf->command_value, 5);

	TO_SPI(SPIRQ_PAD_UNSELECTED(0xff, 1));
	TO_SPI(SPIRQ_W_8(buf->command_value, 6, 1));
	TO_SPI(SPIRQ_WAIT_VALUE(0xff, 16, check_cmd_accepted));

	switch (cmd) {
	case SDMMC_CMD_READ_SINGLE_BLOCK:
		/*
		  When reading, command queue will be:
		  * Write Single block read cmd
		  * Wait for status
		  * Wait for data token
		  * Read block
		  * Read CRC
		  */
		TO_SPI(SPIRQ_WAIT_VALUE(0xff, pv->access_timeout, wait_token_data_start));
		TO_SPI(SPIRQ_R_8(bdrq->data[bdrq->progress], p->blk_size, 0xff, 1));
		TO_SPI(SPIRQ_R_8(buf->crc, 2, 0xff, 1));
		break;
	case SDMMC_CMD_WRITE_SINGLE_BLOCK:
		buf->tmp[0] = SDMMC_TOKEN_DATA_START;
		TO_SPI(SPIRQ_W_8(buf->tmp, 1, 1));
		TO_SPI(SPIRQ_W_8(bdrq->data[bdrq->progress], p->blk_size, 1));
		TO_SPI(SPIRQ_W_8(buf->crc, 2, 1));
		TO_SPI(SPIRQ_WAIT_VALUE(0xff, 4, get_data_response));
		TO_SPI(SPIRQ_WAIT_VALUE(0xff, pv->access_timeout, wait_not_busy));
		break;
	default:
		assert(!"Unimplemented");
	};
	buf->spi_request.command_count = cmdc;
	buf->spi_request.callback = sd_mmc_read_done;
	buf->spi_request.pvdata = &buf->state;
	buf->spi_request.command = buf->spi_commands;
	buf->spi_request.device_id = pv->spi_lun;
}

static void __sd_mmc_read(struct device_s *dev, struct dev_block_rq_s *rq)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

/* 	printk("SD/MMC read lba %d\n", rq->lba); */

	sdmmc_rw_command_buffer_init(
		dev,
		rq,
		SDMMC_CMD_READ_SINGLE_BLOCK,
		rq->lba + (rq->progress << pv->csd.read_bl_len));
	dev_spi_request(pv->spi, &pv->rw_cmd_buffer.spi_request);
}

static void __sd_mmc_write(struct device_s *dev, struct dev_block_rq_s *rq)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	sdmmc_rw_command_buffer_init(
		dev,
		rq,
		SDMMC_CMD_WRITE_SINGLE_BLOCK,
		rq->lba + (rq->progress << pv->csd.read_bl_len));
	dev_spi_request(pv->spi, &pv->rw_cmd_buffer.spi_request);
}

static inline
void sdmmc_swap_csd( void *blob )
{
	uint32_t *blob32 = blob;
	uint_fast8_t i;

	for (i=0; i<4; ++i)
		blob32[i] = endian_be32(blob32[i]);
}

static inline
void sdmmc_update_card_caracteristics(struct sd_mmc_context_s *pv)
{
	struct dev_block_params_s *p = &pv->params;
	size_t c_size = (pv->csd.c_size_high10 << 2) | pv->csd.c_size_low2;
	p->blk_count = (c_size + 1) << (pv->csd.c_size_mult + 2);
	p->blk_size = 1 << pv->csd.read_bl_len;

	printk("%s Card, %d blocks of %d bytes, %d MiB total\n",
		   pv->csd.csd_structure == 0 ? "SD" : "MMC",
		   p->blk_count, p->blk_size,
		   p->blk_count * p->blk_size / 1024 / 1024
		);
}

static inline uint32_t sdmmc_max_baudrate(struct sdmmc_csd_s *csd)
{
	if (csd->tran_speed_reserved)
		return 0;
	uint32_t speed = 0;
	// We take mantissa*10 and exponent/10 so we manipulate integers
	switch (csd->tran_speed_mantissa) {
	case 0: return 0;
	case 1:  speed = 10; break;
	case 2:  speed = 12; break;
	case 3:  speed = 13; break;
	case 4:  speed = 15; break;
	case 5:  speed = 20; break;
	case 6:  speed = 25; break;
	case 7:  speed = 30; break;
	case 8:  speed = 35; break;
	case 9:  speed = 40; break;
	case 10: speed = 45; break;
	case 11: speed = 50; break;
	case 12: speed = 55; break;
	case 13: speed = 60; break;
	case 14: speed = 70; break;
	case 15: speed = 80; break;
	}
	switch (csd->tran_speed_exp) {
	default: return 0;
	case 0: return speed * 10000; // 100Kb/s
	case 1: return speed * 100000; // 1Mb/s
	case 2: return speed * 1000000; // 10Mb/s
	case 3: return speed * 10000000; // 100Mb/s
	}
}

static inline
void sdmmc_update_baudrate(
	struct sd_mmc_context_s *pv,
	uint32_t desired_baudrate)
{
	pv->bus_freq = dev_spi_set_baudrate(pv->spi, pv->spi_lun, desired_baudrate, xfer_delay, cs_delay);

//	printk("New bus freq: asked %d Hz, got %d Hz\n", desired_baudrate, pv->bus_freq);

	uint32_t taac = pv->csd.taac_value * (pv->bus_freq / 800);
	uint_fast8_t tmp;
	for ( tmp = 7; tmp > pv->csd.taac_exponent; --tmp )
		taac /= 10;

//	printk("Read timeout: %de%d ns\n", pv->csd.taac_value, pv->csd.taac_exponent);

	pv->access_timeout = 100*taac + 100*pv->csd.nsac;
//	printk("Read timeout: %d words\n",pv->access_timeout);
}

error_t sd_mmc_rehash(struct device_s *dev)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;
	size_t i;

	lock_spin(&dev->lock);

	dev_spi_set_data_format(pv->spi, pv->spi_lun, 8, sd_mode, cs_bet);

	// We dont call sdmmc_update_baudrate
	// because we dont know CSD yet
	pv->bus_freq = dev_spi_set_baudrate(pv->spi, pv->spi_lun, base_br, xfer_delay, cs_delay);

	/*
	  On reset, command queue will be:
	  * Pad 10 words without select
	  * Write reset cmd
	  * Wait R1 response
	  * Write init cmd
	  * Get CSD
	 */

	printk("SD/MMC Initialization...\n");

//	for ( i=0; i<10; ++i ) {
		if ( __sd_mmc_to_spi_mode(dev) != 0xff )
			goto in_spi_mode;
//	}
	goto fail;
  in_spi_mode:

	{
		uint8_t state;
		pv->mode = MODE_SD;
		state = __sd_mmc_send_acmd(dev, SDMMC_ACMD_SEND_OP_COND, 0);
		if ( state & 0x7c ) {
			pv->mode = MODE_MMC;
			state = __sd_mmc_send_command(dev, SDMMC_CMD_SEND_OP_COND, 0);
			if ( state & 0x7c )
				goto fail;
		}
	}

	for ( i=0; i<128; ++i ) {
		uint8_t state;

		if ( pv->mode == MODE_SD )
			state = __sd_mmc_send_acmd(dev, SDMMC_ACMD_SEND_OP_COND, 0);
		else
			state = __sd_mmc_send_command(dev, SDMMC_CMD_SEND_OP_COND, 0);

		__sd_mmc_stuff_bytes(dev, 10);
		if ( state & 0x7c )
			goto fail;
		if ( ! (state & 1) )
			goto noidle;
	}
	goto fail;
  noidle:
	{
		if ( __sd_mmc_send_command(dev, SDMMC_CMD_SEND_CSD, 0) & 0x7c )
			goto fail;

		if ( __sd_mmc_get_block(dev, &pv->csd, 16) )
			goto fail;
	}

	sdmmc_swap_csd(&pv->csd);
	sdmmc_update_card_caracteristics(pv);

	sdmmc_update_baudrate(pv, sdmmc_max_baudrate(&pv->csd));

	pv->usable = 1;
	lock_release(&dev->lock);
	return 0;

  fail:
	printk("  failed\n");

	pv->usable = 0;
	lock_release(&dev->lock);
	return -1;
}

static void
sd_mmc_handle_next_req(struct device_s *dev)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;
    struct dev_block_params_s *p = &pv->params;
	struct dev_block_rq_s *rq;

	rq = dev_blk_queue_head(&pv->queue);
	if ( rq == NULL ) {
		return;
	}

	if ( !pv->usable ) {
        rq->progress = -EIO;
        rq->callback(rq, 0, rq + 1);
		return;
	}

    dev_block_lba_t lba = rq->lba + rq->progress;
    dev_block_lba_t count = rq->count - rq->progress;

    if (lba + count > p->blk_count) {
        rq->progress = -ERANGE;
        rq->callback(rq, 0, rq + 1);
		return;
    }

	switch (rq->type & DEV_BLOCK_OPMASK) {
	case DEV_BLOCK_READ:
		__sd_mmc_read(dev, rq);
		break;
	case DEV_BLOCK_WRITE:
		__sd_mmc_write(dev, rq);
		break;

	default:
	  rq->progress = -ENOTSUP;
	  rq->callback(rq, 0, rq + 1);
	  break;
	}
}

DEVBLOCK_REQUEST(sd_mmc_request)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	bool_t must_start = !dev_blk_queue_head(&pv->queue);

	dev_blk_queue_pushback(&pv->queue, rq);

	if (must_start)
		sd_mmc_handle_next_req(dev);
}

DEVBLOCK_GETPARAMS(sd_mmc_get_params)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

    return &(pv->params);
}

DEVBLOCK_GETRQSIZE(sd_mmc_get_rqsize)
{
  return sizeof(struct dev_block_rq_s);
}

#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct driver_param_binder_s sd_mmc_binder[] =
{
	PARAM_BIND(struct sd_mmc_param_s, spi, PARAM_DATATYPE_DEVICE_PTR),
	PARAM_BIND(struct sd_mmc_param_s, spi_lun, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	sdmmc_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("sdmmc_spi", sizeof(struct sd_mmc_param_s), sd_mmc_binder),
	{ 0 }
};
#endif

const struct driver_s   sd_mmc_drv =
{
    .class      = device_class_block,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = sdmmc_ids,
#endif
    .f_init     = sd_mmc_init,
    .f_cleanup      = sd_mmc_cleanup,
	.f.blk = {
		.f_request      = sd_mmc_request,
		.f_getparams    = sd_mmc_get_params,
		.f_getrqsize	= sd_mmc_get_rqsize,
    },
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(sd_mmc_drv);
#endif

DEV_INIT(sd_mmc_init)
{
	struct sd_mmc_context_s   *pv;
	struct sd_mmc_param_s *param = params;

	dev->drv = &sd_mmc_drv;

	/* allocate private driver data */
	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

	if (!pv)
		return -1;

	dev->drv_pv = pv;

	pv->spi = param->spi;
	pv->spi_lun = param->spi_lun;

	device_obj_refnew(pv->spi);

	dev_blk_queue_init(&pv->queue);

	dev_spi_set_data_format(pv->spi, pv->spi_lun,
							8, SPI_MODE_0, 1);

	if ( sd_mmc_rehash(dev) )
		printk("SD/MMC not found, please rehash status when ready.\n");

	return 0;
}

DEV_CLEANUP(sd_mmc_cleanup)
{
    struct sd_mmc_context_s *pv = dev->drv_pv;

	device_obj_refdrop(pv->spi);

    mem_free(pv);
}

