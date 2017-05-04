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

#include "pthread_pv.h"


/** cancelation context linked list head */
CONTEXT_LOCAL struct __pthread_cleanup_s *__pthread_cleanup_list = NULL;

void __pthread_cancel_self(void)
{
  struct __pthread_cleanup_s		*c;

  if (atomic_bit_test(&pthread_self()->state, _PTHREAD_STATE_NOCANCEL))
    return;

  cpu_interrupt_disable();

  /* call thread cleanup handlers */
  for (c = CONTEXT_LOCAL_GET(__pthread_cleanup_list); c; c = c->prev)
    c->fcn(c->arg);

  pthread_exit(PTHREAD_CANCELED);
}

error_t
pthread_setcancelstate(int_fast8_t state, int_fast8_t *oldstate)
{
  struct pthread_s	*self = pthread_self();
  int_fast8_t o;

  if (state == PTHREAD_CANCEL_ENABLE)
    o = atomic_bit_testclr(&self->state, _PTHREAD_STATE_NOCANCEL);
  else
    o = atomic_bit_testset(&self->state, _PTHREAD_STATE_NOCANCEL);

  if (oldstate)
    *oldstate = o;

  return 0;
}

error_t
pthread_setcanceltype(int_fast8_t type, int_fast8_t *oldtype)
{
  struct pthread_s	*self = pthread_self();
  int_fast8_t o;

  if (type == PTHREAD_CANCEL_DEFERRED)
    o = atomic_bit_testclr(&self->state, _PTHREAD_STATE_CANCELASYNC);
  else
    o = atomic_bit_testset(&self->state, _PTHREAD_STATE_CANCELASYNC);

  if (oldtype)
    *oldtype = o;

  return 0;
}

