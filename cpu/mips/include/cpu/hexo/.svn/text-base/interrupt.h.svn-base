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

#include "cpu/hexo/specific.h"

#ifdef CONFIG_DRIVER_ICU_MIPS
struct device_s;
extern CPU_LOCAL struct device_s cpu_icu_dev;
#endif

void mips_interrupt_entry(void);

static inline void
cpu_interrupt_disable(void)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    ".set push			\n"
		    ".set noat			\n"
		    ".set reorder		\n"
# if (CONFIG_CPU_MIPS_VERSION >= 322)
		    "di				\n"
		    "ehb			\n"
# else
		    "mfc0	$1,	$12	\n"
		    "ori	$1,	0x1	\n"
		    "addiu	$1,	-1	\n"
		    ".set noreorder		\n"
		    "mtc0	$1,	$12	\n"
# endif
		    "MTC0_WAIT			\n"
		    ".set pop			\n"

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
		    ".set push			\n"
		    ".set noat			\n"
		    ".set reorder		\n"
# if (CONFIG_CPU_MIPS_VERSION >= 322)
		    "ei				\n"
		    "ehb			\n"
# else
		    "mfc0	$1,	$12	\n"
		    "ori	$1,	1	\n"
		    "mtc0	$1,	$12	\n"
# endif
		    ".set pop			\n"

                    :
                    :
                    : "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline void
cpu_interrupt_savestate(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
		    "mfc0	%0,	$12	\n"
		    : "=r" (*state)
		    );
#endif
}

static inline void
cpu_interrupt_savestate_disable(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
  __asm__ volatile (
# if (CONFIG_CPU_MIPS_VERSION >= 322)
		    "di	%0			\n"
		    "ehb			\n"
# else
		    ".set push				\n"
		    ".set noat				\n"
		    ".set reorder			\n"
		    "mfc0	%0,	$12		\n"
		    "ori	$1,	%0,	1	\n"
		    "addiu	$1,	-1		\n"
		    ".set noreorder			\n"
		    "mtc0	$1,	$12		\n"
		    "MTC0_WAIT				\n"
		    ".set pop				\n"
# endif
		    : "=r" (*state)
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
		    "mtc0	%0,	$12		\n"
# if (CONFIG_CPU_MIPS_VERSION >= 322)
		    "ehb			\n"
# endif
		    :
		    : "r" (*state)
                    : "memory"     /* compiler memory barrier */
		    );
#endif
}

static inline void
cpu_interrupt_process(void)
{
#ifdef CONFIG_HEXO_IRQ
  cpu_interrupt_enable();
  __asm__ volatile (
		    "nop"
		    :
		    :
    /* memory clobber is important here as cpu_interrupt_process()
       will let pending intterupts change global variables checked in
       a function loop (scheduler root queue for instance) */
		    : "memory"
		    );
#endif
}

static inline bool_t
cpu_interrupt_getstate(void)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t		state;

  __asm__ volatile (
		    "mfc0	%0,	$12		\n"
		    : "=r" (state)
		    );

  return state & 0x01;
#else
  return 0;
#endif
}

static inline bool_t
cpu_is_interruptible(void)
{
#ifdef CONFIG_HEXO_IRQ
  reg_t		state;

  __asm__ volatile (
		    "mfc0	%0,	$12		\n"
		    : "=r" (state)
		    );

  // erl and exl masks interrupts
  return ( ! (state & 0x6)
		   && (state & 0x1) );
#else
  return 0;
#endif
}

#ifdef CONFIG_CPU_WAIT_IRQ
static inline void cpu_interrupt_wait(void)
{
# ifdef CONFIG_HEXO_IRQ
  __asm__ volatile ("wait"	/* Mips32 */
		    ::: "memory");
# endif
}
#endif

#endif

