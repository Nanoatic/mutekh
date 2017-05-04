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

    Synchronous read and write functions for i2c devices.

*/

#include <device/device.h>
#include <device/i2c.h>
#include <device/driver.h>

#ifdef CONFIG_MUTEK_SCHEDULER
# include <mutek/scheduler.h>
# include <hexo/lock.h>
#endif


struct dev_i2c_wait_rq_s
{
#ifdef CONFIG_MUTEK_SCHEDULER
	lock_t lock;
	struct sched_context_s *ctx;
#endif
	bool_t done;
	error_t error;
};

static DEVI2C_CALLBACK(dev_i2c_wait_request_cb)
{
  struct dev_i2c_wait_rq_s *status = priv;

  status->error = error;
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

error_t dev_i2c_wait_request(
	struct device_s *dev,
	struct dev_i2c_rq_s *rq)
{
  struct dev_i2c_wait_rq_s status;

  rq->callback = dev_i2c_wait_request_cb;
  rq->pvdata = &status;

#ifdef CONFIG_MUTEK_SCHEDULER
  lock_init(&status.lock);
  status.ctx = NULL;
  status.done = 0;

  dev_i2c_request(dev, rq);

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
  status.done = 0;

  dev_i2c_request(dev, rq);

  while (!status.done)
    order_compiler_mem();
#endif

  return status.error;

}

