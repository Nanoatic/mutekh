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
   @file CPU atomic operations
*/

#if !defined(ATOMIC_H_) || defined(CPU_ATOMIC_H_)
#error This file can not be included directly
#else

#define CPU_ATOMIC_H_

#define HAS_CPU_ATOMIC_INC

#if !defined(CONFIG_ARCH_EMU) || defined(CONFIG_ARCH_SMP)
# define _SMPLOCK "lock\n"
#else
# define _SMPLOCK
#endif

static inline bool_t
cpu_atomic_inc(atomic_int_t *a)
{
  uint8_t		zero;

  asm volatile (_SMPLOCK
                "incl	%0	\n"
		"setnz		%1	\n"
		: "=m" (*a), "=q" (zero)
                :: "cc"
		);

  return zero;
}

#define HAS_CPU_ATOMIC_DEC

static inline bool_t
cpu_atomic_dec(atomic_int_t *a)
{
  uint8_t		zero;

  asm volatile (_SMPLOCK
                "decl	%0	\n"
		"setnz		%1	\n"
		: "=m" (*a), "=q" (zero)
                :: "cc"
		);

  return zero;
}

#define HAS_CPU_ATOMIC_TESTSET

static inline bool_t
cpu_atomic_bit_testset(atomic_int_t *a, uint_fast8_t n)
{
  uint8_t		isset;

  asm volatile (_SMPLOCK
                "btsl	%2, %0	\n"
		"setc		%1	\n"
		: "=m,m" (*a), "=q,q" (isset)
		: "r,I" (n)
                : "cc"
		);

  return isset;
}

#define HAS_CPU_ATOMIC_WAITSET

static inline void
cpu_atomic_bit_waitset(atomic_int_t *a, uint_fast8_t n)
{
  asm volatile ("1:	" _SMPLOCK
                "btsl	%1, %0          \n"
                "pause                  \n"
		"jc		1b	\n"
		: "=m,m" (*a)
		: "r,I" (n)
                : "cc"
		);
}

#define HAS_CPU_ATOMIC_TESTCLR

static inline bool_t
cpu_atomic_bit_testclr(atomic_int_t *a, uint_fast8_t n)
{
  uint8_t		isset;

  asm volatile (_SMPLOCK
                "btrl	%2, %0          \n"
		"setc		%1	\n"
		: "=m,m" (*a), "=q,q" (isset)
		: "r,I" (n)
                : "cc"
		);

  return isset;
}


#define HAS_CPU_ATOMIC_WAITCLR

static inline void
cpu_atomic_bit_waitclr(atomic_int_t *a, uint_fast8_t n)
{
  asm volatile ("1:	" _SMPLOCK
                "btrl	%1, %0          \n"
                "pause                  \n"
		"jnc		1b	\n"
		: "=m,m" (*a)
		: "r,I" (n)
                : "cc"
		);
}

#define HAS_CPU_ATOMIC_SET

static inline void
cpu_atomic_bit_set(atomic_int_t *a, uint_fast8_t n)
{
  asm volatile (_SMPLOCK
                "btsl	%1, %0	\n"
		: "=m,m" (*a)
		: "r,I" (n)
                : "cc"
		);
}

#define HAS_CPU_ATOMIC_CLR

static inline void
cpu_atomic_bit_clr(atomic_int_t *a, uint_fast8_t n)
{
  asm volatile (_SMPLOCK
                "btrl	%1, %0	\n"
		: "=m,m" (*a)
		: "r,I" (n)
                : "cc"
		);
}

static inline bool_t
cpu_atomic_compare_and_swap(atomic_int_t *a, atomic_int_t old, atomic_int_t future)
{
  uint8_t		done;

  asm volatile (_SMPLOCK
                "cmpxchgl	%0, %3	\n"
		"setz		%1	\n"
		: "=m,m" (*a), "=q,q" (done)
		: "a" (old), "r" (future)
                : "cc"
		);

  return done;
}

#undef _SMPLOCK

#endif

