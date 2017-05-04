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


/**
   @file Task local and CPU local variables access
  */

#if !defined(LOCAL_H_) || defined(CPU_LOCAL_H_)
#error This file can not be included directly
#else

#define CPU_LOCAL_H_

/************************************************************************/

#define CONTEXT_LOCAL	__attribute__((section (".contextdata")))

#define CONTEXT_LOCAL_SET(n, v)  { __asm__ ("mov %1, %%gs:%0" : "=m" (n) : "r" ((typeof(n))v)); }

#define CONTEXT_LOCAL_GET(n)    ({ typeof(n) _val_; __asm__ ("mov %%gs:%1, %0" : "=r" (_val_) : "m" (n)); _val_; })

#define CONTEXT_GET_TLS()   ({ uintptr_t _ptr_; __asm__ ("movl %%gs:__context_data_base, %0" : "=r" (_ptr_)) ; _ptr_; })

/************************************************************************/

#ifdef CONFIG_ARCH_SMP

# undef CPU_LOCAL
# define CPU_LOCAL	__attribute__((section (".cpudata")))

# define CPU_LOCAL_SET(n, v)  { __asm__ ("mov %1, %%fs:%0" : "=m" (n) : "r" ((typeof(n))v)); }

# define CPU_LOCAL_GET(n)    ({ typeof(n) _val_; __asm__ ("mov %%fs:%1, %0" : "=r" (_val_) : "m" (n)); _val_; })

# define CPU_GET_CLS()   ({ uintptr_t _ptr_; __asm__ ("movl %%fs:__cpu_data_base, %0" : "=r" (_ptr_)); _ptr_; })

#else /* CONFIG_ARCH_SMP */

# define CPU_LOCAL

#endif /* !CONFIG_ARCH_SMP */

/** pointer to cpu local storage itself */
extern CPU_LOCAL void *__cpu_data_base;
/** pointer to context local storage itself */
extern CONTEXT_LOCAL void *__context_data_base;

#endif

