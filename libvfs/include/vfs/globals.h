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
   @short Keep track of global file system root and cwd nodes

   Depending on configuration (@ref #CONFIG_VFS_GLOBAL_CWD and @ref
   #CONFIG_VFS_GLOBAL_ROOT), root and current working directories may
   be global or thread-local.

   If root is not chosen to be a system-wide global, root must be
   correctly set on thread creation.

   If root is a system-wide global, it only has to be set once.

   In any case, cwd may be set up, if not previously set, it defaults
   to root.
 */

#ifndef _VFS_GLOBALS_H_
#define _VFS_GLOBALS_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/local.h>

/** @hidden */
struct vfs_node_s;

/** @this returns the global vfs root node */
static inline struct vfs_node_s *vfs_get_root();

/** @this change the global vfs root node */
static inline void vfs_set_root(struct vfs_node_s *root);

/**
   @this returns the global current working directory node
   
   If current working directory is undefined, this sets the cwd to
   value returned by @ref vfs_get_root.
*/
static inline struct vfs_node_s *vfs_get_cwd();

/** @this changes the global current working directory node */
static inline void vfs_set_cwd(struct vfs_node_s *cwd);





#if defined(CONFIG_VFS_GLOBAL_CWD)
/** @hidden */
extern struct vfs_node_s *_vfs_cwd;

/** @hidden */
static inline struct vfs_node_s *vfs_get_cwd()
{
    if ( _vfs_cwd == NULL )
        vfs_set_cwd(vfs_get_root());

    return _vfs_cwd;
}

/** @hidden */
static inline void vfs_set_cwd(struct vfs_node_s *cwd)
{
	struct vfs_node_s *old = _vfs_cwd;

    _vfs_cwd = cwd ? vfs_node_refnew(cwd) : NULL;

	if ( old )
		vfs_node_refdrop( old );
}

#else
/** @hidden */
extern CONTEXT_LOCAL struct vfs_node_s *_vfs_cwd;

/** @hidden */
static inline struct vfs_node_s *vfs_get_cwd()
{
    if ( CONTEXT_LOCAL_GET(_vfs_cwd) == NULL )
        vfs_set_cwd(vfs_get_root());

    return CONTEXT_LOCAL_GET(_vfs_cwd);
}

/** @hidden */
static inline void vfs_set_cwd(struct vfs_node_s *cwd)
{
	struct vfs_node_s *old = CONTEXT_LOCAL_GET(_vfs_cwd);

    CONTEXT_LOCAL_SET(_vfs_cwd, cwd ? vfs_node_refnew(cwd) : NULL);

	if ( old )
		vfs_node_refdrop( old );
}
#endif

#if defined(CONFIG_VFS_GLOBAL_ROOT)
/** @hidden */
extern struct vfs_node_s *_vfs_root;

/** @hidden */
static inline struct vfs_node_s *vfs_get_root()
{
    assert(_vfs_root);

    return _vfs_root;
}

/** @hidden */
static inline void vfs_set_root(struct vfs_node_s *root)
{
	struct vfs_node_s *old = _vfs_root;

    _vfs_root = root ? vfs_node_refnew(root) : NULL;

	if ( old )
		vfs_node_refdrop( old );
}
#else
/** @hidden */
extern CONTEXT_LOCAL struct vfs_node_s *_vfs_root;

/** @hidden */
static inline struct vfs_node_s *vfs_get_root()
{
    assert(CONTEXT_LOCAL_GET(_vfs_root));

    return CONTEXT_LOCAL_GET(_vfs_root);
}

/** @hidden */
static inline void vfs_set_root(struct vfs_node_s *root)
{
	struct vfs_node_s *old = CONTEXT_LOCAL_GET(_vfs_root);

    CONTEXT_LOCAL_SET(_vfs_root,
                      root ? vfs_node_refnew(root) : NULL);

	if ( old )
		vfs_node_refdrop( old );
}
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

