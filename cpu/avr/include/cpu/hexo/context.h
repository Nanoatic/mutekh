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

#if 0
/* This is the boolean table of registers clobbered on function calls
   from gcc sources */
# define CALL_USED_REGISTERS {                   \
1,1,/* r0 r1 */                               \
    0,0,/* r2 r3 */                             \
    0,0,/* r4 r5 */                             \
    0,0,/* r6 r7 */                             \
    0,0,/* r8 r9 */                             \
    0,0,/* r10 r11 */                           \
    0,0,/* r12 r13 */                           \
    0,0,/* r14 r15 */                           \
    0,0,/* r16 r17 */                           \
    1,1,/* r18 r19 */                           \
    1,1,/* r20 r21 */                           \
    1,1,/* r22 r23 */                           \
    1,1,/* r24 r25 */                           \
    1,1,/* X r26 r27 */                           \
    0,0,/* Y r28 r29 */                           \
    1,1,/* Z r30 r31 */                           \
    1,1,/*  STACK */                            \
    1,1 /* arg pointer */  }

 */
#endif

static inline void
cpu_context_switch(struct context_s *old, struct context_s *future)
{
  register void	*tmp0, *tmp1;

  asm volatile (
		/* save execution pointer based on current PC */
		"	rcall	1f				\n"
		"	rjmp	2f				\n"
		"1:						\n"

		"	push	r28				\n"
		"	push	r29				\n"

		/* save flags */
		"	in	r0, 0x3f			\n"
		"	push	r0				\n"
		"	cli					\n"
		/* save context local storage on stack */
		"	lds	r0, __context_data_base + 1	\n"
		"	push	r0				\n"
		"	lds	r0, __context_data_base	\n"
		"	push	r0				\n"
		/* switch stack pointer */
		"	in	r0, 0x3d			\n"
		"	st	Z+, r0				\n"
		"	in	r0, 0x3e			\n"
		"	st	Z, r0				\n"
		"	out	0x3d, %A1			\n"
		"	out	0x3e, %B1			\n"
		/* restore tls */
		"	pop	r0				\n"
		"	sts	__context_data_base, r0	\n"
		"	pop	r0				\n"
		"	sts	__context_data_base + 1, r0	\n"
		/* restore flags */
		"	pop	r0				\n"
		"	out	0x3f, r0			\n"

		"	pop	r29				\n"
		"	pop	r28				\n"

		"	ret					\n"
		"2:						\n"

		: "=z" (tmp0)
		, "=x" (tmp1)

		: "0" (&old->stack_ptr)
		, "1" (future->stack_ptr)
		/* These GP registers will be saved by the compiler */
		: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
		, "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17"
		, "r18", "r19", "r20", "r21"
		, "r22", "r23"
		, "r24", "r25"
		/* , "r26", "r27" used as input */
		/* , "r28", "r29" preserved on function calls, saved in
		   assembly code above since gcc complains about this
		   register being clobbered */
		/* , "r30", "r31" used as input */
		);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_jumpto(struct context_s *future)
{
  asm volatile (
		"	out	0x3d, %A0			\n"
		"	out	0x3e, %B0			\n"
		/* restore tls */
		"	pop	r0				\n"
		"	sts	__context_data_base, r0	\n"
		"	pop	r0				\n"
		"	sts	__context_data_base + 1, r0	\n"
		/* restore flags */
		"	pop	r0				\n"
		"	out	0x3f, r0			\n"

		"	pop	r29				\n"
		"	pop	r28				\n"

		"	ret					\n"
		"2:						\n"
		: 
		: "z" (future->stack_ptr)

		/* These GP registers will be saved by the compiler */
		: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
		, "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17"
		, "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25"
		, "r26", "r27" /* , "r28", "r29" */ /* "r30", "r31" used as input */
		);
}

static inline void
__attribute__((always_inline, noreturn))
cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
  asm volatile (
		"	out	0x3d, %A0			\n"
		"	out	0x3e, %B0			\n"
		"	ijmp					\n"
		:
		: "e" (stack + stack_size - CONFIG_HEXO_STACK_ALIGN)
		, "z" (jumpto)
		);
}

#endif

