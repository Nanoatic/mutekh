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

#include <hexo/types.h>

#include <device/fb.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <string.h>

#include "fb-vga.h"

#include "fb-vga-private.h"

/* 
 * device close operation
 */

DEV_CLEANUP(fb_vga_cleanup)
{
  struct fb_vga_context_s	*pv = dev->drv_pv;

  lock_destroy(&pv->lock);

  mem_free(pv);
}

DEVFB_GETBUFFER(fb_vga_getbuffer)
{
  struct fb_vga_context_s	*pv = dev->drv_pv;

  return FB_VGA_FB_ADDRESS + page * pv->mode->xres * pv->mode->yres;
}

/* 
 * device open operation
 */

const struct driver_s	fb_vga_drv =
{
  .class		= device_class_fb,
  .f_init		= fb_vga_init,
  .f_cleanup		= fb_vga_cleanup,
  .f.fb = {
    .f_setmode		= fb_vga_setmode,
    .f_getbuffer	= fb_vga_getbuffer,
    .f_flippage		= fb_vga_flippage,
    .f_setpalette	= fb_vga_setpalette,
  }
};

DEV_INIT(fb_vga_init)
{
  struct fb_vga_context_s	*pv;

  dev->drv = &fb_vga_drv;

  /* alocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  lock_init(&pv->lock);

  dev->drv_pv = pv;

  return 0;
}

