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


#ifndef TTY_VGA_PRIVATE_H_
#define TTY_VGA_PRIVATE_H_

#include <hexo/types.h>
#include <device/device.h>
#include <hexo/lock.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_ring.h>
#include <hexo/gpct_lock_hexo.h>

#if defined(CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD) || defined(CONFIG_DRIVER_CHAR_VGATTY_ANSI)
# define DRIVER_CHAR_VGATTY_HAS_FIFO
#endif

/**************************************************************/

/*
 * In memory vga text buffer mapping
 */

struct vga_text_char_s
{
  uint8_t		c;
  union {
#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI
    struct {
      uint8_t		foreground:3, bright:1, background:3, blink:1;
    };
#endif
    uint8_t		attrs;
  };
};

#define VGA_CRTCREG_CURPOS_LO	0x0f
#define VGA_CRTCREG_CURPOS_HI	0x0e

typedef volatile struct vga_text_char_s * vga_text_buf_t;

/**************************************************************/

/*
 * Private vgz tty device context
 */

typedef void tty_vga_char_process_t (struct device_s *dev, uint8_t c);

#ifdef CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD
typedef void tty_vga_key_process_t  (struct device_s *dev, uint8_t scancode);
#endif

CONTAINER_TYPE(tty_fifo, RING, uint8_t, 32);
CONTAINER_FUNC(tty_fifo, RING, static inline, tty_fifo);

#define VGA_TTY_MAX_ANSI_PARAMS		4

struct tty_vga_context_s
{
  uint_fast8_t			width, height;
  uint_fast8_t			xpos, ypos;

  /* function used to process next char */
  tty_vga_char_process_t	*process;

#ifdef CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD
  /* function used to process next scancode byte */
  tty_vga_key_process_t		*scancode;

  uint_fast8_t			key_state;
#endif

#ifdef DRIVER_CHAR_VGATTY_HAS_FIFO
  /* tty input request queue and char fifo */
  dev_char_queue_root_t		read_q;
  tty_fifo_root_t		read_fifo;
#endif

#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI
  uint_fast8_t			xsave, ysave;
  uint_fast8_t			forecolor, backcolor;
  bool_t			blink, bright, reverse;
  bool_t			linewrap, insert, nlmode;
  uint_fast8_t			ansi_index;
  uint_fast8_t			ansi_param[VGA_TTY_MAX_ANSI_PARAMS];
#endif
};

#define VGA_KS_SCROLL	0x01
#define VGA_KS_NUM	0x02
#define VGA_KS_CAPS	0x04
#define VGA_KS_SHIFT	0x08
#define VGA_KS_CTRL	0x10
#define VGA_KS_ALT	0x20
#define VGA_KS_ALTGR	0x40

#ifdef CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD

void tty_vga_scancode_default(struct device_s *dev, uint8_t scancode);
void tty_vga_scancode_led(struct device_s *dev, uint8_t scancode);

#endif

void tty_vga_try_read(struct device_s *dev);

void tty_vga_reset(struct device_s *dev);

void tty_vga_updatecursor(struct device_s *dev);

void tty_vga_clear(struct device_s *dev, int_fast8_t rowstart, int_fast8_t rowend);

void tty_vga_clear_row(struct device_s *dev, uint_fast8_t row,
		       int_fast8_t colstart, int_fast8_t colend);

bool_t tty_vga_setcursor(struct device_s *dev, int_fast8_t x, int_fast8_t y);

void tty_vga_scroll_down(struct device_s *dev, uint_fast8_t count);

void tty_vga_scroll_up(struct device_s *dev, uint_fast8_t count);

void tty_vga_ansi_insert(struct device_s *dev);

void tty_vga_process_default(struct device_s *dev, uint8_t c);

#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI
void tty_vga_process_ansi(struct device_s *dev, uint8_t c);
#endif

/**************************************************************/

#endif

