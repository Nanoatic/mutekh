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
   @short Operations on path strings

   These operations implements the layer typically used by a @tt
   libunix or a @tt libc. These functions use null-terminated strings,
   don't rely on hardwired root and cwd nodes, and return errors
   directly usable for errno.
 */

#ifndef _VFS_HELPERS_H_
#define _VFS_HELPERS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <vfs/types.h>

/**
   @this gets a path relative to the parent node.

   @param root Current root directory
   @param cwd Current base directory
   @param path Path of the node to lookup, relative (from @tt cwd, or
   absolute from @tt root) @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @param node Found node, if available (return value)
   @return 0 if found, or an error
   @this transfers the ownership to caller.
   @see vfs_node_lookup
*/
error_t vfs_lookup(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path,
				   struct vfs_node_s **node);

/**
   @this creates a path relative to the parent node. The path may be a
   relative path with more than one directory member. For the node
   creation to succeed, parent of final node must already exist.

   @param root Current root directory
   @param cwd Current base directory
   @param path Path of the node to create, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @param node Found node, if no error (return value)
   @return 0 if created, or an error
   @this transfers the ownership to caller.
   @see vfs_node_create
*/
error_t vfs_create(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path,
				   enum vfs_node_type_e type,
				   struct vfs_node_s **node);


/**
   @this opens a path for reading and/or writing. If path is a
   directory, the only permitted operation is read (see ... for more
   information). If target node does not exist, it may be created if
   @ref VFS_OPEN_CREATE is passed in flags. A directory cannot be
   created by this call.

   @param root Current root directory
   @param cwd Current base directory
   @param path Path of the node to open, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @param flags Opening mode flags
   @param file Opened file descriptor, if succeeded
   @return 0 if opened, or an error
   @see vfs_node_open
*/
error_t vfs_open(struct vfs_node_s *root,
				 struct vfs_node_s *cwd,
				 const char *path,
				 enum vfs_open_flags_e flags,
				 struct vfs_file_s **file);

/**
   @this retrieves information about a given file.

   @param root Current root directory
   @param cwd Current base directory
   @param path Path of the node to open, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @param stat User-provided buffer to hold node information
   @return 0 if node was found, or an error
   @see vfs_node_stat
*/
error_t vfs_stat(struct vfs_node_s *root,
				 struct vfs_node_s *cwd,
				 const char *path,
				 struct vfs_stat_s *stat);

/**
   @this deletes a node from the file system.

   @param root Current root directory
   @param cwd Current base directory
   @param path Path of the node to delete, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @return 0 if node was found and deleted, or an error
   @see vfs_node_unlink
*/
error_t vfs_unlink(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path);

/**
   @this links a path to another

   @param root Current root directory
   @param cwd Current base directory
   @param src Path of the node to link from, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @param dst Path of the node to link to, relative (from @tt cwd, or
   absolute from @tt root).  @tt path may contain one or more @tt
   '/'. It must end with a @tt '\0'
   @return 0 if node was linked, or an error
   @see vfs_node_unlink
*/
error_t vfs_link(struct vfs_node_s *root,
                 struct vfs_node_s *cwd,
                 const char *src,
                 const char *dst);

/**
   @this dumps the present tree state of VFS starting at @tt root.

   @param root Where to start dump from
 */
void vfs_dump(struct vfs_node_s *root);

/**
   @this dumps the present node LRU state, using @ref vfs_dump for
   each node in LRU.

   @param root File-system root to use for LRU walking.
 */
void vfs_dump_lru(struct vfs_node_s *root);

C_HEADER_END

#endif
