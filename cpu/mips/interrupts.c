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
    Copyright (c) 2010, Nicolas Pouillon <nipo@ssji.net>
*/

#include <hexo/asm.h>

#ifdef CONFIG_ARCH_SMP
# define CPU_LOCAL_GET(reg, name) "    lw " #reg ", " #name "($27) \n"
#else
# define CPU_LOCAL_GET(reg, name) "    lw " #reg ", " #name "\n"
#endif

#if __mips >= 32
# define CPU_ID(reg)                                                   \
    "   mfc0    " #reg ",    $15,    1           \n"                   \
    "   andi    " #reg ",    " #reg ",    0x3ff  \n"
# define STATUS_UM_BIT "0x10"
#else
# define CPU_ID(reg)                                                   \
    "   mfc0    " #reg ",    $15                 \n"                   \
    "   andi    " #reg ",    " #reg ",    0x3ff  \n"
# define STATUS_UM_BIT "0x8"
#endif

// Not in macros: $0, $26 (k0), $27 (k1), $29 (sp)

// 11 registers (but as we call ABI-compliant func, t* may not be saved)
// so only gp and ra must be saved.
// Even if we end-up switching threads, switch() will finally save them
#define DO_CALLEE_SAVED(op, reg)                 \
    "   " #op "  $28,   28 * 4(" #reg ") \n"     \
    "   " #op "  $31,   31 * 4(" #reg ") \n"
/*
    "   " #op "  $16,   16 * 4(" #reg ") \n"     \
    "   " #op "  $17,   17 * 4(" #reg ") \n"     \
    "   " #op "  $18,   18 * 4(" #reg ") \n"     \
    "   " #op "  $19,   19 * 4(" #reg ") \n"     \
    "   " #op "  $20,   20 * 4(" #reg ") \n"     \
    "   " #op "  $21,   21 * 4(" #reg ") \n"     \
    "   " #op "  $22,   22 * 4(" #reg ") \n"     \
    "   " #op "  $23,   23 * 4(" #reg ") \n"     \
    "   " #op "  $30,   30 * 4(" #reg ") \n"     \
*/


// 10 registers
#define DO_TEMP_REGS(op, reg)                    \
    "   " #op "   $8,    8 * 4(" #reg ") \n"     \
    "   " #op "   $9,    9 * 4(" #reg ") \n"     \
    "   " #op "  $10,   10 * 4(" #reg ") \n"     \
    "   " #op "  $11,   11 * 4(" #reg ") \n"     \
    "   " #op "  $12,   12 * 4(" #reg ") \n"     \
    "   " #op "  $13,   13 * 4(" #reg ") \n"     \
    "   " #op "  $14,   14 * 4(" #reg ") \n"     \
    "   " #op "  $15,   15 * 4(" #reg ") \n"     \
    "   " #op "  $24,   24 * 4(" #reg ") \n"     \
    "   " #op "  $25,   25 * 4(" #reg ") \n"

#define DO_AT_REG(op, reg)                       \
    "   " #op "   $1,    1 * 4(" #reg ") \n"

#define DO_RVAL_REGS(op, reg)                    \
    "   " #op "   $2,    2 * 4(" #reg ") \n"     \
    "   " #op "   $3,    3 * 4(" #reg ") \n"

#define DO_ARG_REGS(op, reg)                     \
    "   " #op "   $4,    4 * 4(" #reg ") \n"     \
    "   " #op "   $5,    5 * 4(" #reg ") \n"     \
    "   " #op "   $6,    6 * 4(" #reg ") \n"     \
    "   " #op "   $7,    7 * 4(" #reg ") \n"

asm(
        ".section        .excep,\"ax\",@progbits         \n"

        ".set push                                       \n"
        ".set noreorder                                  \n"
        "   b       1f                                   \n"
#if __mips >= 32
        ".space 0x17c                                    \n"
#else
        ".space 0x7c                                     \n"
#endif
        ".set pop                                        \n"

        FUNC_START(mips_interrupt_entry)
        ".set push                                       \n"
        ".set noat                                       \n"

        "1:                                              \n"

        /* restore cpu local storage */
#if defined(CONFIG_HEXO_USERMODE)

        /* event from user mode ? */
        "   mfc0    $26,    $12                          \n"
        "   andi    $26,    $26,  " STATUS_UM_BIT "      \n"

        ".set noreorder                                  \n"
        /* zero if from Kernel mode */
        "   beqz    $26,    1f                           \n"
        /* abuse the delay slot to get the current SP (sp_user_kernel) */
        "   move    $26,    $sp                          \n"
        ".set reorder                                    \n"

# if defined(CONFIG_ARCH_SMP)
        /* Restore CLS if we are from user mode */
        CPU_ID($26)
        "   sll     $26,    $26,    2                    \n"
        "   lw      $27,    cpu_local_storage($26)       \n"
# endif

        /* Get user's SP (sp_user_user) */
        "   move    $26,    $sp                          \n"

        /* restore kernel stack ! */
        CPU_LOCAL_GET($sp, __context_data_base)
        "   lw      $sp,    %lo(context_kstack)($sp)     \n"
        "1:                                              \n"
#else  /* !defined(CONFIG_HEXO_USERMODE) */
        /* Get current SP (sp_user_none) */
        "   move    $26,    $sp                          \n"
#endif

        /*
          Either:
          * We are not handling user mode and $26 is last known sp (sp_user_none)
          * We are handling user mode
           * From user mode, $26 is user's sp (sp_user_user)
           * From kernel mode, $26 is last known sp (sp_user_kernel)
         */

        /* save registers useful to syscall */
#if defined(CONFIG_LIBELF_RTLD_TLS)
        /* add room for hwrena and tls registers */
        "   addu    $sp,    -4*35                        \n"
#else
        "   addu    $sp,    -4*33                        \n"
#endif
        "   sw      $26,    29*4($sp)                    \n"

        DO_ARG_REGS(sw, $sp)

        /* read and extract cause */
        "   mfc0    $4,     $13                          \n"
        "   andi    $6,     $4,     0x3c                 \n"

        "   mfc0    $5,     $12                          \n"
        "   sb      $5,     32*4($sp)                    \n"
        /* Let's say we are in kernel mode, no exception, no irq */
# if __mips >= 32
        "   ori     $5,     $5,     0x1f                 \n"
        "   xori    $5,     $5,     0x1f                 \n"
# else
#  error Implement me
# endif
        "   mtc0    $5,     $12                          \n"

#if defined(CONFIG_LIBELF_RTLD_TLS)
        /* read & save hwrena */
        "   mfc0    $7,     $7                           \n"
        "   sw      $7,     33*4($sp)                    \n"
        /* read & save tls */
        "   mfc0    $7,     $4, 2                        \n"
        "   sw      $7,     34*4($sp)                    \n"
#endif

#if 0 && defined(CONFIG_HEXO_USERMODE)
        /* increment the user-mode counter */
        CPU_LOCAL_GET($5, __context_data_base)
        "   lw      $7,     %lo(usermode_counter)($5)    \n"
        "   addiu   $7,     $7, 1                        \n"
        "   sw      $7,     %lo(usermode_counter)($5)    \n"
#endif

        "   li      $7,     32                           \n"
        "   beq     $6,     $7,     interrupt_sys        \n"

        /* save extra registers for hw interrupts and exceptions only */

        DO_AT_REG(sw, $sp)
        DO_RVAL_REGS(sw, $sp)
        DO_TEMP_REGS(sw, $sp)
        DO_CALLEE_SAVED(sw, $sp)

        /* read & save EPC */
        "   mfc0    $5,     $14                          \n"
        "   sw      $5,     0*4($sp)                     \n"

        "   beq     $6,     $0,     interrupt_hw         \n"

        /*************************************************************
          exception handling
         **************************************************************/
        "interrupt_ex:                                   \n"

        /* exception function arguments */
        /* adjust cause arg */
        "   srl     $4,     $6,     2                    \n"
        /* execution pointer */
        //"   move    $5,     $5                           \n"
        /* bad address if any */
        "   mfc0    $6,     $8                           \n"
        /* register table on stack */
        "   addiu   $7,     $sp,    0                    \n"

        "   addiu   $sp,    $sp,    -5*4                 \n"
        "   sw      $26,    4*4($sp)                     \n"
        CPU_LOCAL_GET($1, cpu_exception_handler)
        "   jalr    $1                                   \n"
        "   addiu   $sp,    $sp,    5*4                  \n"

        /* get EPC value */
        "   lw      $26,    0*4($sp)                     \n"

        "   j       return                               \n"

        /*************************************************************
          syscall handling
         **************************************************************/
        "interrupt_sys:                                  \n"

        /* single trap on mips: id = 0 */
        "   move    $4,     $0                           \n"
        /* register table on stack */
        "   addiu   $5,     $sp,    0                    \n"
        "   addiu   $sp,    $sp,    -4*4                 \n"
        CPU_LOCAL_GET($31, __context_data_base)
        "   lw      $1,     cpu_syscall_handler($11)     \n"
        "   jalr    $1                                   \n"
        "   addiu   $sp,    $sp,    4*4                  \n"

        /* get EPC value */
        "   lw      $26,    0*4($sp)                     \n"
        /* increment epc for not doing the syscall again */
        "   addiu   $26,    4                            \n"

#ifdef CONFIG_SYSCALL_CLEAN_REGS
        /* FIXME cleanup all caller saved registers here */
#endif

        "   j       return_val                           \n"

        /*************************************************************
          hardware interrupts handling
         **************************************************************/
        "interrupt_hw:                                   \n"

#if defined(CONFIG_HEXO_IRQ)
        /* hw interrupt line id */
        "   srl     $4,     $4,     10                   \n"
        "   andi    $4,     $4,     0xff                 \n"

        "   addiu   $sp,    $sp,    -4*4                 \n"
        CPU_LOCAL_GET($1, cpu_interrupt_handler)
        "   jalr    $1                                   \n"
        "   addiu   $sp,    $sp,    4*4                  \n"
#endif

        /* get EPC value */
        "   lw      $26,    0*4($sp)                     \n"

        /************************************************************/
        /* restore registers */
        "return:                                         \n"

        DO_AT_REG(lw, $sp)
        DO_ARG_REGS(lw, $sp)
        DO_TEMP_REGS(lw, $sp)

        "return_val:                                     \n"

        DO_RVAL_REGS(lw, $sp)

        /* reload lower byte of status */
        "   lbu     $28,    32*4($sp)                    \n"
        "   mfc0    $31,    $12                          \n"
        "   ori     $31,    $31,    0xff                 \n"
        "   xori    $31,    $31,    0xff                 \n"
        "   or      $31,    $31,    $28                  \n"
        "   mtc0    $31,    $12                          \n"

#if defined(CONFIG_LIBELF_RTLD_TLS)
        /* reload hwrena */
        "   lw      $31,    33*4($sp)                    \n"
        "   mtc0    $31,    $7                           \n"
        /* reload tls */
        "   lw      $31,    34*4($sp)                    \n"
        "   mtc0    $31,    $4, 2                        \n"
#endif

        DO_CALLEE_SAVED(lw, $sp)

        /* restore user stack */
        "   lw      $sp,    29*4($sp)                    \n"

# if __mips >= 32
        /* restore epc for eret */
        "   mtc0    $26,    $14                          \n"
        "   eret                                         \n"
# else
        ".set noreorder                                  \n"
        "   jr      $26                                  \n"
        "   rfe                                          \n"
# endif

        ".set pop                                        \n"
        FUNC_END(mips_interrupt_entry)
        );

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

