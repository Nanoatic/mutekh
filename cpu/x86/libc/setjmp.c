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
	       "movl	%%ebx, 4(%1)	\n"
	       "movl	%%ecx, 8(%1)	\n"
	       "movl	%%edx, 12(%1)	\n"
	       "movl	%%esi, 16(%1)	\n"
	       "movl	%%edi, 20(%1)	\n"
	       "movl	%%esp, 24(%1)	\n"
	       "movl	%%ebp, 28(%1)	\n"
	       "call	1f		\n"

	       /* long jump entry is here */
	       /* restore gp registers */
	       "movl	4(%1), %%ebx	\n"
	       "movl	8(%1), %%ecx	\n"
	       "movl	12(%1), %%edx	\n"
	       "movl	16(%1), %%esi	\n"
	       "movl	20(%1), %%edi	\n"
	       "movl	24(%1), %%esp	\n"
	       "movl	28(%1), %%ebp	\n"

	       /* get return value */
	       "movl	36(%1), %0	\n"
	       "jmp	2f		\n"

	       "1:			\n"
	       /* save eip pushed by call instruction */
	       "popl	32(%1)		\n"
	       /* return is 0 */
	       "xorl	%0, %0		\n"
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
	       "movl	%1, 36(%0)	\n"
	       /* jump back to saved eip */
	       "jmpl	*32(%0)		\n"
	       :
	       : "a" (env)
	       , "r" (val)
	       : "memory"
	       );
}

