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

*/

/**
   @file ARCH lock
*/

#if !defined(LOCK_H_) || defined(ARCH_LOCK_H_)
#error This file can not be included directly
#else

#define ARCH_LOCK_H_

#include <hexo/cpu.h>

struct		arch_lock_s
{
};

#define ARCH_LOCK_INITIALIZER	{ }

static inline error_t arch_lock_init(struct arch_lock_s *lock)
{
  cpu_trap();
  return 0;
}

static inline void arch_lock_destroy(struct arch_lock_s *lock)
{
  cpu_trap();
}

static inline bool_t arch_lock_try(struct arch_lock_s *lock)
{
  cpu_trap();
  return 0;
}

static inline void arch_lock_spin(struct arch_lock_s *lock)
{
  cpu_trap();
}

static inline bool_t arch_lock_state(struct arch_lock_s *lock)
{
  cpu_trap();
  return 0;
}

static inline void arch_lock_release(struct arch_lock_s *lock)
{
  cpu_trap();
}

#endif

