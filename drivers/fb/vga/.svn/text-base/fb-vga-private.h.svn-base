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


#ifndef FB_VGA_PRIVATE_H_
#define FB_VGA_PRIVATE_H_

#include <hexo/types.h>
#include <device/driver.h>
#include <hexo/lock.h>
#include <hexo/error.h>

struct fb_vga_mode_s;

struct fb_vga_context_s
{
  lock_t			lock;
  const struct fb_vga_mode_s	*mode;
};

struct fb_vga_mode_s
{
  uint_fast16_t		xres;
  uint_fast16_t		yres;
  uint_fast8_t		bpp;
  uint_fast8_t		packing;
  uint_fast8_t		maxpage;
  const uint8_t		regs[60];
};

#define FB_VGA_FB_ADDRESS	0x000a0000

/* VGA index register ports */
#define FB_VGA_CRT_IC  0x3D4		/* CRT Controller Index - color emulation */
#define FB_VGA_CRT_IM  0x3B4		/* CRT Controller Index - mono emulation */
#define FB_VGA_ATT_IW  0x3C0		/* Attribute Controller Index & Data Write Register */
#define FB_VGA_GRA_I   0x3CE		/* Graphics Controller Index */
#define FB_VGA_SEQ_I   0x3C4		/* Sequencer Index */
#define FB_VGA_PEL_IW  0x3C8		/* PEL Write Index */
#define FB_VGA_PEL_IR  0x3C7		/* PEL Read Index */

/* VGA data register ports */
#define FB_VGA_CRT_DC  0x3D5		/* CRT Controller Data Register - color emulation */
#define FB_VGA_CRT_DM  0x3B5		/* CRT Controller Data Register - mono emulation */
#define FB_VGA_ATT_R   0x3C1		/* Attribute Controller Data Read Register */
#define FB_VGA_GRA_D   0x3CF		/* Graphics Controller Data Register */
#define FB_VGA_SEQ_D   0x3C5		/* Sequencer Data Register */
#define FB_VGA_MIS_R   0x3CC		/* Misc Output Read Register */
#define FB_VGA_MIS_W   0x3C2		/* Misc Output Write Register */
#define FB_VGA_IS1_RC  0x3DA		/* Input Status Register 1 - color emulation */
#define FB_VGA_IS1_RM  0x3BA		/* Input Status Register 1 - mono emulation */
#define FB_VGA_PEL_D   0x3C9		/* PEL Data Register */
#define FB_VGA_PEL_MSK 0x3C6		/* PEL mask register */

/* 8514/MACH regs we need outside of the mach32 driver.. */
#define FB_VGA_PEL8514_D	0x2ED
#define FB_VGA_PEL8514_IW	0x2EC
#define FB_VGA_PEL8514_IR	0x2EB
#define FB_VGA_PEL8514_MSK	0x2EA

/* EGA-specific registers */

#define FB_VGA_GRA_E0	0x3CC		/* Graphics enable processor 0 */
#define FB_VGA_GRA_E1	0x3CA		/* Graphics enable processor 1 */

/* standard VGA indexes max counts */
#define FB_VGA_CRT_C   24		/* 24 CRT Controller Registers */
#define FB_VGA_ATT_C   21		/* 21 Attribute Controller Registers */
#define FB_VGA_GRA_C   9		/* 9  Graphics Controller Registers */
#define FB_VGA_SEQ_C   5		/* 5  Sequencer Registers */
#define FB_VGA_MIS_C   1		/* 1  Misc Output Register */

/* VGA registers saving indexes */
#define FB_VGA_CRT     0		/* CRT Controller Registers start */
#define FB_VGA_ATT     (FB_VGA_CRT + FB_VGA_CRT_C)	/* Attribute Controller Registers start */
#define FB_VGA_GRA     (FB_VGA_ATT + FB_VGA_ATT_C)	/* Graphics Controller Registers start */
#define FB_VGA_SEQ     (FB_VGA_GRA + FB_VGA_GRA_C)	/* Sequencer Registers */
#define FB_VGA_MIS     (FB_VGA_SEQ + FB_VGA_SEQ_C)	/* General Registers */
#define FB_VGA_EXT     (FB_VGA_MIS + FB_VGA_MIS_C)	/* SVGA Extended Registers */


#endif

