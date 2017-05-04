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
#include <vfs/fs.h>
#include <vfs/file.h>

#include "fat-private.h"
#include "fat-defs.h"
#include "fat-sector-cache.h"

OBJECT_CONSTRUCTOR(fat_file)
{
    obj->extent = fat_node_refnew(va_arg(ap, struct fs_node_s *));

    obj->cluster_index = 0;
    obj->zone_start = obj->extent->first_cluster;
    obj->zone_end = obj->extent->first_cluster + 1;
    fat_file_list_pushback(&obj->extent->files, obj);

    return 0;
}

OBJECT_DESTRUCTOR(fat_file)
{
    fat_file_list_remove(&obj->extent->files, obj);
    fat_node_refdrop(obj->extent);
}

OBJECT_CONSTRUCTOR(fat_node)
{
    obj->fat = va_arg(ap, struct fat_s *);
    obj->first_cluster = va_arg(ap, uint32_t);
    obj->file_size = va_arg(ap, size_t);
    obj->type = va_arg(ap, enum vfs_node_type_e);

    vfs_printk("<%s: fc: %d size: %d %s>",
               __FUNCTION__, obj->first_cluster,
               obj->file_size, obj->type == VFS_NODE_DIR ? "dir" : "file");

    fat_file_list_init(&obj->files);
    semaphore_init(&obj->lock, 1);

    fat_node_pool_push(&obj->fat->nodes, obj);

    return 0;
}

OBJECT_DESTRUCTOR(fat_node)
{
    fat_file_list_destroy(&obj->files);
    semaphore_destroy(&obj->lock);
}

static inline void dump_file(const struct fat_file_s *file)
{
    vfs_printk("<fat file: first cluster: %d, size: %d, zone: @%d %d-%d>",
               file->extent->first_cluster, file->extent->file_size,
               file->cluster_index, file->zone_start, file->zone_end);
}

/** @this must be called with file->extent->lock taken. */
static
common_cluster_t fat_file_translate_cluster(
    struct fat_file_s *file,
    common_cluster_t cluster
    )
{
    common_cluster_t ret = cluster - file->cluster_index + file->zone_start;

    if ( ret < file->zone_end )
        return ret;

    return 0;
}

/** @this must be called with file->extent->lock wrlocked. */
static
common_cluster_t fat_file_get_cluster(
    struct fat_file_s *file,
    common_cluster_t cluster)
{
    struct fs_node_s *node = file->extent;
    common_cluster_t ret = 0;

    ret = fat_file_translate_cluster(file, cluster);
    if ( ret )
        goto found;

    /* First, get the most advanced in all other file for the same node */
    struct fat_file_s *best = NULL;
    CONTAINER_FOREACH(fat_file_list, CLIST, &node->files, {
            if ( (item->cluster_index < cluster) &&
                 ((best == NULL) || (item->cluster_index > best->cluster_index)) )
                best = item;
        });

    /* Copy the best one */
    if ( best == file ) {
        /* Noop */
    } else if ( best ) {
        file->cluster_index = best->cluster_index;
        file->zone_start = best->zone_start;
        file->zone_end = best->zone_end;
    } else {
        file->cluster_index = 0;
        file->zone_start = node->first_cluster;
        file->zone_end = node->first_cluster + 1;
    }

    /* Walk for the rest */
    while ( (ret = fat_file_translate_cluster(file, cluster)) == 0 ) {
        ret = node->fat->ops->entry_get(node->fat, file->zone_end - 1);
        if ( fat_entry_is_end(ret) ) {
            ret = 0;
            break;
        } else if ( ret == file->zone_end ) {
            file->zone_end += 1;
        } else {
            file->cluster_index += file->zone_end - file->zone_start;
            file->zone_start = ret;
            file->zone_end = ret + 1;
        }
    }

  found:
    return ret;
}

/**
   @this retrieves the maximal contiguous range of sectors from the
   current point in file (which MUST be aligned on a sector boundary).

   last_sector is the first sector after contiguous zone.
*/
static
error_t fat_file_get_biggest_sector_range(
    struct fat_file_s *ffile, off_t offset,
    sector_t *first_sector, sector_t *last_sector)
{
    error_t err = -EUNKNOWN;
    struct fs_node_s *node = ffile->extent;
    struct fat_s *fat = node->fat;

    sector_t vsector = offset >> fat->sect_size_pow2;
    common_cluster_t vcluster = vsector >> fat->sect_per_clust_pow2;
    sector_t psector_mask = (1 << fat->sect_per_clust_pow2) - 1;
    sector_t psector_offset = (vsector & psector_mask) ^ psector_mask;
    common_cluster_t pcluster;

    semaphore_take(&node->lock, 1);
    pcluster = fat_file_get_cluster(ffile, vcluster);
    if ( pcluster == 0 )
        goto err;

    *first_sector = fat->cluster0_sector + (pcluster << fat->sect_per_clust_pow2) + psector_offset;
    // Catch up as far as possible, therefore, from the zone end.
    pcluster = ffile->zone_end;
    for (;;) {
        // remember zone_end is the first cluster after contiguous zone.
        pcluster = fat->ops->entry_get(fat, pcluster-1);
        if ( pcluster == ffile->zone_end )
            ffile->zone_end += 1;
        else
            break;
    }
    *last_sector = fat->cluster0_sector + (ffile->zone_end << fat->sect_per_clust_pow2);

    err = 0;

  err:
    semaphore_give(&node->lock, 1);
    return err;
}

static
sector_t fat_file_get_sector(struct fat_file_s *ffile, off_t offset)
{
    struct fs_node_s *node = ffile->extent;
    struct fat_s *fat = node->fat;

    sector_t vsector = offset >> fat->sect_size_pow2;
    common_cluster_t vcluster = vsector >> fat->sect_per_clust_pow2;
    sector_t psector_mask = (1 << fat->sect_per_clust_pow2) - 1;
    sector_t psector_offset = vsector & psector_mask;
    common_cluster_t pcluster;

    semaphore_take(&node->lock, 1);
    pcluster = fat_file_translate_cluster(ffile, vcluster);
    if ( pcluster )
        goto found;
    pcluster = fat_file_get_cluster(ffile, vcluster);
    if ( pcluster )
        goto found;
    semaphore_give(&node->lock, 1);
    return 0;

    vfs_printk("<%s: ", __FUNCTION__);
    dump_file(ffile);
    vfs_printk(" %d->%d>", pcluster, psector_offset + (pcluster << fat->sect_per_clust_pow2) + fat->cluster0_sector);

  found:
    semaphore_give(&node->lock, 1);
    return psector_offset + (pcluster << fat->sect_per_clust_pow2) + fat->cluster0_sector;
}

VFS_FILE_CLOSE(fat_file_close)
{
    struct fat_file_s *ffile = file->priv;
    fat_file_refdrop(ffile);
    return 0;
}

static
ssize_t fat_file_read_part(struct fat_file_s *ffile,
                           off_t offset,
                           void *buffer, size_t size)
{
    struct fat_s *fat = ffile->extent->fat;

    assert(size < (1<<fat->sect_size_pow2));

    sector_t sect = fat_file_get_sector(ffile, offset);

    if ( sect == 0 )
        return 0;

    off_t block_offset = offset & ((1 << fat->sect_size_pow2) - 1);

    vfs_printk("<%s %d bytes @ %d, block %d>", __FUNCTION__, size, block_offset, sect);
    dump_file(ffile);

    error_t err = fat_sector_lock_and_load(fat->sector, fat->dev, sect);
    if ( err )
        return -err;

    memcpy(buffer, fat->sector->data+block_offset, size);
    fat_sector_lock_release(fat->sector);

    return size;
}

static
ssize_t fat_file_read_aligned_sectors(
    struct fat_file_s *ffile,
    off_t offset,
    void *buffer, size_t size)
{
    struct fat_s *fat = ffile->extent->fat;
    struct device_s *dev = fat->dev;
    sector_t max = size >> fat->sect_size_pow2;

    sector_t begin, end;
    error_t err = fat_file_get_biggest_sector_range(ffile, offset, &begin, &end);
    if ( err )
        return err;

    max = __MIN(max, end-begin);
    uint8_t *blocks[max];
    sector_t i;
    for ( i = 0; i < max; ++i )
        blocks[i] = ((uint8_t*)buffer) + (i << fat->sect_size_pow2);

    dump_file(ffile);
    vfs_printk("<%s %d bytes @ %d, block %d, +%d>", __FUNCTION__, size, offset, begin, max);

    err = dev_block_wait_read(dev, blocks, begin, max);
    if ( err )
        return -err;

    return max << fat->sect_size_pow2;
}

#if defined(CONFIG_DRIVER_FS_FAT16)
/* If we only compile FAT32 code, we dont need this hack */
static
ssize_t fat_root_read(
    struct fat_file_s *ffile,
    off_t offset,
    void *buffer, size_t size)
{
    struct fat_s *fat = ffile->extent->fat;

    size_t block_size = (1 << fat->sect_size_pow2);
    size_t block_mask = (block_size - 1);
    size_t block_offset = offset & block_mask;
    sector_t sect = (offset >> fat->sect_size_pow2)
        + fat->root_dir_base;

    if ( offset >= ffile->extent->file_size )
        return 0;

    if ( size + block_offset > block_size )
        size = block_size - block_offset;

    vfs_printk("<%s %d bytes @ %d, block %d>", __FUNCTION__, size, block_offset, sect);

    error_t err = fat_sector_lock_and_load(fat->sector, fat->dev, sect);
    if ( err )
        return -err;

    memcpy(buffer, fat->sector->data+block_offset, size);
    fat_sector_lock_release(fat->sector);

    return size;
}
#endif

ssize_t fat_data_read(
    struct fat_file_s *ffile,
    off_t offset,
    void *buffer, size_t size)
{
#if defined(CONFIG_DRIVER_FS_FAT16)
    if ( ffile->extent->first_cluster == 0 )
        return fat_root_read(ffile, offset, buffer, size);
#endif

    uint8_t *data = buffer;
    size_t sector_size = (1 << ffile->extent->fat->sect_size_pow2);
    size_t sector_mask = sector_size - 1;
    size_t sect_offset = offset & sector_mask;
    ssize_t left = size;

    vfs_printk("<%s %d bytes @ %d>", __FUNCTION__, size, offset);

    size_t part_size = sect_offset ? (sector_size - sect_offset) : 0;
    part_size = __MIN(size, part_size);
    ssize_t ret = 0;

    if ( part_size ) {
        ret = fat_file_read_part(ffile, offset, data, part_size);
        if (ret != part_size)
            goto err;
        data += part_size;
        left -= part_size;
        offset += part_size;
    }
    assert(((offset & sector_mask) == 0) || (left == 0));

    vfs_printk("<%s after part. left: %d bytes @ %d>", __FUNCTION__, size, offset);

    while ( left & ~sector_mask ) {
        part_size = left & ~sector_mask;
        ret = fat_file_read_aligned_sectors(
            ffile, offset, data, part_size);
        if (ret <= 0)
            goto err;
        data += ret;
        left -= ret;
        offset += ret;

        vfs_printk("<%s after aligned read. left: %d bytes @ %d>", __FUNCTION__, size, offset);
    }

    ret = 0;
    if ( left )
        ret = fat_file_read_part(ffile, offset, data, left);

  err:
    if (ret < 0)
        return ret;

    left -= ret;

    vfs_printk("<%s after all. size: %d left: %d bytes @ %d>", __FUNCTION__, size, left, offset);

    return size-left;
}

VFS_FILE_READ(fat_file_read)
{
    struct fat_file_s *ffile = file->priv;

    if ( file->offset >= ffile->extent->file_size )
        return 0;

    size = __MIN(ffile->extent->file_size - file->offset, size);

    ssize_t r = fat_data_read(ffile, file->offset, buffer, size);
    if ( r > 0 )
        file->offset += r;
    return r;
}

#if defined(CONFIG_DRIVER_FS_FAT_RW)
VFS_FILE_WRITE(fat_file_write)
{
    struct fat_file_s *ffile = file->priv;
    (void)ffile;
    return -ENOTSUP;
}
#endif

VFS_FILE_SEEK(fat_file_seek)
{
    struct fat_file_s *ffile = file->priv;

	switch (whence) {
	case VFS_SEEK_SET:
		break;
	case VFS_SEEK_CUR:
		offset += file->offset;
		break;
	case VFS_SEEK_END:
		offset += ffile->extent->file_size;
		break;
	}

	if ( offset > (off_t)ffile->extent->file_size )
		offset = ffile->extent->file_size;
	if ( offset < 0 )
		offset = 0;

	file->offset = offset;

	return offset;
}

VFS_FS_NODE_OPEN(fat_node_open)
{
    struct fat_file_s *ffile = fat_file_new(NULL, node);
    if (ffile == NULL)
        return -ENOMEM;

    struct vfs_file_s *rfile = vfs_file_new(
        NULL, ffile, fat_file_refnew, fat_file_refdrop);
    if (rfile == NULL) {
        fat_file_refdrop(ffile);
        return -ENOMEM;
    }

    switch (node->type) {
	case VFS_NODE_FILE: {
		if ( flags & VFS_OPEN_READ )
            rfile->read = fat_file_read;
#if defined(CONFIG_DRIVER_FS_FAT_RW)
		if ( flags & VFS_OPEN_WRITE )
            rfile->write = fat_file_write;
#endif
		rfile->seek = fat_file_seek;
        break;
    }
	case VFS_NODE_DIR:
		rfile->read = fat_dir_read;
		break;
    }
    rfile->close = fat_file_close;

    rfile->priv = ffile;

    *file = rfile;
    return 0;
}


// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
