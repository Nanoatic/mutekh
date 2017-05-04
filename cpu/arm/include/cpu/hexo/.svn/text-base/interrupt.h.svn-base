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

#ifdef CONFIG_DRIVER_ICU_ARM
struct device_s;
extern CPU_LOCAL struct device_s cpu_icu_dev;
#endif

void mips_interrupt_entry(void);

static inline void
cpu_interrupt_disable(void)
{
#ifdef CONFIG_HEXO_IRQ

	uint32_t tmp;
    THUMB_TMP_VAR;

	asm volatile(
# if __thumb__ && !defined(CONFIG_CPU_ARM_7TDMI)
        "cpsid i"
# else
        THUMB_TO_ARM
		"mrs  %[tmp], cpsr            \n\t"
		"orr  %[tmp], %[tmp], #0x80   \n\t"
		"msr  cpsr, %[tmp]            \n\t"
        ARM_TO_THUMB
# endif
		: [tmp] "=r" (tmp) /*,*/ THUMB_OUT(,)
                :
                : "memory"     /* compiler memory barrier */
        );
#endif
}

static inline void
cpu_interrupt_enable(void)
{
#ifdef CONFIG_HEXO_IRQ
	uint32_t tmp;
    THUMB_TMP_VAR;

	asm volatile(
# if __thumb__ && !defined(CONFIG_CPU_ARM_7TDMI)
        "cpsie i"
# else
        THUMB_TO_ARM
		"mrs  %[tmp], cpsr            \n\t"
		"bic  %[tmp], %[tmp], #0x80   \n\t"
		"msr  cpsr, %[tmp]            \n\t"
        ARM_TO_THUMB
# endif
		: [tmp] "=r" (tmp) /*,*/ THUMB_OUT(,)
                :
                : "memory"     /* compiler memory barrier */
        );
#endif
}

static inline void
cpu_interrupt_savestate(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
	uint32_t tmp;
    THUMB_TMP_VAR;

	asm volatile(
        THUMB_TO_ARM
		"mrs  %[tmp], cpsr        \n\t"
        ARM_TO_THUMB
		: [tmp] "=r" (tmp) /*,*/ THUMB_OUT(,) );

	*state = tmp;
#endif
}

static inline void
cpu_interrupt_savestate_disable(reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
	uint32_t tmp, result;
    THUMB_TMP_VAR;

	asm volatile(
        THUMB_TO_ARM
		"mrs  %[result], cpsr        \n\t"
		"orr  %[tmp], %[result], #0x80   \n\t"
		"msr  cpsr, %[tmp]        \n\t"
        ARM_TO_THUMB
		: [tmp] "=r" (tmp), [result] "=r" (result) /*,*/ THUMB_OUT(,)
                :
                : "memory"     /* compiler memory barrier */
                     );
	*state = result;
#endif
}

static inline void
cpu_interrupt_restorestate(const reg_t *state)
{
#ifdef CONFIG_HEXO_IRQ
    THUMB_TMP_VAR;

	asm volatile(
        THUMB_TO_ARM
		"msr  cpsr, %[state]        \n\t"
        ARM_TO_THUMB
		/* : */ THUMB_OUT(:)
        : [state] "r" (*state)
        : "memory"     /* compiler memory barrier */
                     );
#endif
}

static inline void
cpu_interrupt_process(void)
{
#ifdef CONFIG_HEXO_IRQ
	cpu_interrupt_enable();
    /* memory clobber is important here as cpu_interrupt_process()
       will let pending intterupts change global variables checked in
       a function loop (scheduler root queue for instance) */
	__asm__ volatile ("nop":::"memory");
#endif
}

static inline bool_t
cpu_interrupt_getstate(void)
{
#ifdef CONFIG_HEXO_IRQ
	reg_t		state;
    THUMB_TMP_VAR;

	asm volatile(
        THUMB_TO_ARM
		"mrs  %[state], cpsr        \n\t"
        ARM_TO_THUMB
		: [state] "=r" (state) /*,*/ THUMB_OUT(,) );

	return !(state & 0x80);
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
#  if defined(__ARM_ARCH_6K__)
    THUMB_TMP_VAR;

	asm volatile(
        THUMB_TO_ARM
		"mcr p15, 0, %[zero], c7, c0, 4  \n\t"
        ARM_TO_THUMB
		/*:*/  THUMB_OUT(:)
        : [zero] "r" (0)
	: "memory" );
#  else
# error CONFIG_CPU_WAIT_IRQ should not be defined here
#  endif
# endif
}
#endif

#endif

