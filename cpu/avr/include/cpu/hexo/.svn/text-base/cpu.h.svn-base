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

#if !defined(__CPU_H_) || defined(CPU_CPU_H_)
#error This file can not be included directly
#else

#define CPU_CPU_H_

/** general purpose regsiters count */
#define CPU_GPREG_COUNT	32

#define CPU_GPREG_NAMES { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",	  \
			  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",	  \
			  "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
			  "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31" }

#define CPU_TYPE_NAME avr
#define CPU_NAME_DECL(x) avr_##x

static inline cpu_id_t
cpu_id(void)
{
	return 0;
}

static inline void *cpu_get_cls(cpu_id_t cpu_id)
{
	return NULL;
}

static inline void
cpu_trap()
{
	for(;;)
		;
}

/**
   @return true if processor is the bootstrap processor
*/

static inline bool_t
__attribute__ ((deprecated))
cpu_isbootstrap(void)
{
  return 1;
}

/**
   cpu cycle touner type
*/

typedef uint32_t cpu_cycle_t;

/**
   cpu cycle counter read function
*/

static inline cpu_cycle_t
__attribute__ ((deprecated))
cpu_cycle_count(void)
{
  return 0;
}

/** get cpu cache line size, return 0 if no dcache */
static inline size_t cpu_dcache_line_size()
{
	return 0;
}

/** invalidate the cpu data cache line containing this address */
static inline void cpu_dcache_invld(void *ptr)
{
}

#define CPU_AVR_HI8(x)		(((uintptr_t)(x)) >> 8)
#define CPU_AVR_LO8(x)		(((uintptr_t)(x)) & 0xff)

#endif

