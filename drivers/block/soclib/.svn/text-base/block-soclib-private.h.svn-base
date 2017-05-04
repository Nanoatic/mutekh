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

#ifndef BLOCK_SOCLIB_PRIVATE_H_
#define BLOCK_SOCLIB_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>

#define BLOCK_SOCLIB_MAX_RQ_COUNT 64

struct block_soclib_context_s
{
  struct dev_block_params_s params;
  dev_blk_queue_root_t	queue;
  uint8_t *data_ptr[BLOCK_SOCLIB_MAX_RQ_COUNT];
  uint8_t *buffer;
};

struct block_soclib_rq_s
{
  uint32_t rq_code;
};

#define BLOCK_SOCLIB_BUFFER 0
#define BLOCK_SOCLIB_LBA 4
#define BLOCK_SOCLIB_COUNT 8
#define BLOCK_SOCLIB_OP 12
#define BLOCK_SOCLIB_STATUS 16
#define BLOCK_SOCLIB_IRQ_ENABLE 20
#define BLOCK_SOCLIB_SIZE 24
#define BLOCK_SOCLIB_BLOCK_SIZE 28

#define BLOCK_SOCLIB_OP_NOOP 0
#define BLOCK_SOCLIB_OP_READ 1
#define BLOCK_SOCLIB_OP_WRITE 2

#define BLOCK_SOCLIB_STATUS_IDLE 0
#define BLOCK_SOCLIB_STATUS_BUSY 1
#define BLOCK_SOCLIB_STATUS_READ_SUCCESS 2
#define BLOCK_SOCLIB_STATUS_WRITE_SUCCESS 3
#define BLOCK_SOCLIB_STATUS_READ_ERROR 4
#define BLOCK_SOCLIB_STATUS_WRITE_ERROR 5
#define BLOCK_SOCLIB_STATUS_ERROR 6

#endif
