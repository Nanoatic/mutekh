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

#ifndef TERMUI_GETLINE_H_
#define TERMUI_GETLINE_H_

#include <termui/types.h>

/** Create a getline behavior for specified terminal, with given
    maximum line buffer size */
struct termui_term_behavior_s * termui_getline_alloc(struct termui_term_s *tm, termui_strlen_t size);

/** Destroy getline behavior object */
void termui_getline_free(struct termui_term_behavior_s *bhv);

/** Wait for user input and return edited line */
const char *termui_getline_process(struct termui_term_behavior_s *bhv);



/** Prompt display function prototype. Get pointer to terminal object
    and associated private data */
#define TERMUI_GETLINE_FCN_PROMPT(f) termui_err_t f(struct termui_term_s *tm, void *priv)
typedef TERMUI_GETLINE_FCN_PROMPT(termui_getline_prompt_t);

/** Set prompt display function */
void termui_getline_setprompt(struct termui_term_behavior_s *bhv, termui_getline_prompt_t *p);



/** Enable user entered lines history support */
termui_err_t termui_getline_history_init(struct termui_term_behavior_s *bhv, termui_hist_index_t size);

/** Append a line to history */
termui_err_t termui_getline_history_add(struct termui_term_behavior_s *bhv, const char *str);

/** Get a line from history */
const char * termui_getline_history_get(struct termui_term_behavior_s *bhv, termui_hist_index_t index);

/** Append the last entered line to history */
termui_err_t termui_getline_history_addlast(struct termui_term_behavior_s *bhv);

/** Clear all history */
void termui_getline_history_clear(struct termui_term_behavior_s *bhv);

/** Disable history and cleanup */
void termui_getline_history_cleanup(struct termui_term_behavior_s *bhv);



/** Completion list function prototype. Get pointer to getline object
    and associated private data */
#define TERMUI_GETLINE_FCN_COMPLETE(f)	void f(struct termui_term_behavior_s *bhv, void *priv)
typedef TERMUI_GETLINE_FCN_COMPLETE(termui_getline_complete_t);

/** Enable completion support through specified completion callback function */
termui_err_t termui_getline_complete_init(struct termui_term_behavior_s *bhv, termui_getline_complete_t *f);

/** Get line start pointer in current line buffer. Can be called from completion callback */
const char *termui_getline_line_start(struct termui_term_behavior_s *bhv);

/** Get cursor pointer in current line buffer. Can be called from completion callback */
const char *termui_getline_line_cursor(struct termui_term_behavior_s *bhv);

/** Insert text at given cursor position. Can be called from completion callback */
termui_err_t termui_getline_insert(struct termui_term_behavior_s *bhv, const char *str, termui_strlen_t len);

/** Rewrite prompt after completion list display. Can be called from completion callback  */
void termui_getline_reprompt(struct termui_term_behavior_s *bhv);

termui_err_t termui_getline_delete(struct termui_term_behavior_s *bhv, termui_cursor_pos_t len);

termui_err_t termui_getline_move_forward(struct termui_term_behavior_s *bhv, termui_cursor_pos_t len);

termui_err_t termui_getline_move_backward(struct termui_term_behavior_s *bhv, termui_cursor_pos_t len);

#endif

