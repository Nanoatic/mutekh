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

#if !defined(__ENDIAN_H_) || defined(CPU_ENDIAN_H_)
#error This file can not be included directly
#else

#define CPU_ENDIAN_H_

/** x86 CPU handle nonaligned memory access */
#ifndef CONFIG_CPU_X86_ALIGNCHECK
#define CPU_NATIVE_NONALIGNED_ACCESS
#endif

/** x86 CPU is little endian */
#undef CPU_ENDIAN_ISBIG
#define CPU_ENDIAN_ISLITTLE

#define HAS_CPU_ENDIAN_SWAP16

static inline uint16_t cpu_endian_swap16(uint16_t x)
{
  asm ("rolw	$8, %0"
       : "=r" (x)
       : "0" (x)
       );

  return x;
}

#define HAS_CPU_ENDIAN_SWAP32

static inline uint32_t cpu_endian_swap32(uint32_t x)
{
  asm ("bswap	%0"
       : "=r" (x)
       : "0" (x)
       );

  return x;
}

#endif

