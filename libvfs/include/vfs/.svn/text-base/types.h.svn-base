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

#ifndef _VFS_TYPES_H_
#define _VFS_TYPES_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <vfs/defs.h>
#include <vfs/fs.h>

#include <mutek/semaphore.h>
#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>
#include <gpct/object_refcount.h>
#include <mutek/semaphore.h>

OBJECT_TYPE     (vfs_node, REFCOUNT, struct vfs_node_s);
OBJECT_PROTOTYPE(vfs_node, , vfs_node);

/** @see vfs_node_createnew */
OBJECT_CONSTRUCTOR(vfs_node);
OBJECT_DESTRUCTOR(vfs_node);

//#define CONTAINER_LOCK_vfs_dir_hash MUTEK_SEMAPHORE
#define CONTAINER_LOCK_vfs_lru HEXO_SPIN

CONTAINER_TYPE    (vfs_dir_hash, HASHLIST,
/**
   @this is a node in the VFS.
 */
struct vfs_node_s
{
    /** @internal */
    CONTAINER_ENTRY_TYPE(HASHLIST) hash_entry;
    /** @internal */
    CONTAINER_ENTRY_TYPE(CLIST)    lru_entry;
    /** @internal
        Object-management related */
    vfs_node_entry_t obj_entry;

    /** File system the node is in */
    struct vfs_fs_s *fs;

    /** Name of the node in its parent directory structure.  Anonymous
        (dandling) nodes should not have any name.  Any unused
        characters in the name should be filled with @tt '\0' */
    char name[CONFIG_VFS_NAMELEN];

    /** @internal
        Parent node.

        Root has its own pointer here, dandling nodes have NULL.

        Accesses to this value must be protected for atomicity with
        @tt parent_lock.

        Code external to VFS code MUST use @ref vfs_node_get_parent.
    */
    struct vfs_node_s *parent;
    /** @internal
        Lock protecting accesses to parent */
    lock_t parent_lock;

    /** @internal
        Whether this node is present in LRU */
    bool_t in_lru;

    /** @internal
        Private file system data attached to this node */
    struct fs_node_s *fs_node;

#if defined(CONFIG_VFS_STATS)
    /** @multiple
        Statistics counter
     */
    atomic_t lookup_count;
    atomic_t open_count;
    atomic_t close_count;
    atomic_t stat_count;
#endif

    /**
       @internal
       Children cache hash.
       
       Accesses to this value must be protected through @tt
       dir_semaphore.
    */
    vfs_dir_hash_root_t children;

    /** @internal
        Semaphore protecting @tt children */
    struct semaphore_s dir_semaphore;
}
, hash_entry, 5);

#define CONTAINER_OBJ_vfs_dir_hash vfs_node

CONTAINER_TYPE(vfs_lru, CLIST, struct vfs_node_s, lru_entry);

struct vfs_fs_ops_s;

OBJECT_TYPE     (vfs_fs, SIMPLE, struct vfs_fs_s);

/**
   @this is an opened filesystem state.
 */
struct vfs_fs_s
{
    /** LRU of @ref vfs_node_s used for this filesystem */
    vfs_lru_root_t lru_list;
    /** Root node of the filesystem. This is filled when opening the filesystem */
    struct fs_node_s *root;
    /** A pointer to supported operations table */
    const struct vfs_fs_ops_s *ops;
    /** A simple reference counter for exclusive internal use of FS implementation */
    atomic_t ref;
    /** Whether filesystem is read-only */
    uint8_t flag_ro:1;

    /** Object-management-related */
    vfs_fs_entry_t obj_entry;

    /**
       Mountpoint that was replaced with this filesystem's root on
       mount. NULL for non-mounted filesystems
    */
    struct vfs_node_s *old_node;

#if defined(CONFIG_VFS_STATS)
    /**
       @multiple
       Statistics counter
    */
    atomic_t node_open_count;

    atomic_t lookup_count;
    atomic_t create_count;
    atomic_t link_count;
    atomic_t move_count;
    atomic_t unlink_count;
    atomic_t stat_count;

    atomic_t node_create_count;
    atomic_t node_destroy_count;

    atomic_t file_open_count;
    atomic_t file_close_count;
#endif
};

OBJECT_PROTOTYPE(vfs_fs, , vfs_fs);

/** @see vfs_fs_new */
OBJECT_CONSTRUCTOR(vfs_fs);
OBJECT_DESTRUCTOR(vfs_fs);



/**
   @this is the vfs_node_stat() operation response buffer.
 */
struct vfs_stat_s
{
    /** File or directory */
    enum vfs_node_type_e type;

    /** File size in bytes, or directory entry count excluding "." and
        ".." */
    vfs_file_size_t size;

    /** Count of links to the data on disk */
    size_t nlink;

//  /** Creation timestamp */
//  time_t ctime;
//  /** Access timestamp */
//  time_t atime;
//  /** Modification timestamp */
//  time_t mtime;
//  /** Modes, ... */
//  vfs_node_attr_t attr;
//  /** User ID */
//  uid_t uid;
//  /** Group ID */
//  gid_t gid;
//  /** Device number */
//  dev_t dev;
};

C_HEADER_END

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

