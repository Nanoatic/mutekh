/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2006, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id$
 */

#ifndef SRL_BARRIER_H_
#define SRL_BARRIER_H_

/**
 * @file
 * @module{SRL}
 * @short Barrier operations
 */

#include <srl/srl_public_types.h>

#ifdef CONFIG_PTHREAD

/**
   Barrier wait() operation. @this blocks until the expected count of
   task are blocked at the same time. All tasks are unblocked at the
   same time.

   @param barrier The barrier to wait for
 */
static inline void srl_barrier_wait( srl_barrier_t barrier )
{
	pthread_barrier_wait( barrier );
}

#else

void srl_barrier_wait( srl_barrier_t barrier );

#endif

#endif
