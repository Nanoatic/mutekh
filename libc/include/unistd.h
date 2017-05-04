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

#ifndef UNISTD_H_
#define UNISTD_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

#include <hexo/types.h>
#include <hexo/error.h>
#include <mutek/fileops.h>

#define PATH_MAX 512

typedef int_fast8_t fd_t;
typedef int16_t mode_t;

/* ************************************************** */

enum open_flags_e
  {
    O_RDONLY	= 0x01,
    O_WRONLY	= 0x02,
    O_RDWR	= 0x03,
    O_CREAT	= 0x10,
    O_TRUNC	= 0x20,
    O_APPEND	= 0x40,
  };

#define HAVE_CREAT
fd_t creat(const char *pathname, mode_t mode);

#define HAVE_OPEN
fd_t open(const char *pathname, enum open_flags_e flags, /* mode_t mode */...);

/* ************************************************** */

#define HAVE_LSEEK
off_t lseek(fd_t fildes, off_t offset, enum seek_whence_e whence);

/* ************************************************** */

typedef uint_fast8_t dev_t;
typedef uint_fast32_t ino_t;
typedef uint_fast8_t nlink_t;
typedef uint_fast16_t uid_t;
typedef uint_fast16_t gid_t;
typedef uint_fast16_t blksize_t;
typedef uint_fast32_t blkcnt_t;
typedef uint_fast32_t time_t;

#include <sys/stat.h>

#define HAVE_FSTAT
error_t fstat(fd_t fd, struct stat *buf);

#define HAVE_STAT
error_t stat(const char *path, struct stat *buf);

#define HAVE_LSTAT
error_t lstat(const char *path, struct stat *buf);

/* ************************************************** */

enum access_perm_e
  {
    R_OK,               /* Test for read permission.  */
    W_OK,               /* Test for write permission.  */
    X_OK,               /* Test for execute permission.  */
    F_OK,               /* Test for existence.  */
  };

#define HAVE_ACCESS
error_t access(const char *pathname, enum access_perm_e mode);

/* ************************************************** */

#define HAVE_READ
ssize_t read(fd_t fd, void *buf, size_t count);

#define HAVE_WRITE
ssize_t write(fd_t fd, const void *buf, size_t count);

#define HAVE_CLOSE
error_t close(fd_t fd);

#define HAVE_REMOVE
error_t remove(const char *pathname);

#define HAVE_MKDIR
error_t mkdir(const char *pathname, mode_t mode);

C_HEADER_END

#endif

