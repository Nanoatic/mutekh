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

    Copyright (c) Nicolas Pouillon, <nipo@ssji.net>, 2009

*/

#include <setjmp.h>

reg_t setjmp(jmp_buf env)
{
	register void *env_ asm("5") = env;
	register reg_t ret asm("3");

	asm volatile(
        "stw     14, 14*4(%1)\n"
        "stw     15, 15*4(%1)\n"
        "stw     16, 16*4(%1)\n"
        "stw     17, 17*4(%1)\n"
        "stw     18, 18*4(%1)\n"
        "stw     19, 19*4(%1)\n"
        "stw     20, 20*4(%1)\n"
        "stw     21, 21*4(%1)\n"
        "stw     22, 22*4(%1)\n"
        "stw     23, 23*4(%1)\n"
        "stw     24, 24*4(%1)\n"
        "stw     25, 25*4(%1)\n"
        "stw     26, 26*4(%1)\n"
        "stw     27, 27*4(%1)\n"
        "stw     28, 28*4(%1)\n"
        "stw     29, 29*4(%1)\n"
        "stw     30, 30*4(%1)\n"
        "stw     31, 31*4(%1)\n"

		/* sp */
        "stw      1,  1*4(%1)\n"
		/* lr */
        "mflr     0          \n"
        "stw      0, 32*4(%1)\n"

		"bl       1f         \n"

        "lwz     14, 14*4(%1)\n"
        "lwz     15, 15*4(%1)\n"
        "lwz     16, 16*4(%1)\n"
        "lwz     17, 17*4(%1)\n"
        "lwz     18, 18*4(%1)\n"
        "lwz     19, 19*4(%1)\n"
        "lwz     20, 20*4(%1)\n"
        "lwz     21, 21*4(%1)\n"
        "lwz     22, 22*4(%1)\n"
        "lwz     23, 23*4(%1)\n"
        "lwz     24, 24*4(%1)\n"
        "lwz     25, 25*4(%1)\n"
        "lwz     26, 26*4(%1)\n"
        "lwz     27, 27*4(%1)\n"
        "lwz     28, 28*4(%1)\n"
        "lwz     29, 29*4(%1)\n"
        "lwz     30, 30*4(%1)\n"
        "lwz     31, 31*4(%1)\n"

		/* sp */
        "lwz      1,  1*4(%1)\n"

		/* lr */
        "lwz      0, 32*4(%1)\n"
        "mtlr     0          \n"

		/* Return value should be set by the register...asm magic */
		"b       2f          \n"

		"1:                  \n"
		/* save return address */
        "mflr     0          \n"
        "stw      0, 33*4(%1)\n"

		"li       %0, 0      \n"

		"2:                  \n"

		: "=&r" (ret)
		: "r" (env_)
		: "memory"
       );
	return ret;
}

void longjmp(jmp_buf env, reg_t val)
{
	register void *env_ asm("5") = env;
	register reg_t ret asm("3") = val;

	asm volatile(
		/* Get the return address */
        "lwz      0, 33*4(%0)\n"
        "mtctr    0          \n"

		"bctr                \n"
		:
		: "r" (env_), "r" (ret)
		: "memory"
       );
}
