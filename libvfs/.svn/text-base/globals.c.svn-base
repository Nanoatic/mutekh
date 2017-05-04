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

#include <hexo/local.h>

struct vfs_node_s;

#if defined(CONFIG_VFS_GLOBAL_CWD)
struct vfs_node_s *_vfs_cwd;
#else
CONTEXT_LOCAL struct vfs_node_s *_vfs_cwd;
#endif

#if defined(CONFIG_VFS_GLOBAL_ROOT)
struct vfs_node_s *_vfs_root;
#else
CONTEXT_LOCAL struct vfs_node_s *_vfs_root;
#endif
