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
#include <ctype.h>

#include <termui/term_keys.h>
#include "term_pv.h"

#define ESC	"\x1b["

static TERMUI_TERM_FCN_ATTRIB(termui_term_xterm_attrib)
{
  termui_io_nprintf(tm->out, 32, ESC "%um", attr);

  return 0;
}

static TERMUI_TERM_FCN_READKEY(termui_term_xterm_readkey)
{
  termui_key_t	k;

  switch (k = termui_io_getc(tm->in))
    {
    case (-1):
      return TERMUI_TERM_RET_IOERROR;

    case (033):			/* ESC */

      switch (k = termui_io_getc(tm->in))
	{
	  int_fast8_t n;

	case (-1):
	  return TERMUI_TERM_RET_IOERROR;

	case ('O'):
	  switch (k = termui_io_getc(tm->in))
	    {
	    case (-1): return TERMUI_TERM_RET_IOERROR;
	    case ('A'): return TERMUI_TERM_KEY_UP;
	    case ('B'): return TERMUI_TERM_KEY_DOWN;
	    case ('C'): return TERMUI_TERM_KEY_RIGHT;
	    case ('D'): return TERMUI_TERM_KEY_LEFT;
	    case ('P'): return TERMUI_TERM_KEY_FCN(1);
	    case ('Q'): return TERMUI_TERM_KEY_FCN(2);
	    case ('R'): return TERMUI_TERM_KEY_FCN(3);
	    case ('S'): return TERMUI_TERM_KEY_FCN(4);
	    }
	  break;

	case ('['):

	  n = 0;
	  while (isdigit(k = termui_io_getc(tm->in)))
	    n = n * 10 + k - '0';

	  switch (k)
	    {
	    case (-1): return TERMUI_TERM_RET_IOERROR;
	    case ('A'): return TERMUI_TERM_KEY_UP;
	    case ('B'): return TERMUI_TERM_KEY_DOWN;
	    case ('C'): return TERMUI_TERM_KEY_RIGHT;
	    case ('D'): return TERMUI_TERM_KEY_LEFT;
	    case ('F'): return TERMUI_TERM_KEY_END;
	    case ('H'): return TERMUI_TERM_KEY_HOME;
	    case ('Z'): return TERMUI_TERM_KEY_UNTAB;

	    case ('~'):
	      switch (n)
		{
		case (1): return TERMUI_TERM_KEY_HOME;
		case (2): return TERMUI_TERM_KEY_INSERT;
		case (3): return TERMUI_TERM_KEY_REMOVE;
		case (4): return TERMUI_TERM_KEY_END;
		case (5): return TERMUI_TERM_KEY_PGUP;
		case (6): return TERMUI_TERM_KEY_PGDN;
		case (15): return TERMUI_TERM_KEY_FCN(5);
		case (17): return TERMUI_TERM_KEY_FCN(6);
		case (18): return TERMUI_TERM_KEY_FCN(7);
		case (19): return TERMUI_TERM_KEY_FCN(8);
		case (20): return TERMUI_TERM_KEY_FCN(9);
		case (21): return TERMUI_TERM_KEY_FCN(10);
		case (23): return TERMUI_TERM_KEY_FCN(11);
		case (24): return TERMUI_TERM_KEY_FCN(12);
		}
	      break;
	    }
	  break;

	default:
	  return TERMUI_TERM_KEY_META(k);
	}
      break;

    default:
      if (k >= 0 && k <= 255)		/* control & ascii codes */
	return k;

      break;
    }

  return TERMUI_TERM_RET_INVALID;
}

termui_err_t termui_term_set_xterm(struct termui_term_s *tm)
{
  termui_term_set_vt102(tm);

  tm->mt.attrib = termui_term_xterm_attrib;
  tm->mt.readkey = termui_term_xterm_readkey;

  return 0;
}

