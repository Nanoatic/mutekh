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

  Copyright Alexandre Becoulet, <alexandre.becounet@lip6.fr>, 2009
*/

#include <hexo/endian.h>

#include <mutek/printk.h>
#include <mutek/mem_alloc.h>

#include <device/block.h>
#include <device/driver.h>
#include <device/device.h>
#include <vfs/types.h>
#include <vfs/file.h>
#include <vfs/ops.h>
#include <vfs/fs.h>

#include "fat-sector-cache.h"
#include "fat.h"

#include "fat-defs.h"
#include "fat-private.h"

error_t fat_get_next_dirent(struct fat_file_s *ffile,
                            off_t *offset,
                            union fat_dirent_u *dirent,
                            char *name_83,
                            char *vfs_mangled_name)
{
    while (1) {
        ssize_t r = fat_data_read(ffile, *offset, dirent, sizeof (*dirent));
        if ( r != sizeof(*dirent) )
            return __MIN(r, 0);
        (*offset) += sizeof(*dirent);

        if ( dirent->lfn.id == FAT_DIR_ENTRY_LAST )
            return 0;

        if ( dirent->lfn.id == FAT_DIR_ENTRY_FREE )
            continue;

        if ( dirent->lfn.attr == ATTR_LFN )
            continue;

        /* skip . and .. */
        if (dirent->old.name[0] == '.') {
            if ((dirent->old.name[1] == '.' && dirent->old.name[2] == 0x20) ||
                dirent->old.name[2] == 0x20)
                continue;
        }
        
        if ( dirent->old.ntres & NTRES_LOWER_NAME )
            fat_str_to_lower(dirent->old.name, 8);
        if ( dirent->old.ntres & NTRES_LOWER_EXT )
            fat_str_to_lower(dirent->old.name+8, 3);

        fat_name_to_vfs(vfs_mangled_name, dirent->old.name);
        fat_name_to_vfs(name_83, dirent->old.name);

        return 1;
    }
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
