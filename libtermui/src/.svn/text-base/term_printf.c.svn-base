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

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "term_pv.h"

#define TERMUI_TERM_PRINTF_FCN(f)	termui_strlen_t f(struct termui_term_s *tm, int_fast8_t tag, void *pdata, int64_t idata)

typedef TERMUI_TERM_PRINTF_FCN(termui_term_printf_t);

struct				termui_term_printf_s
{
  termui_bool_t			has_pdata:1,
				has_idata:1;
  termui_term_printf_t			*fcn;
};

static TERMUI_TERM_PRINTF_FCN(termui_term_format_attrib)
{
  termui_term_attrib(tm, tag);

  return 0;
}

#ifdef HAVE_LOCALTIME_R
static TERMUI_TERM_PRINTF_FCN(termui_term_format_date)
{
  time_t ti = idata;
  struct tm *t, t_;
  termui_strlen_r res = 0;

  if (ti && (t = localtime_r(&ti, &t_)))
    {
      if (~tag & 1)
	res += termui_io_nprintf(tm->out, 16, "%02u:%02u:%02u",
			 t->tm_hour, t->tm_min, t->tm_sec);

      if (!tag)
	termui_io_putc(' ', tm->out);

      if (~tag & 2)
	res += termui_io_nprintf(tm->out, 16, "%02u/%02u/%02u",
			 t->tm_mon + 1, t->tm_mday, t->tm_year - 100);
    }

  return res;
}
#endif

static TERMUI_TERM_PRINTF_FCN(termui_term_format_string)
{
  char			fmt[16] = "%s";

  if (tag)
    sprintf(fmt, "%%%is", tag);
  return termui_io_nprintf(tm->out, 1024, fmt, pdata);
}

static TERMUI_TERM_PRINTF_FCN(termui_term_format_int)
{
  char			fmt[16] = "%i";

  if (tag)
    sprintf(fmt, "%%%illi", tag);
  return termui_io_nprintf(tm->out, 32, fmt, idata);
}

static TERMUI_TERM_PRINTF_FCN(termui_term_format_uint)
{
  char			fmt[16] = "%u";

  if (tag)
    sprintf(fmt, "%%%illu", tag);
  return termui_io_nprintf(tm->out, 32, fmt, idata);
}

static TERMUI_TERM_PRINTF_FCN(termui_term_format_hexa)
{
  char			fmt[16] = "%x";

  if (tag)
    sprintf(fmt, "%%0%illx", tag);
  return termui_io_nprintf(tm->out, 32, fmt, idata);
}

static TERMUI_TERM_PRINTF_FCN(termui_term_format_hexastr)
{
  termui_strlen_t i;
  const unsigned char	*h = pdata;

  for (i = 0; i < tag; i++)
    termui_io_nprintf(tm->out, 8, "%02x", *h++);

  return tag * 2;
}

static TERMUI_TERM_PRINTF_FCN(termui_term_format_decimal)
{
  char			fmt[16];
  static const char	prefix[9] = " KMGTPEZY";
  const char		*f = " %s%%.%if ";
  const char		*p = prefix;
  float			val = idata;

  while (val >= 1000)
    {
      f = "%s%%.%if %%c";
      p++;
      val /= 1000;
    }

  if (tag && val >= 100.0f)
    tag--;
  if (tag && val >= 10.0f)
    tag--;

  sprintf(fmt, f, tag ? "" : " ", tag);
  return termui_io_nprintf(tm->out, 32, fmt, val, *p);
}

static struct termui_term_printf_s	termui_term_format[255] = 
  {
    ['A'] = { .fcn = termui_term_format_attrib },
#ifdef HAVE_LOCALTIME_R
    ['D'] = { .fcn = termui_term_format_date, .has_idata = 1 },
#endif
    ['d'] = { .fcn = termui_term_format_decimal, .has_idata = 1 },
    ['s'] = { .fcn = termui_term_format_string, .has_pdata = 1 },
    ['i'] = { .fcn = termui_term_format_int, .has_idata = 1 },
    ['u'] = { .fcn = termui_term_format_uint, .has_idata = 1 },
    ['x'] = { .fcn = termui_term_format_hexa, .has_idata = 1 },
    ['m'] = { .fcn = termui_term_format_hexastr, .has_pdata = 1 },
  };

termui_ssize_t termui_term_printf_va(struct termui_term_s *tm,
		       const char *fmt,
		       va_list list)
{
  termui_ssize_t res = 0;

  while (1)
    {
      switch (*fmt)
	{
	case ('\0'):
	  goto end;

	case ('\n'):
	  termui_term_newline(tm);
	  fmt++;
	  continue;

	case ('%'): {
	  struct termui_term_printf_s	*f;
	  int_fast8_t tag;
	  void *pdata = NULL;
	  int64_t idata = 0;
	  termui_bool_t is_long = 0;

	  tag = strtol(fmt + 1, (char**)&fmt, 10);

	  while (*fmt == 'l')
	    {
	      is_long++;
	      fmt++;
	    }

	  if (*fmt == '\0')
	    goto end;

	  f = termui_term_format + *fmt;

	  if (!f->fcn)
	    break;

	  if (f->has_pdata)
	    pdata = va_arg(list, void*);

	  if (f->has_idata)
	    switch (is_long)
	      {
	      case 0:
		idata = va_arg(list, termui_int_t);
		break;
	      case 1:
		idata = va_arg(list, termui_long_int_t);
		break;
	      default: 
		idata = va_arg(list, termui_long_long_int_t);
		break;
	      }

	  res += f->fcn(tm, tag, pdata, idata);

	  fmt++;
	  continue;
	}

	default:
	  if (*fmt >= ' ')
	    {
	      termui_io_putc(*fmt, tm->out);
	      res++;
	    }

	  fmt++;
	}
    }

 end:

  return res;
}

termui_ssize_t termui_term_printf(struct termui_term_s *tm,
		    const char *fmt, ...)
{
  termui_ssize_t res;
  va_list list;

  va_start(list, fmt);
  res = termui_term_printf_va(tm, fmt, list);
  va_end(list);

  return res;
}

termui_ssize_t termui_term_puts(struct termui_term_s *tm, const char *str)
{
  return termui_io_puts(str, tm->out);
}

