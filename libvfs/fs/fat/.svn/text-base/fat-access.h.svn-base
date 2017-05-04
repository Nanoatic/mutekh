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

#ifndef _FAT_ACCESS_H_
#define _FAT_ACCESS_H_

#include <hexo/types.h>

struct fat_s;
struct fat_extent_s;

/*
  @this opens a block device for reading as a FAT. It may return
  -EINVAL or -ENOENT if the FAT is not correctly detected.
 */
#define FAT_BACKEND_OPEN(x) error_t (x)(struct device_s *dev, struct fat_s **fat, uint32_t flags)

/*
  @this writes back all extents chained lists. (free list and open
  files)
 */
#define FAT_BACKEND_SYNC(x) error_t (x)(struct fat_s *fat)
/*
  When this function is called, it is always valid (this is up to the
  fs layer to check for validity)
 */
#define FAT_BACKEND_CLOSE(x) error_t (x)(struct fat_s *fat)

/*
  @this allocates a new extent chain for storing a new file. @this
  reserves the clusters in the FAT.
 */
#define FAT_EXTENT_GET_NEW(x) error_t (x)(struct fat_s *fat, size_t clusters, struct fat_extent_s **extent)
/*
  @this allocates a new extent chain from the actual FAT on disk.
 */
#define FAT_EXTENT_GET_AT(x) error_t (x)(struct fat_s *fat, size_t first_cluster, struct fat_extent_s **extent)

/*
  @this retrieves the cluster count of a given extent.
 */
#define FAT_EXTENT_GET_SIZE(x) size_t (x)(struct fat_s *fat, struct fat_extent_s *extent)

/*
  @this resizes a given extent. Released clusters are pushed back in
  freelist. Allocated cluster are taken from freelist.
 */
#define FAT_EXTENT_RESIZE(x) error_t (x)(struct fat_s *fat, struct fat_extent_s *extent, size_t new_cluster_count)

/*
  @this destroys a given extent. Released clusters are pushed back in
  freelist.
 */
#define FAT_EXTENT_RELEASE(x) error_t (x)(struct fat_s *fat, struct fat_extent_s *extent)

/*
  @this translates from a given block number in an extent to a real
  block device LBA.
 */
#define FAT_EXTENT_BLOCK2LBA(x) error_t (x)(struct fat_s *fat, struct fat_extent_s *extent, size_t block, size_t *lba)

struct fat_access_ops_s
{
	FAT_BACKEND_OPEN(*backend_open);
	FAT_BACKEND_SYNC(*backend_sync);
	FAT_BACKEND_CLOSE(*backend_close);

	FAT_EXTENT_GET_NEW(*extent_get_new);
	FAT_EXTENT_GET_AT(*extent_get_at);

	FAT_EXTENT_GET_SIZE(*extent_get_size);
	FAT_EXTENT_RESIZE(*extent_resize);
	FAT_EXTENT_RELEASE(*extent_release);
	FAT_EXTENT_BLOCK2LBA(*extent_block2lba);
};

extern const fat_access_ops_s fat16_access;

#endif
