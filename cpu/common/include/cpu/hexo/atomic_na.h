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

#if !defined(ATOMIC_H_) || defined(CPU_ATOMIC_H_)
#error This file can not be included directly
#else

#include <hexo/interrupt.h>

#ifdef CONFIG_ARCH_SMP
# error No valid cpu atomic operations defined for SMP build
#endif

/*
 * We dont need to add extra memory barrier for pseudo atomic
 * operations here because interrupt enable and disable acts as a
 * compiler memory barrier.
 */

#define CPU_ATOMIC_H_

static inline bool_t
cpu_atomic_inc(atomic_int_t *a)
{
  bool_t res;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  (*a)++;
  res = *a ? 1 : 0;
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static inline bool_t
cpu_atomic_dec(atomic_int_t *a)
{
  bool_t res;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  (*a)--;
  res = *a ? 1 : 0;
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static inline bool_t
cpu_atomic_bit_testset(atomic_int_t *a, uint_fast8_t n)
{
  bool_t res;
  atomic_int_t	old;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  old = *a;
  *a |= (1 << n);
  res = !!(old & (1 << n));
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static inline void
cpu_atomic_bit_waitset(atomic_int_t *a, uint_fast8_t n)
{
  while (!cpu_atomic_bit_testset(a, n))
    ;
}

static inline bool_t
cpu_atomic_bit_testclr(atomic_int_t *a, uint_fast8_t n)
{
  bool_t res;
  atomic_int_t	old;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  old = *a;
  *a &= ~(1 << n);
  res = !!(old & (1 << n));
  CPU_INTERRUPT_RESTORESTATE;

  return res;
}

static inline void
cpu_atomic_bit_waitclr(atomic_int_t *a, uint_fast8_t n)
{
  while (cpu_atomic_bit_testclr(a, n))
    ;
}

static inline void
cpu_atomic_bit_set(atomic_int_t *a, uint_fast8_t n)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  *a |= (1 << n);
  CPU_INTERRUPT_RESTORESTATE;
}

static inline void
cpu_atomic_bit_clr(atomic_int_t *a, uint_fast8_t n)
{
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  *a &= ~(1 << n);
  CPU_INTERRUPT_RESTORESTATE;
}

static inline bool_t
cpu_atomic_compare_and_swap(atomic_int_t *a, atomic_int_t old, atomic_int_t future)
{
  bool_t res = 0;

  CPU_INTERRUPT_SAVESTATE_DISABLE;
  if (*a == old)
    {
      *a = future;
      res = 1;
    }
  CPU_INTERRUPT_RESTORESTATE;  

  return res;
}

#endif

