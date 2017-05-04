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

#include <unistd.h>

#include <termui/term_keys.h>
#include "term_pv.h"

#define ESC	"\x1b["

static TERMUI_TERM_FCN_RESET(termui_term_vt100_reset)
{
  termui_ssize_t c;

  termui_io_puts(ESC "c", tm->out);	/* reset term */
  do {
    c = termui_io_getc(tm->in);
  } while (c != 'c' && c >= 0);

  if (c < 0)
    return TERMUI_TERM_RET_IOERROR;

  termui_io_puts(ESC "?7l", tm->out);	/* disable wrap mode */

  return 0;
}

static TERMUI_TERM_FCN_MOVE(termui_term_vt100_move)
{
  if (!n)
    return 0;

  switch (dir)
    {
    case termui_term_dir_up:	termui_io_nprintf(tm->out, 32, ESC "%uA", n); return 0;
    case termui_term_dir_down:	termui_io_nprintf(tm->out, 32, ESC "%uB", n); return 0;
    case termui_term_dir_right:termui_io_nprintf(tm->out, 32, ESC "%uC", n); return 0;
    case termui_term_dir_left:	termui_io_nprintf(tm->out, 32, ESC "%uD", n); return 0;

    default:
      return -1;
    }
}

static TERMUI_TERM_FCN_SETPOS(termui_term_vt100_setpos)
{
  termui_io_nprintf(tm->out, 32, ESC "%u;%uH", y, x);

  return 0;
}

static TERMUI_TERM_FCN_GETPOS(termui_term_vt100_getpos)
{ 
  termui_io_puts(ESC "6n", tm->out);

  if (termui_io_getc(tm->in) != 27 || termui_io_getc(tm->in) != 91)
    return TERMUI_TERM_RET_IOERROR;

  if (termui_io_geti(tm->in, y) != ';')
    return TERMUI_TERM_RET_IOERROR;

  if (termui_io_geti(tm->in, x) != 'R')
    return TERMUI_TERM_RET_IOERROR;

  return 0;
}

static TERMUI_TERM_FCN_GETSIZE(termui_term_vt100_getsize)
{
  termui_cursor_pos_t oldx, oldy;

  return
    termui_term_vt100_getpos(tm, &oldx, &oldy) ||
    termui_term_vt100_move(tm, termui_term_dir_down, (termui_cursor_pos_t)-1) ||
    termui_term_vt100_move(tm, termui_term_dir_right, (termui_cursor_pos_t)-1) ||
    termui_term_vt100_getpos(tm, x, y) ||
    termui_term_vt100_setpos(tm, oldx, oldy);
}

static TERMUI_TERM_FCN_WRITESTR(termui_term_vt100_writestr)
{
  termui_strlen_t i;

  for (i = 0; i < n; i++)
    switch (str[i])
      {
      case '\n':
	termui_io_putc('\r', tm->out);
      default:
	termui_io_putc(str[i], tm->out);
      }

  return 0;
}

static TERMUI_TERM_FCN_ATTRIB(termui_term_vt100_attrib)
{
  switch (attr)
    {
    case termui_term_attr_none:
    case termui_term_attr_under:
    case termui_term_attr_blink:
    case termui_term_attr_reverse:
    case termui_term_attr_bright:
      termui_io_nprintf(tm->out, 32, ESC "%um", attr);
      return 0;

    default:
      return -1;
    }
}

static TERMUI_TERM_FCN_ERASE(termui_term_vt100_erase)
{
  switch (dir)
    {
    case termui_term_dir_down:	termui_io_puts(ESC "0J", tm->out); return 0;
    case termui_term_dir_up:	termui_io_puts(ESC "1J", tm->out); return 0;
    case termui_term_dir_any:	termui_io_puts(ESC "2J", tm->out); return 0;

    default:
      return -1;
    }
}

static TERMUI_TERM_FCN_BEEP(termui_term_vt100_beep)
{
  termui_io_putc(007, tm->out);

  return 0;
}

static TERMUI_TERM_FCN_ERASELINE(termui_term_vt100_eraseline)
{
  switch (dir)
    {
    case termui_term_dir_right:termui_io_puts(ESC "0K", tm->out); return 0;
    case termui_term_dir_left:	termui_io_puts(ESC "1K", tm->out); return 0;
    case termui_term_dir_any:	termui_io_puts(ESC "2K", tm->out); return 0;

    default:
      return -1;
    }

  return -1;  
}

static TERMUI_TERM_FCN_NEWLINE(termui_term_vt100_newline)
{
  termui_io_puts("\r\n", tm->out);

  return 0;
}

static TERMUI_TERM_FCN_READKEY(termui_term_vt100_readkey)
{
  termui_key_t k;

  switch (k = termui_io_getc(tm->in))
    {
    case (-1): return TERMUI_TERM_RET_IOERROR;

    case (033):			/* ESC */

      switch (k = termui_io_getc(tm->in))
	{
	case (-1): return TERMUI_TERM_RET_IOERROR;

	case ('['):
	case ('O'):

	  switch (termui_io_getc(tm->in))
	    {
	    case (-1): return TERMUI_TERM_RET_IOERROR;
	    case ('A'): return TERMUI_TERM_KEY_UP;
	    case ('B'): return TERMUI_TERM_KEY_DOWN;
	    case ('C'): return TERMUI_TERM_KEY_RIGHT;
	    case ('D'): return TERMUI_TERM_KEY_LEFT;
	    }
	  break;

	default:
	  return TERMUI_TERM_KEY_META(k);
	}
      break;

    default:
      if (k <= 255)		/* control & ascii codes */
	return k;

      break;
    }

  return TERMUI_TERM_RET_INVALID;
}

termui_err_t termui_term_set_vt100(struct termui_term_s *tm)
{
  tm->mt.reset = termui_term_vt100_reset;
  tm->mt.getsize = termui_term_vt100_getsize;
  tm->mt.writestr = termui_term_vt100_writestr;
  tm->mt.move = termui_term_vt100_move;
  tm->mt.setpos = termui_term_vt100_setpos;
  tm->mt.getpos = termui_term_vt100_getpos;
  tm->mt.attrib = termui_term_vt100_attrib;
  tm->mt.erase = termui_term_vt100_erase;
  tm->mt.beep = termui_term_vt100_beep;
  tm->mt.eraseline = termui_term_vt100_eraseline;
  tm->mt.newline = termui_term_vt100_newline;
  tm->mt.readkey = termui_term_vt100_readkey;

  return 0;
}

