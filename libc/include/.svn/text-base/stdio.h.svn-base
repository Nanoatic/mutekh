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

#ifndef STDIO_H_
#define STDIO_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

/**
 * @file
 * @module{C library}
 */

#include <hexo/types.h>
#include <hexo/error.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_ring.h>

#include <stdarg.h>
#include <unistd.h>

#define HAVE_SPRINTF
ssize_t sprintf(char *str, const char *format, ...);

#define HAVE_SNPRINTF
ssize_t snprintf(char *str, size_t size, const char *format, ...);

#define HAVE_VSPRINTF
ssize_t vsprintf(char *str, const char *format, va_list ap);

#define HAVE_VSNPRINTF
ssize_t vsnprintf(char *str, size_t size, const char *format, va_list ap);

#define HAVE_SCANF
ssize_t scanf(const char *format, ...);

#define HAVE_SSCANF
ssize_t sscanf(const char *str, const char *format, ...);

#define HAVE_VSCANF
ssize_t vscanf(const char *format, va_list ap);

#define HAVE_VSSCANF
ssize_t vsscanf(const char *str, const char *format, va_list ap);

#ifdef CONFIG_LIBC_STREAM

/** standard BUFSIZ macro */
#define BUFSIZ		CONFIG_LIBC_STREAM_BUFFER_SIZE

CONTAINER_TYPE(stream_fifo, RING, uint8_t, CONFIG_LIBC_STREAM_BUFFER_SIZE);

/** standard EOF macro */
# define EOF			-1

typedef int32_t			fpos_t;

enum				stdio_buf_mode_e
{
    _IONBF, _IOLBF, _IOFBF,
};

struct				file_s
{
  const struct fileops_s	*ops;
  void *hndl;

  error_t			(*rwflush)(struct file_s *file);
  stream_fifo_root_t		fifo_read;
  stream_fifo_root_t		fifo_write;

  fpos_t			pos;
  enum stdio_buf_mode_e		buf_mode;
  bool_t			eof:1, error:1;
};

typedef struct file_s		FILE;

/** internal stream buffered read function.
    @returns 1 on success, 0 on end of stream and < 0 on error.
*/
error_t	__stdio_read(size_t size_, FILE *file, uint8_t *ptr);

/** internal stream buffered write function.
    @returns 0 on success, < 0 on error.
*/
error_t __stdio_write(size_t size_, FILE *file, const uint8_t *ptr);

void __stdio_stream_init(FILE *stream);

#ifdef CONFIG_VFS
# define HAVE_FOPEN
FILE *fopen(const char *path, const char *mode);
#endif

#define HAVE_FCLOSE
error_t fclose(FILE *file);

#define HAVE_FPUTC
int_fast16_t fputc(unsigned char c, FILE *file);

#define HAVE_FGETC
int_fast16_t fgetc(FILE *file);

#define HAVE_FREAD
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *file);

#define HAVE_FWRITE
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *file);

#define HAVE_FGETS
char *fgets(char *str, size_t size, FILE *file);

#define HAVE_FPUTS
error_t fputs(const char *str, FILE *file);

#define HAVE_PUTS
error_t puts(const char *str);

#define HAVE_FSEEK
error_t fseek(FILE *file, fpos_t offset, int_fast8_t whence);

#define HAVE_FFLUSH
error_t fflush(FILE *file);

#define HAVE_FPURGE
error_t fpurge(FILE *file);

#define HAVE_VFPRINTF
ssize_t vfprintf(FILE *file, const char *format, va_list ap);

#define HAVE_FPRINTF
ssize_t fprintf(FILE *file, const char *format, ...);

#define HAVE_FSCANF
ssize_t fscanf(FILE *file, const char *format, ...);

#define HAVE_UNGETC
int_fast16_t ungetc(int_fast16_t c, FILE *file);

#define HAVE_SETVBUF
error_t setvbuf(FILE *file, char *buf, enum stdio_buf_mode_e mode, size_t size);

#define HAVE_FTELL
static inline fpos_t ftell(FILE *file)
{
  return file->pos;
}

#define HAVE_REWIND
static inline void rewind(FILE *file)
{
  fseek(file, 0, SEEK_SET);
}

#define HAVE_FGETPOS
static inline error_t fgetpos(FILE *file, fpos_t *pos)
{
  *pos = file->pos;
  return 0;
}

#define HAVE_FSETPOS
static inline error_t fsetpos(FILE *file, const fpos_t *pos)
{
  return fseek(file, *pos, SEEK_SET);
}

#define HAVE_GETC
static inline int_fast16_t getc(FILE *file)
{
  return fgetc(file);
}

#define HAVE_PUTC
static inline int_fast16_t putc(int_fast16_t c, FILE *file)
{
  return fputc(c, file);
}

#define HAVE_CLEARERR
static inline void clearerr(FILE *stream)
{
  stream->error = 0;
  stream->eof = 0;
}

#define HAVE_FERROR
static inline bool_t ferror(FILE *stream)
{
  return stream->error;
}

#define HAVE_FEOF
static inline bool_t feof(FILE *stream)
{
  return stream->eof;
}

# ifdef CONFIG_LIBC_STREAM_STD

#define HAVE_STDIN
extern FILE * const stdin;

#define HAVE_STDOUT
extern FILE * const stdout;

#define HAVE_STDERR
extern FILE * const stderr;

#define HAVE_GETCHAR
static inline int_fast16_t getchar()
{
  return fgetc(stdin);
}

#define HAVE_PUTCHAR
static inline int_fast16_t putchar(int_fast16_t c)
{
  return fputc(c, stdout);
}

#define HAVE_VPRINTF
static inline ssize_t vprintf(const char *format, va_list ap)
{
  return vfprintf(stdout, format, ap);
}

#define HAVE_PRINTF
ssize_t printf(const char *format, ...);

#define HAVE_GETS
__attribute__((deprecated))
static inline
char *gets(char *s)
{
  return fgets(s, 1024, stdin);
}

#define HAVE_PERROR
void perror(const char *reason);

# endif	/* CONFIG_LIBC_STREAM_STD */

#endif /* CONFIG_LIBC_STREAM */

C_HEADER_END

#endif

