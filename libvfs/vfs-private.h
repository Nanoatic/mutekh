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

/**
   @file
   @module {Virtual File System}
   @short Core file system nodes and mounts types
 */

#ifndef _VFS_PRIVATE_H_
#define _VFS_PRIVATE_H_



static inline
bool_t vfs_node_is_dandling(struct vfs_node_s *node)
{
    return node->parent == NULL;
}

#if 0
struct vfs_node_s * vfs_node_new(void *storage, struct vfs_fs_s *fs,
                                 const char *fullname, size_t fullnamelen,
                                 struct fs_node_s *fs_node);
#endif

/**
   @this creates a new vfs node.

   @param fs associated file system instance
   @param type node type
   @param mangled_name Entry mangled name. Must be exactly
   #CONFIG_VFS_NAMELEN long.
   @return the new vfs node.

   @see vfs_name_mangle
 */
struct vfs_node_s *vfs_node_createnew(
    struct vfs_fs_s *fs,
    const char *mangled_name,
    struct fs_node_s *fs_node);

/**
   @this marks the node as being used. This updates the LRU list of
   nodes.

   @param node used node
 */
void vfs_node_use(struct vfs_node_s *node);

// CONTAINER_FUNC(vfs_dir_hash, HASHLIST, static inline, vfs_dir);

CONTAINER_KEY_TYPE(vfs_dir_hash, PTR, BLOB, name, CONFIG_VFS_NAMELEN);

CONTAINER_KEY_FUNC(vfs_dir_hash, HASHLIST, static inline, vfs_dir, name);
CONTAINER_FUNC    (vfs_dir_hash, HASHLIST, static inline, vfs_dir, name);

static inline void vfs_node_dirlock(struct vfs_node_s *node)
{
    semaphore_take(&node->dir_semaphore, 1);
}

static inline void vfs_node_dirunlock(struct vfs_node_s *node)
{
    semaphore_give(&node->dir_semaphore, 1);
}

static inline bool_t vfs_node_dirtrylock(struct vfs_node_s *node)
{
    return semaphore_try_take(&node->dir_semaphore, 1);
}

void vfs_node_parent_nolock_unset(struct vfs_node_s *node);

void vfs_node_lru_rehash(struct vfs_node_s *node);

#endif
