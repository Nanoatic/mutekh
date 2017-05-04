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
#include <string.h>

#include <termui/term_keys.h>
#include "term_pv.h"

static TERMUI_TERM_FCN_RESET(termui_term_none_reset)
{
  return 0;
}

static TERMUI_TERM_FCN_GETSIZE(termui_term_none_getsize)
{
  *x = 80;
  *y = 24;

  return 0;
}

static TERMUI_TERM_FCN_WRITESTR(termui_term_none_writestr)
{
  termui_io_put(str, n, tm->out);

  return 0;
}

static TERMUI_TERM_FCN_NEWLINE(termui_term_none_newline)
{
  termui_io_puts("\r\n", tm->out);

  return 0;
}

static TERMUI_TERM_FCN_WRITECHAR(termui_term_none_writechar)
{
  termui_ssize_t i;

  for (i = 0; i < n; i++)
    termui_io_putc(c, tm->out);

  return 0;
}

static TERMUI_TERM_FCN_READKEY(termui_term_none_readkey)
{
  termui_key_t	k = termui_io_getc(tm->in);

  if (k >= 0)
    return k % TERMUI_TERM_MAX_KEY;

  return TERMUI_TERM_RET_IOERROR;
}

termui_err_t termui_term_set_none(struct termui_term_s *tm)
{
  memset(&tm->mt, 0, sizeof(tm->mt));

  tm->mt.reset = termui_term_none_reset;
  tm->mt.getsize = termui_term_none_getsize;
  tm->mt.writestr = termui_term_none_writestr;
  tm->mt.writechar = termui_term_none_writechar;
  tm->mt.newline = termui_term_none_newline;
  tm->mt.readkey = termui_term_none_readkey;

  return 0;
}

