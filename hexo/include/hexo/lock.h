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
 * @short Spinlock stuff
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include "types.h"
#include "error.h"
#include "interrupt.h"

struct arch_lock_s;

/** @internal */
static error_t arch_lock_init(struct arch_lock_s *lock);
/** @internal */
static void arch_lock_destroy(struct arch_lock_s *lock);
/** @internal */
static bool_t arch_lock_try(struct arch_lock_s *lock);
/** @internal */
static void arch_lock_spin(struct arch_lock_s *lock);
/** @internal */
static bool_t arch_lock_state(struct arch_lock_s *lock);
/** @internal */
static void arch_lock_release(struct arch_lock_s *lock);

#include "arch/hexo/lock.h"

struct			lock_s
{
#ifdef CONFIG_ARCH_SMP
  /** architecture specific lock data */
  struct arch_lock_s	arch;
#endif
};

typedef struct lock_s	lock_t;

#ifdef CONFIG_ARCH_SMP
# define LOCK_INITIALIZER	{ .arch = ARCH_LOCK_INITIALIZER }
#else
# define LOCK_INITIALIZER	{ }
#endif

/** allocate a new lock and return associated atomic memory location */
static inline error_t lock_init(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  return arch_lock_init(&lock->arch);
#else
  return 0;
#endif
}


/** @this frees lock ressources */
static inline void lock_destroy(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  return arch_lock_destroy(&lock->arch);
#endif
}


/** @this tries to take lock */
static inline bool_t lock_try(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  return arch_lock_try(&lock->arch);
#else
  return 0;
#endif
}


/** @this spins to take lock */
static inline void lock_spin(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  arch_lock_spin(&lock->arch);
#endif
}


/** @this returns the current lock state */
static inline bool_t lock_state(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  return arch_lock_state(&lock->arch);
#else
  return 0;
#endif
}

/** @this saves interrupts state, disables interrupts, and spins to take
    lock. This macro must be matched with the LOCK_RELEASE_IRQ macro. */
#ifdef CONFIG_HEXO_IRQ
# define LOCK_SPIN_IRQ(lock)					\
{								\
  reg_t	__interrupt_state;					\
  cpu_interrupt_savestate_disable(&__interrupt_state);		\
  lock_spin(lock);
#else
# define LOCK_SPIN_IRQ(lock)					\
  lock_spin(lock);
#endif

/** @this releases a lock */
static inline void lock_release(lock_t *lock)
{
#ifdef CONFIG_ARCH_SMP
  arch_lock_release(&lock->arch);
#endif
}

/** @this releases a lock and restore previous interrupts state. This macro
    must be matched with the LOCK_SPIN_IRQ macro. */
#ifdef CONFIG_HEXO_IRQ
# define LOCK_RELEASE_IRQ(lock)					\
  lock_release(lock);						\
  cpu_interrupt_restorestate(&__interrupt_state);		\
}
#else
# define LOCK_RELEASE_IRQ(lock)					\
  lock_release(lock);
#endif

C_HEADER_END

#endif

