/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright (C) 2010, Joel Porquet <joel.porquet@lip6.fr>
    Copyright (C) 2010, Becoulet <alexandre.becoulet@free.fr>

*/

#ifndef LIBUNIX_H_
#define LIBUNIX_H_

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_array.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>
#include <gpct/object_refcount.h>

#include <hexo/error.h>
#include <hexo/types.h>
#include <hexo/scheduler.h>

#include <mutek/fileops.h>
#include <vfs/vfs.h>

struct libunix_s;
typedef uint_fast16_t			libunix_pid_t;
typedef uint_fast16_t			libunix_uid_t;


/***********************************************************************
 *	File descriptors
 */


OBJECT_TYPE     (libunix_fd_obj, REFCOUNT, struct myitem);
OBJECT_PROTOTYPE(libunix_fd_obj, , libunix_fd);

/** @this describes process file descriptor */
struct libunix_fd_s
{
  myobj_entry_t			obj_entry;
  struct fileops_s		*fops;
  void				*handle;
};

#define CONTAINER_OBJ_libunix_fd_table libunix_fd

CONTAINER_TYPE(libunix_fd_table, ARRAY, struct libunix_fd_s *, CONFIG_LIBUNIX_MAX_FD)
CONTAINER_PROTOTYPE(libunix_fd_table, ARRAY, , libunix_fd);


/***********************************************************************
 *	Vmem mapping
 */

/** @this defines process page fault prototype */
#define LIBUNIX_PROC_PGFAULT(n) error_t (n) (struct libunix_vmarea_s *vma);

/** @this defines process page fault function type @csee #LIBUNIX_PROC_PGFAULT */
typedef LIBUNIX_PROC_PGFAULT(libunix_proc_pgfault_t);

/* @this describes a virtual memory mapping area */
struct libunix_vmarea_s
{
  uintptr_t				start;
  uintptr_t				end;
  libunix_proc_pgfault_t		*pgfault;

  CONTAINER_ENTRY_TYPE(CLIST)		list_entry;
};

CONTAINER_TYPE(libunix_vma_table, CLIST, struct libunix_vmarea_s, list_entry)
CONTAINER_KEY_TYPE(libunix_vma_table, PTR, SCALAR, start);

CONTAINER_PROTOTYPE(libunix_vma_table, CLIST, , libunix_vma);
CONTAINER_KEY_PROTOTYPE(libunix_vma_table, CLIST, , libunix_vma, start);


/***********************************************************************
 *	Process descriptor
 */

/** @this describes a unix process */
struct libunix_process_s
{
  /* vfs */
  struct vfs_node_s			*cwd;
  libunix_fd_table_root_t		fd_table;

  /* mem */
  libunix_vma_table_root_t		vmem;
  struct libunix_vmarea_s		text; /* text, rodata */
  struct libunix_vmarea_s		data; /* data, bss, heap */
  struct libunix_vmarea_s		stack; /* stack */

  /* sched */
  libunix_pid_t				pid;
  struct sched_context_s		sched_ctx;
  CONTAINER_ENTRY_TYPE(HASHLIST)	pid_entry;

  struct libunix_process_s		*parent;
  CONTAINER_ENTRY_TYPE(CLIST)		child_entry;

  /* owner */
  libunix_uid_t				uid;
};

CONTAINER_TYPE     (libunix_proc_tree, CLIST, struct libunix_process_s, child_entry);
CONTAINER_PROTOTYPE(libunix_proc_tree, CLIST, , libunix_proc_chld);


/***********************************************************************
 *	Unix instance
 */

/** @this holds a unix os instance */
struct libunix_s
{
  struct vfs_node_s			*root;
  libunix_proc_table_root_t		proc_table;
};


CONTAINER_TYPE    (libunix_proc_table, HASHLIST, struct libunix_process_s, pid_entry, 11);
CONTAINER_KEY_TYPE(libunix_proc_table, PTR, SCALAR, pid);

CONTAINER_PROTOTYPE    (libunix_proc_table, HASHLIST, , libunix_proc, pid);
CONTAINER_KEY_PROTOTYPE(libunix_proc_table, HASHLIST, , libunix_proc, pid);


/** @this creates a new unix instance */
error_t libunix_init(struct libunix_s *lu);

#endif

