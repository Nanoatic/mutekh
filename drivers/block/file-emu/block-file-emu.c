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


#include <hexo/types.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <arch/hexo/emu_syscalls.h>

#include "block-file-emu.h"
#include "block-file-emu-private.h"

/**************************************************************/

DEVBLOCK_REQUEST(block_file_emu_request)
{
  struct block_file_emu_context_s *pv = dev->drv_pv;
  struct dev_block_params_s *p = &pv->params;
  dev_block_lba_t lba = rq->lba + rq->progress;
  dev_block_lba_t count = rq->count - rq->progress;

  if (lba + count > p->blk_count)
    {
      rq->progress = -ERANGE;
      rq->callback(rq, 0, rq + 1);
      return;
    }

  reg_t id;
  size_t b;

  switch (rq->type & DEV_BLOCK_OPMASK)
    {
    case DEV_BLOCK_READ:
      id = EMU_SYSCALL_READ;
      break;

    case DEV_BLOCK_WRITE:
      id = EMU_SYSCALL_WRITE;
      break;

    default:
      rq->progress = -ENOTSUP;
      rq->callback(rq, 0, rq + 1);
      return;
    }

  emu_do_syscall(EMU_SYSCALL_LSEEK, 3, pv->fd, lba * p->blk_size, EMU_SEEK_SET);

  for (b = 0; b < count; b++)
    emu_do_syscall(id, 3, pv->fd, rq->data[b], p->blk_size);

  rq->progress += count;
  rq->callback(rq, count, rq + 1);
}

DEVBLOCK_GETPARAMS(block_file_emu_getparams)
{
  return &(((struct block_file_emu_context_s *)(dev->drv_pv))->params);
}

DEVBLOCK_GETRQSIZE(block_file_emu_getrqsize)
{
  return sizeof(struct dev_block_rq_s);
}

DEV_CLEANUP(block_file_emu_cleanup)
{
  struct block_file_emu_context_s	*pv = dev->drv_pv;

  emu_do_syscall(EMU_SYSCALL_CLOSE, 1, pv->fd);
  mem_free(pv);
}

const struct driver_s	block_file_emu_drv =
{
  .class		= device_class_block,
  .f_init		= block_file_emu_init,
  .f_cleanup		= block_file_emu_cleanup,
  .f.blk = {
    .f_request		= block_file_emu_request,
    .f_getparams	= block_file_emu_getparams,
    .f_getrqsize	= block_file_emu_getrqsize,
  }
};

DEV_INIT(block_file_emu_init)
{
  struct block_file_emu_context_s	*pv;

  dev->drv = &block_file_emu_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    goto err;

  assert(params);

  pv->fd = emu_do_syscall(EMU_SYSCALL_OPEN, 2, params, EMU_O_RDONLY);

  if (pv->fd < 0)
    {
      printk("Unable to open device file %s\n", params);
      pv->params.blk_count = 0;
    }
  else
    {
      size_t off = emu_do_syscall(EMU_SYSCALL_LSEEK, 3, pv->fd, 0, EMU_SEEK_END);
      pv->params.blk_count = off / CONFIG_DRIVER_BLOCK_EMU_BLOCKSIZE;
    }

  pv->params.blk_size = CONFIG_DRIVER_BLOCK_EMU_BLOCKSIZE;
  printk("Emu block device : %u sectors\n", pv->params.blk_count);

  dev->drv_pv = pv;
  return 0;

 err_pv:
  mem_free(pv);
 err:
  return -1;
}

