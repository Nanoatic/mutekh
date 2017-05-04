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
 * @file
 * @module{Hexo}
 * @short Interrupts, exceptions and syscall events management
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include "local.h"
#include "types.h"

/************************************************************ hw irq */

#ifdef CONFIG_HEXO_IRQ

/** CPU interrupt handler function template
    @see cpu_interrupt_handler_t
    @showcontent
*/
#define CPU_INTERRUPT_HANDLER(n) void (n) (uint_fast8_t irq)

/** CPU interrupt handler function type.

    @param irq interrupt line number
    @see #CPU_INTERRUPT_HANDLER
*/
typedef CPU_INTERRUPT_HANDLER(cpu_interrupt_handler_t);

/** @this sets the hardware interrupt handler for the current cpu */
void cpu_interrupt_sethandler(cpu_interrupt_handler_t *handler);

extern CPU_LOCAL struct device_s *cpu_interrupt_handler_dev;

struct device_s;

/** @this sets hardware interrupt handler device for the current cpu */
void cpu_interrupt_sethandler_device(struct device_s *dev);
#endif


/** @this disables all maskable interrupts for the current cpu.
    This acts as a compiler memory barrier. */
__attribute__ ((always_inline))
static inline void cpu_interrupt_disable();

/** @this enables all maskable interrupts for the current cpu.
    This acts as a compiler memory barrier. */
__attribute__ ((always_inline))
static inline void cpu_interrupt_enable();

/** @this saves interrupts enable state (may use stack) */
__attribute__ ((always_inline))
static inline void cpu_interrupt_savestate(reg_t *state);

/** @this saves interrupts enable state end disable interrupts.
    This acts as a compiler memory barrier. */
__attribute__ ((always_inline))
static inline void cpu_interrupt_savestate_disable(reg_t *state);

/** @this restores interrupts enable state (may use stack).
    This acts as a compiler memory barrier. */
__attribute__ ((always_inline))
static inline void cpu_interrupt_restorestate(const reg_t *state);

/** @this reads current interrupts state as boolean */
__attribute__ ((always_inline))
static inline bool_t cpu_interrupt_getstate();

/** @this checks if the cpu is interruptible */
__attribute__ ((always_inline))
static inline bool_t cpu_is_interruptible();

/** @this enables interrupts and give a change to pending requests to
    execute. This function must be used to avoid the "sti; cli"
    sequence which don't let interrupts raise on some
    processors. Memory is marked as clobbered by this function to
    force global variable reload after interrupts occured. */
__attribute__ ((always_inline))
static inline void cpu_interrupt_process();

#ifdef CONFIG_CPU_WAIT_IRQ
/** @this enables interrupts and enters in interrupt wait state. The
    @ref #CONFIG_CPU_WAIT_IRQ token may be used to check for
    availability.  */
__attribute__ ((always_inline))
static inline void cpu_interrupt_wait();
#endif

/** @showcontent
    @this saves interrupts enable state end disable interrupts. This macro
    must be matched with the @ref #CPU_INTERRUPT_RESTORESTATE macro.
    This acts as a compiler memory barrier. */
#define CPU_INTERRUPT_SAVESTATE_DISABLE				\
{								\
  reg_t	__interrupt_state;					\
  cpu_interrupt_savestate_disable(&__interrupt_state);

/** @showcontent
    @this restores interrupts enable state. This macro must be matched with
    the @ref #CPU_INTERRUPT_SAVESTATE_DISABLE macro. 
    This acts as a compiler memory barrier. */
#define CPU_INTERRUPT_RESTORESTATE				\
  cpu_interrupt_restorestate(&__interrupt_state);		\
}


/************************************************************ exceptions */

/** CPU exception handler function template
    @see cpu_exception_handler_t
    @showcontent
*/
#define CPU_EXCEPTION_HANDLER(n) void (n) (uint_fast8_t type, uintptr_t execptr, \
					   uintptr_t dataptr, reg_t *regtable, \
					   uintptr_t stackptr)
/**
   CPU exception handler function type.

   Passed regtable and stack contents may be modified by exception
   handler and are restored modified to context.

   @param type exception ID
   @param execptr faulty instruction pointer
   @param dataptr faulty memory access pointer
   @param regtable register table
   @param stackptr value of stack pointer
   @see #CPU_EXCEPTION_HANDLER
*/
typedef CPU_EXCEPTION_HANDLER(cpu_exception_handler_t);


/** Set exception interrupt handler for the current cpu */
void cpu_exception_sethandler(cpu_exception_handler_t *hndl);

#ifdef CONFIG_HEXO_USERMODE

/** Set user exception interrupt handler for the current context */
void cpu_user_exception_sethandler(cpu_exception_handler_t *hndl);

struct context_s *context;
/** Set user exception interrupt handler for the given context */
void cpu_user_exception_sethandler_ctx(struct context_s *context,
				       cpu_exception_handler_t *hndl);
#endif

/************************************************************ syscalls */

#include <hexo/context.h>

/** CPU syscall handler function template
    @see cpu_syscall_handler_t
    @showcontent
*/
#define CPU_SYSCALL_HANDLER(n) void (n) (uint_fast8_t number, reg_t *regtable)

/** CPU syscall handler function type.

    @param irq interrupt line number
    @see #CPU_SYSCALL_HANDLER
*/
typedef CPU_SYSCALL_HANDLER(cpu_syscall_handler_t);
struct context_s;

/** @this sets the syscall handler for the current context */
void cpu_syscall_sethandler(cpu_syscall_handler_t *hndl);

/** @this sets syscall interrupt handler for a given context */
void cpu_syscall_sethandler_ctx(struct context_s *context,
				cpu_syscall_handler_t *hndl);

/************************************************************/

#include "cpu/hexo/interrupt.h"

C_HEADER_END

#endif

