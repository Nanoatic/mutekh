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

#include "hexo/local.h"

static inline void
cpu_interrupt_disable(void)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile ("cli\n"
                    :
                    :
                    : "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline void
cpu_interrupt_enable(void)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
                    "sei\n"
                    :
                    :
                    : "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline void
cpu_interrupt_process(void)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "sei\n"
    /* nop is required here to let enough time for pending interrupts execution */
		    "nop\n"
		    "nop\n"
		    :
		    :
    /* memory clobber is important here as cpu_interrupt_process()
       will let pending intterupts change global variables checked in
       a function loop (scheduler root queue for instance) */
		    : "memory"
		    );
#endif
}

static inline void
cpu_interrupt_savestate(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "in		%0, 0x3f\n"
		    : "=r" (*state)
		    :
                    : "cc"
		    );
#endif
}

static inline void
cpu_interrupt_savestate_disable(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "in		%0, 0x3f\n"
		    "cli                \n"
		    : "=r" (*state)
		    :
                    : "cc", "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline void
cpu_interrupt_restorestate(const reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "out	0x3f, %0\n"
		    :
		    : "r" (*state)
		    : "cc", "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline bool_t
cpu_interrupt_getstate(void)
{
  bool_t	res = 0;

#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "brid	1f	\n"
		    "ldi	%0, 1	\n"
		    "1:			\n"
		    : "=r" (res)
		    : "0" (res)
		    );
#endif

  return res;
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
static inline void cpu_interrupt_wait(void)
{
# ifdef CONFIG_HEXO_IRQ
  reg_t	tmp;

  __asm__ volatile (
		    /* enable sleep mode */
		    "in		%0, 0x35	\n"
		    "ori	%0, 0x40	\n"
		    "out	0x35, %0	\n"
		    /* then enable interrupts and sleep (atomic) */
		    "sei ; sleep		\n"
		    /* disable sleep mode */
		    "andi	%0, 0xbf	\n"
		    "out	0x35, %0	\n"
		    : "=d" (tmp)
		    :
                    : "memory"
		    );
# endif
}
#endif

#endif

