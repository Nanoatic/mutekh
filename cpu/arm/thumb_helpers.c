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

#include <hexo/interrupt.h>
#include <hexo/context.h>

#if defined(__thumb__)
# error This file must be compiled in ARM mode
#endif

__attribute__((noreturn))
void arm_cpu_context_jumpto(struct context_s *new)
{
    cpu_context_jumpto(new);
}

void arm_cpu_context_switch(struct context_s *old, struct context_s *new)
{
    cpu_context_switch(old, new);
}

__attribute__((noreturn))
void arm_cpu_context_set(uintptr_t stack, size_t stack_size, void *jumpto)
{
    cpu_context_set(stack, stack_size, jumpto);
}
