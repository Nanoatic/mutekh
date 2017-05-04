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


#include <hexo/types.h>

#include <device/spi.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/endian.h>
#include <mutek/printk.h>

#include <assert.h>

#include "spi-spi6088d.h"
#include "spi-spi6088d-private.h"

#define MCK 48000000


#define TX_STATUS SPI6088D_TXEMPTY
#define RX_STATUS SPI6088D_RXBUFF

#define DO_SYNC(...) while (											\
		((cpu_mem_read_32(registers + SPI_SR) & (TX_STATUS|RX_STATUS)) != \
		 (TX_STATUS|RX_STATUS))							\
		__VA_ARGS__ )


static void spi_select_none(struct device_s *dev)
{
//	struct spi_spi6088d_context_s *pv = dev->drv_pv;
//	AT91C_BASE_PIOA->PIO_PER = pv->cs_bits;
}

static void spi_select_normal(struct device_s *dev)
{
//	struct spi_spi6088d_context_s *pv = dev->drv_pv;
//	AT91C_BASE_PIOA->PIO_PDR = pv->cs_bits;
}



static CMD_HANDLER(spi_spi6088d_read_rx_1byte)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	uint32_t data = cpu_mem_read_32(registers + SPI_RDR);
	*(uint8_t *)(pv->rx_ptr) = data;
	pv->rx_ptr += pv->increment;
}

static CMD_HANDLER(spi_spi6088d_read_rx_2bytes)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	uint32_t data = cpu_mem_read_32(registers + SPI_RDR);
	*(uint16_t*)(pv->rx_ptr) = data;	
	pv->rx_ptr += pv->increment;
}

static CMD_HANDLER(spi_spi6088d_void_rx)
{
	uintptr_t registers = (uintptr_t)dev->addr[0];

    cpu_mem_read_32(registers + SPI_RDR);
}

static CMD_HANDLER(spi_spi6088d_write_tx_1byte)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	uint32_t data = *(uint8_t *)pv->tx_ptr
//		| (pv->count == 0 ? (1<<24) : 0)
		| pv->permanent;
	pv->tx_ptr += pv->increment;

    cpu_mem_write_32(registers + SPI_TDR, data);
	pv->count--;
}

static CMD_HANDLER(spi_spi6088d_write_tx_2bytes)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	uint32_t data = *(uint16_t*)(pv->tx_ptr)
//		| (pv->count == 0 ? (1<<24) : 0)
		;
	pv->tx_ptr += pv->increment;
    cpu_mem_write_32(registers + SPI_TDR, data);
	pv->count--;
}

static CMD_HANDLER(spi_spi6088d_pad_tx)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	uint32_t data = pv->pad_byte;
    cpu_mem_write_32(registers + SPI_TDR, data);
	pv->count--;
}

static CMD_HANDLER(spi_spi6088d_wait_value_rx)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];
	struct dev_spi_rq_s *rq = dev_spi_queue_head(&pv->queue);

    uint32_t data = cpu_mem_read_32(registers + SPI_RDR);
	enum devspi_wait_value_answer_e ret = pv->wait_cb(dev, rq, data);
	switch (ret) {
	case DEV_SPI_VALUE_FAIL:
		pv->abort = 1;
		pv->count = 0;
		break;
	case DEV_SPI_VALUE_FOUND:
		pv->count = 0;
		break;
	case DEV_SPI_VALUE_RETRY:
		if ( pv->count == 0 )
			pv->abort = 1;
		break;
	}
}


static bool_t spi_spi6088d_setup_command(struct device_s *dev, struct dev_spi_rq_cmd_s *cmd)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];
	bool_t handled = 0;

	pv->abort = 0;

/* 	select &= ~SPI6088D_PCSDEC; */

	const char *ttype = "unknown";
	switch ( cmd->type ) {
	case DEV_SPI_DESELECT:
		ttype = "deselect";
//		spi_select_none(dev);
        cpu_mem_write_32(registers + SPI_CR, SPI6088D_LASTXFER);
		handled = 1;
		break;
	case DEV_SPI_R_8:
		ttype = "read";
		pv->tx_handler = spi_spi6088d_pad_tx;
		pv->rx_handler = spi_spi6088d_read_rx_1byte;
		pv->increment = cmd->read.ptr_increment;
		pv->rx_ptr = (uintptr_t)cmd->read.data;
		pv->count = cmd->read.size;
		pv->pad_byte = cmd->read.padding;
		break;
	case DEV_SPI_W_8:
		ttype = "write";
		pv->tx_handler = spi_spi6088d_write_tx_1byte;
		pv->increment = cmd->write.ptr_increment;
		pv->rx_handler = spi_spi6088d_void_rx;
		pv->tx_ptr = (uintptr_t)cmd->write.data;
		pv->count = cmd->write.size;
		break;
	case DEV_SPI_RW_8:
		ttype = "read_write";
		pv->rx_handler = spi_spi6088d_read_rx_1byte;
		pv->tx_handler = spi_spi6088d_write_tx_1byte;
		pv->increment = cmd->read_write.ptr_increment;
		pv->rx_ptr = (uintptr_t)cmd->read_write.rdata;
		pv->tx_ptr = (uintptr_t)cmd->read_write.wdata;
		pv->count = cmd->read_write.size;
		break;
	case DEV_SPI_R_16:
		ttype = "read";
		pv->tx_handler = spi_spi6088d_pad_tx;
		pv->rx_handler = spi_spi6088d_read_rx_2bytes;
		pv->increment = cmd->read.ptr_increment;
		pv->rx_ptr = (uintptr_t)cmd->read.data;
		pv->count = cmd->read.size;
		pv->pad_byte = cmd->read.padding;
		break;
	case DEV_SPI_W_16:
		ttype = "write";
		pv->tx_handler = spi_spi6088d_write_tx_2bytes;
		pv->increment = cmd->write.ptr_increment;
		pv->rx_handler = spi_spi6088d_void_rx;
		pv->tx_ptr = (uintptr_t)cmd->write.data;
		pv->count = cmd->write.size;
		break;
	case DEV_SPI_RW_16:
		ttype = "read_write";
		pv->rx_handler = spi_spi6088d_read_rx_2bytes;
		pv->tx_handler = spi_spi6088d_write_tx_2bytes;
		pv->increment = cmd->read_write.ptr_increment;
		pv->rx_ptr = (uintptr_t)cmd->read_write.rdata;
		pv->tx_ptr = (uintptr_t)cmd->read_write.wdata;
		pv->count = cmd->read_write.size;
		break;
	case DEV_SPI_SET_CONSTANT:
		ttype= "set_constant";
		pv->permanent = 
			(pv->permanent & 0xffff0000)
			| cmd->constant.data;
		handled = 1;
		break;
	case DEV_SPI_WAIT_VALUE:
		ttype= "wait_byte_value";
		pv->tx_handler = spi_spi6088d_pad_tx;
		pv->rx_handler = spi_spi6088d_wait_value_rx;
		pv->pad_byte = cmd->wait_value.padding;
		pv->wait_cb = cmd->wait_value.callback;
		pv->count = cmd->wait_value.timeout;
		break;
	case DEV_SPI_PAD_UNSELECTED:
		ttype= "pad_unselected";
		DO_SYNC();
		spi_select_none(dev);
		goto pad;
	case DEV_SPI_PAD:
		ttype= "pad";
	pad:
		pv->tx_handler = spi_spi6088d_pad_tx;
		pv->rx_handler = spi_spi6088d_void_rx;
		pv->pad_byte = cmd->pad.padding;
		pv->count = cmd->pad.size;
		break;
	}

	return handled;
}

static
struct dev_spi_rq_cmd_s *spi_spi6088d_get_next_cmd(struct device_s *dev)
{
	struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	struct dev_spi_rq_s *rq;

	while ((rq = dev_spi_queue_head(&pv->queue))) {
		if ( pv->cur_cmd == (size_t)-1 ) {
			uint32_t select = cpu_mem_read_32(registers + SPI_MR) | 0xf0000;
			select &= ~(0x10000 << rq->device_id);
            cpu_mem_write_32(registers + SPI_MR, select);

			// We are handling a new command queue, setup stuff
			pv->permanent = (0xf ^ (0x1 << rq->device_id))<<16;

			pv->cur_cmd = 0;
		} else {
			// A command terminated, so let's handle the cleanup
			struct dev_spi_rq_cmd_s *last_cmd = &rq->command[pv->cur_cmd];

			if ( last_cmd->type == DEV_SPI_PAD_UNSELECTED ) {
				DO_SYNC();
				spi_select_normal(dev);
			}

			if ( pv->abort ) {
				rq->callback(rq->pvdata, rq, 1);
				dev_spi_queue_remove(&pv->queue, rq);
				pv->cur_cmd = (size_t)-1;
				continue;
			}

			if ( pv->cur_cmd == rq->command_count - 1 ) {
				rq->callback(rq->pvdata, rq, 0);
				dev_spi_queue_remove(&pv->queue, rq);
				pv->cur_cmd = (size_t)-1;
				continue;
			}

			pv->cur_cmd++;
		}
		struct dev_spi_rq_cmd_s *cmd = &rq->command[pv->cur_cmd];

		if ( spi_spi6088d_setup_command(dev, cmd) )
			continue;
		
/* 		printk("Cmd rx: %p, tx: %p, inc: %d, rptr: %p, wptr: %p, count: %d, perm: %p\n", */
/* 			   pv->rx_handler, pv->tx_handler,  */
/* 			   pv->increment, */
/* 			   pv->rx_ptr, pv->tx_ptr, */
/* 			   pv->count, pv->permanent); */

		return &rq->command[pv->cur_cmd];
	}
	return NULL;
}

DEVSPI_REQUEST(spi_spi6088d_request)
{
    struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	CPU_INTERRUPT_SAVESTATE_DISABLE;

	bool_t queue_empty = !dev_spi_queue_head(&pv->queue);

	dev_spi_queue_pushback(&pv->queue, rq);

	if ( ! queue_empty )
		return;
	
	struct dev_spi_rq_cmd_s *cmd = spi_spi6088d_get_next_cmd(dev);
	assert( cmd );
	
	CPU_INTERRUPT_RESTORESTATE;

    cpu_mem_write_32(registers + SPI_IER, SPI6088D_RXBUFF);
    cpu_mem_read_32(registers + SPI_RDR);
	DO_SYNC();
	pv->tx_handler(dev);
}

DEV_IRQ(spi_spi6088d_irq)
{
    struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];

	DO_SYNC(|| pv->abort);

	pv->rx_handler(dev);
	if ( pv->count == 0 ) {
		struct dev_spi_rq_cmd_s *cmd = spi_spi6088d_get_next_cmd(dev);

		if ( !cmd ) {
            cpu_mem_write_32(registers + SPI_IDR, SPI6088D_RXBUFF);
			return 0;
		}
	}
	DO_SYNC();
	pv->tx_handler(dev);

	return 0;
}

DEVSPI_SET_BAUDRATE(spi_spi6088d_set_baudrate)
{
    struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];
	uint32_t new_data;

	if ( device_id >= pv->lun_count )
		return (uint32_t) -1;
	uint32_t divisor = MCK/br;
	if ( MCK/divisor > br )
		divisor++;
	if ( divisor > 255 )
		divisor = 255;
	if ( divisor == 0 )
		divisor = 1;

/* 	dprintk("Setting CSR[%d]'s baudrate, asked %d, got %d\n", */
/* 		   device_id, br, MCK/divisor); */

	new_data = 0
        | (cpu_mem_read_32(registers + SPI_CSR(device_id)) & 0x00ff)
		| ((divisor & 0xff) << 8)
		| ((uint32_t)xfer_delay << 24)
		| ((uint32_t)cs_delay << 16);

/* 	dprintk("Setting CSR[%d] to %p\n", device_id, new_data); */

	cpu_mem_write_32(registers + SPI_CSR(device_id), new_data);

	return MCK/divisor;
}

DEVSPI_SET_DATA_FORMAT(spi_spi6088d_set_data_format)
{
    struct spi_spi6088d_context_s *pv = dev->drv_pv;
	uintptr_t registers = (uintptr_t)dev->addr[0];
	uint32_t new_data = 0;

	if ( device_id >= pv->lun_count )
		return ERANGE;

	if ( bits_per_word > 16 || bits_per_word < 8 )
		return ERANGE;

	new_data = (bits_per_word-8) << 4;
	// Argh ! Atmel, why did you have to invert them ?
	// NPCHA == ! (mode[0])
	new_data |= ((~spi_mode)&1) << 1;
	// CPOL == (mode[1])
	new_data |= (spi_mode&2) >> 1;

	new_data |= (!!keep_cs_active) << 3;

	new_data = 0
#if 1
        | (cpu_mem_read_32(registers + SPI_CSR(device_id)) & ~0xff)
		| (new_data & 0xff)
#else
		| 0x1f02
#endif
		;

/* 	dprintk("Setting CSR[%d] to %p\n", device_id, new_data); */

	cpu_mem_write_32(registers + SPI_CSR(device_id), new_data);
	return 0;
}

#ifdef CONFIG_DRIVER_ENUM_FDT
static const struct driver_param_binder_s spi_spi6088d_param_binder[] =
{
	PARAM_BIND(struct spi_spi6088d_param_s, lun_count, PARAM_DATATYPE_INT),
	{ 0 }
};

static const struct devenum_ident_s	spi_spi6088d_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("spi6088d", sizeof(struct spi_spi6088d_param_s), spi_spi6088d_param_binder),
	{ 0 }
};
#endif

const struct driver_s   spi_spi6088d_drv =
{
    .class      = device_class_spi,
#ifdef CONFIG_DRIVER_ENUM_FDT
    .id_table   = spi_spi6088d_ids,
#endif
    .f_init     = spi_spi6088d_init,
    .f_cleanup  = spi_spi6088d_cleanup,
    .f_irq      = spi_spi6088d_irq,
	.f.spi = {
		.f_request = spi_spi6088d_request,
		.f_set_baudrate = spi_spi6088d_set_baudrate,
		.f_set_data_format = spi_spi6088d_set_data_format,
	},
};

#ifdef CONFIG_DRIVER_ENUM_FDT
REGISTER_DRIVER(spi_spi6088d_drv);
#endif

DEV_INIT(spi_spi6088d_init)
{
	struct spi_spi6088d_context_s   *pv;
	struct spi_spi6088d_param_s *param = params;
	uintptr_t registers = (uintptr_t)dev->addr[0];
	uint_fast8_t i;

	dev->drv = &spi_spi6088d_drv;

	if ( param->lun_count > 4 ) {
		printk("SPI-SPI6088D: Invalid lun count: %d\n", param->lun_count);
		return -1;
	}

	/* allocate private driver data */
	pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

	if (!pv)
		return -1;

	dev->drv_pv = pv;

//	printk("SPI-SPI6088D: configuring %s, pin mask=%x, inputs=%x\n", spi, pins, inputs);

	cpu_mem_write_32(registers + SPI_CR, SPI6088D_SPIDIS);
	{ uint_fast8_t i; for (i=0; i<200; ++i) asm volatile("nop"); }
	cpu_mem_write_32(registers + SPI_CR, SPI6088D_SWRST);
	{ uint_fast8_t i; for (i=0; i<200; ++i) asm volatile("nop"); }
	cpu_mem_write_32(registers + SPI_IDR, 0x3ff);

	cpu_mem_write_32(registers + SPI_MR, 0
		| (30<<24)
//		| SPI6088D_PCSDEC
#if 0
		| SPI6088D_PS_VARIABLE
#else
		| SPI6088D_PS_FIXED
		| (0xd<<16)
#endif
		| SPI6088D_MSTR
        );

	for ( i=0; i<param->lun_count; ++i ) {
        cpu_mem_write_32(registers + SPI_CSR(i), 0
			| SPI6088D_CSAAT
//			| SPI6088D_NCPHA
			| (SPI6088D_SCBR & (0x1f<<8))
//			| (SPI6088D_SCBR & ((48000000/400000+2)<<8))
			| (SPI6088D_DLYBS & (5<<24))
//			| SPI6088D_BITS_8;
            );
	}

	cpu_mem_write_32(registers + SPI_CR, SPI6088D_SPIEN);

	pv->lun_count = param->lun_count;
	pv->cur_cmd = (size_t)-1;

	dev_spi_queue_init(&pv->queue);

	dev_icu_sethndl(dev->icudev, dev->irq, spi_spi6088d_irq, dev);
	dev_icu_enable(dev->icudev, dev->irq, 1, 0x2 );

	return 0;
}

DEV_CLEANUP(spi_spi6088d_cleanup)
{
    struct spi_spi6088d_context_s *pv = dev->drv_pv;

    DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, spi_spi6088d_irq);

    mem_free(pv);
}

