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

.section        .boot,"ax",@progbits

/*******************************************************
*  16 bits initialization, switch to protected mode    *
********************************************************/

/* 
 * 16 bits real mode initialisation code is used just after cpu
 * reset. It can be used when code is embedded in a ROM or for
 * booting non master CPUs in SMP systems.
*/

.code16
	/* disable interrupts */
	cli
	/* setup current real mode DATA segment */
	xorl	%eax, %eax
	movw	%ax, %ds
	/* write GDT null descriptor */
	movl	$0x00000000,	0x00
	movl	$0x00000000,	0x04
	/* write GDT flat 32 bits data segment descriptor */
	movl	$0x0000ffff,	0x08
	movl	$0x00cf9200,	0x0c
	/* write GDT flat 32 bits code segment descriptor */
	movl	$0x0000ffff,	0x10
	movl	$0x00cf9e00,	0x14
	/* set GDT register */
	movw	$0x0018,	0x1a
	movl	$0x00000000,	0x1c
	lgdt	0x1a
	/* switch to protected mode */
	movl	%cr0,		%eax
	orl	$0x00000001,	%eax /* enable protected mode */
	movl	%eax,		%cr0
	/* jump to (load) 32 bits code segment */
	jmpl	$0x10, $1f
.code32
	/* load 32 bits data segment */
1:	movl	$0x08,		%eax
	movw	%ax,		%ds
	movw	%ax,		%es
	movw	%ax,		%fs
	movw	%ax,		%gs
	movw	%ax,		%ss


/*******************************************************
*  32 bits boot vector                                 *
********************************************************/

/*
 * Entry point for already
 */

.globl cpu_boot
cpu_boot:
	cli
	movl	%cr0,		%eax
	andl	$0x9fffffff,	%eax /* enable normal cache mode */
	movl	%eax,		%cr0

	/* get CPU id */
	mov	$0x1b, %ecx
	rdmsr

#ifndef CONFIG_ARCH_SMP
	test	$0x100, %eax	; /* is bootstrap ? */
	jnz 1f
	hlt
1:
#endif

#ifdef CONFIG_ARCH_SMP
	andl	$0xfffff000, %eax /* get local APIC base address */
	mov	0x20(%eax), %eax /* read local apic ID */
	shrl	$24, %eax

	cmpl	$CONFIG_CPU_MAXCOUNT, %eax
	jl 1f
	hlt
1:	
	/* change stack offset depending on CPU id */
	shll	$13, %eax
#else
	mov	$0, %eax
#endif

	add	$0x0001fffc, %eax
	mov	%eax, %esp
	xor	%ebp, %ebp
	/* call arch_init function */
	call	arch_init
	/* stop processor execution */
	cli
	hlt

