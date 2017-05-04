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

    Copyright (c) 2010, Nicolas Pouillon <nipo@ssji.net>
*/

#include <hexo/asm.h>
#include <hexo/interrupt.h>


/*
  Three accessor macros to get CLS/TLS data.
  Depending on the implementation, we'll use different macros
 */

/* get a variable using a cp15 register as base */
#define GET_CP15_REL(name, rd, rt, op2)                                \
    "ldr  " rd ", =" name "             \n\t"                          \
    "mrc p15,0," rt ",c13,c0," #op2 "   \n\t"                          \
    "ldr  " rd ", [" rd ", +" rt "]     \n\t"

/* get a global variable */
#define GET_GLOBAL(name, rd)                                           \
    "ldr  " rd ", =" name "     \n\t"                                  \
    "ldr  " rd ", [" rd "]      \n\t"

/* get a variable using another global as base */
#define GET_GLOBAL_REL(name, rd, rt, var)                              \
    "ldr  " rd ", =" name "            \n\t"                           \
    "ldr  " rt ", =" var "             \n\t"                           \
    "ldr  " rd ", [" rd ", " rt "]     \n\t"

#if defined(CONFIG_CPU_ARM_TLS_IN_C15)
# ifdef CONFIG_ARCH_SMP
#  define CPULOCAL_GET(name, rd, rt) GET_CP15_REL(name, rd, rt, 3)
# else
#  define CPULOCAL_GET(name, rd, rt) GET_GLOBAL(name, rd)
# endif
# define CONTEXTLOCAL_GET(name, rd, rt) GET_CP15_REL(name, rd, rt, 4)
#else
# ifdef CONFIG_ARCH_SMP
#  error Please define me a way to get TLS/CLS
# else
#  define CPULOCAL_GET(name, rd, rt) GET_GLOBAL(name, rd)
#  define CONTEXTLOCAL_GET(name, rd, rt) GET_GLOBAL_REL(name, rd, rt, "__context_data_base")
# endif
#endif


#define SUB_0
#define SUB_4 "sub    lr, lr, #4 \n\t"
#define SUB_8 "sub    lr, lr, #8 \n\t"

asm(
    ".section        .text,\"ax\"         \n"

#define prepare_exception(pc_offset)                                   \
    /* uniformize return address, but avoid a sub when 0 */            \
    SUB_##pc_offset                                                    \
    /* store r0 and exc pc in tmp buffer */                            \
    "stmia  sp, {r0, r1, lr}      \n\t"                                \
    /* Take current msr, save it */                                    \
    "mrs    r0, cpsr              \n\t"                                \
    "str    r0, [sp, #20]         \n\t"                                \
    /* Take old msr, save it */                                        \
    "mrs    r0, spsr              \n\t"                                \
    "str    r0, [sp, #12]         \n\t"                                \
    /* take pointer to tmp buffer */                                   \
    "mov    r1, sp                \n\t"                                \
    /* disable irqs, switch to supervisor */                           \
    "bic    r0, r0, #0xff         \n\t"                                \
    "orr    r0, r0, #0xd3         \n\t"                                \
    "msr    cpsr, r0              \n\t"                                \
    /* save super lr, in case we were in super */                      \
    "str    lr, [r1, #16]         \n\t"

#define restore_exception()                                            \
    /*                                                                 \
     * Here we're back with tmp buf in r1,                             \
     * all registers but r0/r1 should already be restored.             \
     */                                                                \
    /* restore lr, just if we were from super */                       \
    "ldr    lr, [r1, #16]         \n\t"                                \
    /* restore stupid mode */                                          \
    "ldr    r0, [r1, #20]         \n\t"                                \
    "msr    cpsr, r0              \n\t"                                \
    /* restore spsr */                                                 \
    "ldr    r0, [r1, #12]         \n\t"                                \
    "msr    spsr, r0              \n\t"                                \
    /* restore regs */                                                 \
    "ldmia  r1, {r0, r1, lr}      \n\t"                                \
    /* jump back, sp is in target mode. */                             \
    "movs   pc, lr                \n\t"

#define handle_exception(arg)                                          \
    "mov    r0, #" ASM_STR(arg) " \n\t"                                \
    "b      arm_exc_common        \n\t"

#define handle_irq(no)                                                 \
    "mov    r0, #" #no "          \n\t"                                \
    "b      arm_irq_common        \n\t"

    /* r14 is exc pc + 4 */
    FUNC_START(arm_exc_undef)
    prepare_exception(4)
    handle_exception(CPU_EXCEPTION_ILLEGAL_INS)
    FUNC_END(arm_exc_undef)

    /* r14 is error pc + 4 */
    FUNC_START(arm_exc_pabt)
    prepare_exception(4)
    handle_exception(CPU_EXCEPTION_INS_ERROR)
    FUNC_END(arm_exc_pabt)

    /* r14 is error pc + 8 */
    FUNC_START(arm_exc_dabt)
    prepare_exception(8)
    handle_exception(CPU_EXCEPTION_DATA_ERROR)
    FUNC_END(arm_exc_dabt)

#if !defined(CONFIG_CPU_ARM_CUSTOM_IRQ_HANDLER)
    /* r14 is error pc + 4 */
    FUNC_START(arm_exc_irq)
    prepare_exception(4)
    handle_irq(0)
    FUNC_END(arm_exc_irq)

    /* r14 is error pc + 4 */
    FUNC_START(arm_exc_fiq)
    prepare_exception(4)
    handle_irq(1)
    FUNC_END(arm_exc_fiq)
#endif

    /* r14 is swi pc + 4 */
    FUNC_START(arm_exc_swi)
    prepare_exception(0)
    "push  {r1}                    \n\t"
    "push  {r2, r3, ip}            \n\t"
    /* get r0/r1 and put them on stack */
    "ldmia r1, {r2, r3}            \n\t"
    "push  {r2, r3}                \n\t"
    /* stack is [r0, r1, r2, r3, ip, tmp] */
    CONTEXTLOCAL_GET("cpu_syscall_handler", "ip", "r2")
    /* get swi instruction */
    "ldr   r0, [r1, #8]            \n\t"
    "ldr   r0, [r0, #-4]           \n\t"
    /* r0: masked opcode */
    "bic   r0, #0xff000000         \n\t"
    /* r1: regtable pointer */
    "mov   r1, sp                  \n\t"
    /* call handler */
    "mov   lr, pc                  \n\t"
    "bx    ip                      \n\t"
    /* stack is [r0, r1, r2, r3, ip, tmp] */
    /* reget tmp buf */
    "ldr   r1, [sp, #20]           \n\t"
    /* put back r0/r1 to tmp buf */
    "pop   {r2, r3}                \n\t"
    "stmia r1, {r2, r3}            \n\t"
    /* restore, return */
    "pop   {r2, r3, ip}            \n\t"
    "pop   {r1}                    \n\t"
    restore_exception()
    FUNC_END(arm_exc_swi)

    FUNC_START(arm_exc_common)
    "push  {r1}                    \n\t"
    "push  {r2, r3, ip}            \n\t"
    /* get r0/r1 and put them on stack */
    "ldmia r1, {r2, r3}            \n\t"
    "push  {r2, r3}                \n\t"
    /* stack is [r0, r1, r2, r3, ip, tmp] */

    /* Put handler arguments: */
    /* a0: type, already done */
    /* a1: execptr, from [r1, #8] */
    /* a2: dataptr (argh! this is machine dep!) */
    /* a3: regtable */
    /* *sp: sp when faulted */
    CPULOCAL_GET("cpu_exception_handler", "ip", "r2")
    /* push sp ... */
#if defined(CONFIG_USER_MODE)
    /* test whether from user mode */
    "ldr   r2, [r1, #12]           \n\t"
    "ands  r2, r2, #f              \n\t"
    /* sp from kernel, 6 words away */
    "addne r2, sp, #6*4            \n\t"
    "pushne {r2}                   \n\t"
    /* sp from user */
    "pusheq {sp}^                  \n\t"
#else
    /* sp is 6 words away */
    "add   r2, sp, #6*4            \n\t"
    "push  {r2}                    \n\t"
#endif
    /* a1 */
    "ldr   r1, [r1, #8]            \n\t"
    /* a2 */
#if defined(CONFIG_ARCH_SIMPLE_SAM7)
    "ldr   r2, =0xffffff08         \n\t"
    "ldr   r2, [r2]                \n\t"
#elif defined(CONFIG_CPU_ARM_SOCLIB)
    "mrc p15, 0, r2, c6, c0, 0     \n\t"
#else
# warning No way to get data error pointer !
    "ldr   r2, =0xbad0da1a         \n\t"
#endif
    /* a3 */
    "add   r3, sp, #4              \n\t"
    /* call handler */
    "mov   lr, pc                  \n\t"
    "bx    ip                      \n\t"

    /* stack is [sp, r0, r1, r2, r3, ip, tmp] */

    /* remove sp */
    "add   sp, sp, #4              \n\t"

    /* reget tmp buf */
    "ldr   r1, [sp, #20]           \n\t"
    /* put back r0/r1 to tmp buf */
    "pop   {r2, r3}                \n\t"
    "stmia r1, {r2, r3}            \n\t"
    /* restore, return */
    "pop   {r2, r3, ip}            \n\t"
    "pop   {r1}                    \n\t"
    restore_exception()
    FUNC_END(arm_exc_common)


#if !defined(CONFIG_CPU_ARM_CUSTOM_IRQ_HANDLER)
    FUNC_START(arm_irq_common)
#if defined(CONFIG_HEXO_IRQ)
    "push  {r1, r2, r3, ip}        \n\t" /* FIXME may save less registers */
    /* Put handler arguments: */
    /* a0: irq */
    CPULOCAL_GET("cpu_interrupt_handler",     "ip", "r3")
    "mov   lr, pc                  \n\t"
    "bx    ip                      \n\t"
    "pop   {r1, r2, r3, ip}        \n\t"
#endif
    restore_exception()
    FUNC_END(arm_irq_common)
#endif

    );

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
