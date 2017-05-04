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

static inline uint8_t rotate_left_8(uint8_t x, size_t l)
{
    return (x << (8 - l)) | (x >> l);
}

static inline uint8_t lfn_cksum(const char name[11])
{
    uint_fast8_t i;
    uint8_t sum = name[0];

    for (i = 1; i < 11; i++)
        sum = rotate_left_8(sum, 1) + name[i];

    return sum;
}

#define LFN_ID_INVALID   -1
#define LFN_ID_COMPLETED 0

error_t fat_get_next_dirent(struct fat_file_s *ffile,
                            off_t *offset,
                            union fat_dirent_u *dirent,
                            char *name_83,
                            char *vfs_mangled_name)
{
    char fname[256];
    uint_fast8_t flen = 0;
    int_fast8_t id = LFN_ID_INVALID;
    uint8_t cksum = 0;

    while (1) {
        ssize_t r = fat_data_read(ffile, *offset, dirent, sizeof (*dirent));
        if ( r != sizeof(*dirent) )
            return __MIN(r, 0);
        (*offset) += sizeof(*dirent);

        if ( dirent->lfn.id == FAT_DIR_ENTRY_LAST )
            return 0;

        if ( dirent->lfn.id == FAT_DIR_ENTRY_FREE )
            continue;

        if ( dirent->lfn.attr == ATTR_LFN ) {
            /* lfn format entry */
            uint_fast8_t i, pos = dirent->lfn.id & 0x1f;

            if (pos > 20) {
                /* discard */
                id = LFN_ID_INVALID;
                continue;
            }

            if (dirent->lfn.id & 0x40) {
                /* new lfn entry */
                flen = pos * 13;
                id = pos;
                cksum = dirent->lfn.chksum_83;
            } else {
                /* check sequence and chksum change */
                if (id != pos || id == LFN_ID_COMPLETED || cksum != dirent->lfn.chksum_83)
                    id = LFN_ID_INVALID;
            }

            if (id <= 0)
                continue;

            /* start position in target string */
            pos = pos * 13 - 13;

            for (i = 0; pos + i < sizeof(fname) && i < 13; i++) {
                /* offset of utf16 chars in entry */
                static const uint_fast8_t lfn_chars[13] =
                    { 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30 };

                uint16_t ucode = endian_le16_na_load(dirent->raw + lfn_chars[i]);

                /* early end */
                if (ucode == 0) {
                    flen = pos + i;
                    break;
                }

                /* FIXME handle page code here */
                if (ucode > 127)
                    ucode = '_';

                fname[pos + i] = ucode;
            }

            --id;

        } else {
            /* 8.3 format entry */

            /* skip . and .. */
            if (dirent->old.name[0] == '.') {
                if ((dirent->old.name[1] == '.' && dirent->old.name[2] == 0x20) ||
                    dirent->old.name[2] == 0x20)
                    continue;
            }

            /* do check sum */
            if (lfn_cksum(dirent->old.name) != cksum)
                id = LFN_ID_INVALID;

            if ( dirent->old.ntres & NTRES_LOWER_NAME )
                fat_str_to_lower(dirent->old.name, 8);
            if ( dirent->old.ntres & NTRES_LOWER_EXT )
                fat_str_to_lower(dirent->old.name+8, 3);

            if ( id == LFN_ID_COMPLETED && flen )
                vfs_name_mangle(fname, flen, vfs_mangled_name);
            else
                fat_name_to_vfs(vfs_mangled_name, dirent->old.name);

            fat_name_to_vfs(name_83, dirent->old.name);

            return 1;
        }

    }
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
