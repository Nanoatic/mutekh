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

#ifndef _FAT_H_
#define _FAT_H_

#include <hexo/types.h>
#include <vfs/fs.h>

/**
   @this creates a new FAT16 file system instance.

   @param dev A block device to open
   @param fs New fat16 instance (return value)
   @return 0 on successful FAT16 creation
 */
error_t fat16_open(struct device_s *dev, struct vfs_fs_s **fs);

/**
   @this closes an existing FAT16 instance.

   File system must already be unmounted and all files must be closed
   prior to this operation.

   @param fs FAT16 to close.

   @return 0 on successful close
 */
error_t fat16_close(struct vfs_fs_s *fs);

VFS_FS_CAN_UNMOUNT(fat16_can_unmount);
VFS_FS_NODE_REFDROP(fat16_node_refdrop);
VFS_FS_NODE_REFNEW(fat16_node_refnew);


VFS_FS_NODE_OPEN(fat_node_open);
VFS_FS_LOOKUP(fat_lookup);
VFS_FS_CREATE(fat_create);
VFS_FS_MOVE(fat_move);
VFS_FS_LINK(fat_link);
VFS_FS_UNLINK(fat_unlink);
VFS_FS_STAT(fat_stat);

#endif
