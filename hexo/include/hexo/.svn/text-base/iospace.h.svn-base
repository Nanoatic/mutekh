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
 * @short Io and memory address spaces functions access
 */

#ifndef IOSPACE_H_
#define IOSPACE_H_

#include "types.h"

/**
   IO space 8 bits write

   @param addr write address
   @param data value
*/

static void cpu_io_write_8(uintptr_t addr, uint8_t data);

/**
   IO space 8 bits read

   @param addr read address
   @return data value
*/

static uint8_t cpu_io_read_8(uintptr_t addr);

/**
   IO space 16 bits write

   @param addr write address
   @param data value
*/

static void cpu_io_write_16(uintptr_t addr, uint16_t data);

/**
   IO space 16 bits read

   @param addr read address
   @return data value
*/

static uint16_t cpu_io_read_16(uintptr_t addr);

/**
   IO space 32 bits write

   @param addr write address
   @param data value
*/

static void cpu_io_write_32(uintptr_t addr, uint32_t data);

/**
   IO space 32 bits read

   @param addr read address
   @return data value
*/

static uint32_t cpu_io_read_32(uintptr_t addr);

/**
   Memory space 8 bits write

   @param addr write address
   @param data value
*/

static void cpu_mem_write_8(uintptr_t addr, uint8_t data);

/**
   Memory space 8 bits read

   @param addr read address
   @return data value
*/

static uint8_t cpu_mem_read_8(uintptr_t addr);

/**
   Memory space 16 bits write

   @param addr write address
   @param data value
*/

static void cpu_mem_write_16(uintptr_t addr, uint16_t data);

/**
   Memory space 16 bits read

   @param addr read address
   @return data value
*/

static uint16_t cpu_mem_read_16(uintptr_t addr);

/**
   Memory space 32 bits write

   @param addr write address
   @param data value
*/

static void cpu_mem_write_32(uintptr_t addr, uint32_t data);

/**
   Memory space 32 bits read

   @param addr read address
   @return data value
*/

static uint32_t cpu_mem_read_32(uintptr_t addr);

#include "cpu/hexo/iospace.h"

static inline
uint32_t cpu_mem_mask_set_32(uintptr_t addr, uint32_t mask)
{
    uint32_t future = cpu_mem_read_32(addr) | mask;
    cpu_mem_write_32(addr, future);
    return future;
}

static inline
uint32_t cpu_mem_mask_clear_32(uintptr_t addr, uint32_t mask)
{
    uint32_t future = cpu_mem_read_32(addr) & ~mask;
    cpu_mem_write_32(addr, future);
    return future;
}

#endif

