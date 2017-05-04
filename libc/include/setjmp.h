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

    Based on string.h from dietlibc-0.29 http://www.fefe.de/dietlibc/

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006
*/

#ifndef SETJMP_H_
#define SETJMP_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

#include <hexo/cpu.h>
#include <hexo/types.h>

/* last values are pc reg and setjump return value */
typedef reg_t jmp_buf[CPU_GPREG_COUNT + 1 + 1]; 
typedef jmp_buf sigjmp_buf;

reg_t setjmp(jmp_buf env);

static inline reg_t
__attribute__ ((deprecated))
sigsetjmp(sigjmp_buf env, reg_t savesigs)
{
  return setjmp(env);
}

void longjmp(jmp_buf env, reg_t val);

static inline void
__attribute__ ((deprecated))
siglongjmp(sigjmp_buf env, reg_t val)
{
  longjmp(env, val);
}

C_HEADER_END

#endif

