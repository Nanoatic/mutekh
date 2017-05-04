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
 * 1st level interrupt handler (entry point).
 *
 * We have as many cpu handlers as irq on x86, so we must define as
 * many entry point as supported interrupts. Each entry point code
 * must pass its irq number to the called C function.
 *
 * Here we push all registers on stack, pass argument, do
 * function call, pop registers and do interrupt return.
 *
 * Function call is done via cpu-local function pointer.
 */

#define ENTRY_ALIGN	16

# define TLS_SEG(x)	%gs x

#ifdef CONFIG_ARCH_SMP
# define CLS_SEG(x)	%fs x
#else
# define CLS_SEG(x)
#endif

/** generic entry point template */
#define PUT_ENTRY(n)			\
.align ENTRY_ALIGN				; \
        push	$0	/* dummy error code */	; \
	pusha					; \
	mov	$0x##n, %edi			; \
	jmp	3f

/** error code entry point template */
#define PUT_ENTRY_ERR(n)			\
.align ENTRY_ALIGN				; \
	pusha					; \
	mov	$0x##n, %edi			; \
	jmp	3f

/** syscakk callback */
#define PUT_TAIL_SYS(lbl)			\

/** exception callback */
#define PUT_TAIL_EX(callback, lbl)		\

/** exception with error code callback */
#define PUT_TAIL_EX_ERR(callback, lbl)		\

.text

.globl x86_interrupt_ex_entry
.align ENTRY_ALIGN
x86_interrupt_ex_entry:
PUT_ENTRY(00)
PUT_ENTRY(01)
PUT_ENTRY(02)
PUT_ENTRY(03)
PUT_ENTRY(04)
PUT_ENTRY(05)
PUT_ENTRY(06)
PUT_ENTRY(07)
PUT_ENTRY_ERR(08)
PUT_ENTRY(09)
PUT_ENTRY_ERR(0a)
PUT_ENTRY_ERR(0b)
PUT_ENTRY_ERR(0c)
PUT_ENTRY_ERR(0d)
PUT_ENTRY_ERR(0e)
PUT_ENTRY(0f)
PUT_ENTRY(10)
PUT_ENTRY_ERR(11)
PUT_ENTRY(12)
PUT_ENTRY(13)
PUT_ENTRY(14)
PUT_ENTRY(15)
PUT_ENTRY(16)
PUT_ENTRY(17)
PUT_ENTRY(18)
PUT_ENTRY(19)
PUT_ENTRY(1a)
PUT_ENTRY(1b)
PUT_ENTRY(1c)
PUT_ENTRY(1d)
PUT_ENTRY(1e)
PUT_ENTRY(1f)

3:
#ifdef CONFIG_HEXO_USERMODE
	call	x86_seg_restore
#endif
	lea	44(%esp), %ebx
	pushl	%ebx				; /* stackptr */
	lea	4(%esp), %ebx
	pushl	%ebx				; /* regtable */
	movl	%cr2, %ebx
	pushl	%ebx				; /* dataptr */
	pushl	48(%esp)			; /* execptr */
	pushl	%edi				; /* type */

#ifdef CONFIG_HEXO_USERMODE
	cmpl	$0x8, 60(%esp)			; /* test CS on stack, user mode ? */
	je	1f
	call	TLS_SEG(:) *(cpu_user_exception_handler)
	jmp	2f
#endif
1:	
	call	CLS_SEG(:) *(cpu_exception_handler)
2:	
	add	$20, %esp
	jmp	x86_interrupt_end


#ifdef CONFIG_HEXO_IRQ

.globl x86_interrupt_hw_entry
.align ENTRY_ALIGN
x86_interrupt_hw_entry:
PUT_ENTRY(00)
PUT_ENTRY(01)
PUT_ENTRY(02)
PUT_ENTRY(03)
PUT_ENTRY(04)
PUT_ENTRY(05)
PUT_ENTRY(06)
PUT_ENTRY(07)
PUT_ENTRY(08)
PUT_ENTRY(09)
PUT_ENTRY(0a)
PUT_ENTRY(0b)
PUT_ENTRY(0c)
PUT_ENTRY(0d)
PUT_ENTRY(0e)
PUT_ENTRY(0f)
PUT_ENTRY(10)
PUT_ENTRY(11)
PUT_ENTRY(12)
PUT_ENTRY(13)
PUT_ENTRY(14)
PUT_ENTRY(15)
PUT_ENTRY(16)
PUT_ENTRY(17)
PUT_ENTRY(18)
PUT_ENTRY(19)
PUT_ENTRY(1a)
PUT_ENTRY(1b)
PUT_ENTRY(1c)
PUT_ENTRY(1d)
PUT_ENTRY(1e)
PUT_ENTRY(1f)
PUT_ENTRY(20)
PUT_ENTRY(21)
PUT_ENTRY(22)
PUT_ENTRY(23)
PUT_ENTRY(24)
PUT_ENTRY(25)
PUT_ENTRY(26)
PUT_ENTRY(27)
PUT_ENTRY(28)
PUT_ENTRY(29)
PUT_ENTRY(2a)
PUT_ENTRY(2b)
PUT_ENTRY(2c)
PUT_ENTRY(2d)
PUT_ENTRY(2e)
PUT_ENTRY(2f)
PUT_ENTRY(30)
PUT_ENTRY(31)
PUT_ENTRY(32)
PUT_ENTRY(33)
PUT_ENTRY(34)
PUT_ENTRY(35)
PUT_ENTRY(36)
PUT_ENTRY(37)
PUT_ENTRY(38)
PUT_ENTRY(39)
PUT_ENTRY(3a)
PUT_ENTRY(3b)
PUT_ENTRY(3c)
PUT_ENTRY(3d)
PUT_ENTRY(3e)
PUT_ENTRY(3f)
PUT_ENTRY(40)
PUT_ENTRY(41)
PUT_ENTRY(42)
PUT_ENTRY(43)
PUT_ENTRY(44)
PUT_ENTRY(45)
PUT_ENTRY(46)
PUT_ENTRY(47)
PUT_ENTRY(48)
PUT_ENTRY(49)
PUT_ENTRY(4a)
PUT_ENTRY(4b)
PUT_ENTRY(4c)
PUT_ENTRY(4d)
PUT_ENTRY(4e)
PUT_ENTRY(4f)
PUT_ENTRY(50)
PUT_ENTRY(51)
PUT_ENTRY(52)
PUT_ENTRY(53)
PUT_ENTRY(54)
PUT_ENTRY(55)
PUT_ENTRY(56)
PUT_ENTRY(57)
PUT_ENTRY(58)
PUT_ENTRY(59)
PUT_ENTRY(5a)
PUT_ENTRY(5b)
PUT_ENTRY(5c)
PUT_ENTRY(5d)
PUT_ENTRY(5e)
PUT_ENTRY(5f)

3:
# ifdef CONFIG_HEXO_USERMODE
	call	x86_seg_restore
# endif
	pushl	%edi
	call	CLS_SEG(:) *(cpu_interrupt_handler)
	add	$4, %esp
	jmp	x86_interrupt_end

#endif


.globl x86_interrupt_sys_entry
.align ENTRY_ALIGN
x86_interrupt_sys_entry:
PUT_ENTRY(00)
PUT_ENTRY(01)
PUT_ENTRY(02)
PUT_ENTRY(03)
PUT_ENTRY(04)
PUT_ENTRY(05)
PUT_ENTRY(06)
PUT_ENTRY(07)
PUT_ENTRY(08)
PUT_ENTRY(09)
PUT_ENTRY(0a)
PUT_ENTRY(0b)
PUT_ENTRY(0c)
PUT_ENTRY(0d)
PUT_ENTRY(0e)
PUT_ENTRY(0f)
PUT_ENTRY(10)
PUT_ENTRY(11)
PUT_ENTRY(12)
PUT_ENTRY(13)
PUT_ENTRY(14)
PUT_ENTRY(15)
PUT_ENTRY(16)
PUT_ENTRY(17)
PUT_ENTRY(18)
PUT_ENTRY(19)
PUT_ENTRY(1a)
PUT_ENTRY(1b)
PUT_ENTRY(1c)
PUT_ENTRY(1d)
PUT_ENTRY(1e)
PUT_ENTRY(1f)
PUT_ENTRY(20)
PUT_ENTRY(21)
PUT_ENTRY(22)
PUT_ENTRY(23)
PUT_ENTRY(24)
PUT_ENTRY(25)
PUT_ENTRY(26)
PUT_ENTRY(27)
PUT_ENTRY(28)
PUT_ENTRY(29)
PUT_ENTRY(2a)
PUT_ENTRY(2b)
PUT_ENTRY(2c)
PUT_ENTRY(2d)
PUT_ENTRY(2e)
PUT_ENTRY(2f)
PUT_ENTRY(30)
PUT_ENTRY(31)
PUT_ENTRY(32)
PUT_ENTRY(33)
PUT_ENTRY(34)
PUT_ENTRY(35)
PUT_ENTRY(36)
PUT_ENTRY(37)
PUT_ENTRY(38)
PUT_ENTRY(39)
PUT_ENTRY(3a)
PUT_ENTRY(3b)
PUT_ENTRY(3c)
PUT_ENTRY(3d)
PUT_ENTRY(3e)
PUT_ENTRY(3f)
PUT_ENTRY(40)
PUT_ENTRY(41)
PUT_ENTRY(42)
PUT_ENTRY(43)
PUT_ENTRY(44)
PUT_ENTRY(45)
PUT_ENTRY(46)
PUT_ENTRY(47)
PUT_ENTRY(48)
PUT_ENTRY(49)
PUT_ENTRY(4a)
PUT_ENTRY(4b)
PUT_ENTRY(4c)
PUT_ENTRY(4d)
PUT_ENTRY(4e)
PUT_ENTRY(4f)
PUT_ENTRY(50)
PUT_ENTRY(51)
PUT_ENTRY(52)
PUT_ENTRY(53)
PUT_ENTRY(54)
PUT_ENTRY(55)
PUT_ENTRY(56)
PUT_ENTRY(57)
PUT_ENTRY(58)
PUT_ENTRY(59)
PUT_ENTRY(5a)
PUT_ENTRY(5b)
PUT_ENTRY(5c)
PUT_ENTRY(5d)
PUT_ENTRY(5e)
PUT_ENTRY(5f)
PUT_ENTRY(60)
PUT_ENTRY(61)
PUT_ENTRY(62)
PUT_ENTRY(63)
PUT_ENTRY(64)
PUT_ENTRY(65)
PUT_ENTRY(66)
PUT_ENTRY(67)
PUT_ENTRY(68)
PUT_ENTRY(69)
PUT_ENTRY(6a)
PUT_ENTRY(6b)
PUT_ENTRY(6c)
PUT_ENTRY(6d)
PUT_ENTRY(6e)
PUT_ENTRY(6f)
PUT_ENTRY(70)
PUT_ENTRY(71)
PUT_ENTRY(72)
PUT_ENTRY(73)
PUT_ENTRY(74)
PUT_ENTRY(75)
PUT_ENTRY(76)
PUT_ENTRY(77)
PUT_ENTRY(78)
PUT_ENTRY(79)
PUT_ENTRY(7a)
PUT_ENTRY(7b)
PUT_ENTRY(7c)
PUT_ENTRY(7d)
PUT_ENTRY(7e)
PUT_ENTRY(7f)

# ifdef CONFIG_CPU_X86_SYSENTER
.globl x86_interrupt_sys_enter
x86_interrupt_sys_enter:
	pushl	$0x23		; /* user SS */
	pushl	%ebx		; /* stack ptr */
	pushf
	cli
	pushl	$0x1b		; /* user CS */
	pushl	%eax		; /* return address */
	pushl	$0		; /* dummy error code */
	pusha
	mov	0x80, %edi
# endif

3:

#ifdef CONFIG_HEXO_USERMODE
	call	x86_seg_restore
#endif
	pushl	%esp				; /* regtable */
	pushl	%edi				; /* number */
	call	TLS_SEG(:) *(cpu_syscall_handler)
	add	$8, %esp
	jmp	x86_interrupt_end

#ifdef CONFIG_HEXO_USERMODE



	/* restore kernel mode segment registers */
x86_seg_restore:
	movl	$0x10, %eax	; /* restore kernel mode data segments */
	movl	%eax, %ds
	movl	%eax, %es

# ifdef CONFIG_ARCH_SMP
	mov	$0x1b, %ecx			; /* get cpu id */
	rdmsr
	and	$0xfffff000, %eax
	mov	0x20(%eax), %eax
	shr	$24, %eax
	movl	cpu_local_storage_seg(,%eax,2), %eax ; /* restore cls segment */
	shl	$3, %eax
	movl	%eax, CLS_SEG()
# endif
	movl	CLS_SEG(:) (cpu_tls_seg), %eax	; /* restore tls segment */
	movl	%eax, TLS_SEG()
	ret
#endif


	
	/* restore user mode segment registers */
x86_interrupt_end:	
#ifdef CONFIG_HEXO_USERMODE

	cmpl	$0x8, 40(%esp)	; /* test CS on stack, return to kernel code ? */
	je	1f
	mov	$0x20, %eax
	mov	%eax, %ds
	mov	%eax, %es
	mov	%eax, CLS_SEG()
	mov	%eax, TLS_SEG()

# ifdef CONFIG_CPU_X86_SYSENTER
	popa
	add	$4, %esp	; /* error code */

	popl	%edx		; /* return address */
	add	$4, %esp
	popf
	popl	%ecx		; /* user stack pointer */
	add	$4, %esp
	sysexit
# endif

1:
#endif
	popa
	add	$4, %esp	; /* error code */
	iret

