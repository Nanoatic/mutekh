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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <termui/bhv.h>
#include <termui/term.h>
#include "getline_pv.h"

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_complete)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  rl->complete(bhv, termui_term_private(bhv->tm));

  return TERMUI_TERM_RET_CONTINUE;
}

termui_err_t termui_getline_complete_init(struct termui_term_behavior_s *bhv,
				   termui_getline_complete_t *f)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  if (termui_term_move(bhv->tm, termui_term_dir_right, 0) != TERMUI_TERM_RET_OK)
    return TERMUI_TERM_RET_INVALID;

  rl->complete = f;
  bhv->keyevent[TERMUI_TERM_KEY_HT] = bhv_key_complete;  

  return TERMUI_TERM_RET_OK;
}

