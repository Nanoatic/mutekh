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

#ifndef CAPSULE_TYPES_H
#define CAPSULE_TYPES_H

/**
   @file
   @module{Capsule}
   @short Capsule types
 */

#include <hexo/types.h>
#include <hexo/lock.h>
#include <mutek/semaphore.h>

/**
   @this is a condition code, aliased to Hexo error type.
 */
typedef error_t capsule_rc_t;

/**
   @this is the capsule no-error condition constant
 */
#define CAPSULE_S_OK 0

/**
   @this is the capsule abstract mutex type
 */
typedef struct semaphore_s capsule_abs_mutex_t;

/**
   @this is the capsule abstract spinlock type
 */
typedef lock_t capsule_abs_spinlock_t;

#endif
