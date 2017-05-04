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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2010

*/

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_array.h>

#include <unistd.h>
#include <mutek/fileops.h>
#include <mutek/console.h>

#ifdef CONFIG_VFS
#include <vfs/vfs.h>
#endif

struct fd_entry_s
{
  const struct fileops_s *ops;
  void *hndl;
};

CONTAINER_TYPE(fdarray, ARRAY, struct fd_entry_s, CONFIG_LIBC_MAX_FD)
CONTAINER_FUNC(fdarray, ARRAY, static, fdarray);

/* This removes a tedious warning... */
#define gpct_lock_CONTAINER_LOCK_fdarray_initializer {}

static fdarray_root_t fd_array = CONTAINER_ROOT_INITIALIZER(fdarray, ARRAY);

static fd_t fd_new(fdarray_root_t *fda)
{
  fdarray_index_t i;

  for (i = 0; i < fdarray_count(fda); i++)
    if (fdarray_getptr(fda, i)->hndl == NULL)
      return i;

  return fdarray_alloc(fda);
}

static struct fd_entry_s * fd_get(fdarray_root_t *fda, fd_t fd)
{
  if (fd > fdarray_count(fda))
    return NULL;

  return fdarray_getptr(fda, fd);
}

static void fd_free(fdarray_root_t *fda, struct fd_entry_s *e)
{
  e->hndl = NULL;
}

fd_t fd_add(const struct fileops_s *ops, void *hndl)
{
	fd_t fd = fd_new(&fd_array);
	struct fd_entry_s *e = fd_get(&fd_array, fd);

	/* NULL handler would confuse fd allocator */
	assert(hndl != NULL);

	e->ops = ops;
	e->hndl = hndl;

	return fd;
}

/* **********************************************************************
                  File descriptor oriented operations
   ********************************************************************** */

void stdio_in_out_err_init()
{
  fd_t fd;

  fd = fd_add(&console_file_ops, (void*)-1);
  assert(fd == 0);

  fd = fd_add(&console_file_ops, (void*)-1);
  assert(fd == 1);

  fd = fd_add(&console_file_ops, (void*)-1);
  assert(fd == 2);
}

#if defined(CONFIG_VFS)

inline fd_t creat(const char *pathname, mode_t mode)
{
    return open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

static enum open_flags_e flags_to_vfs(const enum vfs_open_flags_e mode)
{
  enum vfs_open_flags_e flags = 0;

  if (mode & O_RDONLY)
    flags |= VFS_OPEN_READ;
  if (mode & O_WRONLY)
    flags |= VFS_OPEN_WRITE;
  if (mode & O_CREAT)
    flags |= VFS_OPEN_CREATE;
  if (mode & O_APPEND)
    flags |= VFS_OPEN_APPEND;
  if ( flags & O_TRUNC )
    flags |= VFS_OPEN_TRUNCATE;

  return (flags);
}

fd_t open(const char *pathname, enum open_flags_e flags, ...)
{
  fd_t fd = fd_new(&fd_array);
  struct fd_entry_s *e;
  mode_t mode = 0;
  va_list ap;

  if (fd < 0)
    return fd;

  e = fd_get(&fd_array, fd);
  e->ops = &vfs_file_fops;

  if (flags & O_CREAT)
    {
      va_start(ap, flags);
      mode = va_arg(ap, __compiler_sint_t);
      va_end(ap);
    }

  struct vfs_file_s *hndl;
  if (vfs_open(vfs_get_root(), vfs_get_cwd(),
			   pathname, flags_to_vfs(flags), &hndl))
    {
      fd_free(&fd_array, e);
      return -1;
    }

  e->hndl = hndl;

  return fd;
}

#endif /* CONFIG_VFS */

off_t lseek(fd_t fd, off_t offset, enum seek_whence_e whence)
{
  struct fd_entry_s *e = fd_get(&fd_array, fd);

  return e && e->ops->lseek ? e->ops->lseek(e->hndl, offset, whence) : -1;
}

ssize_t read(fd_t fd, void *buf, size_t count)
{
  struct fd_entry_s *e = fd_get(&fd_array, fd);

  return e && e->ops->read ? e->ops->read(e->hndl, buf, count) : -1;
}

ssize_t write(fd_t fd, const void *buf, size_t count)
{
  struct fd_entry_s *e = fd_get(&fd_array, fd);

  return e && e->ops->write ? e->ops->write(e->hndl, buf, count) : -1;
}

error_t close(fd_t fd)
{
  struct fd_entry_s *e = fd_get(&fd_array, fd);

  if (e)
    {
      if (e->ops->close)
	{
	  e->ops->close(e->hndl);
	  fd_free(&fd_array, e);
	  return 0;
	}
    }
  return 1;
}

/* **********************************************************************
                  VFS operations
   ********************************************************************** */

#if defined(CONFIG_VFS)

error_t stat(const char *path, struct stat *st)
{
  struct vfs_stat_s vst;

  if (vfs_stat(vfs_get_root(), vfs_get_cwd(),
			   path, &vst))
    return -1;

  memset(st, 0, sizeof(*st));
  st->st_size = vst.size;

  switch (vst.type) {
  case VFS_NODE_DIR:
	  st->st_mode |= S_IFDIR;
	  break;
  case VFS_NODE_FILE:
	  st->st_mode |= S_IFREG;
	  break;
  }

  return 0;
}

error_t lstat(const char *path, struct stat *buf)
{
	return stat(path, buf);
}

error_t access(const char *pathname, enum access_perm_e mode)
{
	return 0;
}

error_t remove(const char *pathname)
{
    return vfs_unlink(vfs_get_root(), vfs_get_cwd(), pathname);
}

error_t mkdir(const char *pathname, mode_t mode)
{
    struct vfs_node_s *node = NULL;
    error_t err = vfs_create(vfs_get_root(), vfs_get_cwd(), pathname, VFS_NODE_DIR, &node);
    if ( err == 0 )
        vfs_node_refdrop(node);
    return err;
}

#endif /* CONFIG_VFS */

