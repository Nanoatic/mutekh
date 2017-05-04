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

#ifndef __GPCT_MUTEKH_RWLOCK_H__
#define __GPCT_MUTEKH_RWLOCK_H__

#include <mutek/rwlock.h>

/*************************************************************
 *	MUTEK_RWLOCK lock functions to use with container
 */

#define GPCT_LOCK_MUTEK_RWLOCK_INITIALIZER	RWLOCK_INITIALIZER
#define gpct_lock_MUTEK_RWLOCK_initializer	RWLOCK_INITIALIZER

typedef struct rwlock_s gpct_lock_MUTEK_RWLOCK_type_t;

static inline gpct_error_t
gpct_lock_MUTEK_RWLOCK_init(struct rwlock_s *lock)
{
  return rwlock_init(lock);
}

static inline void
gpct_lock_MUTEK_RWLOCK_destroy(struct rwlock_s *lock)
{
  rwlock_destroy(lock);
}

static inline void
gpct_lock_MUTEK_RWLOCK_wrlock(struct rwlock_s *lock)
{
  rwlock_wrlock(lock);
}

static inline void
gpct_lock_MUTEK_RWLOCK_rdlock(struct rwlock_s *lock)
{
  rwlock_rdlock(lock);
}

static inline void
gpct_lock_MUTEK_RWLOCK_unlock(struct rwlock_s *lock)
{
  rwlock_unlock(lock);
}

#endif

