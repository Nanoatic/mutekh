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

#include <hexo/init.h>
#include <hexo/types.h>
#include <hexo/endian.h>

#include <hexo/interrupt.h>
#include <hexo/local.h>
#include <hexo/iospace.h>
#include <hexo/lock.h>
#include <hexo/context.h>
#include <hexo/cpu.h>
#include <mutek/scheduler.h>

#include <drivers/char/tty-emu/tty-emu.h>
#include <drivers/timer/emu/timer-emu.h>
#include <drivers/block/file-emu/block-file-emu.h>

#include <device/device.h>
#include <device/driver.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutek/timer.h>
#include <mutek/printk.h>

#if defined(CONFIG_MUTEK_CONSOLE)
extern struct device_s *console_dev;
#endif

#if defined(CONFIG_DRIVER_CHAR_EMUTTY)
struct device_s tty_dev;
#endif

#if defined(CONFIG_DRIVER_TIMER)
struct device_s timer_dev;
#endif

#if defined(CONFIG_DRIVER_BLOCK)
struct device_s block_dev;
#endif

void arch_hw_init()
{
	/* TTY init */
#if defined(CONFIG_DRIVER_CHAR_EMUTTY)
	device_init(&tty_dev);
	tty_emu_init(&tty_dev, NULL);

# if defined(CONFIG_MUTEK_CONSOLE)
	console_dev = &tty_dev;
# endif
#endif

	/* block device */
#if defined(CONFIG_DRIVER_BLOCK)
	device_init(&block_dev);
# if defined(CONFIG_DRIVER_BLOCK_EMU)
	block_file_emu_init(&block_dev, "block.bin");
# else
#  error CONFIG_DRIVER_BLOCK case not handled in hw_init()
# endif
#endif

	/* timer init */
#if defined(CONFIG_DRIVER_TIMER)
	device_init(&timer_dev);
# if defined(CONFIG_DRIVER_TIMER_EMU)
	timer_emu_init(&timer_dev, NULL);
	dev_timer_setperiod(&timer_dev, 0, 0xffff);
	dev_timer_setcallback(&timer_dev, 0, timer_callback, 0);
# else
#  warning CONFIG_DRIVER_TIMER case not handled in hw_init()
# endif
#endif
}
