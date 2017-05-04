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

#include <setjmp.h>

reg_t setjmp(jmp_buf env)
{
  register reg_t	ret;

  /* FIXME remove registers not saved across function calls */

  asm volatile(
	       /* save gp registers */
	       "movq	%%rbx, 8  (%1)	\n"
	       "movq	%%rcx, 16 (%1)	\n"
	       "movq	%%rdx, 24 (%1)	\n"
	       "movq	%%rsi, 32 (%1)	\n"
	       "movq	%%rdi, 40 (%1)	\n"
	       "movq	%%rsp, 48 (%1)	\n"
	       "movq	%%rbp, 56 (%1)	\n"

	       "movq	%%r8,  64 (%1)	\n"
	       "movq	%%r9,  72 (%1)	\n"
	       "movq	%%r10, 80 (%1)	\n"
	       "movq	%%r11, 88 (%1)	\n"
	       "movq	%%r12, 96 (%1)	\n"
	       "movq	%%r13, 104(%1)	\n"
	       "movq	%%r14, 112(%1)	\n"
	       "movq	%%r15, 120(%1)	\n"
	       "call	1f		\n"

	       /* long jump entry is here */
	       /* restore gp registers */
	       "movq	8  (%1), %%rbx	\n"
	       "movq	16 (%1), %%rcx	\n"
	       "movq	24 (%1), %%rdx	\n"
	       "movq	32 (%1), %%rsi	\n"
	       "movq	40 (%1), %%rdi	\n"
	       "movq	48 (%1), %%rsp	\n"
	       "movq	56 (%1), %%rbp	\n"

	       "movq	64 (%1), %%r8	\n"
	       "movq	72 (%1), %%r9	\n"
	       "movq	80 (%1), %%r10	\n"
	       "movq	88 (%1), %%r11	\n"
	       "movq	96 (%1), %%r12	\n"
	       "movq	104(%1), %%r13	\n"
	       "movq	112(%1), %%r14	\n"
	       "movq	120(%1), %%r15	\n"

	       /* get return value */
	       "movq	136(%1), %0	\n"
	       "jmp	2f		\n"

	       "1:			\n"
	       /* save eip pushed by call instruction */
	       "popq	128(%1)	\n"
	       /* return is 0 */
	       "xorq	%0, %0		\n"
	       "2:			\n"

	       : "=&r" (ret)
	       : "a" (env)
	       : "memory"
	       );

  return ret;
}

void longjmp(jmp_buf env, reg_t val)
{
  /* val must not be zero */
  val = (!val) ? 1 : val;

  asm volatile(
	       /* set return value */
	       "movq	%1, 136(%0)	\n"
	       /* jump back to saved eip */
	       "jmpq	*128(%0)		\n"
	       :
	       : "a" (env)
	       , "r" (val)
	       : "memory"
	       );
}

