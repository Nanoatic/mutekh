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

#ifndef _DEVFS_H_
#define _DEVFS_H_

#include <vfs/fs.h>

/**
   @this creates a new DEVFS file system instance.

   @param fs New devfs instance (return value)
   @return 0 on successful DEVFS creation
 */
error_t devfs_open(struct vfs_fs_s **fs);

/**
   @this closes an existing DEVFS instance.

   File system must already be unmounted and all files must be closed
   prior to this operation.

   @param fs DEVFS to close.

   @return 0 on successful close
 */
error_t devfs_close(struct vfs_fs_s *fs);

VFS_FS_LOOKUP(devfs_lookup);

VFS_FS_STAT(devfs_stat);



#endif
