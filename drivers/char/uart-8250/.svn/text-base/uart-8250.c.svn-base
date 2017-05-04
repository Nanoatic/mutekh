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


#include <hexo/types.h>

#include <device/icu.h>
#include <device/char.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>

#include "uart-8250.h"

#include "uart-8250-private.h"

#include "8250.h"


static void uart_8250_try_read(struct device_s *dev)
{
  struct uart_8250_context_s *pv = dev->drv_pv;
  struct dev_char_rq_s *rq;

  while ((rq = dev_char_queue_head(&pv->read_q)))
    {
      size_t size = 0;

      assert(rq->size);

      while (size < rq->size && cpu_io_read_8(dev->addr[0] + UART_8250_IIR) & UART_8250_IIR_RX)
	{
	  rq->data[size] = cpu_io_read_8(dev->addr[0] + UART_8250_RBR);
	  size++;
	}

      if (!size)
	return;

      rq->size -= size;
      rq->error = 0;

      if (rq->callback(dev, rq, size) || rq->size == 0)
	dev_char_queue_remove(&pv->read_q, rq);
      else
	rq->data += size;
    }
}

static void uart_8250_try_write(struct device_s *dev)
{
  struct uart_8250_context_s *pv = dev->drv_pv;
  struct dev_char_rq_s *rq;

  while ((rq = dev_char_queue_head(&pv->write_q)))
    {
      size_t size = 0;

      assert(rq->size);

      while (size < rq->size && cpu_io_read_8(dev->addr[0] + UART_8250_IIR) & UART_8250_IIR_TX)
	{
	  cpu_io_write_8(dev->addr[0] + UART_8250_THR, rq->data[size]);
	  size++;
	}

      if (!size)
	return;

      rq->size -= size;
      rq->error = 0;

      if (rq->callback(dev, rq, size) || rq->size == 0)
	dev_char_queue_remove(&pv->write_q, rq);
      else
	rq->data += size;
    }
}

DEVCHAR_REQUEST(uart_8250_request)
{
  struct uart_8250_context_s	*pv = dev->drv_pv;
  bool_t empty;

  LOCK_SPIN_IRQ(&dev->lock);

  switch (rq->type)
    {
    case DEV_CHAR_READ:
      empty = dev_char_queue_isempty(&pv->read_q);
      dev_char_queue_pushback(&pv->read_q, rq);
      if (empty)
	uart_8250_try_read(dev);
      break;

    case DEV_CHAR_WRITE:
      empty = dev_char_queue_isempty(&pv->write_q);
      dev_char_queue_pushback(&pv->write_q, rq);
      if (empty)
	uart_8250_try_write(dev);
      break;
    }

  LOCK_RELEASE_IRQ(&dev->lock);
}

/* 
 * device close operation
 */

DEV_CLEANUP(uart_8250_cleanup)
{
  struct uart_8250_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, uart_8250_irq);

  dev_char_queue_destroy(&pv->read_q);
  dev_char_queue_destroy(&pv->write_q);
  mem_free(pv);
}

/*
 * IRQ handler
 */

DEV_IRQ(uart_8250_irq)
{
  struct uart_8250_context_s *pv = dev->drv_pv;
  struct dev_char_rq_s *rq;
  bool_t done = 0;

  lock_spin(&dev->lock);

  while (!(cpu_io_read_8(dev->addr[0] + UART_8250_IIR) & UART_8250_IIR_NOPENDING))
    {
      done = 1;
      uart_8250_try_read(dev);
      uart_8250_try_write(dev);
    }

  lock_release(&dev->lock);

  return done;
}

/* 
 * device open operation
 */

const struct driver_s	uart_8250_drv =
{
  .class		= device_class_char,
  .f_init		= uart_8250_init,
  .f_cleanup		= uart_8250_cleanup,
  .f_irq		= uart_8250_irq,
  .f.chr = {
    .f_request		= uart_8250_request,
  }
};

DEV_INIT(uart_8250_init)
{
  struct uart_8250_context_s	*pv;

  dev->drv = &uart_8250_drv;
  
  /* alocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  dev_char_queue_init(&pv->read_q);
  dev_char_queue_init(&pv->write_q);

  dev->drv_pv = pv;

  pv->line_mode = UART_8250_LCR_8BITS | UART_8250_LCR_PARNO | UART_8250_LCR_1STOP;

  /*
  rate div dll dlm
  50  2304  $09  $00
  110 1047  $04  $17
  220  524  $02  $0C
  300  384  $01  $80
  600  192  $00  $C0
  1200  96  $00  $60
  2400  48  $00  $30
  4800  24  $00  $18
  9600  12  $00  $0C
  19200  6  $00  $06
  38400  3  $00  $03
  57600  2  $00  $02
  115200 1  $00  $01
   */
#if 0
  // 9600 baud
  pv->line_speed = 0x000c;
#else
  // 9600 baud
  pv->line_speed = 0x0001;
#endif

  cpu_io_write_8(dev->addr[0] + UART_8250_LCR, 0);

  cpu_io_write_8(dev->addr[0] + UART_8250_IER, UART_8250_IER_RX | UART_8250_IER_TX);

  cpu_io_write_8(dev->addr[0] + UART_8250_FCR, UART_8250_FCR_FIFO | UART_8250_FCR_CLRRX | UART_8250_FCR_CLRTX);
  cpu_io_write_8(dev->addr[0] + UART_8250_FCR, UART_8250_FCR_FIFO);

  cpu_io_write_8(dev->addr[0] + UART_8250_MCR, 0
#if defined(CONFIG_ARCH_IBMPC)
		 /* GP Output pins must be set on ibmpc to activate IRQ routing */
		 | UART_8250_MCR_OUT1 | UART_8250_MCR_OUT2
#endif
		 );

  cpu_io_write_8(dev->addr[0] + UART_8250_LCR, UART_8250_LCR_DLAB);
  cpu_io_write_8(dev->addr[0] + UART_8250_DLL, pv->line_speed & 0xff);
  cpu_io_write_8(dev->addr[0] + UART_8250_DLM, pv->line_speed >> 8);

  cpu_io_write_8(dev->addr[0] + UART_8250_LCR, pv->line_mode);

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, uart_8250_irq);

  return 0;
}

