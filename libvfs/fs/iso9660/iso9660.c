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

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

#include <string.h>
#include <hexo/endian.h>

#include "iso9660.h"
#include "iso9660-private.h"

#if 0
struct fs_node_s *
iso9660_node_new(void *mem,
                 struct iso9660_fs_s *fs, const struct iso9660_dir_s *entry,
                 const char *name, size_t namelen);
#endif

OBJECT_CONSTRUCTOR(iso9660_node)
{
    struct iso9660_fs_s *fs = va_arg(ap, struct iso9660_fs_s *);
    const struct iso9660_dir_s *entry = va_arg(ap, const struct iso9660_dir_s *);
//    const char *name = va_arg(ap, const char *);
//    size_t namelen = va_arg(ap, size_t);

//    enum vfs_node_type_e type = entry->type & iso9660_file_isdir ? VFS_NODE_DIR : VFS_NODE_FILE;

    obj->fs = fs;

    /* copy directory entry without file name buffer */
    memcpy(&obj->entry, entry, sizeof(*entry));

    return 0;
};

OBJECT_DESTRUCTOR(iso9660_node)
{
};

static const struct vfs_fs_ops_s iso9660_ops =
{
    .node_open = iso9660_node_open,
    .lookup = iso9660_lookup,
    .stat = iso9660_stat,
    .can_unmount = iso9660_can_unmount,
    .node_refdrop = iso9660_node_refdrop,
    .node_refnew = iso9660_node_refnew,

    .create = NULL,
    .link = NULL,
    .move = NULL,
    .unlink = NULL,
};

error_t iso9660_open(struct vfs_fs_s **fs, struct device_s *bd)
{
    error_t err;
    struct iso9660_fs_s *mnt;
    const struct dev_block_params_s *bdp = dev_block_getparams(bd);
    uint8_t *ptr;

    vfs_printk("iso9660: opening new iso9660 volume\n");

    if ( bdp->blk_size != ISO9660_BLOCK_SIZE ) {
        vfs_printk("iso9660: unsupported device block size: %d\n", bdp->blk_size);
        return -EINVAL;
    }

    mnt = mem_alloc(sizeof (*mnt), mem_scope_sys);
    if ( mnt == NULL )
        return -ENOMEM;
    memset(mnt, 0, sizeof(*mnt));
    vfs_fs_new(&mnt->fs);

    /* read volume descriptor */
    ptr = mnt->voldesc_;
    if (( err = dev_block_spin_read(bd, &ptr, ISO9660_PRIM_VOLDESC_BLOCK, 1) )) {
        vfs_printk("iso9660: unable to read primary volume descriptor\n");
        goto free_mnt;
    }

    /* check signature */
    if ((mnt->voldesc.vol_desc_type != 1) ||
        strncmp(mnt->voldesc.std_identifier, "CD001", 5) ||
        (mnt->voldesc.vol_desc_version != 1)) {
        vfs_printk("iso9660: bad primary volume descriptor signature\n");
        err = -EINVAL;
        goto free_mnt;
    }

    /* check device size */
    if ( bdp->blk_count < mnt->voldesc.vol_blk_count ) {
        err = -EINVAL;
        vfs_printk("iso9660: device block count smaller than fs\n");
        goto free_mnt;
    }

    /* fs struct */
    atomic_set(&mnt->fs.ref, 0);
    mnt->fs.ops = &iso9660_ops;
    mnt->fs.flag_ro = 1;
    mnt->fs.old_node = NULL;

    /* root node init */
    if ( ! (mnt->voldesc.root_dir.type & iso9660_file_isdir) ) {
        err = -EINVAL;
        vfs_printk("iso9660: root entry is not a directory\n");
        goto free_mnt;
    }

    mnt->fs.root = iso9660_node_new(NULL, &mnt->fs, &mnt->voldesc.root_dir, "", 0);

    if (mnt->fs.root == NULL) {
        return -ENOMEM;
        goto free_mnt;
    }

    mnt->bd = device_obj_refnew(bd);

    // TODO register destructor

    *fs = &mnt->fs;
    return 0;

 free_mnt:
    mem_free(mnt);
    return err;
}

VFS_FS_CAN_UNMOUNT(iso9660_can_unmount)
{
    return 0;
}

VFS_FS_LOOKUP(iso9660_lookup)
{
    struct fs_node_s *isonode = ref;
    struct iso9660_fs_s *isofs = ref->fs;

    size_t count = ALIGN_VALUE_UP(isonode->entry.file_size, ISO9660_BLOCK_SIZE) / ISO9660_BLOCK_SIZE;
    dev_block_lba_t first = isonode->entry.data_blk;
    size_t b;
    error_t err;

    for ( b = 0; b < + count; b++ ) {

        uint8_t dirblk[ISO9660_BLOCK_SIZE];
        uint8_t *ptr = dirblk;
        struct iso9660_dir_s *entry;

        if (( err = dev_block_wait_read(isofs->bd, &ptr, first + b, 1) ))
            return err;

        for ( entry = (void*)dirblk; (uint8_t*)entry < dirblk + ISO9660_BLOCK_SIZE; ) {

            /* skip to next block on zero sized dir entry */
            if ( entry->dir_size == 0 )
                break;

            if ( (uint8_t*)entry + entry->dir_size > dirblk + ISO9660_BLOCK_SIZE ) {
                vfs_printk("iso9660: overlapping directory entry not supported\n");
                return -ENOTSUP;
            }

            /* ignore . and .. entries */
            if ( entry->idf_len > 1 || entry->idf[0] > 1 ) {

                char entryname[255];
                size_t entrynamelen = sizeof(entryname);

                if (( err = iso9660_read_direntry(isofs->bd, entry, entryname, &entrynamelen) ))
                    return err;

                vfs_name_mangle(entryname, entrynamelen, mangled_name);

                if (vfs_name_compare(entryname, entrynamelen, name, namelen)) {
                    *node = (void*)iso9660_node_new(NULL, isofs, entry, entryname, entrynamelen);
                    return *node ? 0 : -ENOMEM;
                }
            }

            entry = (void *) ((uint8_t*)entry + entry->dir_size);
        }

    }

    return -ENOENT;
}

VFS_FS_NODE_OPEN(iso9660_node_open)
{
    struct vfs_file_s *f = vfs_file_new(NULL, node, iso9660_node_refnew, iso9660_node_refdrop);

    assert(! (flags & VFS_OPEN_WRITE) );

    if (!f)
        return -ENOMEM;

    if ( flags & VFS_OPEN_DIR ) {
        f->read = iso9660_dir_read;
    } else {
        f->read = iso9660_file_read;
        f->seek = iso9660_file_seek;
    }

    *file = f;
    return 0;
}


VFS_FS_STAT(iso9660_stat)
{
    struct fs_node_s *isonode = (void*)node;

    stat->nlink = 1;
    stat->size = isonode->entry.file_size;
    stat->type = isonode->entry.type & iso9660_file_isdir ? VFS_NODE_DIR : VFS_NODE_FILE;

    return 0;
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

