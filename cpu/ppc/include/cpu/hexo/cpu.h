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

#ifdef CONFIG_ARCH_SMP
extern void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];
#endif

/** general purpose regsiters count */
#define CPU_GPREG_COUNT	32

#define CPU_GPREG_NAMES {											   \
		"vol", " sp", "sd1", "ar0", "ar1", " a2", " a3", " a4",		   \
			" a5", " a6", " a7", " v0", " v1", "sd0", " l0", " l1",   \
			" l2", " l3", " l4", " l5", " l6", " l7", " l8", " l9",	   \
			"l10", "l11", "l12", "l13", "l14", "l15", "l16", "l17"	   \
}

#define CPU_FAULT_COUNT 6

#define CPU_FAULT_NAMES {       \
"Unknown",                      \
"Program",                      \
"Data storage",                 \
"Instruction storage",          \
"Alignment",                    \
"Other",                        \
}

#define CPU_EXCEPTION_ILLEGAL_INS  0x1
#define CPU_EXCEPTION_DATA_ERROR   0x2
#define CPU_EXCEPTION_INS_ERROR    0x3
#define CPU_EXCEPTION_DATA_ALIGN   0x4
#define CPU_EXCEPTION_OTHER        0x5

#define CPU_TYPE_NAME powerpc

static inline cpu_id_t
cpu_id(void)
{
  reg_t		reg;

  asm volatile (
		"mfdcr %0, 0"
		: "=r" (reg)
		);

  return reg;
}

static inline bool_t
cpu_isbootstrap(void)
{
  return cpu_id() == 0;
}

/**
   cpu cycle touner type
*/

typedef uint32_t cpu_cycle_t;

/**
   cpu cycle counter read function
*/

static inline cpu_cycle_t
cpu_cycle_count(void)
{
  uint32_t      result;

  asm volatile (
		"mftbl %0"
		: "=r" (result)
		);

  return result;
}

static inline void
cpu_trap()
{
  asm volatile ("trap");
}

static inline void *cpu_get_cls(cpu_id_t cpu_id)
{
#ifdef CONFIG_ARCH_SMP
  return cpu_local_storage[cpu_id];
#endif
  return NULL;
}

static inline void cpu_dcache_invld(void *ptr)
{
  asm volatile (
		"dcbi 0, %0"
		:
		: "r" (ptr)
		: "memory"
		);
}

static inline size_t cpu_dcache_line_size()
{
  return CONFIG_CPU_CACHE_LINE;
}

#endif

