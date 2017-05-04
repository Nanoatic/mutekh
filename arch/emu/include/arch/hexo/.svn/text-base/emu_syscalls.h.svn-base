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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006
    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#ifndef EMU_SYSCALLS_H_
#define EMU_SYSCALLS_H_

#include <hexo/types.h>
#include <stdarg.h>

#include "arch/hexo/syscalls_nums.h"
#include "arch/hexo/syscalls_args.h"

extern __compiler_sint_t cpu_pids[CONFIG_CPU_MAXCOUNT];

#if defined(CONFIG_ARCH_EMU_LINUX)

# if defined(CONFIG_CPU_X86_EMU)

static inline reg_t
emu_do_syscall_va(uint_fast16_t id, size_t argc, va_list ap)
{
  reg_t		res;

  /* FIXME registers may be clobbered by syscall */
  if (argc < 6)
    {
      asm volatile ("int $0x80\n"
		    : "=a" (res)
		    : "a" (id)
		    , "b" (va_arg(ap, reg_t))
		    , "c" (va_arg(ap, reg_t))
		    , "d" (va_arg(ap, reg_t))
		    , "S" (va_arg(ap, reg_t))
		    , "D" (va_arg(ap, reg_t))
		    );
    }
  else
    {
      uint_fast8_t	i;
      reg_t		params[argc];

      for (i = 0; i < argc; i++)
	params[i] = va_arg(ap, reg_t);

      asm volatile ("int $0x80\n"
		    : "=a" (res)
		    : "a" (id)
		    , "b" (params)
		    );
    }

  return res;
}

# elif defined (CONFIG_CPU_X86_64_EMU)

static inline reg_t
emu_do_syscall_va(uint_fast16_t id, size_t argc, va_list ap)
{
  reg_t		res;

  register reg_t p1 asm("rdi") = va_arg(ap, reg_t);
  register reg_t p2 asm("rsi") = va_arg(ap, reg_t);
  register reg_t p3 asm("rdx") = va_arg(ap, reg_t);
  register reg_t p4 asm("r10") = va_arg(ap, reg_t);
  register reg_t p5 asm("r8") = va_arg(ap, reg_t);
  register reg_t p6 asm("r9") = va_arg(ap, reg_t);

  asm volatile ("syscall\n"
		: "=a" (res)
		: "a" (id)
		, "r" (p1)
		, "r" (p2)
		, "r" (p3)
		, "r" (p4)
		, "r" (p5)
		, "r" (p6)
		);

  return res;
}

# else
#  error emu_do_syscall_va not available for this processor type
# endif

#elif defined(CONFIG_ARCH_EMU_DARWIN)

# if defined(CONFIG_CPU_X86_EMU)

static inline reg_t
emu_do_syscall_va(uint_fast16_t id, size_t argc, va_list ap)
{
	register reg_t res;

	register uint_fast8_t i;
	reg_t params[argc+1];

	for (i = 0; i < argc; i++)
		params[i+1] = va_arg(ap, reg_t);

	asm volatile (
		"int $0x80\n"
		: "=a" (res)
		: "a" (id), "r" (params)
		);
	return res;
}

# else
#  error CPU not supported
# endif

#else
# error arch not supported
#endif


static inline reg_t
emu_do_syscall(uint_fast16_t id, size_t argc, ...)
{
  va_list	ap;
  reg_t		res;

  va_start(ap, argc);
  res = emu_do_syscall_va(id, argc, ap);
  va_end(ap);

  return res;
}

#define EMU_MAP_FAILED ((void*)-1)

#endif

