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

#ifndef TERMUI_GETLINE_PV_H_
#define TERMUI_GETLINE_PV_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <termui/getline.h>
#include <termui/bhv.h>

struct				termui_getline_s
{
  termui_cursor_pos_t		width;		/* term width */
  termui_cursor_pos_t		offset;		/* prompt len % term width */
  char				*copy;		/* copy and paste buffer */

  char				**hist;		/* history table */
  termui_hist_index_t		hist_cur;
  termui_hist_index_t		hist_count;	/* current entry count */
  termui_hist_index_t		hist_size;	/* max entry count */

  termui_strlen_t		size;		/* line buffer size */
  char				*line;		/* available line buffer */

  /* edit buffer content pointers */
  char				*buf;		/* current edit buffer start */
  char				*cursor;	/* cursor position */
  char				*end;		/* end of current line */
  char				*max;		/* end of max line size */

  termui_getline_complete_t		*complete;
  termui_getline_prompt_t		*prompt;
};

termui_err_t termui_getline_init(struct termui_term_s *tm,
			  struct termui_term_behavior_s *bhv,
			  termui_strlen_t size);

void termui_getline_cleanup(struct termui_term_behavior_s *bhv);

termui_err_t termui_getline_edit_init(struct termui_term_behavior_s *bhv);

void termui_getline_empty(struct termui_term_behavior_s *bhv);

void termui_getline_rewrite(struct termui_term_behavior_s *bhv);

#endif

