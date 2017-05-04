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

    This driver is based on svgalib code (http://www.svgalib.org/)

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <stdlib.h>

#include <device/fb.h>

#include <hexo/types.h>
#include <hexo/iospace.h>
#include <hexo/error.h>

#include "fb-vga-private.h"

static const struct fb_vga_mode_s	vga_modes[2] =
  {
    /* 320 x 200 x 256 */
    {
      .xres = 320,
      .yres = 200,
      .bpp = 8,
      .packing = FB_PACK_INDEX,
      .maxpage = 3,
      .regs = {
	0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f, 0x00, 0x41, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x9c, 0x8e, 0x8f, 0x28, 0x40, 0x96, 0xb9, 0xa3,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff,
	0x03, 0x01, 0x0f, 0x00, 0x0e,
	0x63
      },
    },
    { 0 },
  };

static void fb_vga_reg_outcrtc(uint16_t index, uint8_t val)
{
  uint16_t	v;

  v = ((uint16_t) val << 8) + index;
  cpu_io_write_16(FB_VGA_CRT_IC, v);
}

static uint8_t fb_vga_reg_incrtc(uint8_t index)
{
  cpu_io_write_8(FB_VGA_CRT_IC, index);
  return cpu_io_read_8(FB_VGA_CRT_DC);
}

static void fb_vga_reg_outmisc(uint8_t i)
{
  cpu_io_write_8(FB_VGA_MIS_W ,i);
}

static void fb_vga_reg_outseq(uint16_t index, uint8_t val)
{
  uint16_t	v;

  v = ((uint16_t)val << 8) + index;
  cpu_io_write_16(FB_VGA_SEQ_I, v);
}

static void fb_vga_offset(const struct fb_vga_mode_s *mode, uintptr_t addr)
{
  if (mode->packing == FB_PACK_PLANE)
    {
      switch (mode->bpp)
	{
	case 4:                /* planar 16-color mode */
	  cpu_io_read_8(FB_VGA_IS1_RC);
	  cpu_io_write_8(FB_VGA_ATT_IW, 0x13 + 0x20);
	  cpu_io_write_8(FB_VGA_ATT_IW, (cpu_io_read_8(FB_VGA_ATT_R) & 0xf0) | (addr & 7));
	  /* write sa0-2 to bits 0-2 */
	  addr >>= 3;
	  break;

	case 8:               /* planar 256-color mode */
	  cpu_io_read_8(FB_VGA_IS1_RC);
	  cpu_io_write_8(FB_VGA_ATT_IW, 0x13 + 0x20);
	  cpu_io_write_8(FB_VGA_ATT_IW, (cpu_io_read_8(FB_VGA_ATT_R) & 0xf0) | ((addr & 3) << 1));
	  /* write sa0-1 to bits 1-2 */
	  addr >>= 2;
	  break;
	}
    }

  /* set frame buffer address */
  cpu_io_write_16(FB_VGA_CRT_IC, 0x0d + (addr & 0x00ff) * 256); /* sa0-sa7 */
  cpu_io_write_16(FB_VGA_CRT_IC, 0x0c + (addr & 0xff00));	/* sa8-sa15 */
}

static void fb_vga_palette(void)
{
  uint_fast16_t	i;

  /* Setup palette */
  cpu_io_write_8(FB_VGA_PEL_IW, 0);

  for (i = 0; i < 256; i++)
    {
      /* setup gray palette */
      cpu_io_write_8(FB_VGA_PEL_D, i/4);
      cpu_io_write_8(FB_VGA_PEL_D, i/4);
      cpu_io_write_8(FB_VGA_PEL_D, i/4);
    }
}

static error_t fb_vga_setmode_(const struct fb_vga_mode_s *mode)
{
  const uint8_t	*regs = mode->regs;
  uint_fast16_t	i;

  /* switch to color emulation */
  cpu_io_write_8(FB_VGA_MIS_W, cpu_io_read_8(FB_VGA_MIS_R) | 0x01);

#ifdef CONFIG_FB_VGA_EGA
  /* Enable graphics register modification */
  cpu_io_write_8(FB_VGA_GRA_E0, 0x00);
  cpu_io_write_8(FB_VGA_GRA_E1, 0x01);
#endif

  /* update misc output register */
  fb_vga_reg_outmisc(regs[FB_VGA_MIS]);

  /* synchronous reset on */
  fb_vga_reg_outseq(0x00, 0x01);

  /* write sequencer registers */
  fb_vga_reg_outseq(0x01,regs[FB_VGA_SEQ + 1] | 0x20);

  cpu_io_write_8(FB_VGA_SEQ_I, 1);
  cpu_io_write_8(FB_VGA_SEQ_D, regs[FB_VGA_SEQ + 1] | 0x20);

  for (i = 2; i < FB_VGA_SEQ_C; i++) {
    fb_vga_reg_outseq(i, regs[FB_VGA_SEQ + i]);
  }

  /* synchronous reset off */
  fb_vga_reg_outseq(0x00, 0x03);

#ifndef CONFIG_FB_VGA_EGA
  /* deprotect CRT registers 0-7 */
  fb_vga_reg_outcrtc(0x11, fb_vga_reg_incrtc(0x11) & 0x7f);
#endif

  /* write CRT registers */
  for (i = 0; i < FB_VGA_CRT_C; i++) {
    fb_vga_reg_outcrtc(i,regs[FB_VGA_CRT + i]);
  }

  /* write graphics controller registers */
  for (i = 0; i < FB_VGA_GRA_C; i++) {
    cpu_io_write_8(FB_VGA_GRA_I, i);
    cpu_io_write_8(FB_VGA_GRA_D, regs[FB_VGA_GRA + i]);
  }

  /* write attribute controller registers */
  for (i = 0; i < FB_VGA_ATT_C; i++) {
    cpu_io_read_8(FB_VGA_IS1_RC);		/* reset flip-flop */
    cpu_io_write_8(FB_VGA_ATT_IW, i);
    cpu_io_write_8(FB_VGA_ATT_IW, regs[FB_VGA_ATT + i]);
  }

  /* set display memory offset */
  fb_vga_offset(mode, 0);

  /* set logical width */
  cpu_io_write_16(FB_VGA_CRT_IC, 0x13 + (mode->xres >> 3) * 256);

  /* setup palette */
  fb_vga_palette();

  /* Turn screen on */
#ifndef CONFIG_FB_VGA_EGA
  cpu_io_write_8(FB_VGA_SEQ_I, 0x01);
  cpu_io_write_8(FB_VGA_SEQ_D, cpu_io_read_8(FB_VGA_SEQ_D) & 0xDF);
#endif
  cpu_io_read_8(FB_VGA_IS1_RC);
  cpu_io_write_8(FB_VGA_ATT_IW, 0x20);

  return 0;
}

DEVFB_SETMODE(fb_vga_setmode)
{
  struct fb_vga_context_s	*pv = dev->drv_pv;
  const struct fb_vga_mode_s	*m;

  for (m = vga_modes; m->xres; m++)
    {
      if ((m->xres == xres) && (m->yres == yres) &&
	  (m->bpp == bpp) && (m->packing == packing))
	{
	  pv->mode = m;
	  return fb_vga_setmode_(m);
	}
    }

  return -EINVAL;
}

DEVFB_FLIPPAGE(fb_vga_flippage)
{
  struct fb_vga_context_s	*pv = dev->drv_pv;

  if (page > pv->mode->maxpage)
    return -EINVAL;

  fb_vga_offset(pv->mode, page * pv->mode->xres * pv->mode->yres);

  return 0;
}

DEVFB_SETPALETTE(fb_vga_setpalette)
{
  uint_fast16_t	i;

  /* Setup palette */
  cpu_io_write_8(FB_VGA_PEL_IW, 0);

  for (i = 0; i < __MIN(256, count); i++)
    {
      /* setup each channel */
      cpu_io_write_8(FB_VGA_PEL_D, pal[i].r >> 2);
      cpu_io_write_8(FB_VGA_PEL_D, pal[i].g >> 2);
      cpu_io_write_8(FB_VGA_PEL_D, pal[i].b >> 2);
    }
}
