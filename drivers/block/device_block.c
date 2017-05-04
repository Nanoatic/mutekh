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
#include <device/block.h>
#include <device/driver.h>

#ifdef CONFIG_MUTEK_SCHEDULER
# include <mutek/scheduler.h>
# include <hexo/lock.h>
#endif

#include <alloca.h>

struct dev_block_wait_rq_s
{
#ifdef CONFIG_MUTEK_SCHEDULER
  lock_t lock;
  struct sched_context_s *ctx;
#endif
  bool_t done;
};

static DEVBLOCK_CALLBACK(dev_block_syncl_request)
{
  struct dev_block_wait_rq_s *status = rq->pvdata;

  if (rq->progress < 0 || rq->progress >= rq->count)
    status->done = 1;
}

static error_t dev_block_lock_request(struct device_s *dev, uint8_t **data,
				      dev_block_lba_t lba, size_t count,
				      enum dev_block_rq_type_e type)
{
  struct dev_block_wait_rq_s status;
  struct dev_block_rq_s *rq = alloca(dev_block_getrqsize(dev));

  status.done = 0;
  rq->data = data;
  rq->lba = lba;
  rq->count = count;
  rq->type = type;
  rq->pvdata = &status;
  rq->callback = dev_block_syncl_request;
  rq->progress = 0;

  dev_block_request(dev, rq);

#ifdef CONFIG_HEXO_IRQ
  assert(cpu_is_interruptible());
#endif

  while (!status.done)
    order_compiler_mem();

  return __MIN(rq->progress, 0);
}

#ifdef CONFIG_MUTEK_SCHEDULER
static DEVBLOCK_CALLBACK(dev_block_sync_request)
{
  struct dev_block_wait_rq_s *status = rq->pvdata;

  if (rq->progress < 0 || rq->progress >= rq->count)
    {
      lock_spin(&status->lock);
      if (status->ctx != NULL)
	sched_context_start(status->ctx);
      status->done = 1;
      lock_release(&status->lock);
    }
}

static error_t dev_block_wait_request(struct device_s *dev, uint8_t **data,
				      dev_block_lba_t lba, size_t count,
				      enum dev_block_rq_type_e type)
{
  struct dev_block_wait_rq_s status;
  struct dev_block_rq_s *rq = alloca(dev_block_getrqsize(dev));

  lock_init(&status.lock);
  status.ctx = NULL;
  status.done = 0;
  rq->data = data;
  rq->lba = lba;
  rq->count = count;
  rq->type = type;
  rq->pvdata = &status;
  rq->callback = dev_block_sync_request;
  rq->progress = 0;

  dev_block_request(dev, rq);

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

  return __MIN(rq->progress, 0);
}
#endif

error_t dev_block_wait_read(struct device_s *dev, uint8_t **data,
			    dev_block_lba_t lba, size_t count)
{
#ifdef CONFIG_MUTEK_SCHEDULER
  return dev_block_wait_request(dev, data, lba, count, DEV_BLOCK_READ);
#else
  return dev_block_lock_request(dev, data, lba, count, DEV_BLOCK_READ);
#endif
}

error_t dev_block_spin_read(struct device_s *dev, uint8_t **data,
			    dev_block_lba_t lba, size_t count)
{
  return dev_block_lock_request(dev, data, lba, count, DEV_BLOCK_READ);
}

error_t dev_block_wait_write(struct device_s *dev, uint8_t **data,
			     dev_block_lba_t lba, size_t count)
{
#ifdef CONFIG_MUTEK_SCHEDULER
  return dev_block_wait_request(dev, data, lba, count, DEV_BLOCK_WRITE);
#else
  return dev_block_lock_request(dev, data, lba, count, DEV_BLOCK_WRITE);
#endif
}

error_t dev_block_spin_write(struct device_s *dev, uint8_t **data,
			     dev_block_lba_t lba, size_t count)
{
  return dev_block_lock_request(dev, data, lba, count, DEV_BLOCK_WRITE);
}

