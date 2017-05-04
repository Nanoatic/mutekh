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
    Copyright Dimitri Refauvelet <dimitri.refauvelet@lip6.fr> (c) 2009
*/

/**
 * @file
 * @module{Mutek}
 * @short Memory region allocation stuff
 */

#ifndef MEM_REGION_H_
#define MEM_REGION_H_

#include <mutek/mem_alloc.h>
#include <mutek/memory_allocator.h>

#define CONTAINER_LOCK_region_list HEXO_SPIN_IRQ

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_slist.h>

enum mem_scope_e;

struct mem_region_s
{
  CONTAINER_ENTRY_TYPE(SLIST) list_entry;
  uint_fast16_t priority;
  struct memory_allocator_region_s *region;
};

/** @this init all the scope structure for the given cpu */ 
void mem_region_id_init(cpu_id_t cpu_id);

/** @this returns a farless memory allocatable region, depending to the scope */
struct mem_region_s *mem_region_get_first(enum mem_scope_e scope);

/** @this returns a farless memory allocatable region, depending to the scope */
struct mem_region_s *mem_region_id_get_first(cpu_id_t cpu_id, enum mem_scope_e scope);

/** @this returns a the next memory allocatable region, depending to the scope */
struct mem_region_s *mem_region_get_next(enum mem_scope_e scope, struct mem_region_s *region);

/** @this returns a the next memory allocatable region, depending to the scope */
struct mem_region_s *mem_region_id_get_next(cpu_id_t cpu_id, enum mem_scope_e scope, struct mem_region_s *region);

/** @this add the given region to the scope */
void mem_region_add(enum mem_scope_e scope, struct memory_allocator_region_s *region, uint_fast16_t priority);

/** @this add the given region to the scope of the specified cpu */
void mem_region_id_add(cpu_id_t cpu_id, enum mem_scope_e scope, struct memory_allocator_region_s *region, uint_fast16_t priority);

/** @this remove the given region from the scope */
void mem_region_remove(enum mem_scope_e scope, struct memory_allocator_region_s *region);

/** @this remove the given region from the scope of the specified cpu */
void mem_region_id_remove(cpu_id_t cpu_id, enum mem_scope_e scope, struct memory_allocator_region_s *region);

/** @this lock the given scope*/
void mem_region_lock(enum mem_scope_e scope);

/** @this lock the given scope of the specified cpu*/
void mem_region_id_lock(cpu_id_t cpu_id, enum mem_scope_e scope);

/** @this unlock the given scope*/
void mem_region_unlock(enum mem_scope_e scope);

/** @this unlock the given scope of the specified cpu*/
void mem_region_id_unlock(cpu_id_t cpu_id, enum mem_scope_e scope);

#endif
