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

#include <hexo/asm.h>

#ifdef CONFIG_SOCLIB_MEMCHECK
# include <arch/mem_checker.h>
#endif

asm(
    ".section        .text,\"ax\",@progbits     \n"

    FUNC_START(cpu_boot)

    /* get CPU id and adjust stack */
    "lis    9, __initial_stack@ha    \n"
    "la     1, __initial_stack@l(9)  \n"
    "mfdcr  29,0                         \n"

#ifndef CONFIG_ARCH_SMP
    /* only first CPU is allowed to boot */
    "cmpwi  cr0, 29, 0                  \n"
    "1:                                 \n"
    "bne    cr0, 1b                     \n"
#endif

    "slwi   3,29, " ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) " \n"
    "sub    1,1,3                       \n"

    "li    3, 0                         \n"
    "mtmsr 3                            \n"

#ifdef CONFIG_SOCLIB_MEMCHECK
    "addi   2,  0,  1 << " ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) "\n"

    /* enter memchecker command mode */
    "lis    0, (" ASM_STR(SOCLIB_MC_MAGIC_VAL) ")@h  \n"
    "ori 0, 0, (" ASM_STR(SOCLIB_MC_MAGIC_VAL) ")@l  \n"
    "stw    0,  " ASM_STR(SOCLIB_MC_MAGIC) "(0) \n"

    /* create a new initial memchecker context using cpuid as context id */
    "stw    2,  " ASM_STR(SOCLIB_MC_R2) "(0) \n"
    "subf   2,  2,  1                   \n"
    "stw    2,  " ASM_STR(SOCLIB_MC_R1) "(0) \n"
    "stw    29, " ASM_STR(SOCLIB_MC_CTX_CREATE) "(0) \n"

    /* switch to new memchecker context */
    "stw    29, " ASM_STR(SOCLIB_MC_CTX_SET) "(0) \n"

    /* enable all memchecker checks */
    "addi   0,  0,  " ASM_STR(SOCLIB_MC_CHECK_ALL) " \n"
    "stw    0,  " ASM_STR(SOCLIB_MC_ENABLE) "(0) \n"

# ifdef CONFIG_ARCH_SMP
    /* mark cpu_init_flag variable as initialized */
    "lis    2, cpu_init_flag@ha          \n"
    "la     2, cpu_init_flag@l(2)        \n"
    "stw    2,  " ASM_STR(SOCLIB_MC_INITIALIZED) "(0) \n"
# endif
#endif

    /* adjust stack pointer */
    "addi   1,  1, -" ASM_STR(CONFIG_HEXO_STACK_ALIGN) " \n"

#ifdef CONFIG_SOCLIB_MEMCHECK
    /* leave memchecker command mode */
    "stw    3,  " ASM_STR(SOCLIB_MC_MAGIC) "(0) \n"
#endif

    /* get device-tree and put it in arg[0] */
#ifdef CONFIG_ARCH_DEVICE_TREE
    "lis    3, dt_blob_start@ha          \n"
    "la     3, dt_blob_start@l(3)        \n"
#else
    "li     3, 0                         \n"
#endif
    /* put a null in arg[1] */
    "li     4, 0                         \n"

    "lis    2, __exception_base_ptr@ha   \n"
    "la     2, __exception_base_ptr@l(2) \n"
    "mtevpr 2                            \n"

    /* jumpto arch_init function */
    "lis    2, arch_init@ha              \n"
    "la         2, arch_init@l(2)        \n"
    "mtctr      2                        \n"
    "bctr                                \n"
    FUNC_END(cpu_boot)
    );

asm(
    ".section        .boot,\"ax\",@progbits  \n\t"

    FUNC_START(cpu_boot_pointer)
    "1:                                      \n\t"
    "lis    3, cpu_boot@ha                   \n\t"
    "la     3, cpu_boot@l(3)                 \n\t"
    "mtctr  3                                \n\t"
    "bctr                                    \n\t"
    ".org 0x80-4                             \n\t"
    "b      1b                               \n\t"
    FUNC_END(cpu_boot_pointer)
    );


// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
