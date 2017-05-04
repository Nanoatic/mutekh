/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2005, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id: lock.c 960 2007-02-01 15:53:49Z nipo $
 */

#include "srl.h"
#include "srl_private_types.h"

#include <pthread.h>

void srl_lock_lock(srl_lock_t lockp)
{
	srl_lock_s *l = (srl_lock_s *)lockp;
	pthread_mutex_lock(l);
}

void srl_lock_unlock(srl_lock_t lockp)
{
	srl_lock_s *l = (srl_lock_s *)lockp;
	pthread_mutex_unlock(l);
}
