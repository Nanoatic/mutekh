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

#include <hexo/interrupt.h>
#include <hexo/iospace.h>
#include <hexo/local.h>

#include "pmode.h"
#include "msr.h"
#include <drivers/icu/apic/apic.h>

/** general purpose regsiters count */
#define CPU_GPREG_COUNT	8

#define CPU_GPREG_NAMES { "edi", "esi", "ebp", "esp", "ebx", "edx", "ecx", "eax" }

#define CPU_GPREG_EDI	0
#define CPU_GPREG_ESI	1
#define CPU_GPREG_EBP	2
#define CPU_GPREG_ESP	3
#define CPU_GPREG_EBX	4
#define CPU_GPREG_EDX	5
#define CPU_GPREG_ECX	6
#define CPU_GPREG_EAX	7
		
static inline bool_t
cpu_isbootstrap(void)
{
  uint64_t	msr;

  asm ("rdmsr\n"
       : "=A" (msr)
       : "c" (0x1b)
       );

  return msr & 0x100 ? 1 : 0;
}

/**
   cpu cycle touner type
*/

typedef uint64_t cpu_cycle_t;

/**
   cpu cycle counter read function
*/

static inline cpu_cycle_t
cpu_cycle_count(void)
{
  uint32_t      low, high;

  asm volatile("rdtsc" : "=a" (low), "=d" (high));

  return (low | ((uint64_t)high << 32));
}

#ifdef CONFIG_ARCH_SMP
extern void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];
extern cpu_x86_segsel_t cpu_local_storage_seg[CONFIG_CPU_MAXCOUNT];
extern CPU_LOCAL cpu_x86_segsel_t *cpu_tls_seg;
#endif

#ifdef CONFIG_HEXO_USERMODE
extern volatile CPU_LOCAL struct cpu_x86_tss_s cpu_tss;
#endif

#define CPU_TYPE_NAME x86

static inline cpu_id_t cpu_id(void)
{
#ifdef CONFIG_ARCH_SMP
  uintptr_t x = cpu_x86_read_msr(IA32_APIC_BASE_MSR) & ~0xfff;
  return cpu_mem_read_32(x + APIC_REG_LAPIC_ID) >> 24;
#else
  return 0;
#endif
}

static inline void
cpu_trap()
{
  asm volatile ("int3");
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
#ifndef CONFIG_CPU_CACHE_COHERENCY
# error
#endif
}

static inline size_t cpu_dcache_line_size()
{
  return 0;			/* FIXME */
}

#endif

