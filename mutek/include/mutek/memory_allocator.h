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
 * @short Memory allocation stuff
 */


#ifndef MEMORY_ALLOCATOR_H_ 
#define MEMORY_ALLOCATOR_H_

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/lock.h>

struct memory_allocator_region_s;

extern struct memory_allocator_region_s *default_region;

/** @this initialize a memory region*/
struct memory_allocator_region_s *
memory_allocator_init(struct memory_allocator_region_s *container_region, void *start, void *end);

/** @this extend an existing memory region with a new memory space */
struct memory_allocator_header_s *
memory_allocator_extend(struct memory_allocator_region_s *region, void *start, size_t size);

/** @this resize the given memory block */
void *memory_allocator_resize(void *address, size_t size);

/** @this allocate a new memory block in given region */
void *memory_allocator_pop(struct memory_allocator_region_s *region, size_t size);

/** @this free allocated memory block */
void memory_allocator_push(void *address);

/** @this reserve a memory space in given region */
void *memory_allocator_reserve(struct memory_allocator_region_s *region, void *start, size_t size);

/** @this return the size of given memory block */
size_t memory_allocator_getsize(void *ptr);

/** @this return statistic of memory region use */
error_t memory_allocator_stats(struct memory_allocator_region_s *region,
			size_t *alloc_blocks,
			size_t *free_size,
			size_t *free_blocks);

/** @this make memory region check depending to activated token: guard zone, headers' integrity (crc and size) 
    and if free space was used */
void memory_allocator_region_check(struct memory_allocator_region_s *region);

#endif
