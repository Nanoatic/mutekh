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

#include <device/device.h>
#include <device/char.h>
#include <device/block.h>
#include <device/enum.h>
#include <device/driver.h>

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <vfs/types.h>
#include <vfs/file.h>

#include "devfs-private.h"

VFS_FILE_CLOSE(devfs_file_close)
{
	vfs_file_refdrop(file);
	
	return 0;
}

VFS_FILE_READ(devfs_chardev_read)
{
    struct device_s *dev = file->node->dev;

    return dev_char_wait_read(dev, buffer, size);
}

VFS_FILE_WRITE(devfs_chardev_write)
{
    struct device_s *dev = file->node->dev;

    return dev_char_wait_write(dev, buffer, size);
}

VFS_FILE_READ(devfs_dir_read)
{
	if ( size != sizeof(struct vfs_dirent_s) )
		return -EINVAL;

	off_t cur = file->offset;

	struct fs_node_s *dfs_node = file->node;
    bool_t gotit = devfs_dir_get_nth(dfs_node, buffer, cur);

    file->offset = gotit ? (file->offset + 1) : 0;

	return gotit ? sizeof(struct vfs_dirent_s) : 0;
}

VFS_FS_NODE_OPEN(devfs_node_open)
{
	vfs_printk("<devfs_node_open %p %x ", node, flags);

	struct vfs_file_s *f = vfs_file_new(NULL, node, devfs_node_refnew, devfs_node_refdrop);
	if ( f == NULL ) {
		vfs_printk("err>");
		return -ENOMEM;
	}

	switch (node->type) {
	case DEV_DEVICE_HANDLE: {
        switch ( node->dev->drv->class ) {
        case device_class_char:
            if ( flags & VFS_OPEN_READ )
                f->read = devfs_chardev_read;
            if ( flags & VFS_OPEN_WRITE )
                f->write = devfs_chardev_write;
            f->close = devfs_file_close;
            break;
        case device_class_block:
        default:
            vfs_file_refdrop(f);
            return -ENOTSUP;
        }
        break;
    }
	case DEV_DEVICE_DIR:
		vfs_printk("dir ");
		f->read = devfs_dir_read;
        f->close = devfs_file_close;
		break;
	}

	*file = f;
	vfs_printk("ok: %p>", f);
	return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
