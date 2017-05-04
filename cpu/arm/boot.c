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
    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009

*/

#include <hexo/asm.h>

#ifdef CONFIG_SOCLIB_MEMCHECK
# include <arch/mem_checker.h>
#endif

asm(
    ".section        .boot,\"ax\"           \n"

    FUNC_START(cpu_boot)
    "cpu_boot:                   \n\t"
    "b arm_boot                  \n\t"
    "ldr pc, =arm_exc_undef      \n\t"
    "ldr pc, =arm_exc_swi        \n\t"
    "ldr pc, =arm_exc_pabt       \n\t"
    "ldr pc, =arm_exc_dabt       \n\t"
    "nop                         \n\t"
#if defined(CONFIG_CPU_ARM_CUSTOM_IRQ_HANDLER)
    "ldr pc, =arm_c_irq_handler  \n\t"
    "ldr pc, =arm_c_fiq_handler  \n\t"
#else
    "ldr pc, =arm_exc_irq        \n\t"
    "ldr pc, =arm_exc_fiq        \n\t"
#endif
    FUNC_END(cpu_boot)

    FUNC_START(arm_boot)
#if !defined(CONFIG_ARCH_SIMPLE)
    "mrc  p15,0,r4,c0,c0,5       \n\t"
# ifndef CONFIG_ARCH_SMP
    "cmp    r4, #0              \n\t"
    "1: bne 1b                  \n\t"
# endif

    // Allocate 1K stacks
    "lsl  r5, r4, #" ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) "\n\t"

    "ldr  r13, =__initial_stack   \n\t"
    "subs r13, r13, r5                         \n\t"

# ifdef CONFIG_SOCLIB_MEMCHECK
	    "mov  r0, #1<<" ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) "           \n\t"
	
	    /* enter memchecker command mode */
	    "ldr  r2, =" ASM_STR(SOCLIB_MC_MAGIC_VAL) "  \n\t"
	    "ldr  r1, =" ASM_STR(CONFIG_SOCLIB_MEMCHECK_ADDRESS) " \n\t"
	    "str  r2, [r1, #" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) "] \n\t"
	
	    /* create a new initial memchecker context using cpuid as context id */
	    "str  r0, [r1, #" ASM_STR(SOCLIB_MC_R2_OFFSET) "] \n\t"
	    "sub  r0, sp, r0   \n\t"
	    "add  r0, r0, #4   \n\t"
	    "str  r0, [r1, #" ASM_STR(SOCLIB_MC_R1_OFFSET) "] \n\t"
	    "str  r4, [r1, #" ASM_STR(SOCLIB_MC_CTX_CREATE_OFFSET) "] \n\t"
	
	    /* switch to new memchecker context */
	    "str  r4, [r1, #" ASM_STR(SOCLIB_MC_CTX_SET_OFFSET) "] \n\t"
	
	    /* enable all memchecker checks */
	    "mov  r0, #" ASM_STR(SOCLIB_MC_CHECK_ALL) " \n\t"
	    "str  r0, [r1, #" ASM_STR(SOCLIB_MC_ENABLE_OFFSET) "] \n\t"
	
#  ifdef CONFIG_ARCH_SMP
	    /* mark cpu_init_flag variable as initialized */
	    "ldr  r0, =cpu_init_flag  \n\t"
	    "str  r0, [r1, #" ASM_STR(SOCLIB_MC_INITIALIZED_OFFSET) "] \n\t"
#  endif
# endif

    /* adjust stack pointer, 64 bits aligned */
    "sub  r13, r13, #8         \n\t"

# ifdef CONFIG_SOCLIB_MEMCHECK
	    /* leave memchecker command mode */
	    "mov  r0, #0             \n\t"
	    "str  r0, [r1, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"
# endif

#else /* is ARCH_SIMPLE*/
    "ldr  r13, =__initial_stack - " ASM_STR(CONFIG_HEXO_STACK_ALIGN) "  \n\t"
#endif

    /* Get the device tree and put it in first arg */
#ifdef CONFIG_ARCH_DEVICE_TREE
    "ldr  r0, =dt_blob_start          \n\t"
#else
    "mov  r0, #0                      \n\t"
#endif
    /* Put a 0 in second arg */
    "mov  r1, #0                      \n\t"

    "ldr  r12, =arch_init             \n\t"
    "bx   r12                         \n\t"
    FUNC_END(arm_boot)

    );

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
