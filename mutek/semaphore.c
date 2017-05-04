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

#include <hexo/types.h>
#include <hexo/error.h>

#include <mutek/scheduler.h>
#include <mutek/semaphore.h>

error_t semaphore_init(struct semaphore_s *semaphore, semaphore_count_t value)
{
	semaphore->count = value;
	return sched_queue_init(&semaphore->wait);
}

void semaphore_destroy(struct semaphore_s *semaphore)
{
	sched_queue_destroy(&semaphore->wait);
}

void semaphore_take(struct semaphore_s *semaphore, semaphore_count_t value)
{
	CPU_INTERRUPT_SAVESTATE_DISABLE;
	sched_queue_wrlock(&semaphore->wait);

    semaphore->count -= value;
	if (semaphore->count < 0) {
        /* add current thread in semaphore wait queue */
        semaphore->count += 1;
		sched_wait_unlock(&semaphore->wait);
    } else
		sched_queue_unlock(&semaphore->wait);

	CPU_INTERRUPT_RESTORESTATE;
}

error_t semaphore_try_take(struct semaphore_s *semaphore, semaphore_count_t value)
{
	error_t	res = 0;

	CPU_INTERRUPT_SAVESTATE_DISABLE;
	sched_queue_wrlock(&semaphore->wait);

	if ( (semaphore->count - value) < 0 )
		res = EBUSY;
	else
		semaphore->count -= value;

	sched_queue_unlock(&semaphore->wait);
	CPU_INTERRUPT_RESTORESTATE;

	return res;
}

void semaphore_give(struct semaphore_s *semaphore, semaphore_count_t value)
{
	CPU_INTERRUPT_SAVESTATE_DISABLE;
	sched_queue_wrlock(&semaphore->wait);

    semaphore->count += value;
    while ( (semaphore->count > 0) &&
            sched_wake(&semaphore->wait) )
        semaphore->count -= 1;

	sched_queue_unlock(&semaphore->wait);
	CPU_INTERRUPT_RESTORESTATE;
}

semaphore_count_t semaphore_value(struct semaphore_s *semaphore)
{
    semaphore_count_t ret;

	CPU_INTERRUPT_SAVESTATE_DISABLE;
	sched_queue_rdlock(&semaphore->wait);

	ret = semaphore->count;

	sched_queue_unlock(&semaphore->wait);
	CPU_INTERRUPT_RESTORESTATE;

    return ret;
}

