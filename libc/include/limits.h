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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#ifndef LIMITS_H_
#define LIMITS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

#include <hexo/types.h>

/**
 * @multiple
 * @showcontent
 * This macro defines an integer type limit
 */

/* char type */

#define CHAR_BIT	8

#define SCHAR_MIN	(__MINOF_TYPE(signed char))
#define SCHAR_MAX	(__MAXOF_TYPE(signed char))

#define UCHAR_MAX	(__MAXOF_TYPE(unsigned char))
#define UCHAR_MAX	(__MAXOF_TYPE(unsigned char))

#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX

/* short type */

#define SHRT_MIN	(__MINOF_TYPE(__compiler_sshort_t))
#define SHRT_MAX	(__MAXOF_TYPE(__compiler_sshort_t))
#define USHRT_MIN	(__MAXOF_TYPE(__compiler_ushort_t))
#define USHRT_MAX	(__MAXOF_TYPE(__compiler_ushort_t))

/* int type */

#define INT_MIN		(__MINOF_TYPE(__compiler_sint_t))
#define INT_MAX		(__MAXOF_TYPE(__compiler_sint_t))
#define UINT_MIN	(__MINOF_TYPE(__compiler_uint_t))
#define UINT_MAX	(__MAXOF_TYPE(__compiler_uint_t))

#define LONG_MIN	(__MINOF_TYPE(__compiler_slong_t))
#define LONG_MAX	(__MAXOF_TYPE(__compiler_slong_t))
#define ULONG_MIN	(__MINOF_TYPE(__compiler_ulong_t))
#define ULONG_MAX	(__MAXOF_TYPE(__compiler_ulong_t))

#define LONGLONG_MIN	(__MINOF_TYPE(__compiler_slonglong_t))
#define LONGLONG_MAX	(__MAXOF_TYPE(__compiler_slonglong_t))
#define ULONGLONG_MIN	(__MINOF_TYPE(__compiler_ulonglong_t))
#define ULONGLONG_MAX	(__MAXOF_TYPE(__compiler_ulonglong_t))

C_HEADER_END

#endif
