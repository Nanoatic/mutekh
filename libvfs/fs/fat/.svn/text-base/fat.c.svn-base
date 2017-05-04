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

#include <hexo/endian.h>

#include <mutek/printk.h>
#include <mutek/mem_alloc.h>

#include <device/block.h>
#include <device/driver.h>
#include <device/device.h>
#include <vfs/types.h>
#include <vfs/file.h>
#include <vfs/fs.h>

#include "fat-sector-cache.h"
#include "fat.h"

#include "fat-defs.h"
#include "fat-private.h"

static const struct vfs_fs_ops_s fat_ops;

#if defined(CONFIG_DRIVER_FS_FAT12)
extern const struct fat_ops_s fat12_fat_ops;
#endif
#if defined(CONFIG_DRIVER_FS_FAT16)
extern const struct fat_ops_s fat16_fat_ops;
#endif
#if defined(CONFIG_DRIVER_FS_FAT32)
extern const struct fat_ops_s fat32_fat_ops;
#endif

// loads the bpb and parses it. This fuctnion is checking properties
// and yields error if they are not good
static error_t fat_parse_bpb(struct fat_s *state, struct fat_tmp_sector_s *sector)
{
	struct fat_bpb_s *bpb = (struct fat_bpb_s *)sector->data;
	const struct dev_block_params_s *params = dev_block_getparams(state->dev);

	error_t err = fat_sector_lock_and_load(sector, state->dev, 0);
	if ( err )
		return err;

/*     printk("FAT BPB:\n"); */
/*     hexdumpk(sector->data, params->blk_size); */

	if ( (sector->data[510] != 0x55)
		 || (sector->data[511] != 0xaa) ) {
		printk("FAT Error: 55AA marker not found\n");
		return -ENOENT;
	}

    uint16_t byte_per_sector = endian_le16(bpb->byte_per_sector);
    uint16_t reserved_sect_count = endian_le16(bpb->reserved_sect_count);
    uint16_t root_dirent_count = endian_le16(bpb->root_dirent_count);
    uint16_t total_sector_count16 = endian_le16(bpb->total_sector_count16);
    uint32_t total_sector_count32 = endian_le32(bpb->total_sector_count32);
    uint16_t fat_size16 = endian_le16(bpb->fat_size);
    uint32_t fat_size32 = endian_le32(bpb->bpb32.fat_size);
	if ( byte_per_sector != params->blk_size ) {
		printk("FAT Error: device sector size (%d) does not match FAT's (%d)\n",
			   params->blk_size, byte_per_sector);
		return -EINVAL;
	}

	state->total_sector_count = total_sector_count16
		? total_sector_count16
		: total_sector_count32;

	if ( state->total_sector_count < params->blk_count ) {
		printk("FAT Error: device size (%d blocks) is less than FAT's (%d blocks)\n",
			   params->blk_count, state->total_sector_count);
		return -EINVAL;
	}

	state->sect_size_pow2 = __builtin_ctz(byte_per_sector);
	state->sect_per_clust_pow2 = __builtin_ctz(bpb->sector_per_cluster);
	state->root_dir_secsize = root_dirent_count >> (state->sect_size_pow2 - 5);
	state->fat_count = bpb->fat_count;

    state->fat_secsize = ( fat_size16 == 0 ) ? fat_size32 : fat_size16;

#if defined(CONFIG_DRIVER_FS_FAT_TYPE_FROM_STRING)
    if ( ! memcmp(bpb->bpb16.volume_type, "FAT12   ", 8) ) {
        if ( fat_size16 == 0 ) {
            printk("This fat12 uses fat32 headers, bad\n");
            return -ENOTSUP;
        }
        state->type = FAT12;
    } else if ( ! memcmp(bpb->bpb16.volume_type, "FAT16   ", 8) ) {
        if ( fat_size16 == 0 ) {
            printk("This fat16 uses fat32 headers, bad\n");
            return -ENOTSUP;
        }
        state->type = FAT16;
    } else if ( ! memcmp(bpb->bpb32.volume_type, "FAT32   ", 8) ) {
        state->type = FAT32;
    }
#else    
    uint32_t data_clusters =
        (state->total_sector_count
         - reserved_sect_count
         - state->fat_secsize * state->fat_count
         - state->root_dir_secsize
            ) >> state->sect_per_clust_pow2;

    if ( data_clusters < 4085 ) {
        if ( fat_size16 == 0 ) {
            printk("This fat12 uses fat32 headers, bad\n");
            return -ENOTSUP;
        }
        state->type = FAT12;
    } else if ( data_clusters < 65525 ) {
        if ( fat_size16 == 0 ) {
            printk("This fat16 uses fat32 headers, bad\n");
            return -ENOTSUP;
        }
        state->type = FAT16;
    } else
        state->type = FAT32;
#endif

	state->fat_sect0 = reserved_sect_count;
	state->cluster0_sector =
		reserved_sect_count
		+ state->fat_secsize * state->fat_count
		+ state->root_dir_secsize
		- (2 << state->sect_per_clust_pow2);

    if (state->type == FAT32) {
        state->root_dir_base = endian_le32(bpb->bpb32.root_cluster);
    } else {
        state->root_dir_base = reserved_sect_count
            + state->fat_secsize * state->fat_count;
    }
    state->first_probable_free_cluster = 2;

    printk("FAT%d decoded BPB:\n", state->type);
    printk(" total_sector_count:  %d\n", state->total_sector_count);
    printk(" fat_secsize:         %d\n", state->fat_secsize);
    printk(" cluster0_sector:     %d\n", state->cluster0_sector);
    printk(" root_dir_secsize:    %d\n", state->root_dir_secsize);
    printk(" root_dir_base:       %d\n", state->root_dir_base);
    printk(" fat_sect0:           %d\n", state->fat_sect0);
    printk(" first_probable_free_cluster: %d\n",
                                         state->first_probable_free_cluster);
    printk(" sect_size_pow2:      %d\n", state->sect_size_pow2);
    printk(" sect_per_clust_pow2: %d\n", state->sect_per_clust_pow2);
    printk(" fat_count:           %d\n", state->fat_count);

    fat_sector_lock_release(sector);
	return 0;
}

error_t fat_open(struct device_s *dev, struct vfs_fs_s **fs)
{
	error_t err = -ENOMEM;
	const struct dev_block_params_s *params = dev_block_getparams(dev);
    struct fat_s *fat = mem_alloc(
        sizeof(struct fat_s)
        +sizeof(struct fat_tmp_sector_s)
        +params->blk_size,
        mem_scope_sys);
    struct fs_node_s *root;

	if ( fat == NULL )
		goto cant_alloc;

    struct vfs_fs_s *mnt = vfs_fs_new(&fat->fs);

    fat->sector = (struct fat_tmp_sector_s*)(fat+1);
    fat->sector->lba = (dev_block_lba_t)-1;
    fat->sector->dirty = 0;
    semaphore_init(&fat->sector->semaphore, 1);
	fat->dev = dev;

	err = fat_parse_bpb(fat, fat->sector);
	if ( err )
		goto cant_open;

	atomic_set(&mnt->ref, 0);
	mnt->ops = &fat_ops;

    switch (fat->type) {
#if defined(CONFIG_DRIVER_FS_FAT12)
    case FAT12:
        fat->ops = &fat12_fat_ops;
        break;
#endif
#if defined(CONFIG_DRIVER_FS_FAT16)
    case FAT16:
        fat->ops = &fat16_fat_ops;
        break;
#endif
#if defined(CONFIG_DRIVER_FS_FAT32)
    case FAT32:
        fat->ops = &fat32_fat_ops;
        break;
#endif
    default:
        err = -ENOTSUP;
        goto cant_open;
    }

    printk("fat: opening new fat%d volume\n", fat->type);

	mnt->old_node = NULL;
    fat_node_pool_init(&fat->nodes);

    root = fat_node_new(
        NULL, fat,
        fat->type == FAT16 ? 0 : fat->root_dir_base,
        fat->root_dir_secsize << fat->sect_size_pow2,
        VFS_NODE_DIR);
	if ( root == NULL )
		goto cant_open;

#if !defined(CONFIG_DRIVER_FS_FAT_RW)
    mnt->ro = 1;
#endif

	mnt->root = root;

	*fs = mnt;
	return 0;

  cant_open:
	mem_free(mnt);
  cant_alloc:
	return err;
}

VFS_FS_CAN_UNMOUNT(fat_can_unmount)
{
    // TODO copy first FAT on others

    return 0;
}

static const struct vfs_fs_ops_s fat_ops =
{
    .node_open = fat_node_open,
    .lookup = fat_lookup,
#if defined(CONFIG_DRIVER_FS_FAT_RW)
    .create = fat_create,
    .link = fat_link,
    .move = fat_move,
    .unlink = fat_unlink,
#endif
    .stat = fat_stat,
    .can_unmount = fat_can_unmount,
    .node_refdrop = fat_node_refdrop,
    .node_refnew = fat_node_refnew,
};

void fat_str_to_lower(char *str, size_t size)
{
    size_t i;
    for ( i = 0; i < size; ++i ) {
        if ( str[i] < 'A' || str[i] > 'Z' )
            continue;
        str[i] += 'a' - 'A';
    }
}
