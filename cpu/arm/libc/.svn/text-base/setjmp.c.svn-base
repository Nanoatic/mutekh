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

reg_t setjmp(jmp_buf env);
asm(
	".type setjmp, %function \n"
	".globl setjmp          \n\t"
	"setjmp:                \n\t"
	"stmia   r0, {r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, lr} \n\t"
	"movs    r0, #0         \n\t"
	"bx      lr             \n\t"
	".size setjmp, .-setjmp \n\t"
	);


void longjmp(jmp_buf env, reg_t val);
asm(
	".type longjmp, %function \n"
	".globl longjmp          \n\t"
	"longjmp:                \n\t"
	"mov    r2, r0           \n\t"
	"cmp    r1, #0           \n\t"
	"movne  r0, r1           \n\t"
	"moveq  r0, #1           \n\t"
	"ldmia  r2, {r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, pc} \n\t"
	".size longjmp, .-longjmp \n\t"
	);
