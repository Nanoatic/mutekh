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

#define FDT_INTERNAL
#include <fdt/writer.h>

#include <mutek/printk.h>

#include "fdt_internals.h"

#include <string.h>

struct fdt_writer_s
{
	struct fdt_header_s *header;
	struct fdt_mem_reserve_map_s *mem_reserve_ptr;
	uint32_t *struct_begin;
	uint32_t *struct_ptr;
	char *rst;
	char *end;
	error_t err;
};

error_t fdt_writer_init(
	struct fdt_writer_s *writer,
	void *blob,
	size_t available_size)
{
	writer->header = blob;
	writer->mem_reserve_ptr = (void*)(writer->header+1);
	writer->struct_begin
		= writer->struct_ptr
		= (void*)(writer->mem_reserve_ptr+1);
	writer->end = (void*)((uintptr_t)blob + available_size - 1);
	*writer->end = 0;
	writer->rst = writer->end;
	writer->err = 0;

	return 0;
}


void fdt_writer_add_rsvmap(
	struct fdt_writer_s *writer,
	uint64_t addr,
	uint64_t size)
{
	writer->mem_reserve_ptr->addr = endian_be64(addr);
	writer->mem_reserve_ptr->size = endian_be64(size);
	writer->mem_reserve_ptr++;
	writer->struct_begin
		= writer->struct_ptr
		= (void*)(writer->mem_reserve_ptr+1);
}


uint32_t fdt_writer_node_entry(
	struct fdt_writer_s *writer,
	const char *name)
{
	if ( writer->err || (void*)writer->struct_ptr > (void*)writer->rst ) {
		writer->err = -ENOMEM;
		return 0;
	}

	size_t len = strlen(name);

	size_t offset = 4 * (writer->struct_ptr - writer->struct_begin);
	*writer->struct_ptr++ = endian_be32(FDT_NODE_START);
	memcpy(writer->struct_ptr, name, len);
	writer->struct_ptr += (len+4) >> 2;

	return offset;
}

static
uint32_t fdt_writer_push_string(
	struct fdt_writer_s *writer,
	const char *str)
{
	const char *lookup = writer->rst;
	size_t len = strlen(str);

	while (lookup < writer->end) {
		if ( !strcmp(lookup, str) )
			return writer->end - lookup - len - 1;
		lookup += strlen(lookup)+1;
	}

	writer->rst -= len+1;
	memcpy(writer->rst, str, len+1);
	return writer->end - writer->rst - len - 1;
}

void fdt_writer_node_prop(
	struct fdt_writer_s *writer,
	const char *name,
	void *data,
	size_t len)
{
	if ( writer->err || (void*)writer->struct_ptr + 8 + len > (void*)writer->rst ) {
		writer->err = -ENOMEM;
		return;
	}

	*writer->struct_ptr++ = endian_be32(FDT_PROP);
	*writer->struct_ptr++ = endian_be32(len);
	*writer->struct_ptr++ = endian_be32(fdt_writer_push_string(writer, name));
	memcpy(writer->struct_ptr, data, len);
	writer->struct_ptr += (len+3) >> 2;
}


void fdt_writer_node_leave(struct fdt_writer_s *writer)
{
	if ( writer->err || (void*)writer->struct_ptr > (void*)writer->rst ) {
		writer->err = -ENOMEM;
		return;
	}
	*writer->struct_ptr++ = endian_be32(FDT_NODE_END);
}


error_t fdt_writer_finalize(struct fdt_writer_s *writer, size_t *real_size)
{
	if ( writer->err )
		return writer->err;

	*writer->struct_ptr++ = endian_be32(FDT_END);

	char *string_table = (void*)writer->struct_ptr;
	const char *string_orig = writer->rst;
	while ( string_orig < writer->end ) {
		size_t len = strlen(string_orig);
		size_t offset = writer->end - string_orig - len - 1;
		memcpy(string_table + offset, string_orig, len + 1);
		string_orig += len + 1;
	}

	*real_size = (uintptr_t)(string_table + (writer->end - writer->rst)) - (uintptr_t)writer->header;

	writer->header->magic = endian_be32(FDT_MAGIC);
	writer->header->totalsize = endian_be32(*real_size);
	writer->header->off_dt_struct = endian_be32(
		(uintptr_t)(writer->struct_begin) - (uintptr_t)writer->header);
	writer->header->off_dt_strings = endian_be32(
		(uintptr_t)(string_table) - (uintptr_t)writer->header);
	writer->header->off_mem_rsvmap = endian_be32(
		sizeof(*writer->header));
	writer->header->version = endian_be32(
		17);
	writer->header->last_comp_version = endian_be32(
		16);
	writer->header->boot_cpuid_phys = endian_be32(
		0);
	writer->header->size_dt_strings = endian_be32(
		writer->end - writer->rst);
	writer->header->size_dt_struct = endian_be32(
		4 * (writer->struct_ptr - writer->struct_begin));

	/* Clear last entry of memory reservation map */
	memset((char*)writer->struct_begin - 8, 0, 8);

	return 0;
}

