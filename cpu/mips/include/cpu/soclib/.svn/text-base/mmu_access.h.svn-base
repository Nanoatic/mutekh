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

  Copyright Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2006

*/

#ifndef CPU_SOCLIB_MMU_ACCESS_H
#define CPU_SOCLIB_MMU_ACCESS_H

#include <hexo/cpu.h>


#define cpu_soclib_mmu_get_register(reg_id) \
  (uint32_t) cpu_mips_mfc2( (reg_id), 0 );

#define cpu_soclib_mmu_set_register(reg_id, value ) \
  cpu_mips_mtc2( (reg_id), 0, (value) );

#endif
