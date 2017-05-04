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

#include <hexo/interrupt.h>
#include <hexo/init.h>

/** pointer to context local storage in cpu local storage */
CPU_LOCAL void *__context_data_base;

error_t
cpu_global_init(void)
{
  return 0;
}

struct cpu_cld_s *cpu_init(uint_fast8_t cpu_id)
{
  return NULL;
}

uint_fast8_t cpu_id(void)
{
  return 0;
}

