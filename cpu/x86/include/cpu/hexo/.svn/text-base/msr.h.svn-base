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

/* 
 * x86 specific defs
 */

#ifndef CPU_X86_MSR_H_
#define CPU_X86_MSR_H_

static inline uint64_t
cpu_x86_read_msr(uint32_t index)
{
  reg_t low, high;

  asm volatile("rdmsr\n"
	       : "=a" (low)
	       , "=d" (high)
	       : "c" (index)
	       );

  return low | ((uint64_t)high << 32);
}

static inline void
cpu_x86_write_msr(uint32_t index, uint64_t value)
{
  reg_t high = value >> 32;
  reg_t low = value;

  asm volatile("wrmsr\n"
	       :
	       : "a" (low)
	       , "d" (high)
	       , "c" (index)
	       );
}

#define IA32_APIC_BASE_MSR	0x01b
#define SYSENTER_CS_MSR		0x174
#define SYSENTER_ESP_MSR	0x175
#define SYSENTER_EIP_MSR	0x176

#endif

