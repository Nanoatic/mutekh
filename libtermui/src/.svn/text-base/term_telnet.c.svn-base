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

#include <arpa/telnet.h>

#include <termui/bhv.h>
#include "term_pv.h"

static int termui_term_iac_send(int out, unsigned cmd, unsigned opt)
{
  const char iac[3] = { IAC, cmd, opt };

  return termui_io_put(iac, 3, out) != 3;
}

termui_err_t termui_term_telnet_send_setup(struct termui_term_s *tm)
{
  return termui_term_iac_send(tm->out, DO, TELOPT_ECHO)
    || termui_term_iac_send(tm->out, DO, TELOPT_LFLOW)
    || termui_term_iac_send(tm->out, WILL, TELOPT_ECHO)
    || termui_term_iac_send(tm->out, WILL, TELOPT_SGA);
}

static TERMUI_TERM_FCN_KEYEVENT(bhv_key_iac)
{
  char iac[3] = { key };  
  int_fast16_t c;

  if ((c = termui_io_getc(bhv->tm->in)) < 0)
    return TERMUI_TERM_RET_IOERROR;
  iac[1] = c;

  if ((c = termui_io_getc(bhv->tm->in)) < 0)
    return TERMUI_TERM_RET_IOERROR;
  iac[2] = c;

  /* process IAC here */

  return TERMUI_TERM_RET_CONTINUE;
}

/* add telnet protocol handling to current behavior */

termui_err_t termui_term_telnet_bhv_init(struct termui_term_behavior_s *bhv)
{
  bhv->keyevent[IAC] = bhv_key_iac;

  return 0;
}

