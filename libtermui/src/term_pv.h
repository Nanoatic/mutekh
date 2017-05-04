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

#ifndef TERMUI_TERM_PV_H_
#define TERMUI_TERM_PV_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdarg.h>

#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif

#include <termui/term.h>

#include CONFIG_LIBTERMUI_IOHEADER

/************************************************************************/
/* terminal methods */

typedef	TERMUI_TERM_FCN_RESET(termui_term_reset_t);
typedef TERMUI_TERM_FCN_GETSIZE(termui_term_getsize_t);
typedef	TERMUI_TERM_FCN_MOVE(termui_term_move_t);
typedef	TERMUI_TERM_FCN_SETPOS(termui_term_setpos_t);
typedef	TERMUI_TERM_FCN_GETPOS(termui_term_getpos_t);
typedef TERMUI_TERM_FCN_ATTRIB(termui_term_attrib_t);
typedef TERMUI_TERM_FCN_ERASE(termui_term_erase_t);
typedef TERMUI_TERM_FCN_BEEP(termui_term_beep_t);
typedef TERMUI_TERM_FCN_ERASELINE(termui_term_eraseline_t);
typedef TERMUI_TERM_FCN_DELCHAR(termui_term_delchar_t);
typedef TERMUI_TERM_FCN_DELLINE(termui_term_delline_t);
typedef TERMUI_TERM_FCN_INSSTR(termui_term_insstr_t);
typedef TERMUI_TERM_FCN_WRITESTR(termui_term_writestr_t);
typedef TERMUI_TERM_FCN_WRITECHAR(termui_term_writechar_t);
typedef TERMUI_TERM_FCN_NEWLINE(termui_term_newline_t);
typedef TERMUI_TERM_FCN_READKEY(termui_term_readkey_t);

struct termui_term_methods_s
{
  termui_term_reset_t		*reset; /* mandatory */
  termui_term_getsize_t	*getsize; /* mandatory */
  termui_term_move_t		*move;
  termui_term_setpos_t		*setpos;
  termui_term_getpos_t		*getpos;
  termui_term_attrib_t		*attrib;
  termui_term_erase_t		*erase;
  termui_term_beep_t		*beep;  
  termui_term_eraseline_t	*eraseline;
  termui_term_delchar_t	*delchar;
  termui_term_delline_t	*delline;
  termui_term_insstr_t		*insstr;
  termui_term_writestr_t	*writestr; /* mandatory */
  termui_term_writechar_t	*writechar; /* mandatory */
  termui_term_newline_t	*newline; /* mandatory */
  termui_term_readkey_t	*readkey; /* mandatory */
};

struct termui_term_s
{
  termui_iostream_t in;
  termui_iostream_t out;

  struct termui_term_methods_s	mt;

#ifdef HAVE_TERMIOS_H
  struct termios old;
#endif

  void *priv;
};

#define TERMUI_TERM_TRY(fcn, ...)	(!(fcn) || fcn(__VA_ARGS__))

/************************************************************************/

/* terminal types settings */

termui_err_t termui_term_init(struct termui_term_s *tm, termui_iostream_t in,
		       termui_iostream_t out, void *priv);
void termui_term_cleanup(struct termui_term_s *tm);

termui_ssize_t termui_term_printf_va(struct termui_term_s *tm, const char *fmt, va_list list);

termui_err_t termui_term_set_none(struct termui_term_s *tm);
termui_err_t termui_term_set_vt100(struct termui_term_s *tm);
termui_err_t termui_term_set_vt102(struct termui_term_s *tm);
termui_err_t termui_term_set_xterm(struct termui_term_s *tm);

#endif

