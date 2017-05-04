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

#include "fat-sector-cache.h"
#include "fat-private.h"
#include "fat-defs.h"

static
error_t fat_sector_flush_nolock(struct fat_tmp_sector_s *sector,
                                struct device_s *dev)
{
	error_t err = 0;
	uint8_t *blocks[1] = {sector->data};

    if ( sector->dirty ) {
        err = dev_block_wait_write(dev, blocks, sector->lba, 1);
        if ( err )
            return err;
        sector->dirty = 0;
        sector->lba = CLUSTER_END;
    }

    return err;
}

error_t fat_sector_flush(struct fat_tmp_sector_s *sector,
                         struct device_s *dev)
{
    semaphore_take(&sector->semaphore, 1);
    error_t err = fat_sector_flush_nolock(sector, dev);
    semaphore_give(&sector->semaphore, 1);

    return err;
}

error_t fat_sector_lock_and_load(struct fat_tmp_sector_s *sector,
                                 struct device_s *dev,
                                 dev_block_lba_t lba)
{
	error_t err;
	uint8_t *blocks[1] = {sector->data};

    semaphore_take(&sector->semaphore, 1);
    if ( sector->lba == lba )
        return 0;

    err = fat_sector_flush_nolock(sector, dev);
	if ( err ) {
        semaphore_give(&sector->semaphore, 1);
		return err;
    }

	err = dev_block_wait_read(dev, blocks, lba, 1);
	if ( err ) {
        semaphore_give(&sector->semaphore, 1);
		return err;
    }
    sector->lba = lba;
	return 0;
}

void fat_sector_lock_release(struct fat_tmp_sector_s *sector)
{
    semaphore_give(&sector->semaphore, 1);
}
