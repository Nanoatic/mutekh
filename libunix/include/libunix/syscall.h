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

#ifndef LIBUNIX_SYSCALL_H_
#define LIBUNIX_SYSCALL_H_

#include <hexo/types.h>

struct libunix_syscall_s
{
  sreg_t (*call)();
  uint_fast8_t argc;
};

CPU_SYSCALL_HANDLER(libunix_syscall_handler);

/*
 * Vfs related syscalls
 */

#define LIBUNIX_SYS_OPEN		1
sreg_t libunix_sys_open(const char *filename, sreg_t flags, sreg_t mode);

#define LIBUNIX_SYS_CREAT		2
sreg_t libunix_sys_creat(const char *pathname, sreg_t mode);

#define LIBUNIX_SYS_READ		3
sreg_t libunix_sys_read(sreg_t fd, char *buf, reg_t count);

#define LIBUNIX_SYS_WRITE		4
sreg_t libunix_sys_write(sreg_t fd, const char *buf, reg_t count);

#define LIBUNIX_SYS_CLOSE		5
sreg_t libunix_sys_close(sreg_t fd);

#define LIBUNIX_SYS_FSTAT		6
sreg_t libunix_sys_fstat(sreg_t fd, struct libunix_stat_s *statbuf);

#define LIBUNIX_SYS_STAT		7
sreg_t libunix_sys_stat(char *filename, struct libunix_stat_s *statbuf);

#define LIBUNIX_SYS_LINK		9
sreg_t libunix_sys_link(const char *oldname, const char *newname);

#define LIBUNIX_SYS_UNLINK		10
sreg_t libunix_sys_unlink(const char *pathname);

#define LIBUNIX_SYS_LSEEK		11
sreg_t libunix_sys_lseek(sreg_t fd, sreg_t offset_low, sreg_t offset_high, reg_t origin);

#define LIBUNIX_SYS_CHDIR		12
sreg_t libunix_sys_chdir(const char *filename);

/*
 * Process related syscalls
 */

#define LIBUNIX_SYS_EXECVE		13
sreg_t libunix_sys_execve(const char *path, char **argv, char **envp);

#define LIBUNIX_SYS_FORK		14
sreg_t libunix_sys_fork(reg_t flags);

#define LIBUNIX_SYS_EXIT		15
sreg_t libunix_sys_exit(sreg_t c);

#define LIBUNIX_SYS_GETPID		16
sreg_t libunix_sys_getpid();

#define LIBUNIX_SYS_WAIT4		17
sreg_t libunix_sys_wait4(reg_t pid, reg_t *stat_addr, reg_t options, struct libunix_rusage_s *ru);

#define LIBUNIX_SYS_KILL		18
sreg_t libunix_sys_kill(reg_t pid, reg_t sig);


/*
 * Memory related syscalls
 */

#define LIBUNIX_SYS_SBRK		19
reg_t libunix_sys_sbrk(reg_t inc);

/*
 * Time realted functions
 */

#define LIBUNIX_SYS_TIME		8
reg_t libunix_sys_time(struct libunix_time_s *tloc);


#define LIBUNIX_SYSCALL_COUNT		20

extern struct unix_syscall_s unix_syscall_table[LIBUNIX_SYSCALL_COUNT];

#endif

