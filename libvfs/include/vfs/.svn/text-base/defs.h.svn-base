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
   @short Constant definitions
 */

#ifndef _VFS_DEFS_H_
#define _VFS_DEFS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/types.h>

/** @hidden */
struct fs_node_s;

/** @hidden */
struct vfs_node_s;

typedef size_t vfs_file_size_t;

enum vfs_node_type_e
{
    /** A directory node */
    VFS_NODE_DIR,
    /** A regular file node */
    VFS_NODE_FILE,
};

#if defined(CONFIG_VFS_STATS)
# define VFS_STATS_INC(obj, field) atomic_inc(&(obj)->field)
#else
# define VFS_STATS_INC(x, y) do{}while(0)
#endif

#ifdef CONFIG_VFS_VERBOSE
# include <pthread.h>
# include <mutek/printk.h>
# define vfs_printk(fmt, x...) do{printk("%p: "fmt"\n", pthread_self(), ##x);}while(0)
#else
# define vfs_printk(...) do{}while(0)
#endif

C_HEADER_END

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

