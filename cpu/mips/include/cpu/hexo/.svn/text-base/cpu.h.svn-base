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

#include <hexo/endian.h>

#define CPU_CPU_H_

#ifdef CONFIG_ARCH_SMP
extern void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];
#endif

/** general purpose regsiters count */
#define CPU_GPREG_COUNT	32

#define CPU_GPREG_NAMES {											   \
"pc", "at", "v0", "v1", "a0", "a1", "a2", "a3",						   \
"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",						   \
"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",						   \
"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",						   \
}

#define CPU_FAULT_COUNT 32

#define CPU_FAULT_NAMES {			\
"Interrupt",					\
"TLB Modification",				\
"TLB Load error",				\
"TLB Store error",				\
"Address error (Load)",				\
"Address error (Store)",			\
"Instruction bus error",			\
"Data bus error",				\
"Syscall",					\
"Break point",					\
"Reserved instruction",				\
"Coproc unusable",				\
"Overflow",					\
"Trap",						\
"Reserved",					\
"Floating point",				\
    "-",        "-",    "C2E",      "-",	\
    "-",        "-",    "MDMX",     "WATCH",	\
    "MCheck",   "-",    "-",        "-",	\
    "-",        "-",    "CacheErr", "-"		\
}

#define CPU_EXCEPTION_DATA_ALIGN   0x4 // ADEL
#define CPU_EXCEPTION_INS_ERROR    0x6 // IBE
#define CPU_EXCEPTION_DATA_ERROR   0x7 // DBE
#define CPU_EXCEPTION_BREAKPOINT   0x9 // Bp
#define CPU_EXCEPTION_ILLEGAL_INS  0xa // RI
#define CPU_EXCEPTION_COPROC       0xb // CpU
#define CPU_EXCEPTION_TRAP         0xd // Tr
#define CPU_EXCEPTION_FPE          0xf // FPE

#if __mips >= 32 

#define cpu_mips_mfc0(id, sel)			\
({						\
  reg_t _reg;					\
						\
  asm volatile ("mfc0	%0,	$%1, %2	\n"	\
		: "=r" (_reg)			\
		: "i" (id)			\
		, "i" (sel)			\
		);				\
						\
  _reg;						\
})

#define cpu_mips_mtc0(id, sel, val)		\
({						\
  reg_t _reg = val;				\
						\
  asm volatile ("mtc0	%0,	$%1, %2	\n"	\
				:: "r" (_reg)	\
				, "i" (id)	\
		, "i" (sel)			\
		);				\
})

#define cpu_mips_mfc2(id, sel)			\
({						\
  reg_t _reg;					\
						\
  asm volatile ("mfc2	%0,	$%1, %2	\n"	\
		: "=r" (_reg)			\
		: "i" (id)			\
		, "i" (sel)			\
		);				\
						\
  _reg;						\
})

#define cpu_mips_mtc2(id, sel, val)		\
({						\
  reg_t _reg = val;				\
						\
  asm volatile ("mtc2	%0,	$%1, %2	\n"	\
				:: "r" (_reg)	\
				, "i" (id)	\
		, "i" (sel)			\
		);				\
})


#else

#define cpu_mips_mfc0(id, sel)			\
({						\
  reg_t _reg;					\
						\
  asm volatile ("mfc0	%0,	$%1 \n"		\
		: "=r" (_reg)			\
		: "i" (id)			\
		);				\
						\
  _reg;						\
})

#define cpu_mips_mtc0(id, sel, val)										   \
({						\
  reg_t _reg = val;				\
						\
  asm volatile ("mtc0	%0,	$%1 \n"		\
				:: "r" (_reg)	\
				, "i" (id)	\
		);				\
})

#define cpu_mips_mfc2(id, sel)			\
({						\
  reg_t _reg;					\
						\
  asm volatile ("mfc2	%0,	$%1 \n"		\
		: "=r" (_reg)			\
		: "i" (id)			\
		);				\
						\
  _reg;						\
})

#define cpu_mips_mtc2(id, sel, val)		\
({						\
  reg_t _reg = val;				\
						\
  asm volatile ("mtc2	%0,	$%1 \n"		\
				:: "r" (_reg)	\
				, "i" (id)	\
		);				\
})


#endif

#define CPU_TYPE_NAME mips32

static inline cpu_id_t
cpu_id(void)
{
	return (reg_t)cpu_mips_mfc0(15, 1) & (reg_t)0x000003ff;
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
  return cpu_mips_mfc0(9, 0);
}

static inline void
cpu_trap()
{
  asm volatile ("break 0");
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
#if __mips >= 32
		" cache %0, %1"
		: : "i" (0x11) , "R" (*(uint8_t*)(ptr))
#else
# ifdef CONFIG_ARCH_SOCLIB
		" lw $0, (%0)"
		: : "r" (ptr)
# else
		"nop"::
# endif
#endif
		: "memory"
		);
}

static inline size_t cpu_dcache_line_size()
{
  reg_t r0 = cpu_mips_mfc0(16, 0);
  reg_t r1 = cpu_mips_mfc0(16, 1);

  if (BIT_EXTRACT(r0, 31))
    {
      r1 = BITS_EXTRACT_FL(r1, 10, 12);

      if (r1)
	return 2 << r1;
    }

  return 8;
}

#endif

