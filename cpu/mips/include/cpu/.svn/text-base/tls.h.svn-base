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

   Copyright (C) 2005 Free Software Foundation, Inc.

   Copyright (c) UPMC, Lip6, STMicroelectronics
    Joel Porquet <joel.porquet@lip6.fr>, 2009

   Based on uClibc
*/

#ifndef _MIPS_TLS_H
#define _MIPS_TLS_H

#include <hexo/cpu.h>

/* check we have mips32 */
#if __mips < 32
# error "TLS only available from mips32 version"
#endif

/* Mips uses "variant I": the DTV pointer is allocated at the TP */
/* However, not entirely true for mips: the TP points to the end of TCB, so DTV pointer is at tcb[-1].dtv */
#define TLS_DTVP_AT_TP 1

typedef union 
{
  size_t    counter;
  void      *ptr;
} tls_dtv_t;

typedef struct
{
  tls_dtv_t *dtvp;
  void      *priv;
} tls_tcb_t;

/* for mips, unlike the specs say, tp points after the tcb */
#define TLS_TCB_OFFSET          (sizeof(tls_tcb_t))
#define TLS_SIZE_PRE_OFFSET     (0)
#define TLS_SIZE_POST_OFFSET    (sizeof(tls_tcb_t))

/* The thread pointer (in hardware register $29) points to the end of the TCB + 0x7000, as for PowerPC. */
#define TLS_TP_OFFSET  0x7000

/* A pointer in DTV points to the begin of the corresponding TLS area + 0x8000 */
#define TLS_DTP_OFFSET  0x8000

/* Init the thread pointer register */
static inline void tls_init_tp (uintptr_t tp)
{
    /* first, allow user mode to access the tp register */
    reg_t hwrena = cpu_mips_mfc0(7, 0);
    hwrena |= 0x20000000; // tls register is #29
    cpu_mips_mtc0(7, 0, hwrena);
    /* then, fill in the tp register */
    cpu_mips_mtc0(4, 2, tp + TLS_TP_OFFSET);
}

#endif	/* _MIPS_TLS_H */
