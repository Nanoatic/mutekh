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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2010

*/

#include <hexo/interrupt.h>
#include <device/char.h>

#include <mutek/printk.h>
#include <mutek/fileops.h>

#if defined(CONFIG_MUTEK_CONSOLE)

struct device_s *console_dev = NULL;

PRINTF_OUTPUT_FUNC(__printf_out_tty)
{
#ifdef CONFIG_HEXO_IRQ
  if ( !cpu_is_interruptible() )
    return;
#endif

  while (len > 0)
    {
      ssize_t	res = dev_char_spin_write((struct device_s *)ctx, (uint8_t*)str, len);

      if (res < 0)
	break;
      len -= res;
      str += res;
    }
}

#endif

static FILEOPS_READ(tty_read)
{
#if defined(CONFIG_MUTEK_CONSOLE)
  return dev_char_wait_read(console_dev, buffer, count);
#else
  return 0;
#endif
}

static FILEOPS_WRITE(tty_write)
{
#if defined(CONFIG_MUTEK_CONSOLE)
  return dev_char_wait_write(console_dev, buffer, count);
#else
  return count;
#endif
}

const struct fileops_s console_file_ops =
{
  .read = &tty_read,
  .write = &tty_write,
};

