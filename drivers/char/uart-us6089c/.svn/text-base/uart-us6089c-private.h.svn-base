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


#ifndef UART_US6089C_PRIVATE_H_
#define UART_US6089C_PRIVATE_H_

#include <hexo/types.h>
#include <device/device.h>

#include <hexo/gpct_platform_hexo.h>

/**************************************************************/

struct uart_us6089c_context_s
{
  dev_char_queue_root_t		read_q;
  dev_char_queue_root_t		write_q;
};

// Control Register
#define US_CR 0
// Mode Register
#define US_MR 4
// Interrupt Enable Register
#define US_IER 8
// Interrupt Disable Register
#define US_IDR 0xc
// Interrupt Mask Register
#define US_IMR 0x10
// Channel Status Register
#define US_CSR 0x14
// Receiver Holding Register
#define US_RHR 0x18
// Transmitter Holding Register
#define US_THR 0x1c
// Baud Rate Generator Register
#define US_BRGR 0x20
// Receiver Time-out Register
#define US_RTOR 0x24
// Transmitter Time-guard Register
#define US_TTGR 0x28
// FI_DI_Ratio Register
#define US_FIDI 0x40
// Nb Errors Register
#define US_NER 0x44
// IRDA_FILTER Register
#define US_IF 0x4c

#define US6089C_RXRDY        ((uint32_t)1 <<  0)
#define US6089C_TXRDY        ((uint32_t)1 <<  1)
#define US6089C_ENDRX        ((uint32_t)1 <<  3)
#define US6089C_ENDTX        ((uint32_t)1 <<  4)
#define US6089C_OVRE         ((uint32_t)1 <<  5)
#define US6089C_FRAME        ((uint32_t)1 <<  6)
#define US6089C_PARE         ((uint32_t)1 <<  7)
#define US6089C_TXEMPTY      ((uint32_t)1 <<  9)
#define US6089C_TXBUFE       ((uint32_t)1 << 11)
#define US6089C_RXBUFF       ((uint32_t)1 << 12)
#define US6089C_COMM_TX      ((uint32_t)1 << 30)
#define US6089C_COMM_RX      ((uint32_t)1 << 31)

#define US6089C_RSTRX        ((uint32_t)1 <<  2)
#define US6089C_RSTTX        ((uint32_t)1 <<  3)
#define US6089C_RXEN         ((uint32_t)1 <<  4)
#define US6089C_RXDIS        ((uint32_t)1 <<  5)
#define US6089C_TXEN         ((uint32_t)1 <<  6)
#define US6089C_TXDIS        ((uint32_t)1 <<  7)
#define US6089C_RSTSTA       ((uint32_t)1 <<  8)

#define US6089C_PAR_EVEN       ((uint32_t)0 <<  9)
#define US6089C_PAR_ODD        ((uint32_t)1 <<  9)
#define US6089C_PAR_SPACE      ((uint32_t)2 <<  9)
#define US6089C_PAR_MARK       ((uint32_t)3 <<  9)
#define US6089C_PAR_NONE       ((uint32_t)4 <<  9)
#define US6089C_PAR_MULTI_DROP ((uint32_t)6 <<  9)

#endif

