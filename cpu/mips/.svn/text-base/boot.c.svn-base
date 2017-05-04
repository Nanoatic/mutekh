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
        ".section        .boot,\"ax\",@progbits                                            \n"

        FUNC_START(cpu_boot)

        ".set push                                                                         \n"
        ".set noreorder                                                                    \n"

        /* set up IT disable and kernel mode */
#if __mips >= 32
        /* let status.erl for eret */
# ifdef CONFIG_COMPILE_SOFTFLOAT
        "   ori     $8,     $0,     0x0000fc04                                             \n"
# else
        "   li      $8,     0x2000fc04                                                     \n"
# endif
        "   mtc0    $8,     $12                                                            \n"
# if __mips > 32
        "   ehb                                                                            \n"
# endif
#else
        "   li      $8,     0x00000000                                                     \n"
        "   mtc0    $8,     $12                                                            \n"
#endif

        /* get CPU id and adjust stack */
#if __mips >= 32
        "   mfc0    $9,     $15,    1                                                      \n"
#else
        "   mfc0    $9,     $15                                                            \n"
#endif
        "   la      $sp,    __initial_stack                                                \n"
        "   andi    $9,     $9,     0x000003ff                                             \n"

        "1:                                                                                \n"
#ifndef CONFIG_ARCH_SMP
        /* spin if cpuid != 0 */
        "   bne     $0,     $9,     1b                                                     \n"
        "   nop                                                                            \n"
#else
        /* spin if cpuid >= CONFIG_CPU_MAXCOUNT */
        "   slt     $8,     $9,     " ASM_STR(CONFIG_CPU_MAXCOUNT) "                       \n"
        "   beq     $0,     $8,     1b                                                     \n"

        "   sll     $8,     $9,     " ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) "              \n"
        "   subu    $sp,    $sp,    $8                                                     \n"
#endif

#if defined(CONFIG_COMPILE_FRAMEPTR) && !defined(__OPTIMIZE__)
        "   or      $fp,    $sp,    $0                                                     \n"
#endif

#ifdef CONFIG_SOCLIB_MEMCHECK
	        ".set push                                                                     \n"
	        ".set noat                                                                     \n"
	        "   addiu   $8,     $0,     1 << " ASM_STR(CONFIG_HEXO_RESET_STACK_SIZE) "     \n"
	
			/* enter memchecker command mode */
	        "   li      $1,     " ASM_STR(SOCLIB_MC_MAGIC_VAL) "                           \n"
	        "   sw      $1,     " ASM_STR(SOCLIB_MC_MAGIC) "($0)                           \n"
	
			/* create a new initial memchecker context using cpuid as context id */
	        "   sw      $8,     " ASM_STR(SOCLIB_MC_R2) "($0)                              \n"
	        "   subu    $8,     $sp,    $8                                                 \n"
	        "   sw      $8,     " ASM_STR(SOCLIB_MC_R1) "($0)                              \n"
	        "   sw      $9,     " ASM_STR(SOCLIB_MC_CTX_CREATE) "($0)                      \n"
			/* switch to new memchecker context */
	        "   sw      $9,     " ASM_STR(SOCLIB_MC_CTX_SET) "($0)                         \n"
	
	        /* enable all memchecker checks */
	        "   ori     $1,     $0,     " ASM_STR(SOCLIB_MC_CHECK_ALL) " \n"
	        "   sw      $1,     " ASM_STR(SOCLIB_MC_ENABLE) "($0)                          \n"
	
# ifdef CONFIG_ARCH_SMP
	        /* mark cpu_init_flag variable as initialized */
	        "   la      $8,     cpu_init_flag                                              \n"
	        "   sw      $8,     " ASM_STR(SOCLIB_MC_INITIALIZED) "($0)                     \n"
# endif
#endif

        /* adjust stack pointer, 64 bits aligned, mips abi requires 4 free words */
        "   addiu   $sp,    $sp,     -16                                                   \n"

#ifdef CONFIG_SOCLIB_MEMCHECK
            /* leave memchecker command mode */
	        "   sw      $0,     " ASM_STR(SOCLIB_MC_MAGIC) "($0)                           \n"
	        ".set pop                                                                      \n"
#endif

        "   la      $4, __exception_base_ptr                                               \n"
        "   mtc0    $4, $15, 1                                                             \n"

        /* Get the device tree and put it in first arg */
#ifdef CONFIG_ARCH_DEVICE_TREE
        "   la      $4, dt_blob_start                                                      \n"
#else
        "   move    $4, $0                                                                 \n"
#endif
        /* Put a 0 in second arg */
        "   move    $5, $0                                                                 \n"

        /* jumpto arch_init function */
        "   la      $8,     arch_init                                                      \n"
#if __mips >= 32
        "   mtc0    $8,     $30                                                            \n"
# if __mips > 32
        "   ei                                                                             \n"
# endif
        "   eret                                                                           \n"
#else
        "   j       $8                                                                     \n"
        "   nop                                                                            \n"
#endif

        ".set pop                                                                          \n"
        FUNC_END(cpu_boot)
);

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

