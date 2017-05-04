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

#include "term_pv.h"

#define ESC	"\x1b["

static TERMUI_TERM_FCN_INSSTR(termui_term_vt102_insstr)
{
  termui_io_puts(ESC "4h", tm->out);
  termui_io_put(str, n, tm->out);
  termui_io_puts(ESC "4l", tm->out);

  return 0;
}

static TERMUI_TERM_FCN_DELCHAR(termui_term_vt102_delchar)
{
  termui_io_nprintf(tm->out, 32, ESC "%uP", n);

  return 0;
}

termui_err_t termui_term_set_vt102(struct termui_term_s *tm)
{
  termui_term_set_vt100(tm);

  tm->mt.insstr = termui_term_vt102_insstr;
  tm->mt.delchar = termui_term_vt102_delchar;

  return 0;
}

