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


#include <hexo/error.h>
#include <hexo/types.h>
#include <hexo/context.h>

#include <libunix/syscall.h>
#include <libunix/libunix.h>

static const struct libunix_syscall_s
unix_syscall_table[LIBUNIX_SYSCALL_COUNT] =
  {
    /* Vfs related syscalls */

    [LIBUNIX_SYS_OPEN]   = { .argc = 3, .call = libunix_sys_open },
    [LIBUNIX_SYS_CREAT]  = { .argc = 2, .call = libunix_sys_creat },
    [LIBUNIX_SYS_READ]   = { .argc = 3, .call = libunix_sys_read },
    [LIBUNIX_SYS_WRITE]  = { .argc = 3, .call = libunix_sys_write },
    [LIBUNIX_SYS_CLOSE]  = { .argc = 1, .call = libunix_sys_close },
    [LIBUNIX_SYS_FSTAT]  = { .argc = 2, .call = libunix_sys_fstat },
    [LIBUNIX_SYS_LINK]   = { .argc = 2, .call = libunix_sys_link },
    [LIBUNIX_SYS_UNLINK] = { .argc = 1, .call = libunix_sys_unlink },
    [LIBUNIX_SYS_LSEEK]  = { .argc = 4, .call = libunix_sys_lseek },
    [LIBUNIX_SYS_CHDIR]  = { .argc = 1, .call = libunix_sys_chdir },

    /* Process related syscalls */

    [LIBUNIX_SYS_EXECVE] = { .argc = 3, .call = libunix_sys_execve },
    [LIBUNIX_SYS_FORK]   = { .argc = 1, .call = libunix_sys_fork },
    [LIBUNIX_SYS_EXIT]   = { .argc = 1, .call = libunix_sys_exit },
    [LIBUNIX_SYS_GETPID] = { .argc = 0, .call = libunix_sys_getpid },
    [LIBUNIX_SYS_WAIT4]  = { .argc = 4, .call = libunix_sys_wait4 },
    [LIBUNIX_SYS_KILL]   = { .argc = 2, .call = libunix_sys_kill },

    /* Memory related syscalls */

    [LIBUNIX_SYS_SBRK]   = { .argc = 1, .call = libunix_sys_sbrk },

    /* Time realted functions */

    [LIBUNIX_SYS_TIME]   = { .argc = 1, .call = libunix_sys_time },
  };

