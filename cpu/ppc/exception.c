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

    Copyright (c) 2009-2010, Nicolas Pouillon <nipo@ssji.net>
*/

#include <hexo/asm.h>
#include <hexo/interrupt.h>

/*
  Two accessor macros to get CLS/TLS data.
  Depending on the implementation, we'll use different macros
 */

/* get a variable using a SPR as base */
#define GET_SPRREL(name, rd, rt, spr)                                  \
    "mfspr " #rt ", " #spr "             \n\t"                         \
    "lwz   " #rd ", " name "(" #rt ")    \n\t"

/* get a global variable */
#define GET_GLOBAL(name, rd, rt)                                       \
    "lis   " #rt ", " name "@ha           \n\t"                        \
    "lwz   " #rd ", " name "@l(" #rt ")   \n\t"

#define LA(rd, name)                                                    \
    "lis   " #rd ", (" #name ")@h         \n\t"                        \
    "ori   " #rd ", " #rd ", (" #name ")@l\n\t"

#ifdef CONFIG_ARCH_SMP
# define CPULOCAL_GET(name, rd, rt) GET_SPRREL(name, rd, rt, 0x105)
#else
# define CPULOCAL_GET(name, rd, rt) GET_GLOBAL(name, rd, rt)
#endif

#define CONTEXTLOCAL_GET(name, rd, rt) GET_SPRREL(name, rd, rt, 0x104)


#define SPRG0 "0x110"
#define SPRG1 "0x111"

#define DO_REG_2_3(op, n, off)                                  \
    #op "   2, " #off "+4* 0(" #n ")      \n\t"                 \
    #op "   3, " #off "+4* 1(" #n ")      \n\t"

#define DO_REG_4_12(op, n, off)                                 \
    #op "   4, " #off "+4* 0(" #n ")      \n\t"                 \
    #op "   5, " #off "+4* 1(" #n ")      \n\t"                 \
    #op "   6, " #off "+4* 2(" #n ")      \n\t"                 \
    #op "   7, " #off "+4* 3(" #n ")      \n\t"                 \
    #op "   8, " #off "+4* 4(" #n ")      \n\t"                 \
    #op "   9, " #off "+4* 5(" #n ")      \n\t"                 \
    #op "  10, " #off "+4* 6(" #n ")      \n\t"                 \
    #op "  11, " #off "+4* 7(" #n ")      \n\t"                 \
    #op "  12, " #off "+4* 8(" #n ")      \n\t"

#define DO_REG_2_12(op, n, off)                                        \
    DO_REG_2_3(op, n, off)                                             \
    DO_REG_4_12(op, n, off+8)


/*
  (cls / tls are r/o for user, no need to restore)

  Whatever the exception, we need to:
  * copy r0, r1 -> sprg0, sprg1
  * save cr to r0
  * get cpu "previous" mode
  * if from kernel
    * copy back r1 from sprg1
  * if from user
    * get task's kernel stack to r1
  * reserve 32+5 registers
  * put cr (r0)
  * put exception type in r0
  * put srr0/2 (PC), srr1/3 (msr)

  Generic part:
  * put ctr, lr
  * put sprg[01] on stack
  * put gpr 2-12 on stack
  * (put gpr 13-31 on stack)
  * handle exception
  * restore ctr, cr, lr, srr0 (PC), srr1 (msr)
  * (restore gpr 13-31)
  * restore gpr 2-12
  * load r0 from *sp
  * load sp from *sp
  * rfi

  stack layout:
   0     1    2    3    4         5          6      37
  [sp] [ctr] [lr] [cr] [srr0/pc] [srr1/msr] [r0 .. r31] 
 */

#if defined(CONFIG_USER_MODE)
# define GET_STACK_SAVE_CR                                             \
    /* put msr in cr and do bit test (user mode) */                    \
    "mf" #msr_srr " 1                     \n\t"                        \
    "mtcr   1                             \n\t"                        \
    "bt     17, 1f                        \n\t"                        \
    /* from kernel */                                                  \
    "mfspr 1, " SPRG1 "                   \n\t"                        \
    "stwu  1, -38*4(1)                    \n\t"                        \
    "stw   0,  3 *4(1)                    \n\t"                        \
    "b 2f                                 \n\t"                        \
    "1:                                   \n\t"                        \
    CONTEXTLOCAL_GET("context_kstack", 1, 1)                           \
    "addi  1, -38*4(1)                    \n\t"                        \
    /* save cr */                                                      \
    "stw   0,  3 *4(1)                    \n\t"                        \
    /* save old sp */                                                  \
    "mfspr 0,  " SPRG1 "                  \n\t"                        \
    "stw   0,  0 *4(1)                    \n\t"                        \
    "2:                                   \n\t"
#else
# define GET_STACK_SAVE_CR                                             \
    "stwu  1, -38*4(1)                    \n\t"                        \
    /* save cr */                                                      \
    "stw   0,  3 *4(1)                    \n\t"
#endif

#define prepare_exception(pc_srr, msr_srr)                             \
    "mtspr  " SPRG0 ", 0                  \n\t"                        \
    "mtspr  " SPRG1 ", 1                  \n\t"                        \
    "mfcr   0                             \n\t"                        \
    GET_STACK_SAVE_CR                                                  \
    /* pc */                                                           \
    "mf" #pc_srr " 0                      \n\t"                        \
    "stw    0, 4 *4(1)                    \n\t"                        \
    /* srr1 */                                                         \
    "mf" #msr_srr " 0                     \n\t"                        \
    "stw    0, 5 *4(1)                    \n\t"                        \
    /* ctr */                                                          \
    "mfctr 0                              \n\t"                        \
    "stw   0, 1 *4(1)                     \n\t"

#define handle_exception(name)                                         \
    LA(0, exception_common)                                            \
    "mtctr 0                              \n\t"                        \
    /* exc type */                                                     \
    "li    0, " ASM_STR(name) "           \n\t"                        \
    "bctr                                 \n\t"

#define CRASH(x, y)                                                    \
    prepare_exception(x, y)                                            \
    handle_exception(CPU_EXCEPTION_OTHER)

#define handle_interrupt(no)                                           \
    LA(0, interrupt_common)                                            \
    "mtctr 0                              \n\t"                        \
    "li    0, " #no "                     \n\t"                        \
    "bctr                                 \n\t"

#define handle_syscall()                                               \
    LA(0, syscall_common)                                              \
    "mtctr 0                              \n\t"                        \
    "bctr                                 \n\t"

asm(".section        .text,\"ax\",@progbits  \n"

    FUNC_START(exception_common)
    DO_REG_2_12(stw, 1, 8*4)
    /* lr */
    "mflr  3                              \n\t"
    "stw   3, 2 *4(1)                     \n\t"
    /* r0 */
    "mfspr 3, " SPRG0 "                   \n\t"
    "stw   3, 6 *4(1)                     \n\t"
    /* r1 */
    "mfspr 3, " SPRG1 "                   \n\t"
    "stw   3, 7 *4(1)                     \n\t"
    /* Put handler arguments: */
    /* a0: type */
    /* a1: execptr (srr) */
    /* a2: dataptr (dear) */
    /* a3: regtable (sp + 6*4) */
    /* a4: sp (sp + 22*4) */
    "or    3, 0, 0                        \n\t"
    "lwz   4, 4 *4(1)                     \n\t"
    "addi  6, 1, 6*4                      \n\t"
    "lwz   7, 7 *4(1)                     \n\t"

    /*
     * dont use 0 as 3rd arg, or you'll get crappy things,
     * hijack r5 and do mfdear afterwards
     */
    CPULOCAL_GET("cpu_exception_handler", 0, 5)
    "mtctr 0                               \n\t"
    "mfdear 5                              \n\t"

    /* Call the exception handler */
    "bctrl                                 \n\t"
    "b back_from_exception                 \n\t"
    FUNC_END(exception_common)



    FUNC_START(interrupt_common)
    DO_REG_2_12(stw, 1, 8*4)
    /* lr */
    "mflr  3                              \n\t"
    "stw   3, 2 *4(1)                     \n\t"
    /* r0 */
    "mfspr 3, " SPRG0 "                   \n\t"
    "stw   3, 6 *4(1)                     \n\t"
    /* r1 */
    "mfspr 3, " SPRG1 "                   \n\t"
    "stw   3, 7 *4(1)                     \n\t"
    /* Put handler arguments: */
    /* a0: priv */
    /* a1: irq */

#if defined(CONFIG_HEXO_IRQ)
    CPULOCAL_GET("cpu_interrupt_handler", 5, 5)
    "mr    3, 0                            \n\t"
    "mtctr 5                               \n\t"

    /* Call the interrupt handler */
    "bctrl                                 \n\t"
#endif
    "b back_from_exception                 \n\t"
    FUNC_END(interrupt_common)



    FUNC_START(syscall_common)
    DO_REG_2_12(stw, 1, 8*4)
    /* ctr */
    "mfctr 3                              \n\t"
    "stw   3, 1 *4(1)                     \n\t"
    /* lr */
    "mflr  3                              \n\t"
    "stw   3, 2 *4(1)                     \n\t"
    /* r0 */
    "mfspr 3, " SPRG0 "                   \n\t"
    "stw   3, 6 *4(1)                     \n\t"
    /* r1 */
    "mfspr 3, " SPRG1 "                   \n\t"
    "stw   3, 7 *4(1)                     \n\t"
    /* Put handler arguments: */
    /* a0: number */
    /* a1: regtable */

    CONTEXTLOCAL_GET("cpu_syscall_handler", 5, 5)
    "mtctr 5                              \n\t"

    "li    3, 0                           \n\t"
    "addi  6, 1, 6*4                      \n\t"

    /* Call the syscall handler */
    "bctrl                                 \n\t"
    "b back_from_syscall                   \n\t"
    FUNC_END(syscall_common)





    FUNC_START(back_from_exception)
    DO_REG_4_12(lwz, 1, 10*4)

    /* Dont restore reg 4 to 12 for syscall return */
    "back_from_syscall:                   \n\t"
    DO_REG_2_3(lwz, 1, 8*4)

    /* ctr */
    "lwz   0, 1 *4(1)                     \n\t"
    "mtctr 0                              \n\t"
    /* lr */
    "lwz   0, 2 *4(1)                     \n\t"
    "mtlr  0                              \n\t"
    /* cr */
    "lwz    0,  3 *4(1)                   \n\t"
    "mtcr   0                             \n\t"
    /* srr0 */
    "lwz    0, 4 *4(1)                    \n\t"
    "mtsrr0 0                             \n\t"
    /* srr1 */
    "lwz    0, 5 *4(1)                    \n\t"
    "mtsrr1 0                             \n\t"
    /* r0 */
    "lwz   0, 6 *4(1)                     \n\t"
    /* r1 */
    "lwz   1, 7 *4(1)                     \n\t"
    "rfi                                  \n\t"
    FUNC_END(back_from_exception)
    );



asm(
    ".section        .excep,\"ax\",@progbits        \n"

    /* critical interrupt (critical, async) */
    ".org 0x100\n"
    FUNC_START(ppc_critical_entry)
    prepare_exception(srr2, srr3)
    handle_interrupt(1)
    FUNC_END(ppc_critical_entry)

    /* machine check (critical, async, imprecise) */
    ".org 0x200\n"
    FUNC_START(ppc_machine_entry)
    CRASH(srr2, srr3)
    FUNC_END(ppc_machine_entry)

    /* data storage */
    ".org 0x300\n"
    FUNC_START(ppc_data_storage_entry)
    prepare_exception(srr0, srr1)
    handle_exception(CPU_EXCEPTION_DATA_ERROR)
    FUNC_END(ppc_data_storage_entry)

    /* instr storage */
    ".org 0x400\n"
    FUNC_START(ppc_instr_storage_entry)
    prepare_exception(srr0, srr1)
    handle_exception(CPU_EXCEPTION_INS_ERROR)
    FUNC_END(ppc_instr_storage_entry)

    /* external (async) */
    ".org 0x500\n"
    FUNC_START(ppc_external_entry)
    prepare_exception(srr0, srr1)
    handle_interrupt(0)
    FUNC_END(ppc_external_entry)


    /* alignment */
    ".org 0x600\n"
    FUNC_START(ppc_align_entry)
    prepare_exception(srr0, srr1)
    handle_exception(CPU_EXCEPTION_DATA_ALIGN)
    FUNC_END(ppc_align_entry)

    /* program */
    ".org 0x700\n"
    FUNC_START(ppc_program_entry)
    prepare_exception(srr0, srr1)
    handle_exception(CPU_EXCEPTION_ILLEGAL_INS)
    FUNC_END(ppc_program_entry)

    /* fpu unusable */
    ".org 0x800\n"
    FUNC_START(ppc_fpu_entry)
    CRASH(srr0, srr1)
    FUNC_END(ppc_fpu_entry)

    /* syscall */
    ".org 0xc00\n"
    FUNC_START(ppc_syscall_entry)
    handle_syscall()
    FUNC_END(ppc_syscall_entry)

    /* apu unavailable */
    ".org 0xf20\n"

    /* programmable-interval timer (async) */
    ".org 0x1000\n"

    /* fixed-interval timer (async) */
    ".org 0x1010\n"

    /* watchdog (critical, async) */
    ".org 0x1020\n"

    /* data tlb */
    ".org 0x1100\n"

    /* ins tlb */
    ".org 0x1200\n"

    /* debug (critical, (a)sync) */
    ".org 0x2000\n"

    );


// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
