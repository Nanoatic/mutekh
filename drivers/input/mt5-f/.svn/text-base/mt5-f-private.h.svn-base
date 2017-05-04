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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net>, 2009

*/

#ifndef _MT5F_PRIVATE_H_
#define _MT5F_PRIVATE_H_

struct mt5f_context_s
{
	devinput_callback_t *callback;
	void *priv;
	struct device_s *gpio_dev;
	devgpio_id_t a;
	devgpio_id_t b;
	devgpio_id_t c;
	devgpio_id_t d;
	devgpio_id_t common;
	uint_fast8_t last_sate;
};

#endif

