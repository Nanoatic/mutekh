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


#ifndef TTY_SOCLIB_PRIVATE_H_
#define TTY_SOCLIB_PRIVATE_H_

#include <hexo/types.h>
#include <device/device.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_ring.h>


/**************************************************************/

/*
 * Private vgz tty device context
 */

#ifdef CONFIG_HEXO_IRQ
CONTAINER_TYPE(tty_fifo, RING, uint8_t, 32);
CONTAINER_FUNC(tty_fifo, RING, static inline, tty_fifo);
#endif

struct tty_soclib_context_s
{
  /* tty input request queue and char fifo */
  dev_char_queue_root_t		read_q;
#ifdef CONFIG_HEXO_IRQ
  tty_fifo_root_t		read_fifo;
#endif
};

#endif

