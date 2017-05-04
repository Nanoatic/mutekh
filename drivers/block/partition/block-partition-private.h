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

#ifndef BLOCK_PARTITION_PRIVATE_H_
#define BLOCK_PARTITION_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>

struct block_partition_context_s
{
  dev_block_lba_t	first;
  dev_block_lba_t	size;
  struct dev_block_params_s drv_params;
};

struct partition_table_entry_s
{
  uint8_t			active;
  uint8_t			first_head;
  uint16_t			first_cyl;
  uint8_t			type;
  uint8_t			last_head;
  uint16_t			last_cyl;
  uint32_t			offset;
  uint32_t			size;
} __attribute__ ((packed));

struct partition_table_s
{
  uint8_t				padding[446];
  struct partition_table_entry_s	part[4];
  uint16_t				signature;
} __attribute__ ((packed));

#define PARTITION_TYPE_EMPTY		0x00
#define PARTITION_TYPE_EXTENDED		0x05
#define PARTITION_TYPE_EXTENDED_LBA	0x0f


#endif

