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

#ifndef _FAT_PRIVATE_H_
#define _FAT_PRIVATE_H_

#include <hexo/types.h>
#include <vfs/fs.h>
#include <vfs/types.h>
#include <vfs/file.h>

#include <mutek/semaphore.h>

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>
#include <gpct/object_refcount.h>

#include "fat-defs.h"

struct fat_s;
struct fs_node_s;
struct fat_ops_s;

struct fat_file_s;

OBJECT_TYPE     (fat_file, REFCOUNT, struct fat_file_s);
OBJECT_PROTOTYPE(fat_file, static inline, fat_file);

#define CONTAINER_LOCK_fat_file_list HEXO_SPIN
#define CONTAINER_OBJ_fat_file_list fat_file

CONTAINER_TYPE(fat_file_list, CLIST,
struct fat_file_s
{
    CONTAINER_ENTRY_TYPE(CLIST) list_entry;
    common_cluster_t cluster_index;      // index from start of *file*
	common_cluster_t zone_start; // index of first contiguous block in fat
	common_cluster_t zone_end;   // index of last contiguous block in fat + 1
    struct fs_node_s *extent;
    fat_file_entry_t obj_entry;
}, list_entry);

OBJECT_CONSTRUCTOR(fat_file);
OBJECT_DESTRUCTOR(fat_file);

CONTAINER_FUNC(fat_file_list, CLIST, static inline, fat_file_list, list_entry);

#ifdef __MKDOC__
struct fat_file_s *
fat_file_new(void *storage, struct fs_node_s *node);
#endif




OBJECT_TYPE     (fat_node, REFCOUNT, struct fs_node_s);
OBJECT_PROTOTYPE(fat_node, static inline, fat_node);

#define CONTAINER_LOCK_fat_node_pool HEXO_SPIN
#define CONTAINER_OBJ_fat_node_pool fat_node

CONTAINER_TYPE(fat_node_pool, HASHLIST,
struct fs_node_s
{
    CONTAINER_ENTRY_TYPE(HASHLIST) hash_entry;
    fat_node_entry_t obj_entry;
    struct fat_s *fat;
    fat_file_list_root_t files;
    struct semaphore_s lock;
    uint32_t file_size;
    common_cluster_t first_cluster;
    enum vfs_node_type_e type;
}, hash_entry, 5);

CONTAINER_KEY_TYPE(fat_node_pool, PTR, SCALAR, first_cluster);

OBJECT_CONSTRUCTOR(fat_node);
OBJECT_DESTRUCTOR(fat_node);

CONTAINER_FUNC(fat_node_pool, HASHLIST, static inline, fat_node_pool, first_cluster);
CONTAINER_KEY_FUNC(fat_node_pool, HASHLIST, static inline, fat_node_pool, first_cluster);

#ifdef __MKDOC__
struct fs_node_s *
fat_node_new(void *storage,
             struct fat_s *fat,
             common_cluster_t first_cluster,
             size_t file_size,
             enum vfs_node_type_e type);
#endif


OBJECT_FUNC(fat_file, REFCOUNT, static inline, fat_file, obj_entry);
OBJECT_FUNC(fat_node, REFCOUNT, static inline, fat_node, obj_entry);




#define FAT_ENTRY_GET(x) common_cluster_t (x)                       \
    (struct fat_s *fat,                                             \
     common_cluster_t cluster)

#if defined(CONFIG_DRIVER_FS_FAT_RW)
# define FAT_ENTRY_SET(x) error_t (x)                                   \
    (struct fat_s *fat,                                             \
     common_cluster_t cluster, common_cluster_t next)

# define FAT_ENTRY_FIND_FREE(x) common_cluster_t (x)                 \
    (struct fat_s *fat)
#endif

struct fat_ops_s
{
    FAT_ENTRY_GET(*entry_get);
#if defined(CONFIG_DRIVER_FS_FAT_RW)
    FAT_ENTRY_SET(*entry_set);
    FAT_ENTRY_FIND_FREE(*entry_find_free);
#endif
};

enum fat_type_e
{
    FAT32 = 32,
    FAT16 = 16,
    FAT12 = 12,
};

struct fat_s
{
    struct vfs_fs_s fs;
	struct device_s *dev;
    const struct fat_ops_s *ops;
	common_cluster_t root_dir_secsize;
    // This holds a sector for fat16.
    common_cluster_t root_dir_base;
	common_cluster_t first_probable_free_cluster;
	sector_t total_sector_count;
	sector_t fat_secsize;
	sector_t cluster0_sector; /* == first_data_sector - (2 << sect_per_clust_pow2) */
	sector_t fat_sect0;
    fat_node_pool_root_t nodes;
    struct fat_tmp_sector_s *sector;
	uint8_t sect_size_pow2;
	uint8_t sect_per_clust_pow2;
	uint8_t fat_count;
	enum fat_type_e type;
};

static inline
struct fat_s *fs2fat(struct vfs_fs_s *fs)
{
    return (struct fat_s *)fs;
}

void fat_name_to_vfs(char *dst, const char *src);

union fat_dirent_u;

error_t fat_get_next_dirent(struct fat_file_s *ffile,
                            off_t *offset,
                            union fat_dirent_u *dirent,
                            char *name_83,
                            char *vfs_mangled_name);

ssize_t fat_data_read(
    struct fat_file_s *ffile,
    off_t offset,
    void *buffer, size_t size);

static inline bool_t fat_entry_is_end(common_cluster_t cluster)
{
#if defined(CONFIG_DRIVER_FS_FAT32)
    return ((cluster | 0xf0000007) + 1) == 0;
#else
    return ((cluster | 0x7) + 1) == 0;
#endif
}

VFS_FILE_READ(fat_dir_read);

void fat_str_to_lower(char *str, size_t size);

/*
  Common FAT FS operations
 */

VFS_FS_LOOKUP(fat_lookup);
VFS_FS_CREATE(fat_create);
VFS_FS_LINK(fat_link);
VFS_FS_MOVE(fat_move);
VFS_FS_UNLINK(fat_unlink);
VFS_FS_STAT(fat_stat);

VFS_FILE_CLOSE(fat_file_close);
VFS_FILE_READ(fat_dir_read);
VFS_FILE_READ(fat_file_read);
VFS_FILE_WRITE(fat_file_write);
VFS_FILE_SEEK(fat_file_seek);

#endif
