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

#if !defined(CONFIG_CPU_ARM_TLS_IN_C15)
extern CPU_LOCAL void *__context_data_base;
#endif

struct cpu_context_s
{
};

#if defined(__thumb__)
void arm_cpu_context_switch(struct context_s *old, struct context_s *future);
#endif

static inline void
cpu_context_switch(struct context_s *old, struct context_s *future)
{
#if defined(__thumb__)
    arm_cpu_context_switch(old, future);
#else
    void *old_addr = &old->stack_ptr;
    void *new_addr = &future->stack_ptr;

	/*
	  stack should be
	   * entry point (r12)
	   * fp (r11)
	   * cpsr (r3)
	   * tls (r2)
	 */

    asm volatile (
	/* save execution pointer */
	"adr  r12, 1f                      \n\t"

	/* save status */
	"mrs  r3, cpsr                     \n\t"

	/* save tls */
#if defined(CONFIG_CPU_ARM_TLS_IN_C15)
	"mrc  p15,0,r2,c13,c0,4            \n\t"
#else
	"ldr  r2, 2f                       \n\t"
	"ldr  r2, [r2]                     \n\t"
#endif

	/* save regs on stack */
	"push {r2, r3, r11, r12}           \n\t"

#ifdef CONFIG_SOCLIB_MEMCHECK

		/* enter memchecker command mode */
		"ldr  r2, =" ASM_STR(SOCLIB_MC_MAGIC_VAL) "  \n\t"
		"ldr  r3, =" ASM_STR(CONFIG_SOCLIB_MEMCHECK_ADDRESS) " \n\t"
		"str  r2, [r3, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"

		/* switch to associated memchecker context */
		"str  %1, [r3, #(" ASM_STR(SOCLIB_MC_CTX_SET_OFFSET) ")] \n\t"
#endif

        /* switch stack pointer */
	"str  sp, [%0]                     \n\t"
	"ldr  sp, [%1]                     \n\t"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"mov  r2, #0 \n\t"
		"str  r2, [r3, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"
#endif

	/* restore regs and status */
	"pop  {r2, r3, r11, r12}           \n\t"
	"msr  cpsr, r3                     \n\t"

	/* restore tls and pc */
#if defined(CONFIG_CPU_ARM_TLS_IN_C15)
	"mcr  p15,0,r2,c13,c0,4            \n\t"
	"bx   r12                          \n\t"
#else
	"ldr  r3, 2f                       \n\t"
	"str  r2, [r3]                     \n\t"
	"bx   r12                          \n\t"
	"2: .word __context_data_base  \n\t"
#endif
	"1:                                \n\t"

	: 
	: "r"(old_addr), "r"(new_addr)
	/* These registers will be saved by the compiler */
	: /* "r0",  "r1",*/  "r2", "r3", "r4", "r5", "r6", "r7"
	, "r8", "r9", "r10"
# if !defined(CONFIG_COMPILE_FRAMEPTR) || defined(__OPTIMIZE__)
		, "r11"
# endif
		, "r12", /* "sp", */ "r14"
		, "memory"
		);
#endif
}

#if defined(__thumb__)
__attribute__((noreturn))
void arm_cpu_context_jumpto(struct context_s *future);
#endif

static inline void
__attribute__((always_inline, noreturn))
cpu_context_jumpto(struct context_s *future)
{
#ifdef CONFIG_SOCLIB_MEMCHECK
	reg_t t0 = SOCLIB_MC_MAGIC_VAL;
	reg_t t1 = CONFIG_SOCLIB_MEMCHECK_ADDRESS;
#endif
#if defined(__thumb__)
    arm_cpu_context_jumpto(future);
#else
	void *new_addr = &future->stack_ptr;

	asm volatile (

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* enter memchecker command mode */
		"str  %1, [%2, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"

		/* mark current memchecker context as invalid */
		"ldr  %1, =" ASM_STR(SOCLIB_MC_CTX_ID_CURRENT) " \n\t"
		"str  %1, [%2, #(" ASM_STR(SOCLIB_MC_CTX_INVALIDATE_OFFSET) ")] \n\t"

		/* switch to associated memchecker context */
		"str  %0, [%2, #(" ASM_STR(SOCLIB_MC_CTX_SET_OFFSET) ")] \n\t"
#endif

        /* switch stack pointer */
	"ldr  sp, [%0]                     \n\t"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"mov  %1, #0 \n\t"
		"str  %1, [%2, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"
#endif

	/* restore regs and status */
	"pop  {r2, r3, r11, r12}           \n\t"
	"msr  cpsr, r3                     \n\t"

	/* restore tls */
#if defined(CONFIG_CPU_ARM_TLS_IN_C15)
	"mcr  p15,0,r2,c13,c0,4            \n\t"
#else
	"ldr  r3, =__context_data_base     \n\t"
	"str  r2, [r3]                     \n\t"
#endif

	/* restore execution pointer */
	"bx   r12                          \n\t"

	:
	: "r"(new_addr)
#ifdef CONFIG_SOCLIB_MEMCHECK
	, "r" (t0), "r" (t1)
#endif
	);

	while(1)
		;
#endif
}

#if defined(__thumb__)
__attribute__((noreturn))
void arm_cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto);
#endif

static inline void
__attribute__((always_inline, noreturn))
cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
#ifdef CONFIG_SOCLIB_MEMCHECK
	reg_t t0 = SOCLIB_MC_MAGIC_VAL;
	reg_t t1 = CONFIG_SOCLIB_MEMCHECK_ADDRESS;
#endif

#if defined(__thumb__)
    arm_cpu_context_set(stack, stack_size, jumpto);
#else

	asm volatile (
#ifdef CONFIG_SOCLIB_MEMCHECK
		/* enter memchecker command mode */
		"	str  %3, [%4, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")] \n\t"

		/* mark current context as invalid */
		"	ldr  %3, =" ASM_STR(SOCLIB_MC_CTX_ID_CURRENT) " \n\t"
		"	str  %3, [%4, #(" ASM_STR(SOCLIB_MC_CTX_INVALIDATE_OFFSET) ")] \n\t"

		/* create a new temporary context using passed stack */
		"	str  %0, [%4, #(" ASM_STR(SOCLIB_MC_R1_OFFSET) ")] \n\t"
		"	str  %1, [%4, #(" ASM_STR(SOCLIB_MC_R2_OFFSET) ")] \n\t"
		"	str  %0, [%4, #(" ASM_STR(SOCLIB_MC_CTX_CREATE_TMP_OFFSET) ")] \n\t"

		/* switch to new temporary context */
		"	str  %0, [%4, #(" ASM_STR(SOCLIB_MC_CTX_SET_OFFSET) ")] \n\t"

#endif

	/* Set stack pointer, 64 bits aligned */
	"	add  %0, %0, %1	\n\t"
	"	sub  sp, %0, #8			\n"

#ifdef CONFIG_SOCLIB_MEMCHECK
		/* leave memchecker command mode */
		"	mov  %3, #0						\n"
		"	str  %3, [%4, #(" ASM_STR(SOCLIB_MC_MAGIC_OFFSET) ")]  \n\t"
#endif

	/* jump */
	"	mov  ip, %2			\n"
	"	bx   ip				\n"
		:
		: "r" (stack)
		, "r" (stack_size)
		, "r" (jumpto)
#ifdef CONFIG_SOCLIB_MEMCHECK
		, "r" (t0), "r" (t1)
#endif
		);
	while (1);

#endif
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

