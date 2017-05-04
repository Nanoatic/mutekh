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

#ifndef FILEOPS_H_
#define FILEOPS_H_

/**
 * @file
 * @module{C library}
 */

#include <hexo/types.h>
#include <hexo/error.h>

enum seek_whence_e
  {
    SEEK_SET, SEEK_END, SEEK_CUR
  };

/** File operations read function prototype */
#define FILEOPS_READ(n)  ssize_t (n) (void *file, uint8_t *buffer, size_t count)
/** File operations write function prototype */
#define FILEOPS_WRITE(n) ssize_t (n) (void *file, const uint8_t *buffer, size_t count)
/** File operations lseek function prototype */
#define FILEOPS_LSEEK(n) error_t (n) (void *file, size_t offset, enum seek_whence_e whence)
/** File operations close function prototype */
#define FILEOPS_CLOSE(n) error_t (n) (void *file)

typedef FILEOPS_READ(fileops_read_t);
typedef FILEOPS_WRITE(fileops_write_t);
typedef FILEOPS_LSEEK(fileops_lseek_t);
typedef FILEOPS_CLOSE(fileops_close_t);

struct fileops_s
{
  fileops_read_t *read;
  fileops_write_t *write;
  fileops_lseek_t *lseek;
  fileops_close_t *close;
};

#endif

