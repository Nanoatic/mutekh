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

#ifdef CONFIG_DRIVER_ICU_PPC
struct device_s;
extern CPU_LOCAL struct device_s cpu_icu_dev;
#endif

static inline void
cpu_interrupt_disable(void)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t tmp;

  asm volatile (
		"mfmsr %0		\n\t"
                "ori %0, %0, 0x8000     \n\t"
                "xori %0, %0, 0x8000    \n\t"
		"mtmsr %0		\n\t"
		: "=r" (tmp)
                :: "memory"     /* compiler memory barrier */
	  );
#endif
}

static inline void
cpu_interrupt_enable(void)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t tmp;

  asm volatile (
		"mfmsr %0		\n\t"
                "ori %0, %0, 0x8000     \n\t"
		"mtmsr %0		\n\t"
		: "=r" (tmp)
                :
                : "memory"     /* compiler memory barrier */
	  );
#endif
}

static inline void
cpu_interrupt_process(void)
{
#ifdef CONFIG_HEXO_IRQ
  cpu_interrupt_enable();
  __asm__ volatile ("nop"
		    :
		    :
    /* memory clobber is important here as cpu_interrupt_process()
       will let pending intterupts change global variables checked in
       a function loop (scheduler root queue for instance) */
		    : "memory"
		    );
  cpu_interrupt_disable();
#endif
}

static inline void
cpu_interrupt_savestate(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "mfmsr	%0\n"
		    : "=r" (*state)
		    );
#endif
}

static inline void
cpu_interrupt_savestate_disable(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t tmp;

  asm volatile (
		"mfmsr %1		\n\t"
                "ori %0, %1, 0x8000     \n\t"
                "xori %0, %0, 0x8000    \n\t"
		"mtmsr %0		\n\t"
		: "=r" (tmp),
                  "=r" (*state)
                :
                : "memory"     /* compiler memory barrier */
	  );
#endif
}

static inline void
cpu_interrupt_restorestate(const reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "mtmsr	%0"
		    :
		    : "r" (*state)
                    : "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline bool_t
cpu_interrupt_getstate(void)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t		state;

  __asm__ volatile (
		    "mfmsr	%0"
		    : "=r" (state)
		    );

  return !!(state & 0x8000);
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
static inline void cpu_interrupt_wait(void)
{
# ifdef CONFIG_HEXO_IRQ
  __asm__ volatile ("wait\n"	/* Power ISA 2.0 */
		    ::: "memory");
# endif
}
#endif

#endif

