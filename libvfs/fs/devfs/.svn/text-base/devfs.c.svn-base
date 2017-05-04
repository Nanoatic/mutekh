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

#include <hexo/types.h>
#include <hexo/error.h>

#include <device/device.h>
#include <device/enum.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <vfs/types.h>
#include <vfs/fs.h>
#include <vfs/ops.h>

#include <stdio.h>

#include "devfs.h"
#include "devfs-private.h"

OBJECT_CONSTRUCTOR(devfs_node)
{
    obj->fs = va_arg(ap, struct vfs_fs_s *);
    obj->type = va_arg(ap, enum devfs_node_type_e);
    obj->dev = device_obj_refnew(va_arg(ap, struct device_s *));

    atomic_inc(&obj->fs->ref);
    return 0;
}

OBJECT_DESTRUCTOR(devfs_node)
{
    device_obj_refdrop(obj->dev);
    atomic_dec(&obj->fs->ref);
}


VFS_FS_CAN_UNMOUNT(devfs_can_unmount)
{
	return atomic_get(&fs->ref) == 1;
}

VFS_FS_LOOKUP(devfs_lookup)
{
    struct device_s *dev = ref->dev;

    /* Undriven devices have no children */
    if ( ! dev->drv )
        return -EINVAL;

    if ( ! strncmp(name, "handle", namelen) ) {
        /* For nodes that can be open as files, implement "handle" entry */
        switch ( dev->drv->class ) {
        case device_class_block:
        case device_class_char:
            *node = devfs_node_new(NULL, ref->fs, DEV_DEVICE_HANDLE, dev);
            if ( *node == NULL )
                return -ENOMEM;
            vfs_name_mangle("handle", 6, mangled_name);
            return 0;
        default:
            break;
        }
    }

    if ( device_list_isempty(&dev->children) )
        return -ENOENT;

    char _name[namelen+1];
    memcpy(_name, name, namelen);
    _name[namelen] = 0;

    struct device_s *child = NULL;

    if ( dev->drv->class == device_class_enum ) {
        child = dev_enum_lookup(dev, _name);
    } else if ( ! strncmp("child", name, 5) ) {
        size_t n = ato_intl16(name+5);
        child = device_get_child(dev, n);
    }

    if ( child == NULL )
        return -ENOENT;

    vfs_name_mangle(name, namelen, mangled_name);

    *node = devfs_node_new(NULL, ref->fs, DEV_DEVICE_DIR, child);
    return 0;
}

VFS_FS_STAT(devfs_stat)
{
/*     struct device_s *dev = node->dev; */
/*     enum device_class_e class = device_class_none; */
/*     if ( dev->drv ) { */
/*         class = dev->drv->class; */
/*     } */

    stat->size = 0;
    stat->nlink = 1;

    // TODO put dev type
    stat->type = node->type == DEV_DEVICE_DIR ? VFS_NODE_DIR : VFS_NODE_FILE;

	return 0;
}

error_t devfs_close(struct vfs_fs_s *fs)
{
    mem_free(fs);
    return 0;
}

static const struct vfs_fs_ops_s devfs_ops =
{
    .node_open = devfs_node_open,
    .lookup = devfs_lookup,
    .stat = devfs_stat,
    .can_unmount = devfs_can_unmount,
    .node_refdrop = devfs_node_refdrop,
    .node_refnew = devfs_node_refnew,
};

extern struct device_s enum_root;

error_t devfs_open(struct vfs_fs_s **fs)
{
	struct vfs_fs_s *mnt = vfs_fs_new(NULL);
	if ( mnt == NULL )
		goto nomem_fs;

	atomic_set(&mnt->ref, 0);

    mnt->ops = &devfs_ops;
	mnt->old_node = NULL;

    struct fs_node_s *root = devfs_node_new(NULL, mnt, DEV_DEVICE_DIR, &enum_root);
	if ( root == NULL )
		goto nomem_dir;

	mnt->root = root;

	*fs = mnt;

	return 0;
  nomem_dir:
	mem_free(mnt);
  nomem_fs:
	return -ENOMEM;
}

bool_t devfs_dir_get_nth(struct fs_node_s *node, struct vfs_dirent_s *dirent, size_t n)
{
    struct device_s *dev = node->dev;

    /* Undriven devices have no children */
    if ( ! dev->drv )
        return 0;

    /* For nodes that can be open as files, create a "handle" entry */
    switch ( dev->drv->class ) {
    case device_class_block:
    case device_class_char:
        if ( n == 0 ) {
            strcpy(dirent->name, "handle");
            // TODO: dev type !
            dirent->type = VFS_NODE_FILE;
            dirent->size = 0;
            return 1;
        }
        --n;
        break;
    default:
        break;
    }

    struct device_s *child = device_get_child(dev, n);
    if ( child == NULL )
        return 0;

    dirent->type = VFS_NODE_DIR;
    dirent->size = 0;
    if ( dev->drv->class == device_class_enum
         && dev->drv->f.denum.f_info ) {
        struct dev_enum_info_s info;

        dev_enum_info(dev, child, &info);
        strcpy(dirent->name, info.path);
    } else {
        snprintf(dirent->name, CONFIG_VFS_NAMELEN, "child%d", n);
    }
    return 1;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

