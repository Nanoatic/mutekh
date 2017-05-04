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
    Copyright Joel Porquet <joel.porquet@lip6.fr> (c) 2009

*/

#include <setjmp.h>

reg_t setjmp(jmp_buf env)
{
    register reg_t ret;

    asm volatile(
            ".set push				\n"
            ".set noat				\n"
            /* save gp registers */
            "sw     $16,    16*4(%[env])\n"
            "sw     $17,    17*4(%[env])\n"
            "sw     $18,    18*4(%[env])\n"
            "sw     $19,    19*4(%[env])\n"
            "sw     $20,    20*4(%[env])\n"
            "sw     $21,    21*4(%[env])\n"
            "sw     $22,    22*4(%[env])\n"
            "sw     $23,    23*4(%[env])\n"

            "sw     $24,    24*4(%[env])\n"
            "sw     $25,    25*4(%[env])\n"

            "sw     $gp,    28*4(%[env])\n"
            "sw     $sp,    29*4(%[env])\n"
            "sw     $fp,    30*4(%[env])\n"
            "sw     $ra,    31*4(%[env])\n"

            /* return is 0 */
            "move   %[ret], $0      \n"
            ".set pop			    \n"
            : [ret] "=&r" (ret)
            : [env] "r" (env)
            : "memory"
            );

    return ret;
}

void longjmp(jmp_buf env, reg_t val)
{
    /* val must not be zero */
    val = (!val) ? 1 : val;

    asm volatile(
            ".set push				\n"
            ".set noat				\n"
            /* restore gp registers */
            "lw     $16,    16*4(%[env])\n"
            "lw     $17,    17*4(%[env])\n"
            "lw     $18,    18*4(%[env])\n"
            "lw     $19,    19*4(%[env])\n"
            "lw     $20,    20*4(%[env])\n"
            "lw     $21,    21*4(%[env])\n"
            "lw     $22,    22*4(%[env])\n"
            "lw     $23,    23*4(%[env])\n"

            "lw     $24,    24*4(%[env])\n"
            "lw     $25,    25*4(%[env])\n"

            "lw     $gp,    28*4(%[env])\n"
            "lw     $sp,    29*4(%[env])\n"
            "lw     $fp,    30*4(%[env])\n"
            "lw     $ra,    31*4(%[env])\n"

            /* set return value: */
            "move   $2,     %[val]  \n"
            "jr     $ra             \n"
            ".set pop               \n"
            :
            : [env] "r" (env)
            , [val] "r" (val)
            : "memory"
            );
}
