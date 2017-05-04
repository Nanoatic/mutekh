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

#include <vfs/vfs.h>
#include "vfs-private.h"
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <string.h>

OBJECT_FUNC   (vfs_node, REFCOUNT, , vfs_node, obj_entry);

CONTAINER_KEY_TYPE(vfs_lru, CUSTOM, SCALAR, vfs_node_refcount(vfs_lru_item), vfs_lru_refcount);

CONTAINER_FUNC       (vfs_lru, CLIST, static, vfs_lru, lru_entry);
CONTAINER_FUNC_NOLOCK(vfs_lru, CLIST, static, vfs_lru_nolock, lru_entry);

CONTAINER_KEY_FUNC_NOLOCK(vfs_lru, CLIST, static, vfs_lru, vfs_lru_refcount);
CONTAINER_KEY_FUNC_NOLOCK(vfs_lru, CLIST, static, vfs_lru_nolock, vfs_lru_refcount);


ssize_t vfs_node_get_name(struct vfs_node_s *node,
                          char *name,
                          size_t namelen)
{
    strncpy(name, node->name, CONFIG_VFS_NAMELEN);
    return (strlen(name) < CONFIG_VFS_NAMELEN) ? 0 : -ENOMEM;
}

struct vfs_fs_s *vfs_node_get_fs(struct vfs_node_s *node)
{
    return node->fs;
}

OBJECT_CONSTRUCTOR(vfs_node)
{
	struct vfs_fs_s *fs = va_arg(ap, struct vfs_fs_s *);
	const char *mangled_name = va_arg(ap, const char *);

    if ( mangled_name )
        memcpy(obj->name, mangled_name, CONFIG_VFS_NAMELEN);
    else
        memset(obj->name, 0, CONFIG_VFS_NAMELEN);
	obj->fs = fs;

    obj->fs_node = obj->fs->ops->node_refnew(va_arg(ap, struct fs_node_s *));

	obj->in_lru = 0;
        
	obj->parent = NULL;
    lock_init(&obj->parent_lock);

	atomic_inc(&fs->ref);
	VFS_STATS_INC(fs, node_create_count);

    semaphore_init(&obj->dir_semaphore, 1);

#if defined(CONFIG_VFS_STATS)
    atomic_set(&obj->lookup_count, 0);
    atomic_set(&obj->open_count, 0);
    atomic_set(&obj->close_count, 0);
    atomic_set(&obj->stat_count, 0);
#endif

    vfs_dir_init(&obj->children);

	vfs_printk("<node create %p %p '%s' free: %p>",
               fs, obj, obj->name,
               obj->obj_entry.storage_free);

	return 0;
}

OBJECT_DESTRUCTOR(vfs_node)
{
	vfs_printk("<node delete %p '%s' free: %p", obj, obj->name,
               obj->obj_entry.storage_free);

    struct vfs_node_s *parent = vfs_node_get_parent(obj);
    if ( parent ) {
        vfs_node_dirlock(parent);
        vfs_node_parent_nolock_unset(obj);
        vfs_node_dirunlock(parent);
        vfs_node_refdrop(parent);
    }
    
    vfs_dir_destroy(&obj->children);

    if ( obj->in_lru )
        vfs_lru_remove(&obj->fs->lru_list, obj);

    semaphore_destroy(&obj->dir_semaphore);

    atomic_dec(&obj->fs->ref);

    VFS_STATS_INC(obj->fs, node_destroy_count);

    obj->fs->ops->node_refdrop(obj->fs_node);
    vfs_printk(" done>");
}

void vfs_node_lru_drop(struct vfs_node_s *node)
{
    vfs_lru_wrlock(&node->fs->lru_list);
    if ( node->in_lru )
        vfs_lru_nolock_remove(&node->fs->lru_list, node);
    node->in_lru = 0;
    vfs_lru_unlock(&node->fs->lru_list);
}

void vfs_node_lru_rehash(struct vfs_node_s *node)
{
    vfs_lru_wrlock(&node->fs->lru_list);
    if ( node->in_lru )
        vfs_lru_nolock_remove(&node->fs->lru_list, node);
    node->in_lru = 1;
    vfs_lru_nolock_insert_ascend(&node->fs->lru_list, node);
    vfs_lru_unlock(&node->fs->lru_list);
}

struct vfs_node_s *vfs_node_createnew(
    struct vfs_fs_s *fs,
    const char *mangled_name,
    struct fs_node_s *fs_node)
{
    object_storage_free_t *old_storage_free = NULL;
    struct vfs_node_s *node = NULL;

    vfs_printk("<node createnew ");

    assert( vfs_lru_check(&fs->lru_list) == 0 );

    for (;;) {
        node = vfs_lru_pop(&fs->lru_list);

        if ( node == NULL ) {
            vfs_printk("empty lru ");
            break;
        }

        node->in_lru = 0;

        if ( vfs_node_refcount(node) > 1 ) {
            vfs_printk("first is reffed ");
            vfs_node_lru_rehash(node);
            node = NULL;
            break;
        }

        struct vfs_node_s *parent = vfs_node_get_parent(node);
        if ( !parent ) {
            vfs_printk("first is dandling ");
            node = NULL;
            break;
        }

        if ( vfs_node_dirtrylock(parent) != 0 ) {
            vfs_printk("first is locked ");
            vfs_node_lru_rehash(node);
            vfs_node_refdrop(parent);
            node = NULL;
            break;
        }

        /*
          Check whether we have the last reference and
          parent did not change while we did the few lines above...
        */
        if ( vfs_node_refcount(node) == 1
             && parent == node->parent )
        {
            vfs_node_refnew(node);
            vfs_node_parent_nolock_unset(node);
            vfs_node_dirunlock(parent);

            /* We must have the last ref */
            assert( vfs_node_refcount(node) == 1 );
            old_storage_free = node->obj_entry.storage_free;
            node->obj_entry.storage_free = NULL;

            vfs_node_refdrop(node);

            /* Now node is distroyed, and ready to be created again */
            break;
        } else {
            vfs_node_dirunlock(parent);

            vfs_printk("first got dandling ");

            vfs_node_lru_rehash(node);
            node = NULL;
        }

        vfs_node_refdrop(parent);
    }

    vfs_printk("used %p from lru ", node);
    node = vfs_node_new(node, fs, mangled_name, fs_node);

    if ( old_storage_free != NULL )
        node->obj_entry.storage_free = old_storage_free;

    vfs_printk(">");

    return node;
}

struct vfs_node_s *vfs_node_get_parent(struct vfs_node_s *node)
{
    struct vfs_node_s *parent = NULL;

    lock_spin(&node->parent_lock);
	if ( !vfs_node_is_dandling(node) )
        parent = vfs_node_refnew(node->parent);
    lock_release(&node->parent_lock);

    return parent;
}

void
vfs_node_parent_nolock_unset(struct vfs_node_s *node)
{
    lock_spin(&node->parent_lock);
	if ( !vfs_node_is_dandling(node) ) {
        struct vfs_node_s *parent = node->parent;
        vfs_dir_remove(&parent->children, node);
        node->parent = NULL;
        vfs_node_refdrop(parent);
        vfs_node_lru_drop(node);
    }
    lock_release(&node->parent_lock);
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

