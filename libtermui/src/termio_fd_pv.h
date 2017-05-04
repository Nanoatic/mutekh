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

#ifndef TERMIO_FD_PV_H_
#define TERMIO_FD_PV_H_

#include <unistd.h>
#include <string.h>
#include <stdio.h>

static inline termui_ssize_t
termui_io_nprintf(termui_iostream_t fd, termui_size_t n, const char *fmt, ...)
{
  char buf[n];
  termui_ssize_t res;
  va_list list;

  va_start(list, fmt);
  res = vsnprintf(buf, n, fmt, list);
  write(fd, buf, res > n ? n : res);
  va_end(list);

  return res;
}

static inline termui_ssize_t
termui_io_puts(const char *str, termui_iostream_t fd)
{
  termui_size_t len = strlen(str);
  return write(fd, str, len);
}

static inline termui_ssize_t
termui_io_put(const char *str, termui_size_t len, termui_iostream_t fd)
{
  return write(fd, str, len);
}

static inline termui_ssize_t
termui_io_putc(char c, termui_iostream_t fd)
{
  return write(fd, &c, 1);
}

static inline int_fast16_t
termui_io_getc(termui_iostream_t fd)
{
  unsigned char c;
  termui_ssize_t res = read(fd, &c, 1);

  return res == 1 ? c : -1;
}

static inline termui_ssize_t
termui_io_geti(termui_iostream_t fd, termui_cursor_pos_t *result)
{
  termui_cursor_pos_t c, res;

  for (res = 0; (c = termui_io_getc(fd)) >= '0' && c <= '9'; 
       res = res * 10 + c - '0')
    ;

  *result = res;
  return c;
}

#endif

