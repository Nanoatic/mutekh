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

#ifndef __ASSERT_H_
#define __ASSERT_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

#include <hexo/types.h>
#include <hexo/cpu.h>

#ifdef NDEBUG
# warning NDEBUG is deprecated here, use CONFIG_LIBC_ASSERT or CONFIG_DEBUG
#endif

# if defined(CONFIG_LIBC_ASSERT)

void
__assert_fail(const char *file,
			  uint_fast16_t line,
			  const char *func,
			  const char *expr);

/** @multiple @this is the standard @tt assert macro */
#  define assert(expr) ((void) ((expr) ? 0 : __assert_fail(__FILE__, __LINE__, __func__, #expr)))
# else
#  define assert(expr) ((void) 0)
# endif

# if defined(CONFIG_LIBC_ASSERT)
/** @multiple @this macro does the same as the @ref #assert macro, but still execute @tt expr when @ref #CONFIG_LIBC_ASSERT is disabled */
#  define ensure(expr) assert(expr)
# else
#  define ensure(expr) ((void) (expr))
# endif

C_HEADER_END

#endif

