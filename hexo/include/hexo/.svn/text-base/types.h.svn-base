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

/**
 * @file
 * @module{Hexo}
 * @short Standard integer types definitions
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include "cpu/hexo/types.h"

/* define fixed width types */

typedef unsigned char		uint8_t;
typedef signed char		int8_t;

#if CPU_SIZEOF_INT == 16
typedef unsigned int		uint16_t;
typedef signed int		int16_t;
#elif CPU_SIZEOF_SHORT == 16
typedef unsigned short		uint16_t;
typedef signed short		int16_t;
#endif

#if CPU_SIZEOF_INT == 32
typedef unsigned int		uint32_t;
typedef signed int		int32_t;
#elif CPU_SIZEOF_LONG == 32
typedef unsigned long		uint32_t;
typedef signed long		int32_t;
#endif

typedef unsigned long long	uint64_t;
typedef signed long long	int64_t;

/* define other iso c99 integer types */

#define _DEFINE_INT_TYPE_(a, b, c) a##b##c
#define _DEFINE_INT_TYPE(a, b, c) _DEFINE_INT_TYPE_(a, b, c)

#define _DEFINE_FAST_INT(size)					\
  typedef _DEFINE_INT_TYPE(int, INT_FAST##size##_SIZE, _t)	\
       int##_fast##size##_t;					\
  typedef _DEFINE_INT_TYPE(uint, INT_FAST##size##_SIZE, _t)	\
       uint##_fast##size##_t;

#define _DEFINE_PTR_INT(size)					\
  typedef _DEFINE_INT_TYPE(uint, size, _t)	uintptr_t;	\
  typedef _DEFINE_INT_TYPE(int, size, _t)	intptr_t;	\
  typedef _DEFINE_INT_TYPE(int, size, _t)	ptrdiff_t;

#define _DEFINE_REG_INT(size)					\
  typedef _DEFINE_INT_TYPE(uint, size, _t)	reg_t;		\
  typedef _DEFINE_INT_TYPE(int, size, _t)	sreg_t;

#define _DEFINE_ATOMIC_INT(size)                                \
  typedef _DEFINE_INT_TYPE(int, size, _t)       atomic_int_t;

#ifdef CONFIG_HEXO_INTTYPES_SMALL
# undef INT_FAST8_SIZE
# define INT_FAST8_SIZE          8
# undef INT_FAST16_SIZE
# define INT_FAST16_SIZE         16
# undef INT_FAST32_SIZE
# define INT_FAST32_SIZE         32
# undef INT_FAST64_SIZE
# define INT_FAST64_SIZE         64
#endif

_DEFINE_FAST_INT(8)
_DEFINE_FAST_INT(16)
_DEFINE_FAST_INT(32)
_DEFINE_FAST_INT(64)
_DEFINE_PTR_INT(INT_PTR_SIZE)
_DEFINE_REG_INT(INT_REG_SIZE)
_DEFINE_ATOMIC_INT(INT_ATOMIC_SIZE)

#include "arch/hexo/types.h"

/** Physical address type*/
#if defined( CONFIG_HEXO_MMU_PADDR )
# if CONFIG_HEXO_MMU_PADDR <= 32
typedef uint32_t paddr_t;
# else
typedef uint64_t paddr_t;
# endif
#else
typedef uintptr_t paddr_t;
#endif

/** @this is used to prevent ld script symbols from being placed in
   @tt .sdata section. the @ref __ldscript_symbol_s is never defined. */
typedef struct __ldscript_symbol_s __ldscript_symbol_t;

/** @multiple @internal @this compiler native integer types defined
 for compiler type dependant special cases. */

typedef signed short		__compiler_sshort_t;
typedef signed int		__compiler_sint_t;
typedef signed long		__compiler_slong_t;
typedef signed long long	__compiler_slonglong_t;

typedef unsigned short		__compiler_ushort_t;
typedef unsigned int		__compiler_uint_t;
typedef unsigned long		__compiler_ulong_t;
typedef unsigned long long	__compiler_ulonglong_t;

#ifdef CONFIG_HEXO_INTTYPES_DEPRECATED
/** @this prevents use of compiler native short type,
    @tt int_fast*_t and @tt uint_fast*_t types are prefered. */
typedef short _dont_use_native_short_type_t __attribute__ ((deprecated));
# define short	_dont_use_native_short_type_t

/** @this prevents use of compiler native int type,
    @tt int_fast*_t and @tt uint_fast*_t types are prefered. */
typedef int _dont_use_native_int_type_t __attribute__ ((deprecated));
# define int	_dont_use_native_int_type_t

/** @this prevents use of compiler native long type,
    @tt int_fast*_t and @tt uint_fast*_t types are prefered. */
typedef long _dont_use_native_long_type_t __attribute__ ((deprecated));
# define long	_dont_use_native_long_type_t
#endif

/** @this returns max integer value for a type */
#define __MINOF_TYPE(t)        ((typeof(t))(((typeof(t))-1) < 0 ?  (((typeof(t))1) << (sizeof(typeof(t)) * 8 - 1)) :  0))

/** @this returns min integer value for a type */
#define __MAXOF_TYPE(t)        ((typeof(t))(((typeof(t))-1) < 0 ? ~(((typeof(t))1) << (sizeof(typeof(t)) * 8 - 1)) : -1))

/** NULL pointer definition */
#ifndef __cplusplus
# define NULL	((void*)0)
#else
# define NULL 0
#endif

C_HEADER_END

#endif

