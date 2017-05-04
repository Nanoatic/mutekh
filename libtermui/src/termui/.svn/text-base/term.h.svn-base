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

#ifndef TERMUI_TERM_H_
#define TERMUI_TERM_H_

#include <termui/types.h>

#define TERMUI_TERM_RET_CONTINUE	1
#define TERMUI_TERM_RET_OK		0
#define TERMUI_TERM_RET_INVALID		-1
#define TERMUI_TERM_RET_IOERROR		-2

/************************************************************************/
/* terminal methods */

enum termui_term_direction_e
  {
    termui_term_dir_any,
    termui_term_dir_up,
    termui_term_dir_down,
    termui_term_dir_right,
    termui_term_dir_left
  };

enum termui_term_attrib_e
  {
    /* base attributes */
    termui_term_attr_none		= 0,
    termui_term_attr_bright		= 1,
    termui_term_attr_under		= 4,
    termui_term_attr_blink		= 5,
    termui_term_attr_reverse		= 7,

    /* colors */
    termui_term_attr_black		= 30,
    termui_term_attr_red		= 31,
    termui_term_attr_green		= 32,
    termui_term_attr_yellow		= 33,
    termui_term_attr_blue		= 34,
    termui_term_attr_magenta		= 35,
    termui_term_attr_cyan		= 36,
    termui_term_attr_white		= 37,
  };

struct				termui_term_s;
struct				termui_term_behavior_s;

/*
 *  terminal API
 */

struct termui_term_s *termui_term_alloc(termui_iostream_t in, termui_iostream_t out, void *priv);
termui_err_t termui_term_set(struct termui_term_s *tm, const char *type);
termui_err_t termui_term_behave(struct termui_term_behavior_s *bhv);
void termui_term_free(struct termui_term_s *tm);
void *termui_term_private(struct termui_term_s *tm);

termui_ssize_t termui_term_printf(struct termui_term_s *tm, const char *fmt, ...);
termui_ssize_t termui_term_puts(struct termui_term_s *tm, const char *str);

/*
 *  direct terminal access
 */

#define	TERMUI_TERM_FCN_RESET(f)	\
  termui_err_t f(struct termui_term_s *tm)

#define TERMUI_TERM_FCN_GETSIZE(f)	\
  termui_err_t f(struct termui_term_s *tm, termui_cursor_pos_t *x, termui_cursor_pos_t *y)

#define	TERMUI_TERM_FCN_MOVE(f)	\
  termui_err_t f(struct termui_term_s *tm, enum termui_term_direction_e dir, termui_cursor_pos_t n)

#define	TERMUI_TERM_FCN_SETPOS(f)	\
  termui_err_t f(struct termui_term_s *tm, termui_cursor_pos_t x, termui_cursor_pos_t y)

#define	TERMUI_TERM_FCN_GETPOS(f)	\
  termui_err_t f(struct termui_term_s *tm, termui_cursor_pos_t *x, termui_cursor_pos_t *y)

#define TERMUI_TERM_FCN_ATTRIB(f)	\
  termui_err_t f(struct termui_term_s *tm, enum termui_term_attrib_e attr)

#define TERMUI_TERM_FCN_ERASE(f)	\
  termui_err_t f(struct termui_term_s *tm, enum termui_term_direction_e dir)

#define TERMUI_TERM_FCN_BEEP(f)	\
  termui_err_t f(struct termui_term_s *tm)

#define TERMUI_TERM_FCN_ERASELINE(f)	\
  termui_err_t f(struct termui_term_s *tm, enum termui_term_direction_e dir)

#define TERMUI_TERM_FCN_DELCHAR(f)	\
  termui_err_t f(struct termui_term_s *tm, termui_cursor_pos_t n)

#define TERMUI_TERM_FCN_DELLINE(f)	\
  termui_err_t f(struct termui_term_s *tm, enum termui_term_direction_e dir, termui_cursor_pos_t n)

#define TERMUI_TERM_FCN_INSSTR(f)	\
  termui_err_t f(struct termui_term_s *tm, const char * str, termui_strlen_t n)

#define TERMUI_TERM_FCN_WRITESTR(f)	\
  termui_err_t f(struct termui_term_s *tm, const char * str, termui_strlen_t n)

#define TERMUI_TERM_FCN_WRITECHAR(f)	\
  termui_err_t f(struct termui_term_s *tm, const char c, termui_cursor_pos_t n)

#define TERMUI_TERM_FCN_NEWLINE(f)	\
  termui_err_t f(struct termui_term_s *tm)


#define TERMUI_TERM_FCN_READKEY(f)	\
  termui_key_t f(struct termui_term_s *tm)


TERMUI_TERM_FCN_RESET(termui_term_reset);
TERMUI_TERM_FCN_GETSIZE(termui_term_getsize);
TERMUI_TERM_FCN_MOVE(termui_term_move);
TERMUI_TERM_FCN_SETPOS(termui_term_setpos);
TERMUI_TERM_FCN_GETPOS(termui_term_getpos);
TERMUI_TERM_FCN_ATTRIB(termui_term_attrib);
TERMUI_TERM_FCN_ERASE(termui_term_erase);
TERMUI_TERM_FCN_BEEP(termui_term_beep);
TERMUI_TERM_FCN_ERASELINE(termui_term_eraseline);
TERMUI_TERM_FCN_DELCHAR(termui_term_delchar);
TERMUI_TERM_FCN_DELLINE(termui_term_delline);
TERMUI_TERM_FCN_INSSTR(termui_term_insstr);
TERMUI_TERM_FCN_WRITESTR(termui_term_writestr);
TERMUI_TERM_FCN_WRITECHAR(termui_term_writechar);
TERMUI_TERM_FCN_NEWLINE(termui_term_newline);
TERMUI_TERM_FCN_READKEY(termui_term_readkey);

/*
 * telnet protocol
 */

/* send basic telnet client initialisation commands */
termui_err_t termui_term_telnet_send_setup(struct termui_term_s *tm);

/* add telnet protocol response handling to behavior */
termui_err_t termui_term_telnet_bhv_init(struct termui_term_behavior_s *bhv);

#endif

