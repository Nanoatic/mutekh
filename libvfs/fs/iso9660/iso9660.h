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

#ifndef _ISO9660_H_
#define _ISO9660_H_

#include <vfs/fs.h>
#include <vfs/ops.h>

/**
   @this creates a new ISO9660 file system instance.

   @param fs New iso9660 instance (return value)
   @param bd Block device containing the file system
   @return 0 on success
 */
error_t iso9660_open(struct vfs_fs_s **fs, struct device_s *bd);

#endif

