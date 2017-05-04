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

#include <string.h>

#include <hexo/types.h>
#include <device/char.h>
#include <device/device.h>
#include <device/driver.h>
#include <hexo/iospace.h>

#include "tty-vga.h"

#include "tty-vga-private.h"

/*
 * keyboard interrupt
 */

struct tty_vga_ansi_keycode_s
{
  const char	*lower;
  const char	*upper;
  const char	*numlock;
  const char	*ctrl;
  uint8_t	mstate;
  uint8_t	bstate;
};

static const struct tty_vga_ansi_keycode_s ansi_keycode_1[0x80] =
  {
    /* Alpha keys */

    [0x1e] = { .lower = "a", .upper = "A", .ctrl = "\x01" },
    [0x30] = { .lower = "b", .upper = "B", .ctrl = "\x02" },
    [0x2e] = { .lower = "c", .upper = "C", .ctrl = "\x03" },
    [0x20] = { .lower = "d", .upper = "D", .ctrl = "\x04" },
    [0x12] = { .lower = "e", .upper = "E", .ctrl = "\x05" },
    [0x21] = { .lower = "f", .upper = "F", .ctrl = "\x06" },
    [0x22] = { .lower = "g", .upper = "G", .ctrl = "\x07" },
    [0x23] = { .lower = "h", .upper = "H", .ctrl = "\x08" },
    [0x17] = { .lower = "i", .upper = "I", .ctrl = "\x09" },
    [0x24] = { .lower = "j", .upper = "J", .ctrl = "\x0a" },
    [0x25] = { .lower = "k", .upper = "K", .ctrl = "\x0b" },
    [0x26] = { .lower = "l", .upper = "L", .ctrl = "\x0c" },
    [0x32] = { .lower = "m", .upper = "M", .ctrl = "\x0d" },
    [0x31] = { .lower = "n", .upper = "N", .ctrl = "\x0e" },
    [0x18] = { .lower = "o", .upper = "O", .ctrl = "\x0f" },
    [0x19] = { .lower = "p", .upper = "P", .ctrl = "\x10" },
    [0x10] = { .lower = "q", .upper = "Q", .ctrl = "\x11" },
    [0x13] = { .lower = "r", .upper = "R", .ctrl = "\x12" },
    [0x1f] = { .lower = "s", .upper = "S", .ctrl = "\x13" },
    [0x14] = { .lower = "t", .upper = "T", .ctrl = "\x14" },
    [0x16] = { .lower = "u", .upper = "U", .ctrl = "\x15" },
    [0x2f] = { .lower = "v", .upper = "V", .ctrl = "\x16" },
    [0x11] = { .lower = "w", .upper = "W", .ctrl = "\x17" },
    [0x2d] = { .lower = "x", .upper = "X", .ctrl = "\x18" },
    [0x15] = { .lower = "y", .upper = "Y", .ctrl = "\x19" },
    [0x2c] = { .lower = "z", .upper = "Z", .ctrl = "\x1a" },

    /* Num keys, top row */
    [0x29] = { .lower = "`", .upper = "~" },
    [0x0b] = { .lower = "0", .upper = ")" },
    [0x02] = { .lower = "1", .upper = "!" },
    [0x03] = { .lower = "2", .upper = "@" },
    [0x04] = { .lower = "3", .upper = "#", .ctrl = "\x1b" },
    [0x05] = { .lower = "4", .upper = "$", .ctrl = "\x1c" },
    [0x06] = { .lower = "5", .upper = "%", .ctrl = "\x1d" },
    [0x07] = { .lower = "6", .upper = "^", .ctrl = "\x1e" },
    [0x08] = { .lower = "7", .upper = "&", .ctrl = "\x1f"  },
    [0x09] = { .lower = "8", .upper = "*" },
    [0x0a] = { .lower = "9", .upper = "(" },
    [0x0c] = { .lower = "-", .upper = "_" },
    [0x0d] = { .lower = "=", .upper = "+" },

    /* Others keys */
    [0x2b] = { .lower = "\\", .upper = "|" },
    [0x0e] = { .lower = "\x7f" },	/* BACKSPACE */
    [0x39] = { .lower = " ", .ctrl = "\0" }, /* SPACE */
    [0x0f] = { .lower = "\t" },	/* TAB */
    [0x1c] = { .lower = "\x0d" },
    [0x01] = { .lower = "\x1b" }, /* ESC */
    [0x1a] = { .lower = "[", .upper = "{", .ctrl = "\x1b" },
    [0x1b] = { .lower = "]", .upper = "}", .ctrl = "\x1d" },
    [0x27] = { .lower = ";", .upper = ":" },
    [0x28] = { .lower = "'", .upper = "\"" },
    [0x33] = { .lower = ",", .upper = "<" },
    [0x34] = { .lower = ".", .upper = ">" },
    [0x35] = { .lower = "/", .upper = "?", .ctrl = "\x1c" },

    /* State keys */
    [0x3a] = { .mstate = VGA_KS_CAPS },
    [0x2a] = { .mstate = VGA_KS_SHIFT, .bstate = VGA_KS_SHIFT }, /* Left */
    [0x1d] = { .mstate = VGA_KS_CTRL, .bstate = VGA_KS_CTRL },
    [0x38] = { .mstate = VGA_KS_ALT, .bstate = VGA_KS_ALT },
    [0x36] = { .mstate = VGA_KS_SHIFT, .bstate = VGA_KS_SHIFT }, /* Right */
    [0x46] = { .mstate = VGA_KS_SCROLL },
    [0x45] = { .mstate = VGA_KS_NUM },

#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI

    /* Top row functions keys */
    [0x3b] = { .lower = "\x1b[OP" }, /* F1 */
    [0x3c] = { .lower = "\x1b[OQ" }, /* F2 */
    [0x3d] = { .lower = "\x1b[OR" }, /* F3 */
    [0x3e] = { .lower = "\x1b[OS" },
    [0x3f] = { .lower = "\x1b[15~" },
    [0x40] = { .lower = "\x1b[17~" },
    [0x41] = { .lower = "\x1b[18~" },
    [0x42] = { .lower = "\x1b[19~" },
    [0x43] = { .lower = "\x1b[20~" },
    [0x44] = { .lower = "\x1b[21~" },
    [0x57] = { .lower = "\x1b[23~" },
    [0x58] = { .lower = "\x1b[24~" },
#endif

    /* numpad keys */
    [0x37] = { .lower = "*" },
    [0x4a] = { .lower = "-" },
    [0x4e] = { .lower = "+" },
    [0x53] = { .lower = "\x1b[3~", .numlock = "." },
    [0x52] = { .lower = "\x1b[2~", .numlock = "0" },
    [0x4f] = { .lower = "\x1b[F",  .numlock = "1" },
    [0x50] = { .lower = "\x1b[B",  .numlock = "2" },
    [0x51] = { .lower = "\x1b[6~", .numlock = "3" },
    [0x4b] = { .lower = "\x1b[D",  .numlock = "4" },
    [0x4c] = {                     .numlock = "5" },
    [0x4d] = { .lower = "\x1b[C",  .numlock = "6" },
    [0x47] = { .lower = "\x1b[H",  .numlock = "7" },
    [0x48] = { .lower = "\x1b[A",  .numlock = "8" },
    [0x49] = { .lower = "\x1b[5~", .numlock = "9" },
  };

static const struct tty_vga_ansi_keycode_s ansi_keycode_2[0x80] =
  {
    [0x1d] = { .mstate = VGA_KS_CTRL, .bstate = VGA_KS_CTRL },
    [0x38] = { .mstate = VGA_KS_ALTGR, .bstate = VGA_KS_ALTGR },
    [0x52] = { .lower = "\x1b[2~" }, /* insert */
    [0x47] = { .lower = "\x1b[H" }, /* home */
    [0x49] = { .lower = "\x1b[5~" }, /* pgup */
    [0x51] = { .lower = "\x1b[6~" }, /* pgdn */
    [0x53] = { .lower = "\x1b[3~", .ctrl = "\xff"  }, /* remove */
    [0x4f] = { .lower = "\x1b[F" }, /* end */
    [0x48] = { .lower = "\x1b[A" }, /* up */
    [0x4b] = { .lower = "\x1b[D" }, /* left */
    [0x4d] = { .lower = "\x1b[C" }, /* right */
    [0x50] = { .lower = "\x1b[B" }, /* down */

    [0x35] = { .lower = "/" },	/* numpad div */
    [0x1c] = { .lower = "\x0d" },	/* numpad enter */
  };

void
tty_vga_scancode_led(struct device_s *dev, uint8_t scancode)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  if (scancode == 0xfa)
    cpu_io_write_8(0x60, pv->key_state & (VGA_KS_SCROLL | VGA_KS_NUM | VGA_KS_CAPS));

  pv->scancode = tty_vga_scancode_default;
}

static void tty_vga_keycode_break(struct device_s *dev,
				  const struct tty_vga_ansi_keycode_s *k)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  pv->key_state ^= k->bstate;
}

static void tty_vga_keycode_make(struct device_s *dev,
				 const struct tty_vga_ansi_keycode_s *k)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  if (k->mstate)
    {
      pv->key_state
	= ((pv->key_state ^ k->mstate) & (VGA_KS_SCROLL | VGA_KS_NUM | VGA_KS_CAPS))
	| ((pv->key_state | k->mstate) & (VGA_KS_SHIFT | VGA_KS_CTRL | VGA_KS_ALTGR | VGA_KS_ALT));

      if (k->mstate & (VGA_KS_SCROLL | VGA_KS_NUM | VGA_KS_CAPS))
	{
	  cpu_io_write_8(0x60, 0xed);
	  pv->scancode = tty_vga_scancode_led;
	}
    }
  else
    {
      const char	*str = k->lower;

      if (k->upper && (pv->key_state & (VGA_KS_CAPS | VGA_KS_SHIFT)))
	str = k->upper;

      if (k->numlock && (pv->key_state & VGA_KS_NUM))
	str = k->numlock;

      if (k->ctrl && (pv->key_state & VGA_KS_CTRL))
	str = k->ctrl;

      if ((pv->key_state & VGA_KS_ALT) && str)
	tty_fifo_pushback(&pv->read_fifo, 0x1b);

      if (str)
	tty_fifo_pushback_array(&pv->read_fifo, (uint8_t*)str, strlen(str));

#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI
      if (*str == 13 && pv->nlmode) /* CR LF in newline mode */
	tty_fifo_pushback(&pv->read_fifo, 10);
#endif
    }
}

static void
tty_vga_scancode_ext(struct device_s *dev, uint8_t scancode)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  if (scancode & 0x80)
    tty_vga_keycode_break(dev, ansi_keycode_2 + (scancode & 0x7f));
  else
    tty_vga_keycode_make(dev, ansi_keycode_2 + scancode);

  pv->scancode = tty_vga_scancode_default;
}

void
tty_vga_scancode_default(struct device_s *dev, uint8_t scancode)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  switch (scancode)
    {
    case (0xe0):		/* extended scancode */
      pv->scancode = tty_vga_scancode_ext;
      break;

    case (0xfa):
      break;			/* command ack */

    default:
      if (scancode & 0x80)
	tty_vga_keycode_break(dev, ansi_keycode_1 + (scancode & 0x7f));
      else
	tty_vga_keycode_make(dev, ansi_keycode_1 + scancode);

      break;
    }
}

DEV_IRQ(tty_vga_irq)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;
  bool_t			res = 0;

  lock_spin(&dev->lock);

  while (cpu_io_read_8(0x64) & 0x01)
    {
      uint8_t	inbyte = cpu_io_read_8(0x60);

      pv->scancode(dev, inbyte);

      res = 1;
    }

  if (res)
    tty_vga_try_read(dev);

  lock_release(&dev->lock);

  return res;
}

