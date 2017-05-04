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
#include <hexo/cpu.h>
#include <libunix/syscall.h>
#include <libunix/libunix.h>

/*
  eax is syscall number

  parameters:
    if argc < 6:
      1->ebx
      2->ecx
      3->edx
      4->esi
      5->edi
    
    else:
      ebx pointer to parameters table
*/

CPU_SYSCALL_HANDLER(libunix_syscall_handler)
{
  sreg_t res = -ENOSYS;
  reg_t id = regtable[CPU_GPREG_EAX];
  struct libunix_syscall_s *f = unix_syscall_table + id;

  if (id >= LIBUNIX_SYSCALL_COUNT)
    goto end;

  if (f->call == NULL)
    goto end;

  switch(f->argc)
    {
    case 0:
      res = f->call();
      break;

    case 1:
      res = f->call(regtable[CPU_GPREG_EBX]);
      break;

    case 2:
      res = f->call(regtable[CPU_GPREG_EBX],
		    regtable[CPU_GPREG_ECX]);
      break;

    case 3:
      res = f->call(regtable[CPU_GPREG_EBX],
		    regtable[CPU_GPREG_ECX],
		    regtable[CPU_GPREG_EDX]);
      break;

    case 4:
      res = f->call(regtable[CPU_GPREG_EBX],
		    regtable[CPU_GPREG_ECX],
		    regtable[CPU_GPREG_EDX],
		    regtable[CPU_GPREG_ESI]);
      break;

    case 5:
      res = f->call(regtable[CPU_GPREG_EBX],
		    regtable[CPU_GPREG_ECX],
		    regtable[CPU_GPREG_EDX],
		    regtable[CPU_GPREG_ESI],
		    regtable[CPU_GPREG_EDI]);
      break;

    case 6: {
      reg_t *args = (reg_t*)regtable[CPU_GPREG_EBX];
      res = f->call(args[0], args[1], args[2],
		    args[3], args[4], args[5]);
      break;
    }

    default:
      /* invalid syscall table */
      abort();
    }

 end:
  regtable[CPU_GPREG_EAX] = res;  
}

