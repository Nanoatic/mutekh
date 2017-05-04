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

#ifndef _DEVFS_PRIVATE_H_
#define _DEVFS_PRIVATE_H_

#include <hexo/types.h>

#define GPCT_CONFIG_NODEPRECATED
#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/object_refcount.h>

#include <vfs/types.h>
#include <vfs/file.h>

VFS_FS_NODE_OPEN(devfs_node_open);

OBJECT_TYPE     (devfs_node, REFCOUNT, struct fs_node_s);

enum devfs_node_type_e
{
    DEV_DEVICE_DIR,
    DEV_DEVICE_HANDLE,
};

struct fs_node_s
{
    devfs_node_entry_t obj_entry;
    enum devfs_node_type_e type;
    struct vfs_fs_s *fs;
    struct device_s *dev;
};

OBJECT_CONSTRUCTOR(devfs_node);
OBJECT_DESTRUCTOR(devfs_node);

#if __MKDOC__
struct fs_node_s *devfs_node_new(void *storage, struct vfs_fs_s *, enum devfs_node_type_e type, struct device_s *dev);
#endif

OBJECT_PROTOTYPE         (devfs_node, static inline, devfs_node);
OBJECT_FUNC              (devfs_node, REFCOUNT, static inline, devfs_node, obj_entry);

bool_t devfs_dir_get_nth(struct fs_node_s *node, struct vfs_dirent_s *dirent, size_t n);

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

