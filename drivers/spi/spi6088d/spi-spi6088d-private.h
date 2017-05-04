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

#ifndef SPI_SPI6088D_PRIVATE_H_
#define SPI_SPI6088D_PRIVATE_H_

#include <hexo/types.h>

#define CMD_HANDLER(x) void x(struct device_s *dev)
typedef CMD_HANDLER(cmd_handler_f);

struct spi_spi6088d_context_s
{
	dev_spi_queue_root_t queue;
	uint32_t cs_bits;
	cmd_handler_f *tx_handler;
	cmd_handler_f *rx_handler;
	size_t cur_cmd;
	devspi_wait_value_callback_t *wait_cb;
	size_t count;
	uint32_t permanent;
	uint_fast8_t increment;

	uintptr_t tx_ptr;
	uintptr_t rx_ptr;

	uint16_t pad_byte;

	uint_fast8_t lun_count;

	bool_t abort;
};

#define SPI_CR 0x0
#define SPI_MR 0x4
#define SPI_RDR 0x8
#define SPI_TDR 0xc
#define SPI_SR 0x10
#define SPI_IER 0x14
#define SPI_IDR 0x18
#define SPI_IMR 0x1c
#define SPI_CSR(x) (0x30 + 4*(x))

#define SPI6088D_SPIEN       ((uint32_t)1 <<  0)
#define SPI6088D_SPIDIS      ((uint32_t)1 <<  1)
#define SPI6088D_SWRST       ((uint32_t)1 <<  7)
#define SPI6088D_LASTXFER    ((uint32_t)1 << 24)

#define SPI6088D_MSTR        ((uint32_t)1 <<  0)
#define SPI6088D_PS          ((uint32_t)1 <<  1)
#define 	SPI6088D_PS_FIXED                ((uint32_t)0 <<  1)
#define 	SPI6088D_PS_VARIABLE             ((uint32_t)1 <<  1)
#define SPI6088D_PCSDEC      ((uint32_t)1 <<  2)
#define SPI6088D_FDIV        ((uint32_t)1 <<  3)
#define SPI6088D_MODFDIS     ((uint32_t)1 <<  4)
#define SPI6088D_LLB         ((uint32_t)1 <<  7)
#define SPI6088D_PCS         ((uint32_t)0xF << 16)
#define SPI6088D_DLYBCS      ((uint32_t)0xFF << 24)

#define SPI6088D_RDRF        ((uint32_t)1 <<  0)
#define SPI6088D_TDRE        ((uint32_t)1 <<  1)
#define SPI6088D_MODF        ((uint32_t)1 <<  2)
#define SPI6088D_OVRES       ((uint32_t)1 <<  3)
#define SPI6088D_ENDRX       ((uint32_t)1 <<  4)
#define SPI6088D_ENDTX       ((uint32_t)1 <<  5)
#define SPI6088D_RXBUFF      ((uint32_t)1 <<  6)
#define SPI6088D_TXBUFE      ((uint32_t)1 <<  7)
#define SPI6088D_NSSR        ((uint32_t)1 <<  8)
#define SPI6088D_TXEMPTY     ((uint32_t)1 <<  9)
#define SPI6088D_SPIENS      ((uint32_t)1 << 16)

#define SPI6088D_CPOL        ((uint32_t)1 <<  0)
#define SPI6088D_NCPHA       ((uint32_t)1 <<  1)
#define SPI6088D_CSAAT       ((uint32_t)1 <<  3)
#define SPI6088D_BITS        ((uint32_t)0xF <<  4)
#define SPI6088D_SCBR        ((uint32_t)0xFF <<  8)
#define SPI6088D_DLYBS       ((uint32_t)0xFF << 16)
#define SPI6088D_DLYBCT      ((uint32_t)0xFF << 24)


#endif
