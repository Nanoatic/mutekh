/*
    This file is part of libtermui.

    libtermui is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libtermui is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libtermui.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2006, Alexandre Becoulet <alexandre.becoulet@free.fr>

*/

#ifndef BHV_H_
#define BHV_H_

#include <termui/term_keys.h>
#include <termui/types.h>

/************************************************************************/
/* terminal behaviors */

struct			termui_term_s;
struct			termui_term_behavior_s;

#define TERMUI_TERM_FCN_EVENT(f)	termui_err_t f(struct termui_term_behavior_s *bhv)
#define TERMUI_TERM_FCN_KEYEVENT(f)	termui_err_t f(termui_key_t key, struct termui_term_behavior_s *bhv)

typedef TERMUI_TERM_FCN_EVENT(termui_term_event_t);
typedef TERMUI_TERM_FCN_KEYEVENT(termui_term_keyevent_t);

struct			termui_term_behavior_s
{
  struct termui_term_s		*tm;
  void				*bhvctx;
  termui_key_t			lastkey;

  termui_term_event_t		*bhvstart;
  termui_term_event_t		*resize;
  termui_term_keyevent_t	*keyevent[TERMUI_TERM_MAX_KEY];
};

#endif

