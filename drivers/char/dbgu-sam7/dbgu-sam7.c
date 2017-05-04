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

#include <stdint.h>
#include <mutek/printk.h>

#include "arch/sam7/at91sam7x256.h"

static PRINTF_OUTPUT_FUNC(__sam7_dbgu_output)
{
	while (len > 0) {
		while ( !(AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_COMM_TX) )
			;
		asm volatile ("MCR	p14, 0, %0, c1, c0, 0" :: "r" (*str));
		
		len--;
		str++;
    }
}

void sam7_dbgu_init()
{
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_TXDIS | AT91C_US_RXDIS;

	AT91C_BASE_PMC->PMC_PCER = 1<<1;

	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RSTSTA;
	AT91C_BASE_DBGU->DBGU_CR = AT91C_US_TXEN;

	AT91C_BASE_DBGU->DBGU_IDR = (uint32_t)-1;

	printk_set_output(__sam7_dbgu_output, NULL);
}
