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

#include <hexo/cpu.h>
#include <hexo/endian.h>

#if defined(CONFIG_CPU_CACHE)

void cpu_dcache_invld_buf(void *ptr, size_t size)
{
  size_t ls = cpu_dcache_line_size();

  if (ls != 0)
    {
      uint8_t *ptr_;

      for (ptr_ = ALIGN_ADDRESS_LOW(ptr, ls);
	   ptr_ < (uint8_t*)ptr + size;
	   ptr_ += ls)
	cpu_dcache_invld(ptr_);
    }
}

#endif
