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

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <vfs/types.h>
#include <vfs/file.h>

#include "ramfs_file.h"
#include "ramfs-private.h"
#include "ramfs_data.h"

VFS_FILE_CLOSE(ramfs_file_close)
{
	if ( file->node->type == VFS_NODE_FILE ) {
        struct ramfs_data_s *rfs_data = file->priv;
		ramfs_data_refdrop(rfs_data);
    }
	vfs_file_refdrop(file);
	
	return 0;
}

VFS_FILE_READ(ramfs_file_read)
{
	struct ramfs_data_s *data = file->priv;
	ssize_t left = data->actual_size - file->offset;
	if ( size < left )
		left = size;
	if ( left < 0 )
		return -EINVAL;

	memcpy(buffer, (void*)(((uintptr_t)data->data)+file->offset), left);

	file->offset += left;

	return left;
}

VFS_FILE_READ(ramfs_dir_read)
{
	if ( size != sizeof(struct vfs_dirent_s) )
		return -EINVAL;

	uintptr_t cur = (uintptr_t)file->priv;

	struct fs_node_s *rfs_node = file->node;
    bool_t gotit = ramfs_dir_get_nth(rfs_node, buffer, cur);

    if ( gotit )
        file->priv = (void *)(uintptr_t)(cur+1);
    else
        file->priv = (void *)(uintptr_t)0;

	return gotit ? sizeof(struct vfs_dirent_s) : 0;
}

VFS_FILE_WRITE(ramfs_file_write)
{
	struct ramfs_data_s *data = file->priv;
	ssize_t offset_after = file->offset + size;
	if ( offset_after > (off_t)data->allocated_size ) {
		size_t new_size = offset_after;
		new_size |= 0xfff;
		new_size += 1;

		error_t err = ramfs_data_realloc(data, new_size);
		if ( err )
			return err;
	}

	memcpy((void*)(((uintptr_t)data->data)+file->offset), buffer, size);

	file->offset += size;
	if ( file->offset > (off_t)data->actual_size )
		data->actual_size = file->offset;

	return size;
}

VFS_FILE_TRUNCATE(ramfs_file_truncate)
{
    struct ramfs_data_s *data = file->priv;
    if ( new_size > (off_t)data->allocated_size ) {
        new_size |= 0xfff;
        new_size += 1;

        error_t err = ramfs_data_realloc(data, new_size);
        if ( err )
            return err;

        memset(data->data+data->actual_size, 0, new_size - data->actual_size);
    }

    data->actual_size = new_size;

    return 0;
}

VFS_FILE_WRITE(ramfs_file_append)
{
 	struct ramfs_data_s *data = file->priv;
    file->offset = data->actual_size;
    return ramfs_file_write(file, buffer, size);
}

VFS_FILE_SEEK(ramfs_file_seek)
{
	struct ramfs_data_s *data = file->priv;

	switch (whence) {
	case VFS_SEEK_SET:
		break;
	case VFS_SEEK_CUR:
		offset += file->offset;
		break;
	case VFS_SEEK_END:
		offset += data->actual_size;
		break;
	}

	if ( offset > (off_t)data->actual_size )
		offset = data->actual_size;
	if ( offset < 0 )
		offset = 0;

	file->offset = offset;

	return offset;
}


VFS_FS_NODE_OPEN(ramfs_node_open)
{
	vfs_printk("<ramfs_node_open %p %x ", node, flags);

	struct vfs_file_s *f = vfs_file_new(NULL, node, ramfs_node_refnew, ramfs_node_refdrop);
	if ( f == NULL ) {
		vfs_printk("err>");
		return -ENOMEM;
	}

	switch (node->type) {
	case VFS_NODE_FILE: {
		vfs_printk("file ");
		if ( flags & VFS_OPEN_READ )
            f->read = ramfs_file_read;
		if ( flags & VFS_OPEN_WRITE )
            f->write = ramfs_file_write;
        if ( flags & VFS_OPEN_APPEND )
            f->write = ramfs_file_append;
        if ( flags & VFS_OPEN_TRUNCATE )
            ramfs_file_truncate(f, 0);
		f->seek = ramfs_file_seek;
		f->priv = ramfs_data_refnew(node->data);
        f->close = ramfs_file_close;
        f->truncate = ramfs_file_truncate;
        break;
    }
	case VFS_NODE_DIR:
		vfs_printk("dir ");
		f->read = ramfs_dir_read;
		f->priv = (void *)(uintptr_t)0;
        f->close = ramfs_file_close;
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
