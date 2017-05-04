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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#ifndef TIMER_H_
#define TIMER_H_

/**
 * @file
 * @module{Mutek}
 * @short Kernel timer API
 */

#include <hexo/types.h>
#include <hexo/error.h>

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_clist.h>

struct timer_event_s;

/*
 * typedef for timer delays.
 */

typedef uint_fast32_t	timer_delay_t;

/*
 * callback functions prototype.
 */

#define TIMER_CALLBACK(f)	void (f) (struct timer_event_s	*timer,	\
					  void			*pv)

typedef TIMER_CALLBACK(timer_event_callback_t);

/*
 * this structure declares a timer with its delay and callback.
 */

struct				timer_event_s
{
  struct timer_s		*timer;
  timer_event_callback_t	*callback;
  void				*pv;
  timer_delay_t			start;
  timer_delay_t			delay;

  CONTAINER_ENTRY_TYPE(CLIST)	list_entry;
};

/*
 * container type for timer list.
 */

#define CONTAINER_LOCK_timer HEXO_SPIN
#define CONTAINER_ORPHAN_CHK_timer

CONTAINER_TYPE(timer, CLIST, struct timer_event_s, list_entry);

/*
 * this structure declares a timer list.
 */

struct			timer_s
{
  timer_root_t		root;
  timer_delay_t		ticks;
};

/*
 * functions prototypes.
 */

error_t	timer_add_event(struct timer_s		*timer,
			struct timer_event_s	*event);
error_t	timer_cancel_event(struct timer_event_s	*event,
			   bool_t		callback);
void	timer_inc_ticks(struct timer_s		*timer,
			timer_delay_t		ticks);
inline timer_delay_t	timer_get_tick(struct timer_s		*timer);

CONTAINER_PROTOTYPE(timer, inline, timer);

#ifdef CONFIG_MUTEK_TIMERMS
extern struct timer_s	timer_ms;
#endif

#define TIMER_BUSY_WAIT(timer, delay, cond)			\
({								\
  bool_t _res;							\
  timer_delay_t t = timer_get_tick(timer);			\
								\
  while ((_res = (cond)) && (delay > timer_get_tick(timer) - t)) \
    ;								\
								\
  _res;								\
})

#endif

