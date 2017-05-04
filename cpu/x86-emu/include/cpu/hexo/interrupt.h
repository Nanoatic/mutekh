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
   @file

   CPU specific interrupt handling
*/

#if !defined(INTERRUPT_H_) || defined(CPU_INTERRUPT_H_)
#error This file can not be included directly
#else

#define CPU_INTERRUPT_H_

#include <hexo/local.h>
#include <hexo/ordering.h>

#ifdef CONFIG_HEXO_IRQ
void emu_interrupts_process(__compiler_sint_t sig);
void emu_interrupts_wait(void);
void emu_interrupts_init(void);
void emu_interrupts_set(bool_t state);
bool_t emu_interrupts_get(void);
void emu_interrupts_post(cpu_id_t cpu);
#endif

static inline void
cpu_interrupt_disable(void)
{
#ifdef CONFIG_HEXO_IRQ
  emu_interrupts_set(0);
  order_compiler_mem();
#endif
}

static inline void
cpu_interrupt_enable(void)
{
#ifdef CONFIG_HEXO_IRQ
  order_compiler_mem();
  emu_interrupts_set(1);
#endif
}

static void
cpu_interrupt_process(void)
{
#ifdef CONFIG_HEXO_IRQ
  emu_interrupts_process(0);
#endif
}

static inline void
cpu_interrupt_savestate(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  *state = emu_interrupts_get();
#endif
}

static inline void
cpu_interrupt_savestate_disable(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  cpu_interrupt_savestate(state);
  cpu_interrupt_disable();
  order_compiler_mem();
#endif
}

static inline void
cpu_interrupt_restorestate(const reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  order_compiler_mem();
  emu_interrupts_set(*state);
#endif
}

static inline bool_t
cpu_interrupt_getstate(void)
{
#ifdef CONFIG_HEXO_IRQ
  return emu_interrupts_get();
#else
  return 0;
#endif
}

static inline bool_t
cpu_is_interruptible(void)
{
#ifdef CONFIG_HEXO_IRQ
  return cpu_interrupt_getstate();
#else
  return 0;
#endif
}

#ifdef CONFIG_CPU_WAIT_IRQ
static inline void
cpu_interrupt_wait(void)
{
# ifdef CONFIG_HEXO_IRQ
  emu_interrupts_wait();
# endif
}
#endif

#endif

