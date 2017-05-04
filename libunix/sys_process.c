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

#include <libunix/syscall.h>
#include <libunix/libunix.h>

sreg_t libunix_sys_execve(const char *path, char **argv, char **envp)
{
  return -EINVAL;
}

sreg_t libunix_sys_fork(reg_t flags)
{
  return -EINVAL;
}

sreg_t libunix_sys_exit(sreg_t c)
{
  return -EINVAL;
}

sreg_t libunix_sys_getpid()
{
  return -EINVAL;
}

sreg_t libunix_sys_wait4(reg_t pid, reg_t *stat_addr, reg_t options, struct libunix_rusage_s *ru)
{
  return -EINVAL;
}

sreg_t libunix_sys_kill(reg_t pid, reg_t sig)
{
  return -EINVAL;
}

