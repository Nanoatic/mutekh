/*
  This file is part of MutekH.

  MutekH is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; version 2.1 of the
  License.
    
  MutekH is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
    
  You should have received a copy of the GNU Lesser General Public
  License along with MutekH; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA

  Copyright Nicolas Pouillon, <nipo@ssji.net>, 2009
*/

#ifndef _RAMFS_H_
#define _RAMFS_H_

#include <vfs/fs.h>

/**
   @this creates a new RAMFS file system instance.

   @param fs New ramfs instance (return value)
   @return 0 on successful RAMFS creation
 */
error_t ramfs_open(struct vfs_fs_s **fs);

/**
   @this closes an existing RAMFS instance.

   File system must already be unmounted and all files must be closed
   prior to this operation.

   @param fs RAMFS to close.

   @return 0 on successful close
 */
error_t ramfs_close(struct vfs_fs_s *fs);

VFS_FS_LOOKUP(ramfs_lookup);

VFS_FS_CREATE(ramfs_create);

VFS_FS_LINK(ramfs_link);

VFS_FS_UNLINK(ramfs_unlink);

VFS_FS_STAT(ramfs_stat);



#endif
