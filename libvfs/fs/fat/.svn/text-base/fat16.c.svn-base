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

#define FAT_16

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


static inline void fat16_cut_fat_cluster_no(
    struct fat_s *fat,
    cluster_t cluster_no,
    cluster_t *fat_block_no,
    cluster_t *fat_offset)
{
    *fat_block_no = (cluster_no >> (fat->sect_size_pow2 - 1)) + fat->fat_sect0;
    *fat_offset = cluster_no & ((1 << (fat->sect_size_pow2 - 1)) - 1);
}

static common_cluster_t fat16_entry_get(
    struct fat_s *fat,
    common_cluster_t cluster_no)
{
    uint16_t fat_sector;
    uint16_t fat_offset;
    fat16_cut_fat_cluster_no(fat, cluster_no, &fat_sector, &fat_offset);

    if ( fat_sector >= fat->fat_secsize )
        return CLUSTER_TO_COMMON(CLUSTER_END);

    error_t err = fat_sector_lock_and_load(fat->sector, fat->dev, fat_sector);
    if ( err )
        return CLUSTER_TO_COMMON(CLUSTER_END);
    uint16_t *data = (uint16_t*)fat->sector->data;

    uint16_t ret = endian_le16(data[fat_offset]);

    vfs_printk("<%s(%d) = %d>", __FUNCTION__, cluster_no, ret);

    fat_sector_lock_release(fat->sector);
    return CLUSTER_TO_COMMON(ret);
}

#if defined(CONFIG_DRIVER_FS_FAT_RW)
static common_cluster_t fat16_entry_find_free(
    struct fat_s *fat)
{
    size_t i;

    for ( i = fat->first_probable_free_cluster;
          i < fat->total_sector_count;
          ++i ) {
        cluster_t next_pointer = fat16_entry_get(fat, i);
        if ( next_pointer == 0 ) {
            fat->first_probable_free_cluster = i + 1;
            return i;
        }
    }
    return CLUSTER_TO_COMMON(CLUSTER_END);
}

static error_t fat16_entry_set(
    struct fat_s *fat,
    common_cluster_t cluster_no,
    common_cluster_t next_cluster_no)
{
    uint16_t fat_sector;
    uint16_t fat_offset;
    fat16_cut_fat_cluster_no(fat, cluster_no, &fat_sector, &fat_offset);

    error_t err = fat_sector_lock_and_load(fat->sector, fat->dev, fat_sector);
    if ( err )
        return err;

    uint16_t *data = (uint16_t*)fat->sector->data;
    data[fat_offset] = endian_le16(next_cluster_no);
    fat_sector_lock_release(fat->sector);
    return 0;
}
#endif

const struct fat_ops_s fat16_fat_ops =
{
    .entry_get = fat16_entry_get,
#if defined(CONFIG_DRIVER_FS_FAT_RW)
    .entry_set = fat16_entry_set,
    .entry_find_free = fat16_entry_find_free,
#endif
};
