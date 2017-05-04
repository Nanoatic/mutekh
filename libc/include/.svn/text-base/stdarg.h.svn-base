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

    Based on dietlibc-0.29 http://www.fefe.de/dietlibc/

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef STDARG_H_
#define STDARG_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

typedef __builtin_va_list va_list;
/**
 * @multiple
 * @showcontent
 * @tt stdarg standard funtion
 */
#if defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ >= 4 )
    #define va_start(v, l) __builtin_va_start((v), (l)) 
#else 
    #define va_start(v, l) __builtin_stdarg_start((v), (l)) 
#endif
#define va_end          __builtin_va_end
#define va_arg          __builtin_va_arg
#define va_copy(d,s)    __builtin_va_copy((d),(s))

C_HEADER_END

#endif

