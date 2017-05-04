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

    Synchronous blit and utility functions for LCD devices.

*/

#include <device/device.h>
#include <device/lcd.h>
#include <device/driver.h>

#ifdef CONFIG_MUTEK_SCHEDULER
# include <mutek/scheduler.h>
# include <hexo/lock.h>
#endif


struct dev_lcd_wait_rq_s
{
#ifdef CONFIG_MUTEK_SCHEDULER
  lock_t lock;
  struct sched_context_s *ctx;
#endif
  bool_t done;
};

static DEVLCD_CALLBACK(dev_lcd_handle_request_cb)
{
	struct dev_lcd_wait_rq_s *status = context;

#ifdef CONFIG_MUTEK_SCHEDULER
	lock_spin(&status->lock);
	if (status->ctx != NULL)
		sched_context_start(status->ctx);
	status->done = 1;
	lock_release(&status->lock);
#else
	status->done = 1;
#endif
}

static ssize_t dev_lcd_handle_request(
	struct device_s *dev,
	struct lcd_req_s *req)
{
  struct dev_lcd_wait_rq_s status;

#ifdef CONFIG_MUTEK_SCHEDULER
  lock_init(&status.lock);
  status.ctx = NULL;
#endif
  status.done = 0;
  req->callback = dev_lcd_handle_request_cb;
  req->callback_data = &status;

  error_t err = dev_lcd_request(dev, req);
  if ( err )
	  return err;

#ifdef CONFIG_MUTEK_SCHEDULER
  /* ensure callback doesn't occur here */

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  lock_spin(&status.lock);

  if (!status.done)
    {
      status.ctx = sched_get_current();
      sched_context_stop_unlock(&status.lock);
    }
  else
    lock_release(&status.lock);

  CPU_INTERRUPT_RESTORESTATE;
  lock_destroy(&status.lock);
#else
  while (!status.done)
	  order_compiler_mem();
#endif

  return 0;
}

ssize_t dev_lcd_set_palette(struct device_s *dev, struct lcd_pal_s *palette, size_t count)
{
	struct lcd_req_s req = {
		LCD_REQ_SET_PALETTE,
		{ .palette = {
				.pal = palette,
				.count = count,
			}
		},
	};

	return dev_lcd_handle_request(dev, &req);
}

ssize_t dev_lcd_blit(struct device_s *dev,
					 lcd_coord_t xmin,
					 lcd_coord_t xmax,
					 lcd_coord_t ymin,
					 lcd_coord_t ymax,
					 const uint8_t *src )
{
	struct lcd_req_s req = {
		LCD_REQ_BLIT,
		{ .blit = {
				.xmin = xmin,
				.xmax = xmax,
				.ymin = ymin,
				.ymax = ymax,
				.src = src,
			}
		},
	};

	return dev_lcd_handle_request(dev, &req);
}

ssize_t dev_lcd_setmode(struct device_s *dev,
						uint_fast8_t bpp,
						uint_fast8_t packing,
						uint_fast8_t flags)
{
	struct lcd_req_s req = {
		LCD_REQ_SET_MODE,
		{ .mode = {
				.packing = packing,
				.bpp = bpp,
				.flags = flags,
			}
		},
	};

	return dev_lcd_handle_request(dev, &req);
}

ssize_t dev_lcd_setcontrast(struct device_s *dev,
							uint_fast8_t contrast)
{
	struct lcd_req_s req = {
		LCD_REQ_SET_CONTRAST,
		{ .contrast = {
				.value = contrast,
			}
		},
	};

	return dev_lcd_handle_request(dev, &req);
}
