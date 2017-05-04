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

#include <vfs/vfs.h>
#include "vfs-private.h"
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

OBJECT_FUNC   (vfs_fs, SIMPLE, , vfs_fs, obj_entry);

CONTAINER_FUNC(vfs_lru, CLIST, static inline, vfs_lru, lru_entry);

OBJECT_CONSTRUCTOR(vfs_fs)
{
    atomic_set(&obj->ref, 0);
    vfs_lru_init(&obj->lru_list);

	return 0;
}

OBJECT_DESTRUCTOR(vfs_fs)
{
    vfs_lru_destroy(&obj->lru_list);
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

