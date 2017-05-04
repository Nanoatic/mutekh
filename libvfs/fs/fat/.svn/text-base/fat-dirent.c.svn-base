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
#include <hexo/endian.h>
#include <vfs/fs.h>
#include <vfs/file.h>
#include "fat-private.h"
#include "fat-defs.h"
#include "fat-sector-cache.h"

void fat_name_to_vfs(char *dst, const char *src)
{
    memset(dst, 0, FAT_83_NAMELEN);
    size_t i;
    for ( i=0; i<11; ++i ) {
        if ( src[i] != ' ' )
            *dst++ = src[i];
        if ( i == 7 )
            *dst++ = '.';
    }
    if ( dst[-1] == '.' )
        dst[-1] = 0;
}

#if defined(CONFIG_DRIVER_FS_FAT_RW)
VFS_FS_CREATE(fat_create)
{
    return -ENOTSUP;
}

VFS_FS_LINK(fat_link)
{
    return -ENOTSUP;
}

VFS_FS_MOVE(fat_move)
{
    return -ENOTSUP;
}

VFS_FS_UNLINK(fat_unlink)
{
    return -ENOTSUP;
}
#endif

VFS_FS_STAT(fat_stat)
{
    stat->type = node->type;
    stat->size = node->type == VFS_NODE_DIR ? 0 : node->file_size;
    stat->nlink = 1;

    return 0;
}

VFS_FS_LOOKUP(fat_lookup)
{
    struct fat_file_s *ffile = fat_file_new(NULL, ref);
    struct fat_s *fat = ref->fat;

    if ( ffile == NULL )
        return -ENOMEM;

    union fat_dirent_u fat_dirent[1];
    char name_83[FAT_83_NAMELEN];

    off_t offset = 0;

    do {
        ssize_t r = fat_get_next_dirent(ffile, &offset, fat_dirent, name_83, mangled_name);
        if ( r != 1 ) {
            fat_file_refdrop(ffile);
            return -ENOENT;
        }

        vfs_printk("<%s: @%d wanted: '%s', 8.3: '%s', lfn: '%s'>",
                   __FUNCTION__, offset, name, name_83, mangled_name);

        if ( !strncasecmp(mangled_name, name, namelen) || !strncasecmp(name_83, name, namelen) )
            break;
    } while (1);

    fat_file_refdrop(ffile);

    common_cluster_t cluster =
        (endian_le16(fat_dirent->old.clust_hi) << 16)
        | endian_le16(fat_dirent->old.clust_lo);

    struct fs_node_s *rnode = fat_node_pool_lookup(&fat->nodes, cluster);
    if ( rnode == NULL ) {
        rnode = fat_node_new(NULL, fat, cluster,
                             (fat_dirent->old.attr & ATTR_DIRECTORY) ? 0 : endian_le32(fat_dirent->old.file_size),
                             (fat_dirent->old.attr & ATTR_DIRECTORY) ? VFS_NODE_DIR : VFS_NODE_FILE);
    }

    if ( rnode == NULL )
        return -ENOMEM;
    
    *node = rnode;

    return 0;
}

VFS_FILE_READ(fat_dir_read)
{
    struct fat_file_s *ffile = file->priv;

    union fat_dirent_u fat_dirent[1];
    struct vfs_dirent_s *vfs_dirent = buffer;
    char name_83[FAT_83_NAMELEN];

    if ( size != sizeof(*vfs_dirent) )
        return -EINVAL;

    do {
        ssize_t r = fat_get_next_dirent(ffile, &file->offset, fat_dirent, name_83, vfs_dirent->name);
        if ( r != 1 )
            return 0;

        if ( vfs_dirent->name[0] == 0 )
            strcpy(vfs_dirent->name, name_83);

        vfs_dirent->size = endian_le32(fat_dirent->old.file_size);
        vfs_dirent->type = fat_dirent->old.attr & ATTR_DIRECTORY
            ? VFS_NODE_DIR
            : VFS_NODE_FILE;

        return sizeof(*vfs_dirent);
    } while (1);
}
