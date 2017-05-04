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
   @file

   CPU dependant IO spaces access functions.
 */

#if !defined(IOSPACE_H_) || defined(CPU_IOSPACE_H_)
#error This file can not be included directly
#else

#define CPU_IOSPACE_H_

static inline void
cpu_mem_write_8(uintptr_t addr, uint8_t data)
{
  volatile uint8_t	*ptr = (uint8_t*)addr;

  *ptr = data;
}

static inline uint8_t
cpu_mem_read_8(uintptr_t addr)
{
  volatile uint8_t	*ptr = (uint8_t*)addr;

  return *ptr;
}

static inline void
cpu_mem_write_16(uintptr_t addr, uint16_t data)
{
  volatile uint16_t	*ptr = (uint16_t*)addr;

  *ptr = data;
}

static inline uint16_t
cpu_mem_read_16(uintptr_t addr)
{
  volatile uint16_t	*ptr = (uint16_t*)addr;

  return *ptr;
}

static inline void
cpu_mem_write_32(uintptr_t addr, uint32_t data)
{
  volatile uint32_t	*ptr = (uint32_t*)addr;

  *ptr = data;
}

static inline uint32_t
cpu_mem_read_32(uintptr_t addr)
{
  volatile uint32_t	*ptr = (uint32_t*)addr;

  return *ptr;
}

/****************************************************/

static inline void
cpu_io_write_8(uintptr_t addr, uint8_t data)
{
  cpu_mem_write_8(addr, data);
}

static inline uint8_t
cpu_io_read_8(uintptr_t addr)
{
  return cpu_mem_read_8(addr);
}

static inline void
cpu_io_write_16(uintptr_t addr, uint16_t data)
{
  cpu_mem_write_16(addr, data);
}

static inline uint16_t
cpu_io_read_16(uintptr_t addr)
{
  return cpu_mem_read_16(addr);
}

static inline void
cpu_io_write_32(uintptr_t addr, uint32_t data)
{
  cpu_mem_write_32(addr, data);
}

static inline uint32_t
cpu_io_read_32(uintptr_t addr)
{
  return cpu_mem_read_32(addr);
}


#endif

