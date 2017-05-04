/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <hexo/types.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <libc/formatter.h>

#ifdef CONFIG_LIBC_STREAM
static inline void
__printf_out_stream(void *ctx, const char *str, size_t offset, size_t len)
{
  FILE *stream = ctx;
  __stdio_write(len, stream, (uint8_t*)str);
}
#endif

struct __printf_str_s
{
  size_t	size;
  char		*out;
};

static inline void
__printf_out_str(void *ctx_, const char *str, size_t offset, size_t len)
{
  struct __printf_str_s	*ctx = ctx_;

  if (ctx->size > offset)
    {
      size_t	size = __MIN(ctx->size - offset, len);
      memcpy(ctx->out + offset, str, size);
    }
}

inline ssize_t vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
  ssize_t	res;

  struct __printf_str_s	ctx = {
    .size = size,
    .out = str,
  };

  res = formatter_printf(&ctx, __printf_out_str, format, ap);

  /* add final \0 to output string */
  if (ctx.size > res)
    str[res] = '\0';

  /* don't count the trailing '\0' in the res */
  return res;
}

ssize_t snprintf(char *str, size_t size, const char *format, ...)
{
  ssize_t	res;
  va_list	ap;

  va_start(ap, format);
  res = vsnprintf(str, size, format, ap);
  va_end(ap);

  return res;
}

inline ssize_t vsprintf(char *str, const char *format, va_list ap)
{
  return vsnprintf(str, -1, format, ap);
}

ssize_t sprintf(char *str, const char *format, ...)
{
  ssize_t	res;
  va_list	ap;

  va_start(ap, format);
  res = vsprintf(str, format, ap);
  va_end(ap);

  return res;
}

#ifdef CONFIG_LIBC_STREAM

ssize_t vfprintf(FILE *stream, const char *format, va_list ap)
{
  return formatter_printf(stream, __printf_out_stream, format, ap);
}

ssize_t fprintf(FILE *stream, const char *format, ...)
{
  ssize_t	res;
  va_list	ap;

  va_start(ap, format);
  res = vfprintf(stream, format, ap);
  va_end(ap);

  return res;
}

# ifdef CONFIG_LIBC_STREAM_STD
ssize_t printf(const char *format, ...)
{
  ssize_t	res;
  va_list	ap;

  va_start(ap, format);
  res = vprintf(format, ap);
  va_end(ap);

  return res;
}

error_t puts(const char *str)
{
  if (fputs(str, stdout))
    return EOF;
  return fputc('\n', stdout) < 0 ? EOF : 0;
}
# endif

#endif

