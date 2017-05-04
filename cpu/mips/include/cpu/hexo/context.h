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

#if !defined(CONTEXT_H_) || defined(CPU_CONTEXT_H_)
#error This file can not be included directly
#else

#include <hexo/cpu.h>
#include "cpu/hexo/specific.h"

#ifdef CONFIG_SOCLIB_MEMCHECK
# include <arch/mem_checker.h>
#endif

struct cpu_context_s
{
};

static inline void
cpu_context_switch(struct context_s *old, struct context_s *future)
{
  void	*unused1, *unused2, *unused3;

  asm volatile (
	".set push			\n"
	".set noat			\n"
#ifdef CONFIG_COMPILE_PIC
	/* save execution pointer based on current PC */
	"	addiu	$sp, 	-4*4		\n"
	"	bal	1f			\n"
	"	b	2f			\n"
	"1:	sw	$31,	3*4($sp)	\n"
#else
	/* save execution pointer based on static label address */
	"	addiu	$sp, 	-4*4		\n"
	"	la	$1,	2f		\n"
	"	sw	$1,	3*4($sp)	\n"
#endif
	"	lw	$15,	(%2)		\n"

	/* save frame pointer */
	"	sw	$30,	2*4($sp)	\n"

	/* save status */
	"	mfc0	$1,	$12		\n"
	"	sw	$1,	1*4($sp)	\n"

	/* disable interrupts */
	"	ori	$1,	0x1		\n"
	"	addiu	$1,	-1		\n"
	"	mtc0	$1,	$12		\n"
	"	MTC0_WAIT			\n"

	/* save context local storage on stack */
	"	sw	$15,	0*4($sp)	\n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* enter memchecker command mode */
		"	li	$1,	" ASM_STR(SOCLIB_MC_MAGIC_VAL) " \n"
		"	sw	$1,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"

		/* switch to associated memchecker context */
		"	sw	%1,	" ASM_STR(SOCLIB_MC_CTX_SET) "($0) \n"
#endif

	/* switch stack pointer */
	"	sw	$sp,	(%0)		\n"
	"	lw	$sp,	(%1)		\n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"	sw	$0,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"
#endif

	/* restore status & tls */
	"	lw	$1,	1*4($sp)	\n"
	"	lw	$15,	0*4($sp)	\n"
	"	mtc0	$1,	$12		\n"

	/* restore frame pointer */
	"	lw	$30,	2*4($sp)	\n"

	/* restore execution pointer */
	"	lw	$1,	3*4($sp)	\n"
	"	sw	$15,	(%2)		\n"
	"	addiu	$sp,	4*4		\n"
	"	jr	$1			\n"
	"2:					\n"
	".set pop				\n"
	: "=r" (unused1)
	, "=r" (unused2)
	, "=r" (unused3)

	: "0" (&old->stack_ptr)
	, "1" (&future->stack_ptr)
	, "2" (CPU_LOCAL_ADDR(__context_data_base))

          /* These GP registers will be saved by the compiler */
        : "$2", "$3"	/* return values */
        , /*"$4", "$5",  "$6",*/  "$7" /* arguments. r4 to r6 are left for asm input */
        , "$8",  "$9",  "$10",  "$11", "$12",  "$13",  "$14",  "$15" /* temp */
        , "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23" /* saved accros function call */
        , "$24", "$25"	/* temp */
# if !defined(CONFIG_COMPILE_FRAMEPTR) || defined(__OPTIMIZE__)
	, "$30"
# endif
	, "$31"		/* return value */

        , "memory", "lo", "hi"

#ifndef CONFIG_COMPILE_SOFTFLOAT
        /* FIXME use smart fpu regs context switch */
	, "$f0", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7"
	, "$f8", "$f9", "$f10", "$f11", "$f12", "$f13", "$f14", "$f15"
	, "$f16", "$f17", "$f18", "$f19", "$f20", "$f21", "$f22", "$f23"
	, "$f24", "$f25", "$f26", "$f27", "$f28", "$f29", "$f30", "$f31"
#endif
	);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_jumpto(struct context_s *future)
{
  asm volatile (
	".set push			\n"
	".set noat			\n"
	"	lw	$15,	(%0)		\n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* enter memchecker command mode */
		"	li	$1,	" ASM_STR(SOCLIB_MC_MAGIC_VAL) " \n"
		"	sw	$1,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"

		/* mark current memchecker context as invalid */
		"	addiu	$1,	$0,	" ASM_STR(SOCLIB_MC_CTX_ID_CURRENT) " \n"
		"	sw	$1,	" ASM_STR(SOCLIB_MC_CTX_INVALIDATE) "($0) \n"

		/* switch to associated memchecker context */
		"	sw	%0,	" ASM_STR(SOCLIB_MC_CTX_SET) "($0) \n"
#endif

        /* switch stack pointer */
	"	move	$sp,	$15		\n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"	sw	$0,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"
#endif

	/* restore status & tls */
	"	lw	$1,	1*4($sp)	\n"
	"	lw	$15,	0*4($sp)	\n"
	"	mtc0	$1,	$12		\n"

	/* restore frame pointer */
	"	lw	$30,	2*4($sp)	\n"

	/* restore execution pointer */
	"	lw	$1,	3*4($sp)	\n"
	"	sw	$15,	(%1)		\n"
	"	addiu	$sp,	4*4		\n"
	"	jr	$1			\n"
	".set pop				\n"
	:
	: "r" (&future->stack_ptr)
	, "r" (CPU_LOCAL_ADDR(__context_data_base))
	);

  while (1)
    ;
}

void __attribute__((noreturn,always_inline))
cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
  asm volatile (
		".set push			\n"
		".set noat			\n"
#ifdef CONFIG_SOCLIB_MEMCHECK
		/* enter memchecker command mode */
		"	li	$1,	" ASM_STR(SOCLIB_MC_MAGIC_VAL) " \n"
		"	sw	$1,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"

		/* mark current memchecker context as invalid */
		"	addiu	$1,	$0,	" ASM_STR(SOCLIB_MC_CTX_ID_CURRENT) " \n"
		"	sw	$1,	" ASM_STR(SOCLIB_MC_CTX_INVALIDATE) "($0) \n"

		/* create a new temporary memchecker context using passed stack */
		"	sw	%0,	" ASM_STR(SOCLIB_MC_R1) "($0) \n"
		"	sw	%1,	" ASM_STR(SOCLIB_MC_R2) "($0) \n"
		"	sw	%0,	" ASM_STR(SOCLIB_MC_CTX_CREATE_TMP) "($0) \n"

		/* switch to new temporary memchecker context */
		"	sw	%0,	" ASM_STR(SOCLIB_MC_CTX_SET) "($0) \n"
#endif

	/* Set stack pointer, 64 bits aligned */
	"	addu	%0,	%0,	%1 \n"
	"	addiu	$sp,	%0,	-8 \n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"	sw	$0,	" ASM_STR(SOCLIB_MC_MAGIC) "($0) \n"
#endif

        /* Jump */
	"	j	%2		\n"
	".set pop			\n"
		:
		: "r" (stack)
                , "r" (stack_size)
		, "r" (jumpto)
		);

  while (1)
    ;
}

# if defined(CONFIG_HEXO_USERMODE)

/** kernel stack pointer value on user entry */
extern CONTEXT_LOCAL uintptr_t context_kstack;

void
__attribute__((noreturn))
cpu_context_set_user(uintptr_t kstack, uintptr_t ustack,
		     user_entry_t *entry, void *param);

# endif

#endif

