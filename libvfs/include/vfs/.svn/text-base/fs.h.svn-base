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
   @short Filesystem driver operations interface

   Filesystems are drivers to the low-level aspects of a given
   filesystem. They basically manipulate 3 structures:
   @list

   @item @ref vfs_fs_s: A filesystem state. It is unique for an open
   filesystem. This structure is used to mount a filesystem in
   another (with @ref vfs_mount).

   @item @ref fs_node_s: A filesystem-level node. It represents an
   actual node in a filesystem. This type is
   filesystem-implementation dependant, VFS sees it as an abstract
   type. Is is also refcounted through @tt node_refnew and @tt
   node_refdrop function pointers of @ref vfs_fs_ops_s.

   @item @ref vfs_file_s: An open file descriptor. It is created by
   @tt node_open (in @ref vfs_fs_ops_s) and must be closed on finish.

   @end list

   Filesystems may be read-only, or even read-write only at the
   file-level. This means operations like @tt lookup, @tt move, @tt
   link and others are not possible. If so, corresponding entries in
   @ref vfs_fs_ops_s may be NULL.
 */

#ifndef _VFS_FS_H_
#define _VFS_FS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/types.h>
#include <hexo/atomic.h>
#include <hexo/error.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_dlist.h>
#include <gpct/object_simple.h>

/** @hidden */
struct vfs_fs_s;

/** @hidden */
enum vfs_node_type_e;
/** @hidden */
enum vfs_open_flags_e;

/** @hidden */
struct fs_node_s;
/** @hidden */
struct vfs_file_s;

/** @hidden */
struct vfs_stat_s;

/** @this defines the fs unmountable test prototype */
#define VFS_FS_CAN_UNMOUNT(x) bool_t (x)(struct vfs_fs_s *fs)

/**
   This function asks the file system whether its current internal state
   allows it to be unmounted.  This function should take opened files,
   directories and anonymous nodes into account.

   This function does not have to ensure the filesystem will stay unmountable.
   Ensuring nobody takes new references on the file system being
   unmounted is VFS's job.

   @param fs Fs state to probe for unmounting
   @return 1 if file system can be unmounted, else 0

   @csee #VFS_FS_CAN_UNMOUNT
 */
typedef VFS_FS_CAN_UNMOUNT(vfs_fs_can_unmount_t);

/** @this defines the fs node open operation prototype */
#define VFS_FS_NODE_OPEN(x) error_t (x)(struct fs_node_s *node,	\
										enum vfs_open_flags_e flags,			\
										struct vfs_file_s **file)

/**
   This function opens a node for read/write operations.  

   @param node Node to open. It may be an anonymous node (not present
   in any directory)
   @param flags Mode to open the file in
   @param file Returned file descriptor on success
   @return 0 on success, or an error code

   Some checks are already performed on @tt and node type, see @ref
   vfs_node_open for details. This function transfers the ownership
   of @tt *file to the caller.

   @csee #VFS_FS_NODE_OPEN
   @see vfs_node_open @see vfs_open
 */
typedef VFS_FS_NODE_OPEN(vfs_fs_node_open_t);


/** @this defines the fs lookup operation prototype */
#define VFS_FS_LOOKUP(x) error_t (x)(struct fs_node_s *ref,             \
                                     const char *name,                  \
                                     size_t namelen,					\
                                     struct fs_node_s **node,           \
                                     char *mangled_name)

/**
   This function searches for a given name in a directory node. This function is only
   valid for directory nodes.

   This function must not create new nodes in the actual file system.

   @param ref Reference directory node
   @param name Name to lookup for, it may not end with @tt '\0'.
   @param namelen Length of name, excluding any @tt '\0'
   @param node Returned node if found.  File system must not insert
   the @tt node in the @tt ref's children hash
   @param mangled_name Mangled name of returned node. The buffer is
   #CONFIG_VFS_NAMELEN long.
   @return 0 on success, or an error code

   This function transfers the ownership of @tt *node to the caller

   @csee #VFS_FS_LOOKUP
   @see vfs_lookup @see vfs_node_lookup
 */
typedef VFS_FS_LOOKUP(vfs_fs_lookup_t);


/** @this defines the fs create operation prototype */
#define VFS_FS_CREATE(x) error_t (x)(struct vfs_fs_s *fs,	\
									   enum vfs_node_type_e type,		\
									   struct fs_node_s **node)

/**
   This function creates a new anonymous node in a given file system.

   @param fs The fs state
   @param type Node type
   @param node Returned created node
   @return 0 on success, or an error code

   This function transfers the ownership of @tt *node to the caller

   @csee #VFS_FS_CREATE
   @see vfs_node_create @see vfs_create
 */
typedef VFS_FS_CREATE(vfs_fs_create_t);

/** @this defines the fs link operation prototype */
#define VFS_FS_LINK(x) error_t (x)(struct fs_node_s *node,             \
                                   struct fs_node_s *parent,           \
                                   const char *name,				   \
                                   size_t namelen,                     \
                                   struct fs_node_s **rnode,           \
                                   char *mangled_name)

/**
   This function links a node in a parent directory node.  Filesystem may not
   support linking operation on all node types, or may not support
   linking at all.  The only operation expected to succeed everywhere
   is linking an anonymous node created with @ref vfs_fs_create_t.

   As nodes must be unique in the VFS, the node present at destination
   point on link after the operation (@tt{rnode}) may be different from
   the passed node (@tt{node}).

   @param node Node to attach in @tt parent
   @param parent Parent directory node to attach @tt node in
   @param name Name to lookup for, it may not end with @tt '\0'.
   @param namelen Length of name, excluding any @tt '\0'
   @param rnode Actually attached node, may be @tt node or another new
   node.
   @param mangled_name Mangled name of returned node. The buffer is
   #CONFIG_VFS_NAMELEN long.
   @return 0 on success, or an error code

   This function transfers the ownership of @tt *rnode to the caller, even if
   @tt *rnode is actually @tt node

   @csee #VFS_FS_LINK
   @see vfs_node_link
 */
typedef VFS_FS_LINK(vfs_fs_link_t);


/** @this defines the fs move operation prototype */
#define VFS_FS_MOVE(x) error_t (x)(struct fs_node_s *node,             \
                                   struct fs_node_s *parent,           \
                                   const char *name,				   \
                                   size_t namelen)

/**
   This function moves a node in another parent directory node.

   @param node Node to attach in @tt parent
   @param parent Parent directory to attach @tt node in
   @param name Name to set for new node name, it may not end with @tt '\0'.
   @param namelen Length of name, excluding any @tt '\0'
   @return 0 on success, or an error code

   @csee #VFS_FS_MOVE
   @see vfs_node_move
 */
typedef VFS_FS_MOVE(vfs_fs_move_t);


/** @this defines the fs unlink operation prototype */
#define VFS_FS_UNLINK(x) error_t (x)(struct fs_node_s *parent,  \
									   const char *name,			   \
									   size_t namelen)

/**
   This function removes a node from the file system.  Node may still be
   referenced or open, and may become a dandling anonymous file.

   @param parent Directory node where to unlink a child
   @param name Name of child to unlink, it may not end with @tt '\0'.
   @param namelen Length of name, excluding any @tt '\0'
   @return 0 on success, or an error code

   @csee #VFS_FS_UNLINK
   @see vfs_node_unlink @see vfs_unlink
 */
typedef VFS_FS_UNLINK(vfs_fs_unlink_t);



/** @this defines the fs stat operation prototype */
#define VFS_FS_STAT(x) error_t (x)(struct fs_node_s *node,  \
								   struct vfs_stat_s *stat)

/**
   This function retrieves informations about a given node.  This node may be
   a file or a directory.

   @param node Node to retrieve information about
   @param stat Stat buffer to store information into
   @return 0 on success, or an error code

   @csee #VFS_FS_STAT
   @see vfs_node_stat @see vfs_stat
 */
typedef VFS_FS_STAT(vfs_fs_stat_t);



/** @this defines the fs node refnew operation prototype */
#define VFS_FS_NODE_REFNEW(x) struct fs_node_s * (x)(struct fs_node_s *node)

/**
   This function notifies the file system a node is not referenced any
   more.

   @param node Node not used any more

   @csee #VFS_FS_NODE_REFNEW
   @see vfs_node_refnew
 */
typedef VFS_FS_NODE_REFNEW(vfs_fs_node_refnew_t);

/** @this defines the fs node refdrop operation prototype */
#define VFS_FS_NODE_REFDROP(x) void (x)(struct fs_node_s *node)

/**
   This function notifies the file system a node is not referenced any
   more.

   @param node Node not used any more

   @csee #VFS_FS_NODE_REFDROP
   @see vfs_node_refdrop
 */
typedef VFS_FS_NODE_REFDROP(vfs_fs_node_refdrop_t);


#ifdef __MKDOC__
/**
   @this creates a new FS object and initializes all its common
   fields.

   @param storage Optional pre-allocated buffer to use as fs. @this
   allocates a new bufer if NULL.
   @return a new fs, NULL on error.
 */
struct vfs_fs_s *vfs_fs_new(void *storage);
#endif

void vfs_fs_dump_stats(struct vfs_fs_s *fs);

struct vfs_fs_ops_s
{
    vfs_fs_node_open_t *node_open;  //< mandatory
    vfs_fs_lookup_t *lookup;        //< mandatory
    vfs_fs_create_t *create;        //< optional, may be NULL
    vfs_fs_link_t *link;            //< optional, may be NULL
    vfs_fs_move_t *move;            //< optional, may be NULL
    vfs_fs_unlink_t *unlink;        //< optional, may be NULL
    vfs_fs_stat_t *stat;            //< mandatory
    vfs_fs_can_unmount_t *can_unmount; //< mandatory
    vfs_fs_node_refnew_t *node_refnew; //< mandatory
    vfs_fs_node_refdrop_t *node_refdrop; //< mandatory
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
