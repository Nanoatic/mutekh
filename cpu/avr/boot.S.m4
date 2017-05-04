/*
   Based on code from avr libgcc.S

   Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

.section        .boot,"ax",@progbits
.global cpu_boot
cpu_boot:

	rjmp .__reset_entry

m4_forloop(i, 0, m4_decr(CONFIG_CPU_AVR_IRQ_COUNT), `
	rcall	 m4_concat(__irq_entry_, i, )
	')

.global __irq_entry_default
__irq_entry_default:
	/* save registers on stack */
	push	r28
	push	r29
	push	r30
	push	r31

	/* read interrupt id */
	in	r28, 0x3d
	in	r29, 0x3e
	ldd	r24, Y+5

	lds	r30, cpu_interrupt_handler
	lds	r31, cpu_interrupt_handler + 1

	icall

	pop	r31
	pop	r30
	pop	r29
	pop	r28

	/* discard return value on stack */
	pop	r1
	pop	r1
	clr	r1

	reti



.__reset_entry:

	/* setup stack pointer */
	ldi	r16, hi8(__system_heap_end)
	out	0x3e, r16
	ldi	r16, lo8(__system_heap_end)
	out	0x3d, r16

	/* clear r1 register */
	clr	r1

	/* call arch_init function */
	call	arch_init

	/* stop processor execution */
	cli
	in	r16, 0x35
	ori	r16, 0x40
	out	0x35, r16
	sleep
1:	rjmp 1b

