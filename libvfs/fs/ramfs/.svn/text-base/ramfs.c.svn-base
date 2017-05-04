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

#define GPCT_CONFIG_NODEPRECATED

#include <hexo/types.h>
#include <hexo/error.h>

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <vfs/types.h>
#include <vfs/fs.h>
#include <vfs/ops.h>

#include "ramfs.h"
#include "ramfs-private.h"
#include "ramfs_data.h"
#include "ramfs_file.h"

CONTAINER_FUNC           (ramfs_dir_hash, HASHLIST, static inline, ramfs_dir, name);
CONTAINER_FUNC_NOLOCK    (ramfs_dir_hash, HASHLIST, static inline, ramfs_dir_nolock, name);
CONTAINER_KEY_FUNC       (ramfs_dir_hash, HASHLIST, static inline, ramfs_dir, name);
CONTAINER_KEY_FUNC_NOLOCK(ramfs_dir_hash, HASHLIST, static inline, ramfs_dir_nolock, name);

/*
  Here are two helpers when we need to take two locks at the same time
  in two directories. We MUST always take them in the same order to
  avoid deadlocks. So we take them in pointer order, and release them
  in the opposite order.
 */
void ramfs_2dir_wrlock(ramfs_dir_hash_root_t *d1,
                       ramfs_dir_hash_root_t *d2)
{
    if ( d1 < d2 ) {
        ramfs_dir_wrlock(d1);
        ramfs_dir_wrlock(d2);
    } else {
        ramfs_dir_wrlock(d2);
        ramfs_dir_wrlock(d1);
    }
}

void ramfs_2dir_unlock(ramfs_dir_hash_root_t *d1,
                       ramfs_dir_hash_root_t *d2)
{
    if ( d1 < d2 ) {
        ramfs_dir_unlock(d2);
        ramfs_dir_unlock(d1);
    } else {
        ramfs_dir_unlock(d1);
        ramfs_dir_unlock(d2);
    }
}

OBJECT_CONSTRUCTOR(ramfs_node)
{
    vfs_printk("<ramfs_node_ctor");
    enum vfs_node_type_e type = va_arg(ap, enum vfs_node_type_e);
    obj->type = type;
    obj->parent = NULL;
    if ( type == VFS_NODE_FILE ) {
        struct ramfs_data_s *data = va_arg(ap, struct ramfs_data_s*);
        if ( data == NULL ) {
            obj->data = ramfs_data_new(NULL);
            if ( obj->data == NULL ) {
                return -ENOMEM;
            }
        } else
            obj->data = ramfs_data_refnew(data);
    } else {
        ramfs_dir_init(&obj->children);
    }
    vfs_printk(">");
    return 0;
}

OBJECT_DESTRUCTOR(ramfs_node)
{
    vfs_printk("<ramfs_node_dtor");
    if ( obj->type == VFS_NODE_DIR ) {
        ramfs_dir_clear(&obj->children);
    } else {
        ramfs_data_refdrop(obj->data);
    }
    vfs_printk(">");
}

void ramfs_dump_item(struct fs_node_s *node, size_t pf)
{
    size_t i;
    for ( i=0; i<pf; ++i )
        printk(" ");
    printk(" %s %d '%s': %d\n",
           node->type == VFS_NODE_DIR ? ">" : "-",
           node->obj_entry.refcnt.value,
           node->name,
           node->type == VFS_NODE_FILE
           ? ramfs_data_refcount(node->data)
           : 0);
    if ( node->type == VFS_NODE_DIR )
		CONTAINER_FOREACH(ramfs_dir_hash, HASHLIST, &node->children, {
                ramfs_dump_item(item, pf+2);
            });
}

void ramfs_dump(struct vfs_fs_s *fs)
{
    printk("Ramfs dump. ref: %d\n", atomic_get(&fs->ref));
    vfs_fs_dump_stats(fs);
    struct fs_node_s *root = fs->root;
    ramfs_dump_item(root, 0);
}



VFS_FS_CAN_UNMOUNT(ramfs_can_unmount)
{
	return atomic_get(&fs->ref) == 1;
}

VFS_FS_LOOKUP(ramfs_lookup)
{
    vfs_printk("<%s %s/%d ", __FUNCTION__, name, namelen);

    if ( ref->type != VFS_NODE_DIR )
        return -EINVAL;

    vfs_name_mangle(name, namelen, mangled_name);

    struct fs_node_s *child = ramfs_dir_lookup(&ref->children, mangled_name);
    if ( child ) {
        *node = child;
        vfs_printk(" ok>");
        return 0;
    }
    vfs_printk(" noent>");
    return -ENOENT;
}

VFS_FS_CREATE(ramfs_create)
{
	vfs_printk("<%s ", __FUNCTION__);

    struct fs_node_s *rfs_node;
    rfs_node = ramfs_node_new(NULL, type, NULL);
	if ( !rfs_node )
		goto err_priv;

	*node = rfs_node;

	vfs_printk("ok>\n");

	return 0;

  err_priv:
	vfs_printk("err>\n");
	return -ENOMEM;
}

VFS_FS_LINK(ramfs_link)
{
	if ( namelen >= CONFIG_VFS_NAMELEN )
		return -EINVAL;
    
    if ( parent->type != VFS_NODE_DIR )
        return -EINVAL;
    
	vfs_printk("<%s %s ", __FUNCTION__, name);

    if ( parent->parent == NULL ) {
        vfs_printk("dandling>");
        return -ENOTSUP;
    }

	/* We cant support hardlinks of directories */
	if ( (node->parent != NULL)
         && (node->type == VFS_NODE_DIR) ) {
        vfs_printk("isdir>");
        return -EISDIR;
    }

    struct fs_node_s *ret_node;
	if ( node->parent != NULL ) {
		vfs_printk("clone ");
        ret_node = ramfs_node_new(NULL, VFS_NODE_FILE, node->data);
        if ( ret_node == NULL ) {
			vfs_printk("node_new fail>");
			return -ENOMEM;
        }
	} else {
		vfs_printk("use ");
		ret_node = ramfs_node_refnew(node);
	}

    vfs_name_mangle(name, namelen, mangled_name);
	memcpy(ret_node->name, mangled_name, CONFIG_VFS_NAMELEN);

    ramfs_dir_wrlock(&parent->children);
    struct fs_node_s *old_file = ramfs_dir_nolock_lookup(&parent->children, ret_node->name);
    if ( old_file ) {
        old_file->parent = NULL;
        ramfs_dir_nolock_remove(&parent->children, old_file);
    }
    ret_node->parent = parent;
    ramfs_dir_nolock_push(&parent->children, ret_node);
    ramfs_dir_unlock(&parent->children);

    if ( old_file )
        ramfs_node_refdrop(old_file);

	vfs_printk("ok>");

    *rnode = ret_node;

	return 0;
}

VFS_FS_MOVE(ramfs_move)
{
	if ( namelen >= CONFIG_VFS_NAMELEN )
		return -EINVAL;
    
    if ( parent->type != VFS_NODE_DIR )
        return -EINVAL;

    if ( node->parent == NULL
         || node->parent == node )
        return -EINVAL;
    
	vfs_printk("<%s %s ", __FUNCTION__, name);

    if ( parent->parent == NULL ) {
        vfs_printk("dandling>");
        return -ENOTSUP;
    }

    struct fs_node_s *parent_src = node->parent;
    ramfs_2dir_wrlock(&parent_src->children, &parent->children);
    ramfs_dir_nolock_remove(&parent_src->children, node);
	memset(node->name, 0, CONFIG_VFS_NAMELEN);
	memcpy(node->name, name, namelen);

    struct fs_node_s *old_file = ramfs_dir_nolock_lookup(&parent->children, node->name);
    if ( old_file ) {
        old_file->parent = NULL;
        ramfs_dir_nolock_remove(&parent->children, old_file);
    }

    node->parent = parent;
    ramfs_dir_nolock_push(&parent->children, node);
    ramfs_2dir_unlock(&parent->children, &parent_src->children);

    if ( old_file )
        ramfs_node_refdrop(old_file);

	vfs_printk("ok>");

	return 0;
}

VFS_FS_UNLINK(ramfs_unlink)
{
	char tmpname[CONFIG_VFS_NAMELEN];
	memset(tmpname, 0, CONFIG_VFS_NAMELEN);
	memcpy(tmpname, name, namelen);

	vfs_printk("<%s %s ", __FUNCTION__, tmpname);

    ramfs_dir_wrlock(&parent->children);

    struct fs_node_s *node = ramfs_dir_nolock_lookup(&parent->children, tmpname);
	if ( node == NULL ) {
        ramfs_dir_unlock(&parent->children);
        vfs_printk("not found>");
		return -ENOENT;
    }

    if ( (node->type == VFS_NODE_DIR)
         && (ramfs_dir_nolock_count(&node->children) != 0) ) {
        ramfs_node_refdrop(node);
        ramfs_dir_unlock(&parent->children);
        vfs_printk("nonempty>");
		return -EBUSY;
    }

    node->parent = NULL;
    ramfs_dir_nolock_remove(&parent->children, node);
    ramfs_dir_unlock(&parent->children);

    ramfs_node_refdrop(node);

	vfs_printk("ok>");

	return 0;
}

VFS_FS_STAT(ramfs_stat)
{
	stat->type = node->type;

	switch ( node->type ) {
	case VFS_NODE_DIR:
		stat->size = ramfs_dir_count(&node->children);
        stat->nlink = 1;
		break;
	case VFS_NODE_FILE:
		stat->size = node->data->actual_size;
		stat->nlink = ramfs_data_refcount(node->data);
		break;
	}

	return 0;
}

error_t ramfs_close(struct vfs_fs_s *fs)
{
	return -EBUSY;
}

static const struct vfs_fs_ops_s ramfs_ops =
{
    .node_open = ramfs_node_open,
    .lookup = ramfs_lookup,
    .create = ramfs_create,
    .link = ramfs_link,
    .move = ramfs_move,
    .unlink = ramfs_unlink,
    .stat = ramfs_stat,
    .can_unmount = ramfs_can_unmount,
    .node_refdrop = ramfs_node_refdrop,
    .node_refnew = ramfs_node_refnew,
};

error_t ramfs_open(struct vfs_fs_s **fs)
{
	struct vfs_fs_s *mnt = vfs_fs_new(NULL);
	if ( mnt == NULL )
		goto nomem_fs;

    vfs_printk("ramfs: opening new ramfs volume\n");

	atomic_set(&mnt->ref, 0);

    mnt->ops = &ramfs_ops;
	mnt->old_node = NULL;

    struct fs_node_s *root = ramfs_node_new(NULL, VFS_NODE_DIR);
	if ( root == NULL )
		goto nomem_dir;

    root->parent = root;

	mnt->root = root;

	*fs = mnt;

	return 0;
  nomem_dir:
	mem_free(mnt);
  nomem_fs:
	return -ENOMEM;
}


bool_t ramfs_dir_get_nth(struct fs_node_s *node, struct vfs_dirent_s *dirent, size_t n)
{
	size_t i;

	ramfs_dir_rdlock(&node->children);

    struct fs_node_s *ent;

	i = 0;
    ent = ramfs_dir_nolock_head(&node->children);
    while ( (i < n) && (ent != NULL) ) {
        i++;
        struct fs_node_s *nent;
        nent = ramfs_dir_nolock_next(&node->children, ent);
        ramfs_node_refdrop(ent);
        ent = nent;
    }

    if ( ent == NULL ) {
        ramfs_dir_unlock(&node->children);
        return 0;
    }

	memcpy( dirent->name, ent->name, CONFIG_VFS_NAMELEN );
	dirent->type = ent->type;
	dirent->size = ent->type == VFS_NODE_FILE
		? ent->data->actual_size
		: ramfs_dir_count(&ent->children);

	ramfs_dir_unlock(&node->children);

    ramfs_node_refdrop(ent);

    return 1;
}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

