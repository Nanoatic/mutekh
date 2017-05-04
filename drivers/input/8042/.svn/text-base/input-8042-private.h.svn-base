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

#ifndef INPUT_8042_PRIVATE_H_
#define INPUT_8042_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_bitmap.h>

CONTAINER_TYPE(input_state, BITMAP, uint32_t, INPUT_8042_KEYCOUNT);
CONTAINER_FUNC(input_state, BITMAP, static inline, input_state);

struct input_8042_context_s;

typedef void input_key_process_t  (struct device_s *dev,
				   uint8_t scancode);

struct input_8042_context_s
{
  lock_t			lock;
  input_key_process_t		*scancode;
  input_state_root_t		key_state;
  uint_fast8_t			led_state;

  struct 
  {
    devinput_callback_t		*callback;
    void			*priv;
    uint_fast8_t		type;
  }				events[INPUT_8042_KEYCOUNT];
};

#endif

