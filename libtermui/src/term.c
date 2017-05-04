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
#include <stdlib.h>

#include "term_pv.h"
#include <termui/bhv.h>

TERMUI_TERM_FCN_RESET(termui_term_reset)
{
  return tm->mt.reset(tm);
}

TERMUI_TERM_FCN_GETSIZE(termui_term_getsize)
{
  return tm->mt.getsize(tm, x, y);
}

TERMUI_TERM_FCN_MOVE(termui_term_move)
{
  return tm->mt.move ? tm->mt.move(tm, dir, n) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_SETPOS(termui_term_setpos)
{
  return tm->mt.setpos ? tm->mt.setpos(tm, x, y) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_GETPOS(termui_term_getpos)
{
  return tm->mt.getpos ? tm->mt.getpos(tm, x, y) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_ATTRIB(termui_term_attrib)
{
  return tm->mt.attrib ? tm->mt.attrib(tm, attr) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_ERASE(termui_term_erase)
{
  return tm->mt.erase ? tm->mt.erase(tm, dir) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_BEEP(termui_term_beep)
{
  return tm->mt.beep ? tm->mt.beep(tm) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_ERASELINE(termui_term_eraseline)
{
  return tm->mt.eraseline ? tm->mt.eraseline(tm, dir) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_DELCHAR(termui_term_delchar)
{
  return tm->mt.delchar ? tm->mt.delchar(tm, n) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_DELLINE(termui_term_delline)
{
  return tm->mt.delline ? tm->mt.delline(tm, dir, n) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_INSSTR(termui_term_insstr)
{
  return tm->mt.insstr ? tm->mt.insstr(tm, str, n) : TERMUI_TERM_RET_INVALID;
}

TERMUI_TERM_FCN_WRITESTR(termui_term_writestr)
{
  return tm->mt.writestr(tm, str, n);
}

TERMUI_TERM_FCN_WRITECHAR(termui_term_writechar)
{
  return tm->mt.writechar(tm, c, n);
}

TERMUI_TERM_FCN_NEWLINE(termui_term_newline)
{
  return tm->mt.newline(tm);
}

TERMUI_TERM_FCN_READKEY(termui_term_readkey)
{
  return tm->mt.readkey(tm);
}

/* terminal context init */

#ifdef HAVE_TERMIOS_H

static termui_err_t
termui_term_set_raw(struct termui_term_s *tm)
{
  struct termios	tio;
  int			fd = tm->in;

  if (!isatty(fd))
    return TERMUI_TERM_RET_INVALID;

  if (!tcgetattr(fd, &tio))
    {
      tm->old = tio;

#if 1
      tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
      tio.c_oflag &= ~OPOST;
      tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
      tio.c_cflag &= ~(CSIZE | PARENB);
      tio.c_cflag |= CS8;
#else
      cfmakeraw(&tio);
#endif

      if (!tcsetattr(fd, 0, &tio))
	return TERMUI_TERM_RET_OK;
    }

  return TERMUI_TERM_RET_IOERROR;
}

void
termui_term_cleanup(struct termui_term_s *tm)
{
  if (isatty(tm->in))
    tcsetattr(tm->in, 0, &tm->old);
}

#endif

termui_err_t
termui_term_init(struct termui_term_s *tm, termui_iostream_t in,
	  termui_iostream_t out, void *private)
{
  tm->in = in;
  tm->out = out;
  tm->priv = private;

  termui_term_set_none(tm);
#ifdef HAVE_TERMIOS_H
  termui_term_set_raw(tm);
#endif

  return TERMUI_TERM_RET_OK;
}

struct termui_term_s *
termui_term_alloc(termui_iostream_t in, termui_iostream_t out, void *private)
{
  struct termui_term_s *tm;

  if ((tm = malloc(sizeof (struct termui_term_s))))
    termui_term_init(tm, in, out, private);

  return tm;
}

termui_err_t termui_term_set(struct termui_term_s *tm, const char *type)
{
  termui_term_set_none(tm);

  if (!type)
    return TERMUI_TERM_RET_INVALID;

  if (!strcmp(type, "dumb"))
    return 0;

  if (!strcmp(type, "xterm") ||
      !strcmp(type, "rxvt") ||
      !strcmp(type, "linux") ||
      !strcmp(type, "screen"))
    return termui_term_set_xterm(tm);

  if (!strcmp(type, "vt100"))
    return termui_term_set_vt100(tm);

  if (!strcmp(type, "vt102"))
    return termui_term_set_vt102(tm);

  return TERMUI_TERM_RET_INVALID;
}

void
termui_term_free(struct termui_term_s *tm)
{
#ifdef HAVE_TERMIOS_H
  termui_term_cleanup(tm);
#endif
  free(tm);
}

void *termui_term_private(struct termui_term_s *tm)
{
  return tm->priv;
}

/* behavior process */

termui_err_t
termui_term_behave(struct termui_term_behavior_s *bhv)
{
  struct termui_term_s *tm = bhv->tm;
  termui_key_t k;

  /* pre-initialise context */
  if (bhv->bhvstart)
    {
      termui_err_t res = bhv->bhvstart(bhv);

      if (res < 0)
	return res;
    }

  while (1)
    {
      switch (k = termui_term_readkey(tm))
	{
	default: {
	  termui_term_keyevent_t *ev = bhv->keyevent[k];

	  if (ev)
	    {
	      termui_err_t res = ev(k, bhv);

	      if (res <= 0)
		return res;

	      bhv->lastkey = k;
	    }

	} break;

	case (TERMUI_TERM_RET_IOERROR):
	  return TERMUI_TERM_RET_IOERROR;

	case (TERMUI_TERM_RET_INVALID):
	  break;
	}
    }
}

