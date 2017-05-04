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

    Synchronous read and write functions for block device.

*/

#include <device/device.h>
#include <device/char.h>
#include <device/driver.h>

#ifdef CONFIG_MUTEK_SCHEDULER
# include <mutek/scheduler.h>
# include <hexo/lock.h>
#endif


struct dev_char_wait_rq_s
{
#ifdef CONFIG_MUTEK_SCHEDULER
  lock_t lock;
  struct sched_context_s *ctx;
#endif
  bool_t done;
};


static DEVCHAR_CALLBACK(dev_char_lock_request_cb)
{
  struct dev_char_wait_rq_s *status = rq->pvdata;
  status->done = 1;
  return 1;
}

static DEVCHAR_CALLBACK(dev_char_lock_request_whole_cb)
{
  struct dev_char_wait_rq_s *status = rq->pvdata;

  if ( rq->size == 0 || rq->error ) {
	  status->done = 1;
	  return 1;
  }
  return 0;
}

static ssize_t dev_char_lock_request(struct device_s *dev, uint8_t *data,
				     size_t size, enum dev_char_rq_type_e type,
				     devchar_callback_t *callback)
{
  struct dev_char_rq_s rq;
  struct dev_char_wait_rq_s status;

  if (size == 0)
    return 0;

  status.done = 0;
  rq.type = type;
  rq.pvdata = &status;
  rq.callback = callback;
  rq.error = 0;
  rq.data = data;
  rq.size = size;

  dev_char_request(dev, &rq);

#ifdef CONFIG_HEXO_IRQ
  assert(cpu_is_interruptible());
#endif

  while (!status.done)
    order_compiler_mem();

  assert(rq.error >= 0);
  return rq.error ? -rq.error : size - rq.size;
}


#ifdef CONFIG_MUTEK_SCHEDULER
static DEVCHAR_CALLBACK(dev_char_wait_request_cb)
{
  struct dev_char_wait_rq_s *status = rq->pvdata;

  lock_spin(&status->lock);
  if (status->ctx != NULL)
	  sched_context_start(status->ctx);
  status->done = 1;
  lock_release(&status->lock);

  return 1;
}

static DEVCHAR_CALLBACK(dev_char_wait_request_whole_cb)
{
  struct dev_char_wait_rq_s *status = rq->pvdata;

  if ( rq->size == 0 || rq->error ) {
	  lock_spin(&status->lock);
	  if (status->ctx != NULL)
		  sched_context_start(status->ctx);
	  status->done = 1;
	  lock_release(&status->lock);
	  return 1;
  }
  return 0;
}

static ssize_t dev_char_wait_request(struct device_s *dev, uint8_t *data,
				     size_t size, enum dev_char_rq_type_e type,
				     devchar_callback_t *callback)
{
  struct dev_char_rq_s rq;
  struct dev_char_wait_rq_s status;

  if (size == 0)
    return 0;

  lock_init(&status.lock);
  status.ctx = NULL;
  status.done = 0;
  rq.type = type;
  rq.pvdata = &status;
  rq.callback = callback;
  rq.error = 0;
  rq.data = data;
  rq.size = size;

  dev_char_request(dev, &rq);

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

  assert(rq.error >= 0);
  return rq.error ? -rq.error : size - rq.size;

}
#endif




ssize_t dev_char_wait_read(struct device_s *dev, uint8_t *data, size_t size)
{
#ifdef CONFIG_MUTEK_SCHEDULER
	return dev_char_wait_request(dev, data, size, DEV_CHAR_READ, dev_char_wait_request_cb);
#else
	return dev_char_lock_request(dev, data, size, DEV_CHAR_READ, dev_char_lock_request_cb);
#endif
}

ssize_t dev_char_spin_read(struct device_s *dev, uint8_t *data, size_t size)
{
	return dev_char_lock_request(dev, data, size, DEV_CHAR_READ, dev_char_lock_request_cb);
}

ssize_t dev_char_wait_write(struct device_s *dev, const uint8_t *data, size_t size)
{
#ifdef CONFIG_MUTEK_SCHEDULER
	return dev_char_wait_request(dev, (uint8_t*)data, size, DEV_CHAR_WRITE, dev_char_wait_request_whole_cb);
#else
	return dev_char_lock_request(dev, (uint8_t*)data, size, DEV_CHAR_WRITE, dev_char_lock_request_whole_cb);
#endif
}

ssize_t dev_char_spin_write(struct device_s *dev, const uint8_t *data, size_t size)
{
	return dev_char_lock_request(dev, (uint8_t*)data, size, DEV_CHAR_WRITE, dev_char_lock_request_whole_cb);
}

