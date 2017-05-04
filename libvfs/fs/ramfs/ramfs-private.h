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

#ifndef _RAMFS_PRIVATE_H_
#define _RAMFS_PRIVATE_H_

#include <hexo/types.h>
#define GPCT_CONFIG_NODEPRECATED
#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/object_refcount.h>

#include <vfs/types.h>
#include <vfs/file.h>

VFS_FS_NODE_OPEN(ramfs_node_open);

OBJECT_TYPE     (ramfs_node, REFCOUNT, struct fs_node_s);

#define CONTAINER_LOCK_ramfs_dir_hash HEXO_SPIN

CONTAINER_TYPE    (ramfs_dir_hash, HASHLIST,
struct fs_node_s
{
    ramfs_node_entry_t obj_entry;
    char name[CONFIG_VFS_NAMELEN];
	CONTAINER_ENTRY_TYPE(HASHLIST) hash_entry;
    enum vfs_node_type_e type;
    struct fs_node_s *parent;
    union {
        struct ramfs_data_s *data;
        ramfs_dir_hash_root_t children;
    };
}, hash_entry, 5);

#define CONTAINER_OBJ_ramfs_dir_hash ramfs_node

CONTAINER_KEY_TYPE(ramfs_dir_hash, PTR, BLOB, name, CONFIG_VFS_NAMELEN);
//CONTAINER_PROTOTYPE(ramfs_dir_hash, HASHLIST, static inline);

OBJECT_CONSTRUCTOR(ramfs_node);
OBJECT_DESTRUCTOR(ramfs_node);

OBJECT_PROTOTYPE         (ramfs_node, static inline, ramfs_node);
OBJECT_FUNC              (ramfs_node, REFCOUNT, static inline, ramfs_node, obj_entry);

struct fs_node_s;

bool_t ramfs_dir_get_nth(struct fs_node_s *node, struct vfs_dirent_s *dirent, size_t n);

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

