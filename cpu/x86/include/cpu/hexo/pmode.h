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
   @file

   x86 Protected mode
 */

/* 
 * x86 specific defs
 */

#ifndef CPU_PMODE_H_
#define CPU_PMODE_H_

#include "hexo/types.h"

/** x86 segement selector integer type */
typedef uint16_t	cpu_x86_segsel_t;

/**
   x86 segment descriptor
*/

struct cpu_x86_segdesc_s
{
  uint32_t
		limit_0_15:16,
		base_0_15:16;

  uint32_t
		base_16_23:8,

		type:5,
		dpl:2,
		present:1,

		limit_16_19:4,
		available:1,
		res:1,
		deflen:1,	/* 0: 16bits, 1: 32bits */
		granul:1,	/* 0: byte granular, 1: page granular */

		base_24_31:8;
} __attribute__ ((packed));

/** x86 segment type Task 16 bits */
#define CPU_X86_SEG_CONTEXT16	0x01
/** x86 segment type Local Descriptor Table */
#define CPU_X86_SEG_LDT		0x02
/** x86 segment type Task 16 bits busy */
#define CPU_X86_SEG_CONTEXT16_BUSY	0x03
/** x86 segment type Task 32 bits */
#define CPU_X86_SEG_CONTEXT32	0x09
/** x86 segment type Task 32 bits busy */
#define CPU_X86_SEG_CONTEXT32_BUSY	0x0b
/** x86 segment type Data Expand up, Read Only */
#define CPU_X86_SEG_DATA_UP_RO	0x10
/** x86 segment type Data Expand down, Read Only */
#define CPU_X86_SEG_DATA_DN_RO	0x14
/** x86 segment type Data Expand up, Read Write */
#define CPU_X86_SEG_DATA_UP_RW	0x12
/** x86 segment type Data Expand down, Read Write */
#define CPU_X86_SEG_DATA_DN_RW	0x16
/** x86 segment type Code Non Coforming Level */
#define CPU_X86_SEG_EXEC_NC	0x18
/** x86 segment type Code Coforming Level */
#define CPU_X86_SEG_EXEC	0x1c
/** x86 segment type Code Non Coforming Level, Read */
#define CPU_X86_SEG_EXEC_NC_R	0x1a
/** x86 segment type Code Coforming Level, Read */
#define CPU_X86_SEG_EXEC_R	0x1e
/** x86 segment accessed flag */
#define CPU_X86_SEG_ACCESSED	0x01

/**
   Setup a segment descriptor

   @param desc pointer to segment descriptor in GDT/LDT
   @param base segement base address
   @param limit segement size
   @param type segement type
   @param dpl descriptor privilege level
   @param bitint32_t 1: 32 bits segment, 0: 16 bits segment
*/

static inline void cpu_x86_seg_setup(volatile struct cpu_x86_segdesc_s *desc,
				     uint32_t base, uint32_t limit, uint_fast8_t type,
				     uint_fast8_t dpl, bool_t bitint32_t)
{
  uint32_t	limit_ = limit > 0x000fffff ? limit >> 12 : limit;

  const struct cpu_x86_segdesc_s	val =
    {
      .limit_0_15 = limit_ & 0xffff,
      .base_0_15 = base & 0xffff,
      .base_16_23 = (base >> 16) & 0xff,
      .type = type,
      .dpl = dpl,
      .present = 1,
      .limit_16_19 = (limit_ >> 16) & 0xf,
      .available = 0,
      .granul = limit > 0x000fffff ? 1 : 0,
      .res = 0,
      .deflen = bitint32_t,
      .base_24_31 = (base >> 24) & 0xff,
    };

  *desc = val;
}

/**
   x86 gate descriptor
*/

struct cpu_x86_gatedesc_s
{
  uint32_t		offset_0_15:16,
		seg_selector:16;

  uint32_t		params:5,
		res1:3,

		type:4,
		res2:1,
		dpl:2,
		present:1,

		offset_16_31:16;
} __attribute__ ((packed));

/** x86 gate type 16 bits call  */
#define CPU_X86_GATE_CALL16	0x4
/** x86 gate type context */
#define CPU_X86_GATE_CONTEXT	0x5
/** x86 gate type 16 bits interrupt */
#define CPU_X86_GATE_INT16	0x6
/** x86 gate type 16 bits trap */
#define CPU_X86_GATE_TRAP16	0x7
/** x86 gate type 32 bits call */
#define CPU_X86_GATE_CALL32	0xc
/** x86 gate type 32 bits interrupt */
#define CPU_X86_GATE_INT32	0xe
/** x86 gate type 32 bits trap */
#define CPU_X86_GATE_TRAP32	0xf

/**
   Setup a gate descriptor

   @param desc pointer to gate descriptor in GDT/IDT/LDT
   @param segment index of target code segement
   @param offset jump address in code segement
   @param type gate type
   @param dpl descriptor privilege level
   @param params number of params to copy from caller stack
*/


static inline void cpu_x86_gate_setup(volatile struct cpu_x86_gatedesc_s *desc,
				      cpu_x86_segsel_t segment, uint32_t offset,
				      uint_fast8_t type, uint_fast8_t dpl, uint_fast8_t params)
{
  const struct cpu_x86_gatedesc_s	val =
    {
      .offset_0_15 = offset & 0xffff,
      .seg_selector = segment << 3,
      .params = params,
      .res1 = 0,
      .type = type,
      .res2 = 0,
      .dpl = dpl,
      .present = 1,
      .offset_16_31 = (offset >> 16) & 0xffff,
    };

  *desc = val;
}

/*
 * x86 system tables
 */

/** */

union cpu_x86_desc_s
{
  struct cpu_x86_segdesc_s	seg;
  struct cpu_x86_gatedesc_s	gate;
};


/** IDT register and GDT register structure */

struct cpu_x86_table_reg_s
{
  uint16_t		limit;
  uint32_t		base;
} __attribute__ ((packed));


/**
   Set Global Descriptor Table address

   @param address GDT address
   @param count GDT entry count
*/

static inline void
cpu_x86_set_gdt(union cpu_x86_desc_s *address, uint_fast16_t count)
{
  struct cpu_x86_table_reg_s	val =
    {
      .limit = count * sizeof(union cpu_x86_desc_s),
      .base = (uint32_t)address,
    };

  __asm__ volatile (
		    "lgdt	%0\n"
		    :
		    : "m" (val)
		    );
}


/**
   Get Global Descriptor Table address

   @param address IDT address
   @param count IDT entry count
*/

static inline union cpu_x86_desc_s *
cpu_x86_get_gdt(void)
{
  struct cpu_x86_table_reg_s	val;

  __asm__ volatile (
		    "sgdt	%0\n"
		    : "=m" (val)
		    );

  return (union cpu_x86_desc_s*)val.base;
}


/**
   Set Interrupt Descriptor Table address

   @param address IDT address
   @param count IDT entry count
*/

static inline void
cpu_x86_set_idt(struct cpu_x86_gatedesc_s *address, uint_fast16_t count)
{
  struct cpu_x86_table_reg_s	val =
    {
      .limit = count * sizeof(struct cpu_x86_gatedesc_s),
      .base = (uint32_t)address,
    };

  __asm__ volatile (
		    "lidt	%0\n"
		    :
		    : "m" (val)
		    );
}


/**
   Get Interrupt Descriptor Table address

   @param address IDT address
   @param count IDT entry count
*/

static inline struct cpu_x86_gatedesc_s *
cpu_x86_get_idt(void)
{
  struct cpu_x86_table_reg_s	val;

  __asm__ volatile (
		    "sidt	%0\n"
		    : "=m" (val)
		    );

  return (struct cpu_x86_gatedesc_s*)val.base;
}

/* 
 * x86 segmentation
 */

/** in memory segment:offset address descriptor structure */

struct			cpu_x86_seg_offset_s
{
  uint32_t			offset;
  cpu_x86_segsel_t	seg;
} __attribute__ ((packed));

/**
   Setup current data segment

   @param index segment descriptor index in GDT
   @param rpl requested privilege level
*/

static inline void
cpu_x86_dataseg_use(uint_fast16_t index, uint_fast8_t rpl)
{
  cpu_x86_segsel_t	val = (index << 3) | (rpl);

  __asm__ volatile (
		    "movw	%0, %%ds\n"
		    "movw	%0, %%es\n"
		    :
		    : "r" (val)
		    : "memory"
		    );
}

static inline void
cpu_x86_datasegfs_use(uint_fast16_t index, uint_fast8_t rpl)
{
  cpu_x86_segsel_t	val = (index << 3) | (rpl);

  __asm__ volatile (
		    "movw	%0, %%fs\n"
		    :
		    : "r" (val)
		    : "memory"
		    );
}


static inline void
cpu_x86_dataseggs_use(uint_fast16_t index, uint_fast8_t rpl)
{
  cpu_x86_segsel_t	val = (index << 3) | (rpl);

  __asm__ volatile (
		    "movw	%0, %%gs\n"
		    :
		    : "r" (val)
		    : "memory"
		    );
}

/**
   Setup current stack segment

   @param index segment descriptor index in GDT
   @param rpl requested privilege level
*/

static inline void
cpu_x86_stackseg_use(uint_fast16_t index, uint_fast8_t rpl)
{
  cpu_x86_segsel_t	val = (index << 3) | (rpl);

  __asm__ volatile (
		    "movw	%0, %%ss\n"
		    :
		    : "r" (val)
		    : "memory"
		    );
}

/**
   Setup current code segment

   @param index segment descriptor index in GDT
   @param rpl requested privilege level
*/

static inline void
cpu_x86_codeseg_use(uint_fast16_t index, uint_fast8_t rpl)
{
  __asm__ volatile (
		    "pushl %0		\n" /* CS */
		    "pushl $1f		\n" /* EIP */
		    "lret		\n"
		    "1:			\n"
		    :
		    : "r" ((index << 3) | rpl)
		    : "memory"
		    );
}

/**
   Setup current TSS segment

   @param index segment descriptor index in GDT
*/

static inline void
cpu_x86_taskseg_use(uint_fast16_t index)
{
  __asm__ volatile ("ltr %0		\n"
		    :
		    : "r,m" ((uint16_t)(index << 3))
		    );
}


struct cpu_x86_tss_s
{
  uint16_t	link, res0;

  uint32_t	esp0;
  uint16_t	ss0, res1;

  uint32_t	esp1;
  uint16_t	ss1, res2;

  uint32_t	esp2;
  uint16_t	ss2, res3;

  uint32_t	cr3;
  uint32_t	eip;
  uint32_t	eflags;
  uint32_t	eax, ecx, edx, ebx, esp, ebp, esi, edi;

  uint16_t	es, res4;
  uint16_t	cs, res5;
  uint16_t	ss, res6;
  uint16_t	ds, res7;
  uint16_t	fs, res8;
  uint16_t	gs, res9;
  uint16_t	ldt, res10;

  uint16_t	trap;
  uint16_t	iomap;
} __attribute__ ((packed));



/** index of the mandatory GDT null descriptor */
#define ARCH_GDT_NULL		0

/** index of the code segment descriptor in GDT */
#define ARCH_GDT_CODE_INDEX	1

/** index of the data segment descriptor in GDT */
#define ARCH_GDT_DATA_INDEX	2

/** index of the user level code segment descriptor in GDT */
#define ARCH_GDT_USER_CODE_INDEX	3

/** index of the user level data segment descriptor in GDT */
#define ARCH_GDT_USER_DATA_INDEX	4

/** First GDT available descriptor */
#define ARCH_GDT_FIRST_ALLOC	5

/** size of the Globale Descriptor Table for x86 CPU */
#define ARCH_GDT_SIZE		256


#define CPU_X86_SEG_SEL(index, rpl) (((index) << 3) | rpl)

/**
   allocate a descriptor in GDT and setup a segment
   @param addr segement base address
   @param type segement type
   @return segment selector in GDT, 0 if none available
*/

cpu_x86_segsel_t cpu_x86_segment_alloc(uintptr_t addr,
				       uint32_t size, uint_fast8_t type);

/**
   free a segment descriptor in GDT
 */

void cpu_x86_segdesc_free(cpu_x86_segsel_t sel);

/**************************************/
#endif

