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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#ifndef _NE2000_H
#define _NE2000_H

/* memory size */
#define NE2000_MEM_8K	32
#define NE2000_MEM_16K	64
#define NE2000_MEM_32K	128

#define NE2000_TX_BUFSZ	6


/* command register bits */
#define NE2000_STP	(1 << 0)
#define NE2000_STA	(1 << 1)
#define NE2000_TXP	(1 << 2)
#define NE2000_RD0	(1 << 3)
#define NE2000_RD1	(1 << 4)
#define NE2000_RD2	(1 << 5)
#define NE2000_PS0	(1 << 6)
#define NE2000_PS1	(1 << 7)

#define NE2000_DMA_MASK	(NE2000_RD0 | NE2000_RD1 | NE2000_RD2)
#define NE2000_PG_MASK	(NE2000_PS0 | NE2000_PS1)

/* shortcuts for DMA transfers */
#define NE2000_DMA_RD	NE2000_RD0
#define NE2000_DMA_WR	NE2000_RD1
#define NE2000_DMA_SEND	(NE2000_RD1 | NE2000_RD0)
#define NE2000_DMA_ABRT	NE2000_RD2

/* shortcuts for page select */
#define NE2000_P0	0
#define NE2000_P1	NE2000_PS0
#define NE2000_P2	NE2000_PS1

/* interrupt register bits */
#define NE2000_PRX	(1 << 0)
#define NE2000_PTX	(1 << 1)
#define NE2000_RXE	(1 << 2)
#define NE2000_TXE	(1 << 3)
#define NE2000_OVW	(1 << 4)
#define NE2000_CNT	(1 << 5)
#define NE2000_RDC	(1 << 6)

/* interrupt mask */
#define NE2000_PRXE	(1 << 0)
#define NE2000_PTXE	(1 << 1)
#define NE2000_RXEE	(1 << 2)
#define NE2000_TXEE	(1 << 3)
#define NE2000_OVWE	(1 << 4)
#define NE2000_CNTE	(1 << 5)
#define NE2000_RDCE	(1 << 6)

/* receive status register flags */
#define NE2000_SPRX	(1 << 0)

/* register addresses */
#define NE2000_CMD	0x0
#define NE2000_PSTART	0x1
#define NE2000_PAR	0x1
#define NE2000_PSTOP	0x2
#define NE2000_BOUND	0x3
#define NE2000_TPSR	0x4
#define NE2000_TBCR0	0x5
#define NE2000_TBCR1	0x6
#define NE2000_ISR	0x7
#define NE2000_CURR	0x7
#define NE2000_MAR	0x8
#define NE2000_RSAR0	0x8
#define NE2000_RSAR1	0x9
#define NE2000_RBCR0	0xA
#define NE2000_RBCR1	0xB
#define NE2000_RCR	0xC
#define NE2000_RSR	0xC
#define NE2000_TCR	0xD
#define NE2000_DCR	0xE
#define NE2000_IMR	0xF
#define NE2000_DATA	0x10
#define NE2000_RESET	0x1F

/* data configuration flags */
/* data width */
#define NE2000_8BITS	0
#define NE2000_16BITS	(1 << 0)
/* data endianness */
#define NE2000_LE	0
#define NE2000_BE	(1 << 1)
/* mode */
#define NE2000_LOOPBACK	0
#define NE2000_NORMAL	(1 << 3)
/* FIFO threshold */
#define NE2000_FIFO1	0
#define NE2000_FIFO2	(1 << 5)
#define NE2000_FIFO4	(1 << 6)
#define NE2000_FIFO6	((1 << 5) | (1 << 6))

/* transmit flags */
#define NE2000_NOCRC	(1 << 0)
#define NE2000_AUTOCRC	0

/* receive flags */
/* bad frames */
#define NE2000_REJECT_ON_ERROR	0
#define NE2000_RESUME_ON_ERROR	(1 << 0)
/* broadcast packets */
#define NE2000_REJECT_BCAST	0
#define NE2000_ACCEPT_BCAST	(1 << 2)
/* multicast packets */
#define NE2000_REJECT_MCAST	0
#define NE2000_ACCEPT_MCAST	(1 << 3)
/* enable promiscuous */
#define NE2000_PROMISCUOUS	(1 << 4)
/* enable hardware check of destination and CRC */
#define NE2000_MONITOR		(1 << 5)

/* the following struct is appended to the packet automatically on reception */

struct		ne2000_header_s
{
  uint8_t	status;
  uint8_t	next;
  uint16_t	size;
} __attribute__ ((packed));

/*
 * prototypes
 */

void		ne2000_mem_read(struct device_s	*dev,
				uint_fast16_t	offs,
				void		*dst,
				uint_fast16_t	size);
void		ne2000_dma_init_write(struct device_s	*dev,
				      uint_fast16_t	offs,
				      uint_fast16_t	size);
void		ne2000_dma_do_write(struct device_s	*dev,
				    void		*src,
				    uint_fast16_t	size);
uint_fast8_t	ne2000_probe(struct device_s	*dev);
void		ne2000_init(struct device_s	*dev);

#include <hexo/types.h>
#include <hexo/iospace.h>

/*
 * update the command register.
 */

static inline void	ne2000_command(struct device_s	*dev,
				       uint_fast8_t	cmd)
{
  uint_fast16_t		addr = dev->addr[NET_NE2000_ADDR] + NE2000_CMD;

  cpu_io_write_8(addr, cpu_io_read_8(addr) | cmd);
}

/*
 * update the command register for DMA operations.
 */

static inline void	ne2000_dma(struct device_s	*dev,
				   uint_fast8_t		cmd)
{
  uint_fast16_t		addr = dev->addr[NET_NE2000_ADDR] + NE2000_CMD;

  cpu_io_write_8(addr, (cpu_io_read_8(addr) & ~NE2000_DMA_MASK) | cmd);
}

/*
 * update the command register for page selection.
 */

static inline void	ne2000_page(struct device_s	*dev,
				    uint_fast8_t	cmd)
{
  uint_fast16_t		addr = dev->addr[NET_NE2000_ADDR] + NE2000_CMD;

  cpu_io_write_8(addr, (cpu_io_read_8(addr) & ~NE2000_PG_MASK) | cmd);
}

/*
 * write to the device's memory.
 */

static inline void	ne2000_mem_write(struct device_s	*dev,
					 uint_fast16_t		offs,
					 void			*src,
					 uint_fast16_t		size)
{
  ne2000_dma_init_write(dev, offs, size);

  ne2000_dma_do_write(dev, src, size);
}

#endif
