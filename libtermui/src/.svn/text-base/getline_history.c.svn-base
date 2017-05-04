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

static void termui_getline_history_goto(struct termui_term_behavior_s *bhv,
				 termui_hist_index_t ndx)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  struct termui_term_s		*tm = bhv->tm;
  
  char			*line;

  if ((ndx < 0) || (ndx > rl->hist_count))
    {
      termui_term_beep(tm);
      return;
    }

  /* clear current displayed line */
  termui_getline_empty(bhv);

  /* setup new line edit buffer */
  line = rl->hist[rl->hist_cur = ndx];

  rl->max = line + rl->size - 1;
  rl->cursor = rl->end = line + strlen(line);
  rl->buf = line;

  /* display new line buffer */
  termui_getline_rewrite(bhv);
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_history_next)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_history_goto(bhv, rl->hist_cur - 1);

  return TERMUI_TERM_RET_CONTINUE;
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_history_prev)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_history_goto(bhv, rl->hist_cur + 1);

  return TERMUI_TERM_RET_CONTINUE;
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_history_rsearch)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  struct termui_term_s		*tm = bhv->tm;
  termui_hist_index_t	i;

  for (i = rl->hist_cur + 1; i <= rl->hist_count; i++)
    if (strstr(rl->hist[i], rl->line))
      {
	termui_getline_history_goto(bhv, i);
	return TERMUI_TERM_RET_CONTINUE;
      }

  termui_term_beep(tm);

  return TERMUI_TERM_RET_CONTINUE;
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_history_search)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  struct termui_term_s		*tm = bhv->tm;
  termui_hist_index_t	i;

  for (i = rl->hist_cur - 1; i >= 0; i--)
    if (strstr(rl->hist[i], rl->line))
      {
	termui_getline_history_goto(bhv, i);
	return TERMUI_TERM_RET_CONTINUE;
      }

  termui_term_beep(tm);

  return TERMUI_TERM_RET_CONTINUE;
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_history_valid)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  struct termui_term_s		*tm = bhv->tm;

  /* copy selected history line to current line buffer */
  if (rl->hist_cur != 0)
    strncpy(rl->line, rl->hist[rl->hist_cur], rl->size);

  termui_term_newline(tm);

  return TERMUI_TERM_RET_OK;
}

termui_err_t termui_getline_history_init(struct termui_term_behavior_s *bhv,
				  termui_hist_index_t size)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  if (termui_term_move(bhv->tm, termui_term_dir_right, 0) != TERMUI_TERM_RET_OK)
    return TERMUI_TERM_RET_INVALID;

  bhv->keyevent[TERMUI_TERM_KEY_UP] = bhv_key_history_prev;
  bhv->keyevent[TERMUI_TERM_KEY_DLE] = bhv_key_history_prev;
  bhv->keyevent[TERMUI_TERM_KEY_DOWN] = bhv_key_history_next;
  bhv->keyevent[TERMUI_TERM_KEY_SO] = bhv_key_history_next;
  bhv->keyevent[TERMUI_TERM_KEY_DC2] = bhv_key_history_rsearch;
  bhv->keyevent[TERMUI_TERM_KEY_DC3] = bhv_key_history_search;
  bhv->keyevent[TERMUI_TERM_KEY_RETURN] = bhv_key_history_valid;

  if (!(rl->hist = malloc(size * sizeof(char *))))
    return TERMUI_TERM_RET_IOERROR;

  rl->hist_count = 0;
  rl->hist_size = size;

  return TERMUI_TERM_RET_OK;
}

termui_err_t termui_getline_history_addlast(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  char			*line = NULL;
  termui_hist_index_t	i;

  if (!rl->hist)
    return TERMUI_TERM_RET_INVALID;    

  if (rl->hist_count + 1 >= rl->hist_size)
    line = rl->hist[rl->hist_count]; /* reuse discarded line buffer */
  else
    rl->hist_count++;

  for (i = rl->hist_count; i > 0; i--) /* shift history entries */
    rl->hist[i] = rl->hist[i - 1];

  rl->line = line;		/* provide next line buffer if any */

  return TERMUI_TERM_RET_OK;
}

termui_err_t termui_getline_history_add(struct termui_term_behavior_s *bhv, const char *str)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  char			*line = NULL;
  termui_hist_index_t	i;

  if (!rl->hist)
    return TERMUI_TERM_RET_INVALID;    

  if (rl->hist_count + 1 >= rl->hist_size)
    line = rl->hist[rl->hist_count]; /* reuse discarded line buffer */
  else
    {
      rl->hist_count++;
      line = malloc(rl->size);
    }

  for (i = rl->hist_count; i > 1; i--) /* shift history entries */
    rl->hist[i] = rl->hist[i - 1];

  strncpy(line, str, rl->size);
  line[rl->size - 1] = '\0';
  rl->hist[1] = line;

  return TERMUI_TERM_RET_OK;
}

const char * termui_getline_history_get(struct termui_term_behavior_s *bhv, termui_hist_index_t index)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  return (!rl->hist) || (index >= rl->hist_count)
    ? NULL : rl->hist[rl->hist_count - index];
}

void termui_getline_history_clear(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_hist_index_t	i;

  for (i = 1; i <= rl->hist_count; i++)
    free(rl->hist[i]);

  rl->hist_count = 0;
}

void termui_getline_history_cleanup(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_history_clear(bhv);
  free(rl->hist);

  rl->hist = NULL;
}

