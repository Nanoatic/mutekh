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
 * @short Processor local and context local, language global variables features
 */

#ifndef LOCAL_H_
#define LOCAL_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/types.h>
#include "cpu/hexo/local.h"

/************************************************************************/

#ifdef CONFIG_ARCH_SMP

extern void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];

# ifndef CPU_GET_CLS_ID
/** @this returns the address of the cpu local storage for a given cpu */
#  define CPU_GET_CLS_ID(cpuid) cpu_local_storage[(cpuid)]
# endif

# ifndef CPU_LOCAL
/** @this must be used as attribute for cpu local variables */
#  warning CPU_LOCAL not defined by cpu code
#  define CPU_LOCAL
# endif

# ifndef CPU_LOCAL_SET
/** @this must be used to set cpu local variables */
#  define CPU_LOCAL_SET(n, v) { *CPU_LOCAL_ADDR(n) = (v); }
# endif

# ifndef CPU_LOCAL_CLS_SET
/** @this must be used to set cpu local variables in a given cpu storage buffer */
#  define CPU_LOCAL_CLS_SET(cls, n, v) { *(typeof(n)*)((uintptr_t)(cls) + (uintptr_t)&(n)) = (v); }
# endif

# ifndef CPU_LOCAL_GET
/** @this must be used to get cpu local variables value */
#  define CPU_LOCAL_GET(n) ({ *CPU_LOCAL_ADDR(n); })
# endif

# ifndef CPU_LOCAL_CLS_GET
/** @this must be used to get cpu local variables value in a given cpu storage buffer */
#  define CPU_LOCAL_CLS_GET(cls, n) ({ *(typeof(n)*)((uintptr_t)(cls) + (uintptr_t)&(n)); })
# endif

#ifndef CPU_LOCAL_ADDR
/** @this returns the address of cpu local variable */
# define CPU_LOCAL_ADDR(n) ( (typeof(n)*)((uintptr_t)CPU_GET_CLS() + (uintptr_t)&(n)) )
#endif

# ifndef CPU_LOCAL_CLS_ADDR
/** @this returns the address of a cpu local variable in given cpu local storage */
#  define CPU_LOCAL_CLS_ADDR(cls, n)	( (typeof(n)*)((uintptr_t)(cls) + (uintptr_t)&(n)) )
# endif

# ifndef CPU_LOCAL_ID_ADDR
/** @this returns the address of a cpu local variable for the given cpu */
#  define CPU_LOCAL_ID_ADDR(cpuid, n) CPU_LOCAL_CLS_ADDR(CPU_GET_CLS_ID(cpuid), n)
# endif

#else

//# define CPU_GET_CLS_ID(cpuid) __cpu_local_h_must_provide_CPU_GET_CLS_ID_macro
//# define CPU_GET_CLS() __cpu_local_h_must_provide_CPU_GET_CLS_macro
# define CPU_LOCAL_ADDR(n)   (&(n))
# define CPU_LOCAL_CLS_ADDR(cls, n) (&(n))
# define CPU_LOCAL_ID_ADDR(cpuid, n) (&(n))
# define CPU_LOCAL_GET(n)    (n)
# define CPU_LOCAL_SET(n, v)  { (n) = (v); }

#endif

/************************************************************************/

# ifndef CONTEXT_LOCAL
#  define CONTEXT_LOCAL
# endif

#ifndef CONTEXT_LOCAL_SET
/** @this must be used to set context local variables */
# define CONTEXT_LOCAL_SET(n, v)	{ *(typeof(n)*)((uintptr_t)CONTEXT_GET_TLS() + (uintptr_t)&(n)) = (v); }
#endif

#ifndef CONTEXT_LOCAL_TLS_SET
/** @this must be used to set context local variables in a given context storage buffer */
# define CONTEXT_LOCAL_TLS_SET(tls, n, v) { *(typeof(n)*)((uintptr_t)(tls) + (uintptr_t)&(n)) = (v); }
#endif

#ifndef CONTEXT_LOCAL_GET
/** @this must be used to get context local variables value */
# define CONTEXT_LOCAL_GET(n) 	({ *(typeof(n)*)((uintptr_t)CONTEXT_GET_TLS() + (uintptr_t)&(n)); })
#endif

#ifndef CONTEXT_LOCAL_TLS_GET
/** @this must be used to get context local variables value in a given context storage buffer */
# define CONTEXT_LOCAL_TLS_GET(tls, n) 	({ *(typeof(n)*)((uintptr_t)(tls) + (uintptr_t)&(n)); })
#endif

#ifndef CONTEXT_LOCAL_ADDR
/** @this returns the address of context local variable */
# define CONTEXT_LOCAL_ADDR(n)	({ (void*)((uintptr_t)CONTEXT_GET_TLS() + (uintptr_t)&(n)); })
#endif

/** @this returns the address of context local object for different context */
#ifndef CONTEXT_LOCAL_TLS_ADDR
# define CONTEXT_LOCAL_TLS_ADDR(tls, n)	({ (void*)((uintptr_t)(tls) + (uintptr_t)&(n)); })
#endif

/************************************************************************/

C_HEADER_END

#endif

