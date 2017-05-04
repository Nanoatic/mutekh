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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2010

*/

#ifndef _HEXO_ORDERING_H_
#define _HEXO_ORDERING_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{Hexo}
 * @short Serializing operations and memory barrier primitives
 */

#if 0
# include <cpu/hexo/ordering.h>
# include <arch/hexo/ordering.h>
#endif

/** @this is a compiler only memory barrier. Memory accesses are not
    reordered by the compiler over this barrier. No other guarantee is
    given. */
#define order_compiler_mem() __asm__ __volatile__ ("" ::: "memory")


#ifndef CPU_ORDER_MEM
# define CPU_ORDER_MEM ""
#endif

#ifdef CONFIG_ARCH_SMP
/** @this is a compiler and processor read/write memory
    barrier. Memory accesses are not reordered by the compiler over
    this barrier and the processor ensure memory accesses do not cross
    this barrier from other processors point of view. This is
    equivalent to a compiler only barrier for single processor builds. */
# define order_smp_mem() __asm__ __volatile__ (CPU_ORDER_MEM ::: "memory")
#else
# define order_smp_mem() order_compiler_mem()
#endif


#ifndef CPU_ORDER_WRITE
# define CPU_ORDER_WRITE ""
#endif

#ifdef CONFIG_ARCH_SMP
/** @this is a compiler and processor write memory barrier. Memory
    accesses are not reordered by the compiler over this barrier and
    the processor ensure all write operations have ended
    before next write operation starts, from other processors point of
    view. Should be paired with a (weak) read barrier.

    This is equivalent to a compiler only barrier for single
    processor builds.  */
# define order_smp_write() __asm__ __volatile__ (CPU_ORDER_WRITE ::: "memory")
#else
# define order_smp_write() order_compiler_mem()
#endif


#ifndef CPU_ORDER_READ
# define CPU_ORDER_READ ""
#endif

#ifdef CONFIG_ARCH_SMP
/** @this is a compiler and processor read memory barrier. Memory
    accesses are not reordered by the compiler over this barrier and
    the processor ensure all read operations have ended
    before next read operation starts, from other processors point of
    view. Should be paired with a write barrier.

    This is equivalent to a compiler only barrier for single
    processor builds. */
# define order_smp_read() __asm__ __volatile__ (CPU_ORDER_READ ::: "memory")
#else
# define order_smp_read() order_compiler_mem()
#endif


#ifndef CPU_ORDER_READ_WEAK
# define CPU_ORDER_READ_WEAK ""
#endif

#ifdef CONFIG_ARCH_SMP
/** @this is a compiler and processor weak read memory barrier. Memory
    accesses are not reordered by the compiler over this barrier and
    the processor ensure data dependency between load
    operations. Should be paired with a write barrier.

    This is equivalent to a compiler only barrier for single
    processor builds. */
# define order_smp_read_weak() __asm__ __volatile__ (CPU_ORDER_READ_WEAK ::: "memory")
#else
# define order_smp_read_weak() order_compiler_mem()
#endif


#ifndef CPU_ORDER_IO_MEM
# define CPU_ORDER_IO_MEM ""
#endif

/** @this is a compiler and processor read/write memory
    barrier. Memory accesses are not reordered by the compiler over
    this barrier and the processor ensure memory accesses do not cross
    this barrier from platform point of view. */
#define order_io_mem()             __asm__ __volatile__ (CPU_ORDER_IO_MEM ::: "memory");

#ifndef CPU_ORDER_IO_WRITE
# define CPU_ORDER_IO_WRITE ""
#endif

/** @this is a compiler and processor write memory barrier. Memory
    accesses are not reordered by the compiler over this barrier and
    the processor ensure all write operations have ended before next
    write operation starts, from platform and devices point of
    view. */
#define order_io_mem_write()       __asm__ __volatile__ (CPU_ORDER_IO_WRITE ::: "memory");

#ifndef CPU_ORDER_IO_READ
# define CPU_ORDER_IO_READ ""
#endif

/** @this is a compiler and processor read memory barrier. Memory
    accesses are not reordered by the compiler over this barrier and
    the processor ensure all read operations have ended before next
    read operation starts, from platform and devices point of
    view. */
#define order_io_mem_read()        __asm__ __volatile__ (CPU_ORDER_IO_READ ::: "memory");

C_HEADER_END

#endif

