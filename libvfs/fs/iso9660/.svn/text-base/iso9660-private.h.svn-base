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

#ifndef _ISO9660_PRIVATE_H_
#define _ISO9660_PRIVATE_H_

#define fs_node_s fs_node_s

#include <vfs/types.h>
#include <vfs/fs.h>
#include <vfs/file.h>

#include "iso9660-bits.h"

#define ISO9660_BURST_BLKCOUNT 64

OBJECT_TYPE     (iso9660_node, REFCOUNT, struct fs_node_s);
OBJECT_PROTOTYPE(iso9660_node, static inline, iso9660_node);

struct iso9660_fs_s;

struct fs_node_s
{
  iso9660_node_entry_t obj_entry;

  struct iso9660_fs_s *fs;
  
  struct {
    uint16_t padding;	  /* align 32bits fields in iso9660_dir_s */
    struct iso9660_dir_s entry;
  } __attribute__ ((packed));
};

OBJECT_CONSTRUCTOR(iso9660_node);
OBJECT_DESTRUCTOR(iso9660_node);

OBJECT_FUNC   (iso9660_node, REFCOUNT, static inline, iso9660_node, obj_entry);

struct iso9660_fs_s
{
  struct vfs_fs_s		fs; /* keep first field */
  struct fs_node_s		*root;
  struct device_s		*bd;

  union {
    uint8_t				voldesc_[ISO9660_BLOCK_SIZE];
    struct iso9660_prim_voldesc_s	voldesc;
  };
};

VFS_FS_NODE_OPEN(iso9660_node_open);
VFS_FS_LOOKUP(iso9660_lookup);
VFS_FS_STAT(iso9660_stat);
VFS_FS_CAN_UNMOUNT(iso9660_can_unmount);

VFS_FILE_SEEK(iso9660_file_seek);
VFS_FILE_READ(iso9660_file_read);
VFS_FILE_READ(iso9660_dir_read);

error_t iso9660_read_direntry(struct device_s *bd,
			      const struct iso9660_dir_s *entry,
                              char *name, size_t *namelen);

#endif

