/*

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr> (C) 2009

*/

#ifndef GPCT_PLATFORM_H_
#define GPCT_PLATFORM_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#define GPCT_STR_ERROR(str)     assert(((void)str, 0))

#define GPCT_ERROR_NA(name)     GPCT_STR_ERROR("operation not available for " #name)

#if defined(GPCT_CONFIG_NOASSERT)
# define GPCT_ASSERT(v)
#else
# define GPCT_ASSERT(v)		assert(v)
#endif

#ifdef __GNUC__
# define GPCT_DEPRECATED        __attribute__ ((deprecated))
# define GPCT_ALWAYS_INLINE     __attribute__ ((always_inline))
# define GPCT_NONNULL(...)      __attribute__ ((nonnull(__VA_ARGS__)))
# define GPCT_UNUSED            __attribute__ ((unused))
#endif

#if (__STDC_VERSION__ >= 199901) || defined (__GNUC__)
# define GPCT_INLINE inline
#endif

/****/

#if defined(GPCT_CONFIG_NODEPRECATED) || defined(GPCT_CONFIG_NOWARNING)
# undef GPCT_DEPRECATED
#endif

/****/

#ifndef GPCT_DEPRECATED
# define GPCT_DEPRECATED
#endif

#ifndef GPCT_ALWAYS_INLINE
# define GPCT_ALWAYS_INLINE
#endif

#ifndef GPCT_NONNULL
# define GPCT_NONNULL(...)
#endif

#ifndef GPCT_UNUSED
# define GPCT_UNUSED
#endif

#ifndef GPCT_INLINE
# define GPCT_INLINE
#endif

#define GPCT_INTERNAL static GPCT_INLINE

/* use of intermediate macro allow macro expansion before concat */
#define GPCT_CPP_CONCAT(a, b, c)        a##b##c
#define GPCT_CPP_CONCAT2(a, b)          a##b

#if defined (__GNUC__) && !defined (__SUNPRO_C)
typedef struct {} gpct_empty_t;
#else
typedef char gpct_empty_t;
#endif

#ifndef GPCT_CONFIG_NOPLATFORM

typedef int             gpct_bool_t;
typedef int             gpct_error_t;
typedef signed int      gpct_sindex_t; /* type for C array index */
typedef unsigned int    gpct_index_t; /* type for C array index */
#endif

#ifndef GPCT_UINT
#define GPCT_UINT unsigned int
#endif

#ifndef GPCT_INT
#define GPCT_INT signed int
#endif

#ifndef GPCT_ULONG
#define GPCT_ULONG unsigned long
#endif

#ifndef GPCT_ULONGLONG
#define GPCT_ULONGLONG unsigned long long
#endif

#endif

