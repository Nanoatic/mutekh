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

#include "null.h"

#include <hexo/types.h>
#include <device/device.h>
#include <device/driver.h>

DEVCHAR_REQUEST(dev_null_request)
{
  switch (rq->type)
    {
      /* Get EOF error */
    case DEV_CHAR_READ: {

      rq->error = EEOF;
      rq->callback(dev, rq, 0);

      break;
    }

      /* Eat everything */
    case DEV_CHAR_WRITE: {
      size_t size = rq->size;

      rq->size = 0;
      rq->error = 0;

      rq->callback(dev, rq, size);
      break;
    }

    }

}

/* 
 * device close operation
 */

DEV_CLEANUP(dev_null_cleanup)
{
}

/* 
 * device open operation
 */

const struct driver_s	dev_null_drv =
{
  .class		= device_class_char,
  .f_init		= dev_null_init,
  .f_cleanup		= dev_null_cleanup,
  .f.chr = {
    .f_request		= dev_null_request,
  }
};

DEV_INIT(dev_null_init)
{
  dev->drv = &dev_null_drv;

  return 0;
}

