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

#include "random.h"
#include "random-private.h"

#include <hexo/types.h>
#include <device/device.h>
#include <device/driver.h>
#include <mutek/mem_alloc.h>

DEVCHAR_REQUEST(dev_random_request)
{
  struct random_context_s	*pv = dev->drv_pv;

  assert(rq->size);

  lock_spin(&dev->lock);

  switch (rq->type)
    {
      /* Get random stream */
    case DEV_CHAR_READ: {
      size_t size = rq->size;

      crypto_arc4_getstream(&pv->arc4_state, rq->data, rq->size);

      rq->size = 0;
      rq->error = 0;
      rq->callback(dev, rq, size);

      break;
    }

      /* Add randomness */
    case DEV_CHAR_WRITE: {
      while (1)
	{
	  size_t size = rq->size > 256 ? 256 : rq->size;

	  crypto_arc4_update(&pv->arc4_state, rq->data, size);

	  rq->size -= size;
	  rq->error = 0;

	  if (rq->callback(dev, rq, size) || !rq->size)
	    break;

	  rq->data += size;
	}
      break;
    }

    }

  lock_release(&dev->lock);
}

/* 
 * device close operation
 */

DEV_CLEANUP(dev_random_cleanup)
{
  struct random_context_s	*pv = dev->drv_pv;

  mem_free(pv);
}

/* 
 * device open operation
 */

const struct driver_s	dev_random_drv =
{
  .class		= device_class_char,
  .f_init		= dev_random_init,
  .f_cleanup		= dev_random_cleanup,
  .f.chr = {
    .f_request		= dev_random_request,
  }
};

DEV_INIT(dev_random_init)
{
  struct random_context_s	*pv;

  dev->drv = &dev_random_drv;

  /* alocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  dev->drv_pv = pv;

  crypto_arc4_init(&pv->arc4_state);

  return 0;
}

