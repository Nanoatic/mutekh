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

    This driver is based on ssocliblib code (http://www.ssocliblib.org/)

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <hexo/types.h>

#include <device/fb.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <string.h>

#include "fb-soclib.h"
#include "fb-soclib-private.h"

/* 
 * device close operation
 */

DEV_CLEANUP(fb_soclib_cleanup)
{
  struct fb_soclib_context_s	*pv = dev->drv_pv;

  //  mem_free(pv);
}

DEVFB_GETBUFFER(fb_soclib_getbuffer)
{
  return dev->addr[0];
}

DEVFB_SETMODE(fb_soclib_setmode)
{
  return 0;
}

DEVFB_FLIPPAGE(fb_soclib_flippage)
{
	uint8_t *flip = (uintptr_t)(dev->addr[0]) + 320*200 + 256*3;
        
	*flip = 0;
  
	return 0;
}

DEVFB_SETPALETTE(fb_soclib_setpalette)
{
  uint_fast16_t	i;
  uint8_t *palette = (uintptr_t)(dev->addr[0]) + 320*200;

  for (i = 0; i < __MIN(256, count); i++)
    {
      /* setup each channel */
		palette[i*3] = pal[i].r;
		palette[i*3+1] = pal[i].g;
		palette[i*3+2] = pal[i].b;
    }
}

/* 
 * device open operation
 */

static const struct devenum_ident_s	fb_soclib_ids[] =
{
	DEVENUM_FDTNAME_ENTRY("soclib:fb", 0, 0),
	{ 0 }
};

const struct driver_s	fb_soclib_drv =
{
  .class		= device_class_fb,
  .id_table		= fb_soclib_ids,
  .f_init		= fb_soclib_init,
  .f_cleanup		= fb_soclib_cleanup,
  .f.fb = {
    .f_setmode		= fb_soclib_setmode,
    .f_getbuffer	= fb_soclib_getbuffer,
    .f_flippage		= fb_soclib_flippage,
    .f_setpalette	= fb_soclib_setpalette,
  }
};

REGISTER_DRIVER(fb_soclib_drv);

DEV_INIT(fb_soclib_init)
{
  struct fb_soclib_context_s	*pv;

  dev->drv = &fb_soclib_drv;

  /* alocate private driver data */
#if 0
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;
#endif

  dev->drv_pv = pv;

  return 0;
}

