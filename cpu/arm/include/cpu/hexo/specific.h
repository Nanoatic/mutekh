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
    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009
*/

#ifndef __ARM_SPECIFIC_H_
#define __ARM_SPECIFIC_H_

#if defined(__thumb__)
# define THUMB_TMP_VAR uint32_t thumb_tmp
# define THUMB_TO_ARM                 \
    ".align 2               \n\t"     \
    "mov  %[adr], pc        \n\t"     \
    "add  %[adr], %[adr], #4\n\t"     \
    "bx   %[adr]            \n\t"     \
    "nop                    \n\t"     \
    ".arm                   \n\t"
# define ARM_TO_THUMB               \
    "add  %[adr], pc, #1  \n\t"     \
    "bx   %[adr]          \n\t"
# define THUMB_OUT(x...) x [adr] "=&l" (thumb_tmp)
#else
# define THUMB_TMP_VAR
# define THUMB_TO_ARM
# define ARM_TO_THUMB
# define THUMB_OUT(x, ...) x
#endif

#endif

