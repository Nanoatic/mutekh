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

  Copyright Alexandre Becoulet, <alexandre.becoulet@free.fr>, 2009
*/

#include <hexo/types.h>
#include <hexo/error.h>

#include <mutek/printk.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <string.h>
#include <hexo/endian.h>

#include "iso9660.h"
#include "iso9660-private.h"

/** read directory entry */
error_t iso9660_read_direntry(struct device_s *bd,
			      const struct iso9660_dir_s *entry,
                              char *name, size_t *namelen)
{
    size_t elen = entry->dir_size;
    size_t idflen = entry->idf_len;

    /* check entry size */
    if (!idflen || sizeof(*entry) + idflen > elen)
        return -EBADDATA;

#ifdef CONFIG_DRIVER_FS_ISO9660_ROCKRIDGE
    size_t rrnamelen = 0;
    uint8_t *end = (uint8_t*)entry + elen;
    struct iso9660_susp_s *susp = (void*)( (uint8_t*)(entry + 1) + idflen +
                                           ( (sizeof (*entry) + idflen) & 1) );

    /* in case of CE */
    uint8_t dirblk[ISO9660_BLOCK_SIZE];

    while ( (uint8_t*)(susp + 1) <= end ) {

//        printk("\n+%04x %p %p %d+\n", susp->sign, susp, end, susp->len);

        if ( (uint8_t*)susp + susp->len > end )
            return -EBADDATA;

        ssize_t datalen = susp->len;

        switch (endian_le16_na_load(&susp->sign)) {

        case 0x4d4e: {           /* NM (name chunk) */
            struct iso9660_susp_nm_s *nm = (void*)susp;
            datalen -= sizeof (*nm);
            if (datalen < 0 || rrnamelen + datalen > *namelen)
                return -EBADDATA;

//            printk("<%S>\n", nm->name, datalen);

            memcpy(name + rrnamelen, nm->name, datalen);
            rrnamelen += datalen;
            break;
        }

        case 0x5850:            /* PX (unix mode/type) */
            break;

        case 0x4654:            /* TF (unix timestamp) */
            break;

        case 0x4543: {            /* CE (continue in other block) */
            struct iso9660_susp_ce_s *ce = (void*)susp;
            error_t err;
            datalen -= sizeof (*ce);
            if (datalen < 0)
                return -EBADDATA;

            uint8_t *ptr = dirblk;
            if (( err = dev_block_wait_read(bd, &ptr,
                                            endian_32_na_load(&ce->block), 1) ))
                return err;

            size_t ceoff = endian_32_na_load(&ce->offset);
            size_t cesize = endian_32_na_load(&ce->size);

            if (ceoff + cesize > ISO9660_BLOCK_SIZE)
                return -EBADDATA;

            susp = (void*)(dirblk + ceoff);
            end = dirblk + ceoff + cesize;
            continue;
        }
        }

        susp = (void*)( (uint8_t*)susp + susp->len );
    }

    if (rrnamelen) {
        *namelen = rrnamelen;
    } else {
#endif

        /* strip annoying ;x suffix */
        if (idflen > 2 && entry->idf[idflen - 2] == ';')
            idflen -= 2;

	if (idflen > *namelen)
	  idflen = *namelen;

        memcpy(name, entry->idf, idflen);
        *namelen = idflen;

#ifdef CONFIG_DRIVER_FS_ISO9660_ROCKRIDGE
    }
#endif

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

