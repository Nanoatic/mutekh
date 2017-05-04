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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/


#include <hexo/types.h>

#include <device/char.h>
#include <device/device.h>
#include <device/driver.h>

#include <arch/hexo/emu_syscalls.h>

#include "tty-emu.h"

/**************************************************************/

/*
 * device read operation
 */

DEVCHAR_REQUEST(tty_emu_request)
{
  reg_t fd;
  reg_t id;

  assert(rq->size);

  switch (rq->type)
    {
    case DEV_CHAR_READ:
      fd = 0;
      id = EMU_SYSCALL_READ;
      break;
    case DEV_CHAR_WRITE:
      fd = 1;
      id = EMU_SYSCALL_WRITE;
      break;
    default:
      return;
    }

  while (1)
    {
      ssize_t size = emu_do_syscall(id, 3, fd, rq->data, rq->size);

      if (size == 0)
	rq->error = EEOF;
      else if (size < 0)
	rq->error = EIO;
      else
	{
	  rq->size -= size;
	  rq->error = 0;
	}

      if (rq->callback(dev, rq, size) || rq->size == 0 || rq->error)
	return;

      rq->data += size;
    }
}

/*
 * device close operation
 */

DEV_CLEANUP(tty_emu_cleanup)
{
}

/*
 * device open operation
 */

const struct driver_s	tty_emu_drv =
{
  .class		= device_class_char,
  .f_init		= tty_emu_init,
  .f_cleanup		= tty_emu_cleanup,
  .f_irq		= NULL,
  .f.chr = {
    .f_request		= tty_emu_request,
  }
};

DEV_INIT(tty_emu_init)
{
  dev->drv = &tty_emu_drv;

  return 0;
}

