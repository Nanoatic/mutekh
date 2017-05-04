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

#include <hexo/local.h>
#include <hexo/cpu.h>

struct cpu_context_s
{
};

static inline void
cpu_context_switch(struct context_s *old, struct context_s *future)
{
  register reg_t	tmp0, tmp1, tmp2;

  /* Note: gcc save and restore registers for us because all registers
     are marked clobbered in the asm statement. This will allow gcc to
     decide which registers must be saved so that we don't need to
     save _all_ registers ourself */

  asm volatile (
                /* preserve x86_64 abi red zone */
                "       subq    $128, %%rsp     \n"

#ifdef CONFIG_COMPILE_PIC
		/* save execution pointer */
		"	callq	1f		\n"
		"	jmp	2f		\n"
		"1:				\n"
#else
		"	pushl	2f		\n"
#endif
		/* save frame pointer */
		"	push	%%rbp		\n"
		/* save flags */
		"	pushf			\n"
//		"	cli			\n" /* FIXME */
		/* save context local storage on stack */
		"	push	(%2)		\n"
		/* switch stack pointer */
		"	movq	%%rsp, (%0)	\n"
		"	movq	(%1), %%rsp	\n"
		/* restore tls */
		"	pop	(%2)		\n"
		/* restore flags */
		"	popf			\n"
		/* restore frame pointer */
		"	pop	%%rbp		\n"
		/* restore execution pointer */
		"	retq			\n"
		"2:				\n"

                /* skip x86_64 abi red zone */
                "       addq    $128, %%rsp     \n"

		/* these input registers will be clobbered */
		: "=a" (tmp0)
		, "=b" (tmp1)
		, "=c" (tmp2)

		/* input args */
		: "0" (&old->stack_ptr)
		, "1" (&future->stack_ptr)
		, "2" (CPU_LOCAL_ADDR(__context_data_base))

		/* remaining registers will be clobbered too */
		: "memory"
		, /* "%rax"*, */ /* "%rbx", */ /* "%rcx", */ "%rdx", "%rsi", "%rdi"
		, "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
		);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_jumpto(struct context_s *future)
{
  asm volatile (
		"	movq	%0, %%rsp	\n"
		/* restore tls */
		"	pop	(%1)		\n"
 		/* restore flags */
		"	popf			\n"
		/* restore frame pointer */
		"	pop	%%rbp		\n"
		/* restore execution pointer */
		"	retq			\n"
		:
		: "r" (future->stack_ptr)
		, "r" (CPU_LOCAL_ADDR(__context_data_base))
		);

  while (1)
    ;
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
  asm volatile (
		"	movq	%0, %%rsp	\n"
		"	xorq	%%rbp, %%rbp	\n"
		"	jmpq	*%1		\n"
		:
		: "r,m" (stack + stack_size - CONFIG_HEXO_STACK_ALIGN)
                , "r,r" (jumpto)
		);

  while (1)
    ;
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_set_user(uintptr_t kstack, uintptr_t ustack, uintptr_t jumpto)
{
  cpu_trap();			/* not supported */
  while (1)
    ;
}

#endif

