/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009-2010
 */

#ifndef CAPSULE_ABS_ITF_H
#define CAPSULE_ABS_ITF_H

/**
   @file
   @module{Capsule}
   @short Capsule locking API
 */

#include <hexo/types.h>
#include <hexo/cpu.h>
#include <capsule_types.h>

/**
   @internal

   Blocking semaphore take operation, which runs other capsule threads
   when blocked.
 */
void _capsule_semaphore_take(struct semaphore_s *sem, int_fast8_t val);


/**
   @this initializes a mutex

   @param mutex The mutex
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_mutex_init (capsule_abs_mutex_t * mutex)
{
    semaphore_init(mutex, 1);
    return 0;
}

/**
   @this destroys a mutex

   @param mutex The mutex
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_mutex_destroy (capsule_abs_mutex_t * mutex)
{
    semaphore_destroy(mutex);
    return 0;
}

/**
   @this takes a mutex, putting the context to sleep if blocking

   @param mutex The mutex
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_mutex_lock (capsule_abs_mutex_t * mutex)
{
    _capsule_semaphore_take(mutex, 1);
    return 0;
}

/**
   @this releases a mutex

   @param mutex The mutex
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_mutex_unlock (capsule_abs_mutex_t * mutex)
{
    semaphore_give(mutex, 1);
    return 0;
}

/**
   @this initializes a spin-lock

   @param spinlock The spin-lock
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_spinlock_init (capsule_abs_spinlock_t * spinlock)
{
    lock_init(spinlock);
    return 0;
}

/**
   @this destroys a spin-lock

   @param spinlock The spin-lock
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_spinlock_destroy (capsule_abs_spinlock_t * spinlock)
{
    lock_destroy(spinlock);
    return 0;
}

/**
   @this takes a spin-lock

   @param spinlock The spin-lock
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_spinlock_lock (capsule_abs_spinlock_t * spinlock)
{
    lock_spin(spinlock);
    return 0;
}

/**
   @this releases a spin-lock

   @param spinlock The spin-lock
   @return 0 when completed correctly
 */
static inline
capsule_rc_t capsule_abs_spinlock_unlock (capsule_abs_spinlock_t * spinlock)
{
    lock_release(spinlock);
    return 0;
}

#include "capsule_mach_64bits_itf.h"

/**
   @this retrieves the real-time-stamp counter of the current CPU.

   @param time A variable where to store the timestamp
 */
static inline
void capsule_abs_get_time(capsule_mach_64bits_t * time)
{
	*time = cpu_cycle_count();
}

/**
   @this retrieves the real-time tick counter

   @param ticks A variable where to store the timestamp
 */
static inline
void capsule_abs_get_ticks(capsule_mach_64bits_t * ticks)
{
	*ticks = cpu_cycle_count();
}


#endif
