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

    Copyright (c) 2009, Nicolas Pouillon <nipo@ssji.net>
*/

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/endian.h>

#include <fdt/reader.h>

#include <assert.h>

#include <mutek/printk.h>


#include "fdt_internals.h"


#include <string.h>

struct fdt_walker_state_s
{
	const void *blob;
	const char *string_table;
	const uint32_t *struct_base;
	const uint32_t *ptr;
};

const uint32_t *fdt_skip_str(const uint32_t *ptr)
{
	uint32_t data;
	do {
		data = *ptr++;
	} while (((data - 0x01010101) & 0x80808080) == 0);
	return ptr;
}

/**
 * Walks a node, start token should already be eaten. Will eat the
 * stop token.
 */
error_t fdt_walk_node(struct fdt_walker_state_s *state, struct fdt_walker_s *walker)
{
	size_t level = 0;
	size_t max_level = 512;
	char full_path[32] = "";

	for (;;) {
		uint32_t token = endian_be32(*state->ptr++);
		switch ( token ) {
		case FDT_NODE_START: {
			level++;

//			printk("start level: %d, max_level: %d, %s\n", level, max_level, state->ptr);
		
			strncat(full_path, "/", 32);
			strncat(full_path, (const char*)state->ptr, 32);

			if ( level < max_level ) {
				bool_t wanted = walker->on_node_entry(
					walker->priv,
					state,
					full_path+1);
				if ( !wanted )
					max_level = level;
			}
			state->ptr = fdt_skip_str(state->ptr);
			break;
		}
		case FDT_PROP: {
			struct fdt_prop_s *prop = (struct fdt_prop_s*)state->ptr;

//			printk("  prop %s %P\n",
/* 				   &state->string_table[endian_be32(prop->strid)], */
/* 				   prop->data, */
/* 				   endian_be32(prop->size)); */

			if ( level < max_level )
				walker->on_node_prop(
					walker->priv,
					state,
					&state->string_table[endian_be32(prop->strid)],
					prop->data,
					endian_be32(prop->size));
			state->ptr += (8 + endian_be32(prop->size) + 3) / 4;
			break;
		}
		case FDT_NODE_END:
//			printk("end level: %d, max_level: %d, %s\n", level, max_level, full_path);

			if ( level < max_level - 1 )
				walker->on_node_leave(walker->priv);
			
			char *end = strrchr(full_path, '/');
			assert(end);
			*(end) = 0;
			if ( max_level == level-- ) {
//				printk("Reverting cloak\n");
				max_level = 512;
			}

//			printk(" post end level: %d, max_level: %d\n", level, max_level);

			if ( level == 0 )
				return 0;
			break;
		case FDT_NOP:
			break;
		case FDT_END:
			return 0;
		default:
			printk("Unhandled FDT Token: %x @ %p\n", token, (void*)state->ptr-state->blob);
			return -EINVAL;
		}
	}
}

static error_t fdt_check_header(const void *blob)
{
	const struct fdt_header_s *header = blob;

    if ( blob == NULL ) {
		printk("Null blob\n");
		return -EINVAL;
	}

	if ( (uintptr_t)blob & 3 ) {
		printk("Unaligned FDT: %p\n", blob);
		return -EINVAL;
	}

	if ( endian_be32(header->magic) != FDT_MAGIC ) {
		printk("FDT bad magic, expected %x, got %x\n",
			   FDT_MAGIC, endian_be32(header->magic));
		return -EINVAL;
	}

	return 0;
}

error_t fdt_walk_blob(const void *blob, struct fdt_walker_s *walker)
{
	struct fdt_walker_state_s state = {
		.blob = blob,
		.ptr = blob,
	};

	const struct fdt_header_s *header = blob;
	error_t err;

	if ( (err = fdt_check_header(blob)) )
		return err;

/* 	printk("FDT magic OK, string table @ %p\n", endian_be32(header->off_dt_strings)); */
/* 	printk("FDT magic OK, struct table @ %p\n", endian_be32(header->off_dt_struct)); */

	state.string_table = (const char*)blob + endian_be32(header->off_dt_strings);
	state.struct_base
		= state.ptr
		= (void*)((uintptr_t)blob + endian_be32(header->off_dt_struct));

	struct fdt_mem_reserve_map_s *reserve_map =
		(void*)((uintptr_t)blob + endian_be32(header->off_mem_rsvmap));
	while ( reserve_map->addr || reserve_map->size ) {
		walker->on_mem_reserve(walker->priv,
							   endian_be64(reserve_map->addr),
							   endian_be64(reserve_map->size));
		reserve_map++;
	}

	if ( endian_be32(*state.ptr) != FDT_NODE_START ) {
		printk("FDT bad token, expected %x, got %x\n",
			   FDT_NODE_START, endian_be32(*state.ptr));
		return -EINVAL;
	}

	return fdt_walk_node(&state, walker);
}

uint32_t fdt_reader_get_struct_offset(struct fdt_walker_state_s *state)
{
	return (uintptr_t)state->ptr - (uintptr_t)state->struct_base - 4;
}

bool_t fdt_reader_has_prop(const struct fdt_walker_state_s *state,
						   const char *propname,
						   const void **propval, size_t *propsize)
{
	const uint32_t *ptr = fdt_skip_str(state->ptr);

	for (;;) {
		uint32_t token = endian_be32(*(ptr++));
		switch (token) {
		case FDT_PROP: {
			struct fdt_prop_s *prop = (struct fdt_prop_s*)ptr;
			const char *pname = &state->string_table[
				endian_be32(prop->strid)];
			if ( ! strcmp(propname, pname) ) {
				*propval = prop->data;
				*propsize = endian_be32(prop->size);
				return 1;
			}
			ptr += (8 + endian_be32(prop->size) + 3) / 4;
			break;
		}
		case FDT_END:
		case FDT_NODE_END:
		case FDT_NODE_START:
			return 0;
		case FDT_NOP:
		default:
			break;
		}
	}
}

error_t fdt_walk_blob_from(const void *blob, struct fdt_walker_s *walker, uint32_t offset)
{
	struct fdt_walker_state_s state = {
		.blob = blob,
		.ptr = blob,
	};

	const struct fdt_header_s *header = blob;
	error_t err;

	if ( (err = fdt_check_header(blob)) )
		return err;

	state.string_table = (const char*)blob + endian_be32(header->off_dt_strings);
	state.struct_base
		= (void*)((uintptr_t)blob + endian_be32(header->off_dt_struct));

	state.ptr = state.struct_base + offset/4;

	if ( endian_be32(*state.ptr) != FDT_NODE_START )
		return -EINVAL;

	return fdt_walk_node(&state, walker);
}

error_t fdt_get_prop_at(const void *blob, uint32_t offset,
						const char *propname,
						const void **data, size_t *datasize)
{
	const struct fdt_header_s *header = blob;
	const uint32_t *ptr = (void*)(
		(uintptr_t)blob + offset + endian_be32(header->off_dt_struct));
	const char *string_table = (const char*)blob + endian_be32(header->off_dt_strings);

	ptr = fdt_skip_str(ptr);

	for (;;) {
		uint32_t token = endian_be32(*(ptr++));
		switch (token) {
		case FDT_PROP: {
			struct fdt_prop_s *prop = (struct fdt_prop_s*)ptr;
			const char *pname = &string_table[
				endian_be32(prop->strid)];
			if ( ! strcmp(propname, pname) ) {
				*data = prop->data;
				*datasize = endian_be32(prop->size);
				return 1;
			}
			ptr += (8 + endian_be32(prop->size) + 3) / 4;
			break;
		}
		case FDT_END:
		case FDT_NODE_END:
		case FDT_NODE_START:
			return 0;
		case FDT_NOP:
		default:
			break;
		}
	}
}

void fdt_get_rsvmap(const void *blob, uint32_t resno,
					uint64_t *addr, uint64_t *size)
{
	const struct fdt_header_s *header = blob;
	const uint32_t *reserve_map =
		(const void*)((uintptr_t)blob + endian_be32(header->off_mem_rsvmap));

	reserve_map += resno*4;

	*addr = ((uint64_t)endian_be32(reserve_map[0]) << 32)
	  | endian_be32(reserve_map[1]);
	*size = ((uint64_t)endian_be32(reserve_map[2]) << 32)
	  | endian_be32(reserve_map[3]);
}

size_t fdt_get_size(void *blob)
{
	const struct fdt_header_s *header = blob;

	if ( fdt_check_header(blob) != 0 )
		return 0;
	
	return endian_be32(header->totalsize);
}
