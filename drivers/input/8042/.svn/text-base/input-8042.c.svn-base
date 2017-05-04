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

#include <device/icu.h>
#include <device/input.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>

#include <assert.h>

#include "input-8042.h"

#include "input-8042-private.h"

#include "8042.h"

/**************************************************************/

/* 
 * device info query
 */

DEVINPUT_INFO(input_8042_info)
{
  info->name = "8042 PC Keyboard controller";
  info->ctrl_button_count = INPUT_8042_KEYCOUNT;
  info->ctrl_axe_count = 0;
}

/* 
 * device read operation
 */

DEVINPUT_READ(input_8042_read)
{
  struct input_8042_context_s	*pv = dev->drv_pv;

  assert(id < INPUT_8042_KEYCOUNT);

  return input_state_get(&pv->key_state, id);
}

/* 
 * device write operations
 */

DEVINPUT_WRITE(input_8042_write)
{
  // struct input_8042_context_s	*pv = dev->drv_pv;

  assert(id < INPUT_8042_KEYCOUNT);

  /* FIXME handle led status change here */

  return -ERANGE;
}

/* 
 * device read operation
 */

DEVINPUT_SETCALLBACK(input_8042_setcallback)
{
  struct input_8042_context_s	*pv = dev->drv_pv;

  assert((id == DEVINPUT_CTRLID_ALL) || (id < INPUT_8042_KEYCOUNT));

  if (id == DEVINPUT_CTRLID_ALL)
    {
      devinput_ctrlid_t	i;

      for (i = 0; i < INPUT_8042_KEYCOUNT; i++)
	{
	  pv->events[i].callback = callback;
	  pv->events[i].priv = priv;
	  pv->events[i].type = type;
	}
    }
  else
    {
      pv->events[id].callback = callback;
      pv->events[id].priv = priv;
      pv->events[id].type = type;
    }

  return 0;
}

static inline void
input_8042_keyevent(struct input_8042_context_s *pv, devinput_ctrlid_t keyid, bool_t down)
{
  input_state_set(&pv->key_state, keyid, down);

  if (pv->events[keyid].callback != NULL)
    {
      if (down && (pv->events[keyid].type & DEVINPUT_EVENT_BUTTON_DOWN))
	pv->events[keyid].callback(keyid, down, pv->events[keyid].priv);
      if (!down && (pv->events[keyid].type & DEVINPUT_EVENT_BUTTON_UP))
	pv->events[keyid].callback(keyid, down, pv->events[keyid].priv);
    }

  /* FIXME handle led status change here depending on key pressed */
}

static void
input_8042_scancode_default(struct device_s *dev, uint8_t scancode);

static void
input_8042_scancode_led(struct device_s *dev, uint8_t scancode)
{
  struct input_8042_context_s	*pv = dev->drv_pv;

  if (scancode == 0xfa)
    cpu_io_write_8(dev->addr[0] + KEYB_8042_REG1, pv->led_state);

  pv->scancode = input_8042_scancode_default;
}

static void
input_8042_scancode_ext(struct device_s *dev, uint8_t scancode)
{
  struct input_8042_context_s	*pv = dev->drv_pv;
  static const devinput_ctrlid_t keycodes[INPUT_8042_KEYCOUNT] =
    {
      [0x38] = INPUT_8042_KEY_RALT,
      [0x1d] = INPUT_8042_KEY_RCTRL,
      [0x52] = INPUT_8042_KEY_INSERT,
      [0x47] = INPUT_8042_KEY_HOME,
      [0x49] = INPUT_8042_KEY_PAGEUP,
      [0x53] = INPUT_8042_KEY_DELETE,
      [0x4f] = INPUT_8042_KEY_END,
      [0x51] = INPUT_8042_KEY_PAGEDOWN,
      [0x48] = INPUT_8042_KEY_UP,
      [0x4b] = INPUT_8042_KEY_LEFT,
      [0x50] = INPUT_8042_KEY_DOWN,
      [0x4d] = INPUT_8042_KEY_RIGHT,
      [0x37] = INPUT_8042_KEY_SYSREQ,

      [0x35] = INPUT_8042_KEY_KP_DIVIDE,
      [0x1c] = INPUT_8042_KEY_KP_ENTER,

      [0x5b] = INPUT_8042_KEY_LWIN,
      [0x5c] = INPUT_8042_KEY_POPUP,
      [0x5d] = INPUT_8042_KEY_RWIN,
    };

  pv->scancode = input_8042_scancode_default;

  if ((scancode & 0x7f) == 0x2a)
    return;

  input_8042_keyevent(pv, keycodes[scancode & 0x7f], !(scancode & 0x80));
}

static void
input_8042_scancode_default(struct device_s *dev, uint8_t scancode)
{
  struct input_8042_context_s	*pv = dev->drv_pv;
  static const devinput_ctrlid_t keycodes[INPUT_8042_KEYCOUNT] =
    {
      [0x01] = INPUT_8042_KEY_ESCAPE,
      [0x3b] = INPUT_8042_KEY_F1,
      [0x3c] = INPUT_8042_KEY_F2,
      [0x3d] = INPUT_8042_KEY_F3,
      [0x3e] = INPUT_8042_KEY_F4,
      [0x3f] = INPUT_8042_KEY_F5,
      [0x40] = INPUT_8042_KEY_F6,
      [0x41] = INPUT_8042_KEY_F7,
      [0x42] = INPUT_8042_KEY_F8,
      [0x43] = INPUT_8042_KEY_F9,
      [0x44] = INPUT_8042_KEY_F10,
      [0x57] = INPUT_8042_KEY_F11,
      [0x58] = INPUT_8042_KEY_F12,
      [0x46] = INPUT_8042_KEY_SCROLLOCK,
      [0x29] = INPUT_8042_KEY_BACKQUOTE,
      [0x02] = INPUT_8042_KEY_EXCLAIM,
      [0x03] = INPUT_8042_KEY_QUOTEDBL,
      [0x04] = INPUT_8042_KEY_HASH,
      [0x05] = INPUT_8042_KEY_DOLLAR,
      [0x06] = INPUT_8042_KEY_PERCENT,
      [0x07] = INPUT_8042_KEY_CARET,
      [0x08] = INPUT_8042_KEY_AMPERSAND,
      [0x09] = INPUT_8042_KEY_ASTERISK,
      [0x0a] = INPUT_8042_KEY_LEFTPAREN,
      [0x0b] = INPUT_8042_KEY_RIGHTPAREN,
      [0x0c] = INPUT_8042_KEY_MINUS,
      [0x0d] = INPUT_8042_KEY_EQUALS,
      [0x0e] = INPUT_8042_KEY_BACKSPACE,
      [0x0f] = INPUT_8042_KEY_TAB,
      [0x10] = INPUT_8042_KEY_Q,
      [0x11] = INPUT_8042_KEY_W,
      [0x12] = INPUT_8042_KEY_E,
      [0x13] = INPUT_8042_KEY_R,
      [0x14] = INPUT_8042_KEY_T,
      [0x15] = INPUT_8042_KEY_Y,
      [0x16] = INPUT_8042_KEY_U,
      [0x17] = INPUT_8042_KEY_I,
      [0x18] = INPUT_8042_KEY_O,
      [0x19] = INPUT_8042_KEY_P,
      [0x1a] = INPUT_8042_KEY_LEFTBRACKET,
      [0x1b] = INPUT_8042_KEY_RIGHTBRACKET,
      [0x1c] = INPUT_8042_KEY_RETURN,

      [0x3a] = INPUT_8042_KEY_CAPSLOCK,
      [0x1e] = INPUT_8042_KEY_A,
      [0x1f] = INPUT_8042_KEY_S,
      [0x20] = INPUT_8042_KEY_D,
      [0x21] = INPUT_8042_KEY_F,
      [0x22] = INPUT_8042_KEY_G,
      [0x23] = INPUT_8042_KEY_H,
      [0x24] = INPUT_8042_KEY_J,
      [0x25] = INPUT_8042_KEY_K,
      [0x26] = INPUT_8042_KEY_L,
      [0x27] = INPUT_8042_KEY_SEMICOLON,
      [0x28] = INPUT_8042_KEY_QUOTE,
      [0x2b] = INPUT_8042_KEY_BACKSLASH,

      [0x2a] = INPUT_8042_KEY_LSHIFT,
      [0x56] = INPUT_8042_KEY_LESS,
      [0x2c] = INPUT_8042_KEY_Z,
      [0x2d] = INPUT_8042_KEY_X,
      [0x2e] = INPUT_8042_KEY_C,
      [0x2f] = INPUT_8042_KEY_V,
      [0x30] = INPUT_8042_KEY_B,
      [0x31] = INPUT_8042_KEY_N,
      [0x32] = INPUT_8042_KEY_M,
      [0x33] = INPUT_8042_KEY_COMMA,
      [0x34] = INPUT_8042_KEY_PERIOD,
      [0x35] = INPUT_8042_KEY_SLASH,
      [0x36] = INPUT_8042_KEY_RSHIFT,

      [0x1d] = INPUT_8042_KEY_LCTRL,
      [0x38] = INPUT_8042_KEY_LALT,
      [0x39] = INPUT_8042_KEY_SPACE,

      [0x45] = INPUT_8042_KEY_NUMLOCK,
      [0x37] = INPUT_8042_KEY_KP_MULTIPLY,
      [0x4a] = INPUT_8042_KEY_KP_MINUS,
      [0x4e] = INPUT_8042_KEY_KP_PLUS,
      [0x53] = INPUT_8042_KEY_KP_PERIOD,
      [0x52] = INPUT_8042_KEY_KP0,
      [0x4f] = INPUT_8042_KEY_KP1,
      [0x50] = INPUT_8042_KEY_KP2,
      [0x51] = INPUT_8042_KEY_KP3,
      [0x4b] = INPUT_8042_KEY_KP4,
      [0x4c] = INPUT_8042_KEY_KP5,
      [0x4d] = INPUT_8042_KEY_KP6,
      [0x47] = INPUT_8042_KEY_KP7,
      [0x48] = INPUT_8042_KEY_KP8,
      [0x49] = INPUT_8042_KEY_KP9,
   };

  switch (scancode)
    {
    case (0xe0):		/* extended scancode */
      pv->scancode = input_8042_scancode_ext;
      break;

    case (0xfa):
      break;			/* command ack */

    default:
      input_8042_keyevent(pv, keycodes[scancode & 0x7f], !(scancode & 0x80));
      break;
    }
}

static void input_8042_updateleds(struct device_s *dev)
{
  struct input_8042_context_s	*pv = dev->drv_pv;

  LOCK_SPIN_IRQ(&pv->lock);

  pv->scancode = input_8042_scancode_led;
  cpu_io_write_8(dev->addr[0] + KEYB_8042_REG1, 0xed);

  LOCK_RELEASE_IRQ(&pv->lock);
}

/*
 * IRQ handler
 */

DEV_IRQ(input_8042_irq)
{
  struct input_8042_context_s	*pv = dev->drv_pv;
  bool_t			res = 0;

  lock_spin(&pv->lock);

  while (cpu_io_read_8(dev->addr[0] + KEYB_8042_REG2) & KEYB_8042_VALUE1)
    {
      uint8_t	inbyte = cpu_io_read_8(dev->addr[0] + KEYB_8042_REG1);

      pv->scancode(dev, inbyte);

      res = 1;
    }

  lock_release(&pv->lock);

  return res;
}

/* 
 * device close operation
 */

DEV_CLEANUP(input_8042_cleanup)
{
  struct input_8042_context_s	*pv = dev->drv_pv;

  DEV_ICU_UNBIND(dev->icudev, dev, dev->irq, input_8042_irq);

  lock_destroy(&pv->lock);
  mem_free(pv);
}

/* 
 * device open operation
 */

const struct driver_s	input_8042_drv =
{
  .class		= device_class_input,
  .f_init		= input_8042_init,
  .f_cleanup		= input_8042_cleanup,
  .f_irq		= input_8042_irq,
  .f.input = {
    .f_info		= input_8042_info,
    .f_read		= input_8042_read,
    .f_setcallback	= input_8042_setcallback,
  }
};

DEV_INIT(input_8042_init)
{
  struct input_8042_context_s	*pv;

  dev->drv = &input_8042_drv;

#if defined(CONFIG_ARCH_IBMPC)
  assert(dev->addr[0] == 0x60);
#endif

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -ENOMEM;

  {
    devinput_ctrlid_t	i;

    for (i = 0; i < INPUT_8042_KEYCOUNT; i++)
      pv->events[i].callback = NULL;
  }

  dev->drv_pv = pv;

  lock_init(&pv->lock);
  input_state_init(&pv->key_state);

  pv->led_state = KEYB_8042_LED_NUM;
  input_8042_updateleds(dev);

  DEV_ICU_BIND(dev->icudev, dev, dev->irq, input_8042_irq);

  return 0;
}

