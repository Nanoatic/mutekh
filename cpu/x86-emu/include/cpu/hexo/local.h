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
   @file Task local and CPU local variables access
  */

#if !defined(LOCAL_H_) || defined(CPU_LOCAL_H_)
#error This file can not be included directly
#else

#include <hexo/types.h>

#define CPU_LOCAL_H_

/************************************************************************/

#if defined(CONFIG_ARCH_EMU_LINUX)
/** context local storage type attribute */
#define CPU_LOCAL	__attribute__((section (".cpudata")))
#elif defined(CONFIG_ARCH_EMU_DARWIN)
#define CPU_LOCAL
#else
# error Hu?
#endif

extern CPU_LOCAL void *__cpu_data_base;
extern __ldscript_symbol_t __cpu_data_start;

#ifdef CONFIG_ARCH_SMP
/** cpu local storage type attribute */
# define CPU_GET_CLS() ((uintptr_t)__cpu_data_base - (uintptr_t)&__cpu_data_start)
# define CPU_LOCAL_CLS_SET(cls, n, v) { *(typeof(n)*)((uintptr_t)(cls) - (uintptr_t)&__cpu_data_start + (uintptr_t)&(n)) = (v); }
# define CPU_LOCAL_CLS_GET(cls, n) 	({ *(typeof(n)*)((uintptr_t)(cls) - (uintptr_t)&__cpu_data_start + (uintptr_t)&(n)); })
# define CPU_LOCAL_CLS_ADDR(cls, n) ((void*)((uintptr_t)(cls) - (uintptr_t)&__cpu_data_start + (uintptr_t)&(n)))
#endif

/************************************************************************/

#if defined(CONFIG_ARCH_EMU_LINUX)
/** context local storage type attribute */
#define CONTEXT_LOCAL	__attribute__((section (".contextdata")))
#elif defined(CONFIG_ARCH_EMU_DARWIN)
#define CONTEXT_LOCAL	__attribute__((section ("__DATA, __contextdata")))
#else
# error Hu?
#endif

extern CPU_LOCAL void *__context_data_base;

/* We do not have VMA to 0 for context local sections on this
   architecture because it causes some loading problems with linux
   exec loader (when /proc/sys/vm/mmap_min_addr is set).  Instead we
   subtract the __context_data_start to the TLS address here. */

extern __ldscript_symbol_t __context_data_start;
#define CONTEXT_GET_TLS() ((uintptr_t)CPU_LOCAL_GET(__context_data_base) - (uintptr_t)&__context_data_start)
#define CONTEXT_LOCAL_TLS_SET(tls, n, v) { *(typeof(n)*)((uintptr_t)(tls) - (uintptr_t)&__context_data_start + (uintptr_t)&(n)) = (v); }
#define CONTEXT_LOCAL_TLS_GET(tls, n) 	({ *(typeof(n)*)((uintptr_t)(tls) - (uintptr_t)&__context_data_start + (uintptr_t)&(n)); })
#define CONTEXT_LOCAL_TLS_ADDR(tls, n) ((void*)((uintptr_t)(tls) - (uintptr_t)&__context_data_start + (uintptr_t)&(n)))

#endif

