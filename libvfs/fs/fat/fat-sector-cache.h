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

#ifndef FAT_TMP_SECTOR_S
#define FAT_TMP_SECTOR_S

#include <hexo/types.h>
#include <mutek/semaphore.h>
#include <device/block.h>
#include <device/device.h>

struct fat_tmp_sector_s
{
    // Keep it ordered like this to have data aligned.
    bool_t dirty;
    uint32_t lba;
    struct semaphore_s semaphore;
	uint8_t data[0];
};

error_t fat_sector_lock_and_load(struct fat_tmp_sector_s *sector,
                                 struct device_s *dev,
                                 dev_block_lba_t lba);

void fat_sector_lock_release(struct fat_tmp_sector_s *sector);

error_t fat_sector_flush(struct fat_tmp_sector_s *sector,
                         struct device_s *dev);

#endif
