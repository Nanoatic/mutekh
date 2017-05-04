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

#include <hexo/types.h>
#include <device/device.h>
#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/endian.h>
#include <hexo/interrupt.h>
#include <pthread.h>
#include <semaphore.h>

#include <netinet/if.h>

#include "net-ne2000.h"

#include "net-ne2000-private.h"
#include "ne2000.h"

/*
 * read from the device's memory.
 */

void				ne2000_mem_read(struct device_s	*dev,
						uint_fast16_t	offs,
						void		*dst,
						uint_fast16_t	size)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;

  /* select register bank 0 */
  ne2000_page(dev, NE2000_P0);

  /* ensure DMA operations are reset */
  ne2000_dma(dev, NE2000_DMA_ABRT);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_ISR, NE2000_RDC);

  /* setup size */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR0, size);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR1, size >> 8);

  /* setup position */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RSAR0, offs);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RSAR1, offs >> 8);

  /* start read operation */
  ne2000_dma(dev, NE2000_DMA_RD);

  /* copy the whole packet */
  if (pv->io_16)
    {
      uint16_t		*d = (uint16_t *)dst;
      uint_fast16_t	size_w = size >> 1;

      while (size_w--)
	*d++ = cpu_io_read_16(dev->addr[NET_NE2000_ADDR] + NE2000_DATA);
      if (size & 0x1)
	*(uint8_t *)d = cpu_io_read_8(dev->addr[NET_NE2000_ADDR] + NE2000_DATA);
    }
  else
    {
      uint8_t	*d = dst;

      while (size--)
	*d++ = cpu_io_read_8(dev->addr[NET_NE2000_ADDR] + NE2000_DATA);
    }
}

/*
 * init a remote DMA writing transfer.
 */

void		ne2000_dma_init_write(struct device_s	*dev,
				      uint_fast16_t	offs,
				      uint_fast16_t	size)
{
  /* select register bank 0 */
  ne2000_page(dev, NE2000_P0);

  /* ensure DMA operations are reset */
  ne2000_dma(dev, NE2000_DMA_ABRT);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_ISR, NE2000_RDC);

  /* setup size */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR0, size);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR1, size >> 8);

  /* setup position */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RSAR0, offs);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RSAR1, offs >> 8);

  /* start write operation */
  ne2000_dma(dev, NE2000_DMA_WR);
}

/*
 * output data for remote DMA.
 */

void				ne2000_dma_do_write(struct device_s	*dev,
						    void		*src,
						    uint_fast16_t	size)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;

  /* copy the whole packet */
  if (pv->io_16)
    {
      uint16_t	*d = (uint16_t *)src;

      size = ALIGN_VALUE_UP(size, 2);

      size >>= 1;
      while (size--)
	cpu_io_write_16(dev->addr[NET_NE2000_ADDR] + NE2000_DATA, *d++);
    }
  else
    {
      uint8_t	*d = src;

      while (size--)
	{
	  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_DATA, *d++);
	}
    }
}

/*
 * try reading and writing a stupid sentence to memory.
 */

static uint_fast8_t	ne2000_rw_test(struct device_s	*dev)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;
  uint_fast8_t			endian;
  uint_fast16_t			timeout = 2000;
  char				ref[] = "MutekH NE2000 Driver";
  char				buf[sizeof (ref)];

  /* configure the device for the test */
  /* send a reset signal */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RESET,
		 cpu_io_read_8(dev->addr[NET_NE2000_ADDR] + NE2000_RESET));

  /* wait a moment */
  while (timeout)
    timeout--;
  timeout = 200000;

  /* stop completely the device */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_CMD,
		 NE2000_P0 | NE2000_DMA_ABRT | NE2000_STP);

#ifdef CPU_ENDIAN_ISBIG
  endian = NE2000_BE;
#else
  endian = NE2000_LE;
#endif

  /* setup data configuration registers */
  if (pv->io_16)
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_DCR,
		   NE2000_16BITS | endian | NE2000_NORMAL | NE2000_FIFO4);
  else
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_DCR,
		   NE2000_8BITS | endian | NE2000_NORMAL | NE2000_FIFO4);
  /* setup transmit and receive in loopback */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RCR, NE2000_MONITOR);
  /* setup RX ring */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_PSTART, pv->tx_buf);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_PSTOP, pv->mem);

  /* start the device */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_CMD,
		 NE2000_P0 | NE2000_DMA_ABRT | NE2000_STA);

  /* do the R/W test */
  ne2000_mem_write(dev, pv->tx_buf << 8, ref, sizeof (ref));
  while (timeout && !(cpu_io_read_8(dev->addr[NET_NE2000_ADDR] + NE2000_ISR) & NE2000_RDC))
    timeout--;
  memset(buf, 0, sizeof (ref));
  ne2000_mem_read(dev, pv->tx_buf << 8, buf, sizeof (ref));

  return !memcmp(ref, buf, sizeof (ref));
}

/*
 * probe device capabilities.
 */

uint_fast8_t			ne2000_probe(struct device_s	*dev)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;
  uint8_t			buf[ETH_ALEN * 2];
  uint_fast8_t			i;
#if 1
  /* try 16 bits mode with 32k */
  pv->io_16 = 1;
  pv->mem = NE2000_MEM_32K;
  pv->tx_buf = NE2000_MEM_16K;
  pv->rx_buf = NE2000_MEM_16K + NE2000_TX_BUFSZ;

  if (ne2000_rw_test(dev))
    goto ok;

  /* try 16 bits mode with 16k */
  pv->io_16 = 1;
  pv->mem = NE2000_MEM_16K;
  pv->tx_buf = NE2000_MEM_8K;
  pv->rx_buf = NE2000_MEM_8K + NE2000_TX_BUFSZ;

  if (ne2000_rw_test(dev))
    goto ok;
#endif
  /* try 8 bits mode with 32k */
  pv->io_16 = 0;
  pv->mem = NE2000_MEM_32K;
  pv->tx_buf = NE2000_MEM_16K;
  pv->rx_buf = NE2000_MEM_16K + NE2000_TX_BUFSZ;

  if (ne2000_rw_test(dev))
    goto ok;

  /* try 8 bits mode with 16k */
  pv->io_16 = 0;
  pv->mem = NE2000_MEM_16K;
  pv->tx_buf = NE2000_MEM_8K;
  pv->rx_buf = NE2000_MEM_8K + NE2000_TX_BUFSZ;

  if (ne2000_rw_test(dev))
    goto ok;

  /* all configuration failed */
  return 0;

 ok:
  /* everything ok */

  /* determine MAC address, the first 6 bytes/words of the PROM */
  ne2000_mem_read(dev, 0, buf, ETH_ALEN * 2);

  if (pv->io_16)
    for (i = 0; i < ETH_ALEN; i++)
      pv->mac[i] = buf[i << 1];
  else
    for (i = 0; i < ETH_ALEN; i++)
      pv->mac[i] = buf[i];

  return 1;
}

/*
 * init device. refer to the 8390 documentation for this sequence.
 */

void				ne2000_init(struct device_s	*dev)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;
  uint_fast8_t			endian;
  uint_fast8_t			i;

  /* stop completely the device */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_CMD,
		 NE2000_P0 | NE2000_DMA_ABRT | NE2000_STP);

#ifdef CPU_ENDIAN_ISBIG
  endian = NE2000_BE;
#else
  endian = NE2000_LE;
#endif

  /* setup data configuration registers */
  if (pv->io_16)
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_DCR,
		   NE2000_16BITS | endian | NE2000_NORMAL | NE2000_FIFO4);
  else
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_DCR,
		   NE2000_8BITS | endian | NE2000_NORMAL | NE2000_FIFO4);
  /* clear dma state */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR0, 0);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RBCR1, 0);
  /* setup receive configuration register */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_RCR,
		 NE2000_REJECT_ON_ERROR | NE2000_ACCEPT_BCAST |
		 NE2000_REJECT_MCAST);
  /* enter loopback mode */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_TCR, 0x2);
  /* initialize TX and RX buffers */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_TPSR, pv->tx_buf);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_PSTART, pv->rx_buf);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_PSTOP, pv->mem);
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_BOUND, pv->rx_buf);
  /* clear ISR */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_ISR, 0xff);
  /* activate interrupts */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_IMR,
		 NE2000_PRXE | NE2000_PTXE | NE2000_TXEE | NE2000_OVWE |
		 NE2000_RDCE);
  /* init MAC */
  ne2000_page(dev, NE2000_P1);
  for (i = 0; i < ETH_ALEN; i++)
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_PAR + i, pv->mac[i]);
  for (i = 0; i < ETH_ALEN; i++)
    cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_MAR + i, 0xff);
  /* init current receive buffer pointer */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_CURR, pv->rx_buf + 1);
  /* bring the device up */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_CMD,
		 NE2000_P0 | NE2000_DMA_ABRT | NE2000_STA);
  /* setup transmit configuration register */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_TCR, NE2000_AUTOCRC);
  /* clear ISR */
  cpu_io_write_8(dev->addr[NET_NE2000_ADDR] + NE2000_ISR, 0xff);
}
