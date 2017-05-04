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

#include <stdlib.h>
#include <string.h>

#include <termui/bhv.h>
#include <termui/term.h>

#include "getline_pv.h"

/* insert string at cursor position */

termui_err_t
termui_getline_insert(struct termui_term_behavior_s *bhv,
	       const char *str, termui_strlen_t len)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	tail = rl->end - rl->cursor;

  /* check for buffer overflow */
  if (len > rl->max - rl->end)
    {
      termui_term_beep(tm);

      if (!(len = rl->max - rl->end))
	return TERMUI_TERM_RET_INVALID;
    }

  if (!tail)
    {
      termui_term_writestr(tm, str, len);
    }
  else if (termui_term_insstr(tm, str, len) != TERMUI_TERM_RET_OK)
    {
      termui_term_writestr(tm, str, len);
      termui_term_writestr(tm, rl->cursor, tail);
      termui_term_move(tm, termui_term_dir_left, tail);
    }

  memmove(rl->cursor + len, rl->cursor, tail + 1);
  memcpy(rl->cursor, str, len);

  rl->end += len;
  rl->cursor += len;

  return TERMUI_TERM_RET_OK;
}

/* delete string at cursor position */

termui_err_t
termui_getline_delete(struct termui_term_behavior_s *bhv,
	       termui_cursor_pos_t len)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	tail = rl->end - rl->cursor;

  /* are we at end of line ? */

  if (!len || len > tail)
    {
      termui_term_beep(tm);
      return TERMUI_TERM_RET_INVALID;
    }

  /* shift line content */
  if (termui_term_delchar(tm, len))
    {
      termui_term_writestr(tm, rl->cursor + len, tail - len);
      termui_term_writechar(tm, ' ', len);
      termui_term_move(tm, termui_term_dir_left, tail);
    }

  memmove(rl->cursor, rl->cursor + len, tail - len + 1);
  rl->end -= len;

  return TERMUI_TERM_RET_OK;
}

termui_err_t
termui_getline_move_forward(struct termui_term_behavior_s *bhv,
		     termui_cursor_pos_t len)
{
  struct termui_term_s *tm = bhv->tm;
  struct termui_getline_s *rl = bhv->bhvctx;
  termui_strlen_t tail = rl->end - rl->cursor;

  if (!len || len > tail)
    {
      termui_term_beep(tm);
      return TERMUI_TERM_RET_INVALID;
    }

  rl->cursor += len;
  termui_term_move(tm, termui_term_dir_right, len);

  return TERMUI_TERM_RET_OK;
}

termui_err_t
termui_getline_move_backward(struct termui_term_behavior_s *bhv,
		      termui_cursor_pos_t len)
{
  struct termui_term_s *tm = bhv->tm;
  struct termui_getline_s *rl = bhv->bhvctx;
  termui_strlen_t head = rl->cursor - rl->buf;

  if (!len || len > head)
    {
      termui_term_beep(tm);
      return TERMUI_TERM_RET_INVALID;
    }

  rl->cursor -= len;
  termui_term_move(tm, termui_term_dir_left, len);

  return TERMUI_TERM_RET_OK;
}

void
termui_getline_empty(struct termui_term_behavior_s *bhv)
{
  struct termui_term_s *tm = bhv->tm;
  struct termui_getline_s *rl = bhv->bhvctx;
  termui_strlen_t head = rl->cursor - rl->buf;
  termui_strlen_t len = rl->end - rl->buf;

  termui_term_move(tm, termui_term_dir_left, head);

  if (termui_term_eraseline(tm, termui_term_dir_right))
    {
      termui_term_writechar(tm, ' ', len);
      termui_term_move(tm, termui_term_dir_left, len);
    }
}

void
termui_getline_rewrite(struct termui_term_behavior_s *bhv)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t head = rl->cursor - rl->buf;
  termui_strlen_t len = rl->end - rl->buf;

  termui_term_writestr(tm, rl->buf, len);  
  termui_term_move(tm, termui_term_dir_left, len - head);
}

void
termui_getline_reprompt(struct termui_term_behavior_s *bhv)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;

  /* rewrite prompt */
  rl->prompt(tm, termui_term_private(tm));

  /* rewrite line content */
  termui_getline_rewrite(bhv);
}

/* initialize context for new line reading */

static TERMUI_TERM_FCN_EVENT(termui_getline_bhvstart)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;

  /* allocate new line buffer if none available */
  if (!rl->line && !(rl->line = malloc(rl->size)))
    return TERMUI_TERM_RET_IOERROR;

  rl->max = rl->line + rl->size - 1;
  rl->cursor = rl->end = rl->buf = rl->line;
  rl->buf[0] = '\0';

  if (rl->hist)
    {
      rl->hist[0] = rl->line;
      rl->hist_cur = 0;
    }

#if 0
  if (termui_term_getsize(tm, &rl->width, &height))
    rl->width = 80;

  rl->offset = rl->prompt(tm, termui_term_private(tm)) % rl->width;
#else
  rl->prompt(tm, termui_term_private(tm));
#endif

  return TERMUI_TERM_RET_OK;
}

const char *
termui_getline_line_start(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  return rl->buf;
}

const char *
termui_getline_line_cursor(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  return rl->cursor;
}

static TERMUI_GETLINE_FCN_PROMPT(termui_getline_default_prompt)
{
  return 0;
}

termui_err_t
termui_getline_init(struct termui_term_s *tm,
	     struct termui_term_behavior_s *bhv,
	     termui_strlen_t size)
{
  struct termui_getline_s	*rl;

  /* allocate readline context */

  if (!(rl = malloc(sizeof(struct termui_getline_s))))
    return TERMUI_TERM_RET_IOERROR;

  bhv->tm = tm;
  bhv->bhvctx = rl;

  bhv->bhvstart = termui_getline_bhvstart;
  bhv->lastkey = TERMUI_TERM_RET_INVALID;

  rl->size = size;
  rl->copy = NULL;
  rl->prompt = termui_getline_default_prompt;

  rl->line = NULL;
  rl->hist = NULL;

  /* define base readline keys actions */

  memset(bhv->keyevent, 0, sizeof(bhv->keyevent));

  termui_getline_edit_init(bhv);

  return TERMUI_TERM_RET_OK;
}

struct termui_term_behavior_s * termui_getline_alloc(struct termui_term_s *tm,
				       termui_strlen_t size)
{
  struct termui_term_behavior_s	*bhv;

  if ((bhv = malloc(sizeof (struct termui_term_behavior_s))))
    termui_getline_init(tm, bhv, size);

  return bhv;
}

void
termui_getline_setprompt(struct termui_term_behavior_s *bhv,
		  termui_getline_prompt_t *p)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  rl->prompt = p;
}

const char *
termui_getline_process(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  switch (termui_term_behave(bhv)) /* use behavior with terminal */
    {
    case TERMUI_TERM_RET_OK:	/* valid line entered */
      return rl->line;

    default:
      return NULL;
    }
}

void
termui_getline_cleanup(struct termui_term_behavior_s *bhv)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  if (rl->hist)
    termui_getline_history_cleanup(bhv);

  if (rl->copy)
    free(rl->copy);

  if (rl->line)
    free(rl->line);

  free(rl);
}

void termui_getline_free(struct termui_term_behavior_s *bhv)
{
  termui_getline_cleanup(bhv);
  free(bhv);
}

