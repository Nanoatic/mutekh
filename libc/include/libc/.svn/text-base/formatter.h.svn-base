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

#ifndef MUTEK_PRINTF_ARG_H_
#define MUTEK_PRINTF_ARG_H_

/**
 * @file
 * @module{C library}
 */

#include <stdarg.h>
#include <hexo/types.h>

#define PRINTF_OUTPUT_FUNC(x) void (x)(void *ctx, const char *str, size_t offset, size_t len)

typedef PRINTF_OUTPUT_FUNC(printf_output_func_t);

ssize_t
formatter_printf(void *ctx, printf_output_func_t * const fcn,
				 const char *format, va_list ap);

void
mutek_hexdump_arg(void *ctx, printf_output_func_t * const fcn,
                  uintptr_t address, const void *base, size_t len);

/** @this write a string from a floating point value */
ssize_t
formatter_dtostr(double d, char *buf, size_t maxlen,
                 size_t prec, size_t prec2, ssize_t g);

#endif
