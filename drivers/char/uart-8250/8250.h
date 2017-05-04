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

#ifndef _8250_H_
#define _8250_H_

#define UART_8250_IIR	2	/* R: int identification */
#define UART_8250_FCR	2	/* W: FIFO control */
#define UART_8250_LCR	3	/* RW: Line control */
#define UART_8250_MCR	4	/* RW: Modem control */
#define UART_8250_LSR	5	/* R: Line status */
#define UART_8250_MSR	6	/* R: Modem status */
#define UART_8250_SCR	7	/* RW: Scratch */

/* DLAB = 0 */
#define UART_8250_RBR	0	/* R: recieve buffer reg */
#define UART_8250_THR	0	/* W: transmiter holding reg */
#define UART_8250_IER	1	/* RW: int enable */

/* DLAB = 1 */
#define UART_8250_DLL	0	/* RW: divisor latch LSB */
#define UART_8250_DLM	1	/* RW: divisor latch MSB */

/* FCR */
#define UART_8250_FCR_FIFO	0x01	/* enable FIFO */
#define UART_8250_FCR_CLRRX	0x02	/* clear RX FIFO */
#define UART_8250_FCR_CLRTX	0x04	/* clear TX FIFO */
#define UART_8250_FCR_RXIRQLV1	0x00	/* RX interrupt level 1 byte */
#define UART_8250_FCR_RXIRQLV4	0x40	/* RX interrupt level 4 bytes */
#define UART_8250_FCR_RXIRQLV8	0x80	/* RX interrupt level 8 bytes */
#define UART_8250_FCR_RXIRQLV14	0xc0	/* RX interrupt level 14 bytes */

/* LCR */
#define UART_8250_LCR_5BITS	0x00	/* 5 Bits char */
#define UART_8250_LCR_6BITS	0x01	/* 6 Bits char */
#define UART_8250_LCR_7BITS	0x02	/* 7 Bits char */
#define UART_8250_LCR_8BITS	0x03	/* 8 Bits char */
#define UART_8250_LCR_1STOP	0x00	/* 1 Stop bit */
#define UART_8250_LCR_2STOP	0x04	/* 1.5/2 Stop bits */
#define UART_8250_LCR_PARNO	0x00	/* No parity */
#define UART_8250_LCR_PARODD	0x08	/* Odd parity */
#define UART_8250_LCR_PAREVEN	0x18	/* Odd parity */
#define UART_8250_LCR_PARHIGH	0x28	/* High parity */
#define UART_8250_LCR_PARLOW	0x38	/* Low parity */
#define UART_8250_LCR_BREAK	0x40	/* Break signal enable */
#define UART_8250_LCR_DLAB	0x80	/* Select registers @0-1 */

/* MCR */
#define UART_8250_MCR_DTR	0x01	/* Data Terminal Ready */
#define UART_8250_MCR_RTS	0x02	/* Ready To Send */
#define UART_8250_MCR_OUT1	0x04	/* GP Ouput pin 1 */
#define UART_8250_MCR_OUT2	0x08	/* GP Ouput pin 2 */
#define UART_8250_MCR_LOOPBACK	0x10	/* Loop Back Mode */

/* IER */
#define UART_8250_IER_RX	0x01	/* RX data available */
#define UART_8250_IER_TX	0x02	/* TX fifo empty */
#define UART_8250_IER_LST	0x04	/* RX line status change */
#define UART_8250_IER_MSST	0x08	/* Modem Satus change */

/* IIR */
#define UART_8250_IIR_NOPENDING	0x01	/* No interrupt pending */
#define UART_8250_IIR_MASK	0x0e	/* Cause value mask */
#define  UART_8250_IIR_MSST	0x00	/* Modem Satus change */
#define  UART_8250_IIR_TX	0x02	/* TX fifo empty */
#define  UART_8250_IIR_RX	0x04	/* RX data available */
#define  UART_8250_IIR_LST	0x06	/* RX line status change */

/* LSR */
#define UART_8250_LSR_RX	0x01	/* RX data available */
#define UART_8250_LSR_OVERRUN	0x02	/* Overrun error */
#define UART_8250_LSR_PARITY	0x04	/* Parity error */
#define UART_8250_LSR_FRAMING	0x08	/* Framing error */

#define UART_8250_LSR_BREAK	0x10	/* Break recieved */
#define UART_8250_LSR_TXEMPTY	0x20	/* TH reg empty */
#define UART_8250_LSR_IDLE	0x40	/* TH reg empty & line idle */
#define UART_8250_LSR_DATAERR	0x80	/*  */

#endif

