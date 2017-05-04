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
 * @internal
 * @short Hexo specific platform definition for GPCT
 */

#ifndef __GPCT_LOCK_HEXO_H__
#define __GPCT_LOCK_HEXO_H__

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/gpct_platform_hexo.h>

#include <hexo/lock.h>

/*************************************************************
 *	HEXO_SPIN mutex lock functions to use with container
 */

#define GPCT_LOCK_HEXO_SPIN_INITIALIZER	LOCK_INITIALIZER
#define gpct_lock_HEXO_SPIN_initializer	LOCK_INITIALIZER

typedef lock_t gpct_lock_HEXO_SPIN_type_t;

static inline gpct_error_t
gpct_lock_HEXO_SPIN_init(lock_t *lock)
{
  return lock_init(lock);
}

static inline void
gpct_lock_HEXO_SPIN_destroy(lock_t *lock)
{
  lock_destroy(lock);
}

static inline void
gpct_lock_HEXO_SPIN_wrlock(lock_t *lock)
{
  lock_spin(lock);
}

static inline void
gpct_lock_HEXO_SPIN_rdlock(lock_t *lock)
{
  lock_spin(lock);
}

static inline void
gpct_lock_HEXO_SPIN_unlock(lock_t *lock)
{
  lock_release(lock);
}

/*************************************************************
 *	HEXO_SPIN_IRQ mutex lock functions to use with container
 */

#define GPCT_LOCK_HEXO_SPIN_IRQ_INITIALIZER	LOCK_INITIALIZER
#define gpct_lock_HEXO_SPIN_IRQ_initializer	LOCK_INITIALIZER

typedef struct 
{
  lock_t lock;
  reg_t irq;
} gpct_lock_HEXO_SPIN_IRQ_type_t;

static inline gpct_error_t
gpct_lock_HEXO_SPIN_IRQ_init(gpct_lock_HEXO_SPIN_IRQ_type_t *lock)
{
  return lock_init(&lock->lock);
}

static inline void
gpct_lock_HEXO_SPIN_IRQ_destroy(gpct_lock_HEXO_SPIN_IRQ_type_t *lock)
{
  lock_destroy(&lock->lock);
}

static inline void
gpct_lock_HEXO_SPIN_IRQ_wrlock(gpct_lock_HEXO_SPIN_IRQ_type_t *lock)
{
  cpu_interrupt_savestate_disable(&lock->irq);
  lock_spin(&lock->lock);
}

static inline void
gpct_lock_HEXO_SPIN_IRQ_rdlock(gpct_lock_HEXO_SPIN_IRQ_type_t *lock)
{
  gpct_lock_HEXO_SPIN_IRQ_wrlock(lock);
}

static inline void
gpct_lock_HEXO_SPIN_IRQ_unlock(gpct_lock_HEXO_SPIN_IRQ_type_t *lock)
{
  lock_release(&lock->lock);
  cpu_interrupt_restorestate(&lock->irq);
}

C_HEADER_END

#endif

