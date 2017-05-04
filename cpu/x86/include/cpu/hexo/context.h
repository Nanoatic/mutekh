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

struct cpu_context_s
{
};

#ifdef CONFIG_ARCH_SMP
# define CLS_SEG	"%%fs:"
#else
# define CLS_SEG
#endif

static inline void
cpu_context_switch(struct context_s *old, struct context_s *future)
{
  reg_t	tmp0, tmp1;

  /* Note: gcc save and restore registers for us because all registers
     are marked clobbered in the asm statement. This will allow gcc to
     decide which registers must be saved so that we don't need to
     save _all_ registers ourself */

  asm volatile (
		/* save execution pointer */
#ifdef CONFIG_COMPILE_PIC
		"	call	1f		\n"
		"	jmp	2f		\n"
		"1:				\n"
#else
		"	pushl	2f		\n"
#endif
		/* save frame pointer */
		"	push	%%ebp		\n"
		/* save flags */
		"	pushf			\n"
		"	cli			\n"
#if 0
		/* save page directory pointer */
		"	movl	%%cr3, %%eax	\n"
		"	pushl	%%eax		\n"
#endif
		/* save context local storage on stack */
		"	push	%%gs		\n"
		/* switch stack pointer */
		"	movl	%%esp, (%0)	\n"
		"	movl	(%1), %%esp	\n"
		/* restore tls */
		"	pop	%%gs		\n"
#ifdef CONFIG_HEXO_USERMODE
		"	mov	%%gs, %%eax	\n"
		"	mov	%%eax, " CLS_SEG " (cpu_tls_seg) 	\n"
#endif
#if 0
		/* restore page directory pointer */
		"	popl	%%edx		\n"
		"	cmpl	%%edx, %%eax	\n" /* avoid useless TLB flush */
		"	jz	3f		\n"
		"	movl	%%edx, %%cr3	\n"
		"3:				\n"
#endif
		/* restore flags */
		"	popf			\n"
		/* restore frame pointer */
		"	pop	%%ebp		\n"
		/* restore execution pointer */
		"	ret			\n"
		"2:				\n"

		/* these input registers will be clobbered */
		: "=b" (tmp0)
		, "=S" (tmp1)

		/* input args */
		: "0" (&old->stack_ptr)
		, "1" (&future->stack_ptr)

		/* remaining registers will be clobbered too */
		: "memory"
		, "%eax", /* "%ebx", */ "%ecx", "%edx"
		, /* "%esi", */ "%edi"
		);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_jumpto(struct context_s *future)
{
  asm volatile (
		"	movl	%0, %%esp	\n"
		/* restore tls */
		"	pop	%%gs		\n"
#ifdef CONFIG_HEXO_USERMODE
		"	mov	%%gs, %%eax	\n"
		"	mov	%%eax, " CLS_SEG " (cpu_tls_seg) 	\n"
#endif
#if 0
		/* restore page directory pointer */
		"	popl	%%edx		\n"
		"	movl	%%edx, %%cr3	\n"
#endif
		/* restore flags */
		"	popf			\n"

		/* restore frame pointer */
		"	pop	%%ebp		\n"

		/* restore execution pointer */
		"	ret			\n"
		:
		: "r" (future->stack_ptr)
		);

  while (1);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
  asm volatile (
		"	movl	%0, %%esp	\n"
		"	xorl	%%ebp, %%ebp	\n"
		"	jmpl	*%1		\n"
		:
		: "r,m" (stack + stack_size - CONFIG_HEXO_STACK_ALIGN),
                  "r,r" (jumpto)
		);

  while (1);
}

# if defined(CONFIG_HEXO_USERMODE)

void __attribute__((noreturn))
cpu_context_set_user(uintptr_t kstack, uintptr_t ustack,
		     user_entry_t *entry, void *param);

# endif

#endif

