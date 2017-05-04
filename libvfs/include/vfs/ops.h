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
   @short Core operations on file system nodes
 */

#ifndef _VFS_OPS_H_
#define _VFS_OPS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <vfs/types.h>
#include <vfs/fs.h>


/* VFS operations */

/**
   @this mounts a file system inside another, replacing the @tt
   mountpoint node in the VFS. @tt mountpoint must be a directory.

   @param mountpoint directory that must be replaced with fs's
   root.
   @param fs new filesystem to attach at mountpoint
   @return 0 if mounted correctly
 */
error_t vfs_mount(struct vfs_node_s *mountpoint,
				  struct vfs_fs_s *fs);

/**
   @this mounts a file system as the root of all.

   @param fs new filesystem to attach at root
   @param mountpoint New node where the filesystem root lies

   If #CONFIG_VFS_GLOBAL_ROOT is set, this call sets global system
   root.

   @return 0 if mounted correctly
 */
error_t vfs_create_root(struct vfs_fs_s *fs,
                        struct vfs_node_s **mountpoint);

/**
   @this unmounts a file system. @tt fs must not have any files
   left open. Old directory node will be restored in VFS.

   @param mountpoint Mountpoint where to umount a file system
   @return 0 if unmounted correctly
 */
error_t vfs_umount(struct vfs_node_s *mountpoint);

/* Node operations */

/** @this increases the node reference count and return the node itself. */
struct vfs_node_s * vfs_node_refnew(struct vfs_node_s * node);

/** @this decreases the node reference count and may delete the node if no more reference exist. */
void vfs_node_refdrop(struct vfs_node_s * node);

/**
   @this looks for a node named @tt name as a child of @tt
   parent. First looks up in the hash table. If @tt name is not found,
   it calls the driver. @tt '.' and @tt '..' are not supported.

   @param parent Node to look up from
   @param name Name of the node, must not contain any @tt '/'. It may
          not end with a @tt '\0' . May be eiter a full length file system
          name or a vfs shortened node name.
   @param namelen Length of node name
   @param node Returned node
   @return 0 if found

   @this transfers the ownership of @tt node to caller.
   @see vfs_fs_lookup_t @see vfs_lookup
   @see vfs_name_mangle
*/
error_t vfs_node_lookup(struct vfs_node_s *parent,
						const char *fullname,
						size_t fullnamelen,
						struct vfs_node_s **node);

/**
   @this opens an existing node in a given FS.

   @tt flags inform the file system about the actions intended on the
   file.  @ref VFS_OPEN_READ and @ref VFS_OPEN_WRITE may be ored
   together.  For directories, the only valid operation is @ref
   VFS_OPEN_READ | @ref VFS_OPEN_DIR.

   @this fail if trying to open a file with @ref VFS_OPEN_DIR flag present.

   This function must only honor @ref VFS_OPEN_READ, @ref VFS_OPEN_WRITE and
   @ref VFS_OPEN_DIR flags.  Other flags must be ignored (even
   @ref VFS_OPEN_CREATE and @ref VFS_OPEN_APPEND).

   This function must not create new files implicitely.

   It relies on the @ref vfs_fs_node_open_t fs drivers operation, refer for details.
   @this transfers the ownership of @tt node to caller.
   @see vfs_open @see vfs_fs_node_open_t
*/
error_t vfs_node_open(struct vfs_node_s *node,
                      enum vfs_open_flags_e flags,
                      struct vfs_file_s **file);

/**
   @this creates a new anonymous node in a given FS.

   It relies on the @ref vfs_fs_create_t fs drivers operation, refer for details.
   @this transfers the ownership of @tt node to caller.
   @see vfs_create
 */
error_t vfs_node_create(struct vfs_fs_s *fs,
						enum vfs_node_type_e type,
						struct vfs_node_s **node);

/**
   @this links a node in a given parent. As a node must be unique in
   the VFS, node may be cloned in order to be attached where wanted.
   Thus the actually attached node returned in @tt rnode may be
   different from @tt node.

   @param node Node to attach
   @param parent Where to attach a new child
   @param fullname Name of the new node, may be a long file system
          entry name but will be shortened for use as vfs node name.
   @param fullnamelen Length of name of the new node
   @param rnode Actually attached node
   @return 0 if created

   @this transfers the ownership of @tt rnode to caller, even if it is
   actually @tt node.
   @see vfs_fs_link_t
   @see vfs_name_mangle
 */
error_t vfs_node_link(struct vfs_node_s *node,
					  struct vfs_node_s *parent,
					  const char *fullname,
					  size_t fullnamelen,
					  struct vfs_node_s **rnode);

/**
   @this moves a node in a given parent.

   @param node Node to attach to a new parent
   @param parent Where to attach node
   @param fullname Name of the new node, may be a long file system
          entry name but will be shortened for use as vfs node name.
   @param fullnamelen Length of name of the new node
   @return 0 if created

   @see vfs_fs_move_t
   @see vfs_name_mangle
 */
error_t vfs_node_move(struct vfs_node_s *node,
					  struct vfs_node_s *parent,
					  const char *fullname,
					  size_t fullnamelen);

/**
   Unlinks a node from its parent.

   @param parent Where to unlink a child
   @param fullname Name of the node, must not contain any @tt '/'. It may
          not end with a @tt '\0' . May be eiter a full length file system
          name or a vfs shortened node name.
   @param fullnamelen Length of name
   @return 0 if unlinked correctly
   @see vfs_fs_unlink_t @see vfs_unlink
 */
error_t vfs_node_unlink(struct vfs_node_s *parent,
						const char *fullname,
						size_t fullnamelen);

/**
   @this retrieves information about a given node.

   @param node Node to retrieve information about
   @param stat User-provided buffer to hold node information
   @return 0 if node was found, or an error
   @see vfs_fs_stat_t @see vfs_stat
*/
error_t vfs_node_stat(struct vfs_node_s *node,
					  struct vfs_stat_s *stat);

/**
   @this compares a full name as described by the on disk file
   system directory entry with a possibly shortened and mangled node
   name as seen by the vfs.

   @param fullname entry full name as described by file system
   @param fullnamelen lenght of full name
   @param vfsname possibly shortened node name
   @param vfsnamelen possibly shortened node name lenght
   @return true if equal
   @see vfs_name_mangle
 */
bool_t vfs_name_compare(const char *fullname, size_t fullnamelen,
                            const char *vfsname, size_t vfsnamelen);

/**
   @this setup a possibly mangled and shortened vfs node name from a
   full lenght file system entry name. No extra @tt '\0' is added to mangled name.

   @param fullname entry full name as described by file system
   @param fullnamelen lenght of full name
   @param vfsname possibly shortened node name
   @return length of resulting mangled name.
   @see vfs_name_compare @see vfs_node_new
 */
size_t vfs_name_mangle(const char *fullname, size_t fullnamelen, char *vfsname);

struct vfs_node_s *vfs_node_get_parent(struct vfs_node_s *node);

ssize_t vfs_node_get_name(struct vfs_node_s *node,
                          char *name,
                          size_t namelen);

struct vfs_fs_s *vfs_node_get_fs(struct vfs_node_s *node);

C_HEADER_END

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
