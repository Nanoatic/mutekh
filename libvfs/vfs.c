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

#include <vfs/vfs.h>
#include "vfs-private.h"
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <string.h>

/*
  Here are two helpers when we need to take two locks at the same time
  in two directories. We MUST always take them in the same order to
  avoid deadlocks. So we take them in pointer order, and release them
  in the opposite order.
 */
void vfs_node_2dirlock(struct vfs_node_s *d1,
                       struct vfs_node_s *d2)
{
    if ( d1 < d2 ) {
        vfs_node_dirlock(d1);
        vfs_node_dirlock(d2);
    } else {
        vfs_node_dirlock(d2);
        vfs_node_dirlock(d1);
    }
}

void vfs_node_2dirunlock(struct vfs_node_s *d1,
                         struct vfs_node_s *d2)
{
    if ( d1 < d2 ) {
        vfs_node_dirunlock(d2);
        vfs_node_dirunlock(d1);
    } else {
        vfs_node_dirunlock(d1);
        vfs_node_dirunlock(d2);
    }
}

static void
vfs_node_parent_nolock_set_for_root(struct vfs_node_s *node, struct vfs_node_s *parent)
{
    lock_spin(&node->parent_lock);
    node->parent = vfs_node_refnew(parent);
    vfs_dir_push(&parent->children, node);
    lock_release(&node->parent_lock);
}

static void
vfs_node_parent_nolock_set(struct vfs_node_s *node, struct vfs_node_s *parent)
{
    lock_spin(&node->parent_lock);
	assert( vfs_node_is_dandling(node) );
    node->parent = vfs_node_refnew(parent);
    vfs_dir_push(&parent->children, node);
    lock_release(&node->parent_lock);
    vfs_node_lru_rehash(node);
}

inline
size_t vfs_name_mangle(const char *fullname, size_t fulllen, char *vfsname)
{
    /* FIXME should gracefully handle shortened names colision */
    if (fulllen > CONFIG_VFS_NAMELEN)
        fulllen = CONFIG_VFS_NAMELEN;
    /* fulllen can be 0 for anonymous nodes only */
    memcpy(vfsname, fullname, fulllen);
	memset(vfsname + fulllen, 0, CONFIG_VFS_NAMELEN - fulllen);

    return fulllen;
}

static struct vfs_node_s *
vfs_dir_mangled_lookup(struct vfs_node_s *node,
                              const char *fullname, size_t fullnamelen)
{
	char tmpname[CONFIG_VFS_NAMELEN];
    vfs_name_mangle(fullname, fullnamelen, tmpname);

	VFS_STATS_INC(node, lookup_count);

	return vfs_dir_lookup(&node->children, tmpname);
}

error_t vfs_mount(struct vfs_node_s *mountpoint,
				  struct vfs_fs_s *fs)
{
    {
        struct vfs_stat_s stat;
        vfs_node_stat(mountpoint, &stat);
        /* Must mount on a directory */
        if ( stat.type != VFS_NODE_DIR )
            return -EINVAL;
    }

	/* Cant mount a mounted fs */
	if ( fs->old_node != NULL )
		return -EBUSY;

	/* Cant mount at the root of the filesystem */
	if ( mountpoint->parent == mountpoint )
		return -EINVAL;

	/* Cant mount at the root of another mount */
	if ( mountpoint->fs->root == mountpoint->fs_node )
		return -EINVAL;

	/* Keep a reference to the mountpoint node, it may be open */
	fs->old_node = vfs_node_refnew(mountpoint);
    /* Keep another reference for the lru not to trash it */
    vfs_node_refnew(mountpoint);

    struct vfs_node_s *new_node = vfs_node_createnew(
        fs, mountpoint->name,
        fs->root);

    if ( new_node == NULL )
        return -ENOMEM;

    struct vfs_node_s *parent = vfs_node_get_parent(mountpoint);

	vfs_node_dirlock(parent);
    vfs_node_parent_nolock_unset(mountpoint);
    vfs_node_parent_nolock_set_for_root(new_node, parent);
	vfs_node_dirunlock(parent);

    vfs_node_refdrop(parent);

	vfs_printk("<mount ok>");

	return 0;
}

error_t vfs_umount(struct vfs_node_s *mountpoint)
{
    struct vfs_fs_s *fs = mountpoint->fs;

    /* Ensure mountpoint is a root */
    if ( fs->root != mountpoint->fs_node )
        return -EINVAL;

	/* Cant umount the global root */
	if ( fs->old_node == NULL )
		return -EINVAL;

	struct vfs_node_s *parent = vfs_node_get_parent(mountpoint);

	/* Is user playing with us ? */
	if ( parent == NULL )
		return -EINVAL;

	vfs_node_dirlock(parent);
	if ( !fs->ops->can_unmount(fs) ) {
        vfs_node_dirunlock(parent);
        vfs_node_refdrop(parent);
        return -EBUSY;
    }
        
    /* Reput the old node where it belongs */
    vfs_node_parent_nolock_unset(mountpoint);
    vfs_node_parent_nolock_set(fs->old_node, parent);
    
    vfs_node_dirunlock(parent);

    /* Do the refdrop without the parent locked */
    vfs_node_refdrop(mountpoint);
    /* Yes, we did have two refs */
    vfs_node_refdrop(mountpoint);
    vfs_node_refdrop(fs->old_node);
    fs->old_node = NULL;

    vfs_node_refdrop(parent);
    vfs_node_lru_rehash(mountpoint);
	return 0;
}

error_t vfs_create_root(struct vfs_fs_s *fs,
                        struct vfs_node_s **mountpoint)
{
	/* Cant mount a mounted fs */
	if ( fs->old_node != NULL )
		return -EBUSY;

    struct vfs_node_s *new_node =
        vfs_node_createnew(fs, NULL, fs->root);

    if ( new_node == NULL )
        return -ENOMEM;

    new_node->parent = new_node;

    fs->old_node = NULL;

	vfs_printk("<mount root ok>");

    *mountpoint = new_node;

#if defined(CONFIG_VFS_GLOBAL_ROOT)
    vfs_set_root(new_node);
#endif

	return 0;

}

/* Node operations */

error_t vfs_node_lookup(struct vfs_node_s *parent,
						const char *name,
						size_t namelen,
						struct vfs_node_s **node)
{
	error_t err = 0;

    struct fs_node_s *fs_node;

	VFS_STATS_INC(parent->fs, lookup_count);

	vfs_printk("<lookup \"%s\"/%d parent: %p [%s]... ", name, namelen, parent, parent->name);

	/* Dandling nodes are valid, but no lookup is authorized on them... */
    if ( vfs_node_is_dandling(parent) )
		return -EINVAL;

	vfs_node_dirlock(parent);

	/* Now lookup inside the hash */
	*node = vfs_dir_mangled_lookup(parent, name, namelen);
	if ( *node ) {
		vfs_printk("ok %p [%s]>", (*node), (*node)->name);
		err = 0;
		goto fini;
	}

    char mangled_name[CONFIG_VFS_NAMELEN];

	/* Last call: ask the FS */
	err = parent->fs->ops->lookup(parent->fs_node, name, namelen, &fs_node, mangled_name);

	if ( err ) {
		vfs_printk("err %d>", err);
		goto fini;
	}

    *node = vfs_node_createnew(parent->fs, mangled_name, fs_node);
    parent->fs->ops->node_refdrop(fs_node);
    if ( *node == NULL ) {
        err = -ENOMEM;
        goto fini;
    }

	/* As FS got a node for this, we can register it in the hash */
    vfs_node_parent_nolock_set(*node, parent);

	vfs_printk("fs %p [%s]>", (*node), (*node)->name);

  fini:
	vfs_node_dirunlock(parent);
	return err;
}

error_t vfs_node_create(struct vfs_fs_s *fs,
						enum vfs_node_type_e type,
						struct vfs_node_s **node)
{
    if ( fs->ops->create == NULL )
        return -ENOTSUP;

    if ( fs->flag_ro )
        return -EPERM;

	VFS_STATS_INC(fs, create_count);

    struct fs_node_s *fs_node;

	error_t err = fs->ops->create(fs, type, &fs_node);
    if ( err )
        return err;

    *node = vfs_node_createnew(fs, NULL, fs_node);
    fs->ops->node_refdrop(fs_node);

    if ( *node == NULL ) {
        return -ENOMEM;
    }

    return 0;
}

error_t vfs_node_open(struct vfs_node_s *node,
                      enum vfs_open_flags_e flags,
                      struct vfs_file_s **file)
{
	vfs_printk(" node_open(%p): ", node);

    assert( node->fs->ops->node_open != NULL );

    if ( (flags & VFS_OPEN_WRITE) && (node->fs->flag_ro) )
        return -EPERM;

	VFS_STATS_INC(node->fs, node_open_count);

    vfs_node_lru_rehash(node);

    return node->fs->ops->node_open(node->fs_node, flags, file);
}

error_t vfs_node_link(struct vfs_node_s *node,
					  struct vfs_node_s *parent,
					  const char *name,
					  size_t namelen,
					  struct vfs_node_s **rnode)
{
	error_t err = 0;

	vfs_printk("<%s '%s' %p [%s] in %p [%s]... ", __FUNCTION__, name, node, node->name, parent, parent->name);

    if ( parent->fs->ops->link == NULL )
        return -ENOTSUP;

    if ( parent->fs != node->fs )
        return -ENOTSUP;

    if ( vfs_node_is_dandling(parent) )
        return -ENOTSUP;

    if ( parent->fs->flag_ro )
        return -EPERM;

	vfs_node_dirlock(parent);

	struct vfs_node_s *prev_node = vfs_dir_mangled_lookup(
        parent, name, namelen);

	VFS_STATS_INC(parent->fs, link_count);

    struct fs_node_s *rfs_node;
    char mangled_name[CONFIG_VFS_NAMELEN];

	err = parent->fs->ops->link(node->fs_node, parent->fs_node,
                                name, namelen, &rfs_node, mangled_name);
	if ( err ) {
		vfs_printk("fail %d>\n", err);
		goto fini;
	}

    if ( prev_node != NULL )
        vfs_node_parent_nolock_unset(prev_node);

    *rnode = vfs_node_createnew(parent->fs, mangled_name, rfs_node);
    parent->fs->ops->node_refdrop(rfs_node);

    if ( *rnode == NULL ) {
        /*
          TODO
          Argh ! we did it on the FS, but we cant create the node
          what should we do ?

          We should not return an error, but if we dont, we must
          provide a valid node. There are options:

          * Change the prototype in order not to return the new node ?
          * Allocate the new node before, but we break the ctor/dtor
            which take valid fs_nodes
         */
        err = -ENOMEM;
        goto fini;
    }

    /* As FS got a node for this, we can register it in the hash */
    vfs_node_parent_nolock_set(*rnode, parent);

	vfs_printk("ok>\n");

	err = 0;
  fini:
	vfs_node_dirunlock(parent);
    if ( prev_node != NULL )
        vfs_node_refdrop(prev_node);
	return err;
}

error_t vfs_node_move(struct vfs_node_s *node,
					  struct vfs_node_s *parent,
					  const char *name,
					  size_t namelen)
{
	error_t err = 0;

	vfs_printk("<%s '%s' %p [%s] in %p [%s]... ", __FUNCTION__, name, node, node->name, parent, parent->name);

    if ( parent->fs->ops->move == NULL )
        return -ENOTSUP;

    if ( parent->fs != node->fs )
        return -ENOTSUP;

    if ( vfs_node_is_dandling(parent) )
        return -ENOTSUP;

    if ( parent->fs->flag_ro )
        return -EPERM;

	VFS_STATS_INC(parent->fs, move_count);

    struct vfs_node_s *parent_src = vfs_node_get_parent(node);
    if ( !parent_src )
        return -EINVAL;

	vfs_node_2dirlock(parent, parent_src);
	struct vfs_node_s *prev_node = vfs_dir_mangled_lookup(parent, name, namelen);

	err = parent->fs->ops->move(node->fs_node, parent->fs_node,
                           name, namelen);
	if ( err ) {
		vfs_printk("fail %d>\n", err);
		goto fini;
	}

    vfs_node_parent_nolock_unset(node);

    if ( prev_node != NULL )
        vfs_node_parent_nolock_unset(prev_node);

    vfs_node_parent_nolock_set(node, parent);

	vfs_printk("ok>\n");

	err = 0;
  fini:
	vfs_node_2dirunlock(parent, parent_src);
    if ( prev_node != NULL )
        vfs_node_refdrop(prev_node);
	return err;
}

error_t vfs_node_unlink(struct vfs_node_s *parent,
						const char *name,
						size_t namelen)
{
    if ( parent->fs->ops->unlink == NULL )
        return -ENOTSUP;

    if ( parent->fs->flag_ro )
        return -EPERM;

	vfs_printk("<%s '%s'... ", __FUNCTION__, name);

	VFS_STATS_INC(parent->fs, unlink_count);

	vfs_node_dirlock(parent);

	struct vfs_node_s *node = vfs_dir_mangled_lookup(parent, name, namelen);

	error_t err = parent->fs->ops->unlink(parent->fs_node, name, namelen);
	if ( err )
		goto fini;

	if ( node )
        vfs_node_parent_nolock_unset(node);

	err = 0;
  fini:
	vfs_node_dirunlock(parent);
	if ( node )
        vfs_node_refdrop(node);
	vfs_printk(" %s>", strerror(err));
	return err;
}

error_t vfs_node_stat(struct vfs_node_s *node,
					  struct vfs_stat_s *stat)
{
	VFS_STATS_INC(node->fs, stat_count);
	VFS_STATS_INC(node, stat_count);

    vfs_node_lru_rehash(node);

	return node->fs->ops->stat(node->fs_node, stat);
}

bool_t vfs_name_compare(const char *fullname, size_t fulllen,
                        const char *vfsname, size_t vfsnamelen)
{
    /* should compare vfsname with both fullname and shortened name */

    assert(vfsnamelen && fulllen);

    if (fulllen == vfsnamelen && !memcmp(fullname, vfsname, fulllen))
        return 1;

    if (fulllen > CONFIG_VFS_NAMELEN) {
        /* FIXME should gracefully handle shortened names compare HERE */
    }

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

