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

    Copyright (c) Nicolas Poullon <nipo@ssji.net>, 2009
	              Alexandre Becoulet <alexandre.becoulet@free.fr>, 2009

*/


/**
   @file Task local and CPU local variables access
  */

#if !defined(LOCAL_H_) || defined(CPU_LOCAL_H_)
#error This file can not be included directly
#else

#define CPU_LOCAL_H_

#if defined(CONFIG_CPU_ARM_TLS_IN_C15)

#include "cpu/hexo/specific.h"

/************************************************************************/

# ifdef CONFIG_ARCH_SMP

#  undef CPU_LOCAL
#  define CPU_LOCAL	__attribute__((section (".cpudata")))

# define CPU_GET_CLS()												   \
	({																   \
		uintptr_t _ptr_;											   \
        THUMB_TMP_VAR;                                                 \
																	   \
		asm (														   \
            THUMB_TO_ARM                                               \
			"mrc p15,0,%[ptr],c13,c0,3\n\t"                            \
            ARM_TO_THUMB                                               \
			: [ptr] "=r" (_ptr_) /*,*/ THUMB_OUT(,)                    \
			);														   \
																	   \
		_ptr_;														   \
	})

#else /* CONFIG_ARCH_SMP */

# define CPU_LOCAL

# endif /* !CONFIG_ARCH_SMP */

/************************************************************************/

/** context local storage type attribute */
# define CONTEXT_LOCAL	__attribute__((section (".contextdata")))

/** get address of cpu local object */
# define CONTEXT_GET_TLS()											   \
	({																   \
		uintptr_t _ptr_;											   \
        THUMB_TMP_VAR;                                                 \
																	   \
		asm (														   \
            THUMB_TO_ARM                                               \
			"mrc p15,0,%[ptr],c13,c0,4\n\t"                            \
            ARM_TO_THUMB                                               \
			: [ptr] "=r" (_ptr_) /*,*/ THUMB_OUT(,)                    \
			);														   \
																	   \
		_ptr_;														   \
	})


#else /* not CONFIG_CPU_ARM_TLS_IN_C15 */

# ifdef CONFIG_ARCH_SMP
/*
 * We could support not having dedicated registers with an indirection
 * through a global table and cpuid(), but for now, we'll be lazy
 * without proper need for it.
 */
#  error No SMP supported without TLS registers in c0
# endif

# define CPU_LOCAL
# define CPU_GET_TLS() ((uintptr_t)0)

extern CPU_LOCAL void *__context_data_base;

# define CONTEXT_LOCAL	__attribute__((section (".contextdata")))
# define CONTEXT_GET_TLS() ((uintptr_t)__context_data_base)

#endif /* end CONFIG_CPU_ARM_TLS_IN_C15 */

#endif

