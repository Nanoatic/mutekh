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
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 */

#include <mutek/scheduler.h>
#include <hexo/error.h>
#include <assert.h>
#include <srl/srl_sched_wait.h>
#include <srl/srl_public_types.h>
#include <srl_private_types.h>

void srl_barrier_wait(srl_barrier_t barrier)
{
	CPU_INTERRUPT_SAVESTATE_DISABLE;
#if 0
	sched_queue_wrlock(&barrier->wait);

	if (barrier->count == 1)
    {
		while (sched_wake(&barrier->wait) != NULL)
			barrier->count++;
		sched_queue_unlock(&barrier->wait);
    }
	else
    {
		barrier->count--;
		sched_wait_unlock(&barrier->wait);
    }

#else
	lock_spin(&barrier->lock);
	cpu_dcache_invld_buf(barrier, sizeof(*barrier));

	if (barrier->count == 1) {
		barrier->count = barrier->max;
		++(barrier->serial);
		lock_release(&barrier->lock);
	} else {
		uint32_t ser = barrier->serial;
		--(barrier->count);
		lock_release(&barrier->lock);
		srl_sched_wait_ne_cpu(&barrier->serial, ser);
	}
#endif
	CPU_INTERRUPT_RESTORESTATE;
}

