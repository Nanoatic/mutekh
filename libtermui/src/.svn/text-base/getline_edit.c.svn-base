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

static inline termui_strlen_t
termui_getline_prev_word(struct termui_getline_s *rl)
{
  char		*word;

  for (word = rl->cursor; word > rl->buf && !isalnum(word[-1]); word--)
    ;

  for ( ; word > rl->buf && isalnum(word[-1]); word--)
    ;

  return rl->cursor - word;
}

static inline termui_strlen_t
termui_getline_next_word(struct termui_getline_s *rl)
{
  char		*word = rl->cursor;

  word += strspn(word, " \t");
  word += strcspn(word, " \t");

  return word - rl->cursor;
}

static inline void
termui_getline_copy(struct termui_getline_s *rl,
	     char *str, termui_strlen_t len)
{
  if ((rl->copy = realloc(rl->copy, len + 1)))
    {
      memcpy(rl->copy, str, len);
      rl->copy[len] = '\0';
    }
}

static inline void
termui_getline_copy_head(struct termui_getline_s *rl,
		  char *str, termui_strlen_t len)
{
  termui_strlen_t clen = 0;

  if (rl->copy)
    clen = strlen(rl->copy);

  if ((rl->copy = realloc(rl->copy, clen + len + 1)))
    {
      memcpy(rl->copy + clen, str, len);
      rl->copy[clen + len] = '\0';
    }
}

static inline void
termui_getline_copy_tail(struct termui_getline_s *rl,
		  char *str, termui_strlen_t len)
{
  termui_strlen_t clen = 0;

  if (rl->copy)
    clen = strlen(rl->copy);

  if ((rl->copy = realloc(rl->copy, clen + len + 1)))
    {
      memmove(rl->copy + len, rl->copy, clen);
      memcpy(rl->copy, str, len);
      rl->copy[clen + len] = '\0';
    }
}

/************************************************************************/

/* Return key pressed or CR in input */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_valid)
{
  struct termui_term_s		*tm = bhv->tm;

  termui_term_newline(tm);

  return TERMUI_TERM_RET_OK;
}

/* Ascii key pressed, adding to buffer */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_ascii)
{
  char			kchar = key;

  termui_getline_insert(bhv, &kchar, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Left key pressed, move cursor */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_backward)
{
  termui_getline_move_backward(bhv, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Right key pressed, move cursor */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_forward)
{
  termui_getline_move_forward(bhv, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Move cursor to end of line */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_end)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_move_forward(bhv, rl->end - rl->cursor);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Move cursor to head of line */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_home)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_move_backward(bhv, rl->cursor - rl->buf);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Delete next character */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_delete_char)
{
  termui_getline_delete(bhv, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Go to next word */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_next_word)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_move_forward(bhv, termui_getline_next_word(rl));

  return TERMUI_TERM_RET_CONTINUE;
}

/* Go to previous word */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_prev_word)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  termui_getline_move_backward(bhv, termui_getline_prev_word(rl));

  return TERMUI_TERM_RET_CONTINUE;
}

/* Delete next word */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_delete_word)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	len;

  if ((len = termui_getline_next_word(rl)))
    {
      if (key == bhv->lastkey)
	termui_getline_copy_head(rl, rl->cursor, len);
      else
	termui_getline_copy(rl, rl->cursor, len);

      termui_getline_delete(bhv, len);
    }

  return TERMUI_TERM_RET_CONTINUE;
}

/* Delete next word */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_backspace_word)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	len;

  if ((len = termui_getline_prev_word(rl)))
    {
      termui_getline_move_backward(bhv, len);

      if (key == bhv->lastkey)
	termui_getline_copy_tail(rl, rl->cursor, len);
      else
	termui_getline_copy(rl, rl->cursor, len);

      termui_getline_delete(bhv, len);
    }

  return TERMUI_TERM_RET_CONTINUE;
}

/* Ctrl-D pressed, EOF */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_eof)
{
  struct termui_term_s		*tm = bhv->tm;

  termui_term_newline(tm);

  return TERMUI_TERM_RET_IOERROR;
}

/* Ctrl-D key pressed, delete or EOF if nothing to delete */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_eot)
{
  struct termui_term_s		*tm = bhv->tm;
  struct termui_getline_s	*rl = bhv->bhvctx;

  if (rl->buf == rl->end)
    {
      termui_term_newline(tm);
      return TERMUI_TERM_RET_IOERROR;
    }

  termui_getline_delete(bhv, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* BackSpace key pressed move backward and delete */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_backspace)
{
  if (!termui_getline_move_backward(bhv, 1))
    termui_getline_delete(bhv, 1);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Delete characters from cursor to end of line */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_killtail)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	tail = rl->end - rl->cursor;

  if (tail)
    termui_getline_copy(rl, rl->cursor, tail);

  termui_getline_delete(bhv, tail);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Delete characters to cursor */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_killhead)
{
  struct termui_getline_s	*rl = bhv->bhvctx;
  termui_strlen_t	head = rl->cursor - rl->buf;

  if (head)
    termui_getline_copy(rl, rl->buf, head);

  if (!termui_getline_move_backward(bhv, head))
    termui_getline_delete(bhv, head);

  return TERMUI_TERM_RET_CONTINUE;
}

/* Paste characters present in copy/paste storage */

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_paste)
{
  struct termui_getline_s	*rl = bhv->bhvctx;

  /* paste buffer is empty */
  if (rl->copy)
    termui_getline_insert(bhv, rl->copy, strlen(rl->copy));

  return TERMUI_TERM_RET_CONTINUE;
}

termui_err_t
termui_getline_edit_init(struct termui_term_behavior_s *bhv)
{
  termui_key_t k;

  /* define base readline keys actions */

  for (k = 32; k < 127; k++)
    bhv->keyevent[k] = bhv_key_ascii;

  bhv->keyevent[TERMUI_TERM_KEY_RETURN] = bhv_key_valid;
  bhv->keyevent[TERMUI_TERM_KEY_EOT] = bhv_key_eof;

  if (termui_term_move(bhv->tm, termui_term_dir_right, 0) != TERMUI_TERM_RET_OK)
    return TERMUI_TERM_RET_OK;

  /* define more readline keys actions */

  bhv->keyevent[TERMUI_TERM_KEY_LEFT] = bhv_key_backward;
  bhv->keyevent[TERMUI_TERM_KEY_STX] = bhv_key_backward;
  bhv->keyevent[TERMUI_TERM_KEY_RIGHT] = bhv_key_forward;
  bhv->keyevent[TERMUI_TERM_KEY_ACK] = bhv_key_forward;

  bhv->keyevent[TERMUI_TERM_KEY_REMOVE] = bhv_key_delete_char;
  bhv->keyevent[TERMUI_TERM_KEY_DELETE] = bhv_key_backspace;

  bhv->keyevent[TERMUI_TERM_KEY_META('d')] = bhv_key_delete_word;
  bhv->keyevent[TERMUI_TERM_KEY_META('D')] = bhv_key_delete_word;

  bhv->keyevent[TERMUI_TERM_KEY_META('b')] = bhv_key_prev_word;
  bhv->keyevent[TERMUI_TERM_KEY_META('B')] = bhv_key_prev_word;

  bhv->keyevent[TERMUI_TERM_KEY_META('f')] = bhv_key_next_word;
  bhv->keyevent[TERMUI_TERM_KEY_META('F')] = bhv_key_next_word;

  bhv->keyevent[TERMUI_TERM_KEY_META(TERMUI_TERM_KEY_DELETE)] = bhv_key_backspace_word;
  bhv->keyevent[TERMUI_TERM_KEY_ETB] = bhv_key_backspace_word;
  bhv->keyevent[TERMUI_TERM_KEY_VT] = bhv_key_killtail;
  bhv->keyevent[TERMUI_TERM_KEY_NAK] = bhv_key_killhead;

  bhv->keyevent[TERMUI_TERM_KEY_EM] = bhv_key_paste;

  bhv->keyevent[TERMUI_TERM_KEY_SOH] = bhv_key_home;
  bhv->keyevent[TERMUI_TERM_KEY_HOME] = bhv_key_home;
  bhv->keyevent[TERMUI_TERM_KEY_ENQ] = bhv_key_end;
  bhv->keyevent[TERMUI_TERM_KEY_END] = bhv_key_end;

  bhv->keyevent[TERMUI_TERM_KEY_EOT] = bhv_key_eot;

  return TERMUI_TERM_RET_OK;
}

