
#include <hexo/error.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <mutek/fileops.h>

#ifdef CONFIG_VFS
#include <vfs/vfs.h>
#include <vfs/file.h>
#include <mutek/mem_alloc.h>
#endif

/***********************************************************************
            Internal buffered stream API
***********************************************************************/

CONTAINER_FUNC(stream_fifo, RING, static inline, stream_fifo);

static error_t	__stdio_no_flush(FILE *stream)
{
  return 0;
}

static error_t	__stdio_write_flush(FILE *stream)
{
  uint8_t	local[CONFIG_LIBC_STREAM_BUFFER_SIZE];
  uint8_t	*ptr = local;
  size_t	size = stream_fifo_pop_array(&stream->fifo_write, ptr, CONFIG_LIBC_STREAM_BUFFER_SIZE);
  ssize_t	res;

  /* write remaining data present in buffer */
  while (size)
    {
      res = stream->ops->write(stream->hndl, ptr, size);

      if (res < 0)
	{
	  stream->error = 1;
	  stream_fifo_pushback_array(&stream->fifo_write, ptr, size);
	  return res;
	}

      size -= res;
      ptr += res;
    };

  /* write buffer is empty here */
  stream->rwflush = &__stdio_no_flush;
  return 0;
}

static error_t	__stdio_read_flush(FILE *stream)
{
  /* seek fd to real pos and flush prefetched data in read buffer */
  stream->ops->lseek(stream->hndl, stream->pos, SEEK_SET);
  stream_fifo_clear(&stream->fifo_read);

  /* read buffer is empty here */
  stream->rwflush = &__stdio_no_flush;
  return 0;
}

static error_t unbuffered_read(size_t size, FILE *stream, uint8_t *ptr)
{
    ssize_t res;

    while (size)
    {
        res = stream->ops->read(stream->hndl, ptr, size);

        if (res <= 0)
        {
            if (res == 0)
                stream->eof = 1;
            else
                stream->error = 1;

            return res;
        }

        stream->pos += res;
        size -= res;
        ptr += res;
    }

    return 1;
}

error_t	__stdio_read(size_t size, FILE *stream, uint8_t *ptr)
{
    if (!stream->ops->read)
        return -EINVAL;

    switch(stream->buf_mode)
    {
        case _IONBF:
        case _IOLBF:
            /* line buffered for LBF is non-sense so let's assume it's unbuffered */
            return unbuffered_read(size, stream, ptr);

        case _IOFBF:
            {
                uint8_t	local[CONFIG_LIBC_STREAM_BUFFER_SIZE];
                ssize_t	res, local_size;

                /* get data from buffer */
                res = stream_fifo_pop_array(&stream->fifo_read, ptr, size);
                size -= res;
                ptr += res;
                stream->pos += res;

                if (size)
                {
                    /* read buffer is empty here */
                    stream->rwflush = &__stdio_no_flush;

                    /* read more data directly from fd */
                    while (size > CONFIG_LIBC_STREAM_BUFFER_SIZE)
                    {
                        size_t s = (size/CONFIG_LIBC_STREAM_BUFFER_SIZE)*CONFIG_LIBC_STREAM_BUFFER_SIZE;
                        res = stream->ops->read(stream->hndl, ptr, s);

                        if (res <= 0)
                        {
                            if (res == 0)
                                stream->eof = 1;
                            else
                                stream->error = 1;

                            return res;
                        }

                        stream->eof = 0;
                        size -= res;
                        ptr += res;
                        stream->pos += res;
                    }
                }

                /* read remaining data in local buffer */
                for (local_size = 0; local_size < size; local_size += res)
                {
                    res = stream->ops->read(stream->hndl, local + local_size,
                            CONFIG_LIBC_STREAM_BUFFER_SIZE - local_size);

                    if (res < 0)
                    {
                        stream->error = 1;
                        return res;
                    }
                    if (res == 0)
                        break;
                }

                memcpy(ptr, local, size);
                stream->pos += size;

                if (local_size >= size)
                {
                    if (local_size > size)
                    {
                        /* if more data than needed, put in read buffer */
                        stream_fifo_pushback_array(&stream->fifo_read, local + size, local_size - size);
                        stream->rwflush = &__stdio_read_flush;
                    }
                    return 1;
                }
                else
                {
                    /* not enough data have been read */
                    stream->eof = 1;
                }
            }
    }
    return 0;
}

static error_t unbuffered_write(size_t size, FILE *stream, const uint8_t *ptr)
{
  ssize_t res;

  while (size)
    {
      res = stream->ops->write(stream->hndl, ptr, size);

      if (res < 0)
	{
	  stream->error = 1;
	  return res;
	}

      stream->pos += res;
      size -= res;
      ptr += res;
    }

  return 0;
}

error_t __stdio_write(size_t size, FILE *stream, const uint8_t *ptr)
{
  if (!stream->ops->write)
    return -EINVAL;

  switch (stream->buf_mode)
    {
    case _IONBF:
      return unbuffered_write(size, stream, ptr);

    case _IOLBF: {
      ssize_t i;

      /* write all ended lines if any */
      for (i = size; i > 0; i--)
	if (ptr[i-1] == '\n')
	  {
	    ssize_t res;

	    if ((res = __stdio_write_flush(stream)))
	      return res;
	    if ((res = unbuffered_write(i, stream, ptr)))
	      return res;

	    ptr += i;
	    size -= i;
	    break;
	  }
    }

    /* remaining data without end of line will be treated as block */

    case _IOFBF:

      /* check if all data can be put in buffer */
      if (stream_fifo_count(&stream->fifo_write) + size > CONFIG_LIBC_STREAM_BUFFER_SIZE)	
	{
	  ssize_t	res;

	  /* write all data present in buffer */
	  if ((res = __stdio_write_flush(stream)))
	    return res;

	  /* write data directly to device if greater than buffer */
	  while (size > CONFIG_LIBC_STREAM_BUFFER_SIZE)
	    {
	      res = stream->ops->write(stream->hndl, ptr, size);

	      if (res < 0)
		{
		  stream->error = 1;
		  return res;
		}

	      size -= res;
	      ptr += res;
	      stream->pos += res;
	    }
	}

      /* fill buffer with remaining data */
      void *tmp = (void*)ptr;
      stream_fifo_pushback_array(&stream->fifo_write, (stream_fifo_item_t*)tmp, size);
      stream->pos += size;
      stream->rwflush = &__stdio_write_flush;
    }

  return 0;
}

/***********************************************************************
            Stdio functions
***********************************************************************/

error_t	fclose(FILE *stream)
{
  error_t	err;

  if (!stream->ops->close)
    return -EINVAL;

  if ((err = stream->rwflush(stream)))
    return err;

  if ((err = stream->ops->close(stream->hndl)))
    return err;

  free(stream);

  return (err);
}

error_t	fflush(FILE *stream)
{
  if (!stream->ops->write)
    return -EINVAL;

  return __stdio_write_flush(stream);
}

error_t	fpurge(FILE *stream)
{
  if (!stream->ops->read)
    return -EINVAL;

  return __stdio_read_flush(stream);
}

/* ************************************************** */

size_t	fread(void *ptr_, size_t size,
	      size_t nmemb, FILE *stream)
{
  uint8_t	*ptr = ptr_;
  size_t	i;

  for (i = 0; i < nmemb; i++)
    {
      if (__stdio_read(size, stream, ptr) <= 0)
	break;

      ptr += size;
    }

  return i;
}

size_t	fwrite(const void *ptr_, size_t size, size_t nmemb, FILE *stream)
{
  const uint8_t	*ptr = ptr_;
  size_t	i;

  for (i = 0; i < nmemb; i++)
    {
      if (__stdio_write(size, stream, ptr))
	break;

      ptr += size;
    }

  return i;
}

/* ************************************************** */

error_t fseek(FILE *stream, fpos_t offset, int_fast8_t whence)
{
  if (!stream->ops->lseek)
    return EOF;

  stream->rwflush(stream);

  stream->pos = stream->ops->lseek(
	  (struct vfs_file_s *)stream->hndl, offset, whence);
  if (stream->pos >= 0)
    return 0;
  else
    return EOF;
}

/* ************************************************** */

int_fast16_t fgetc(FILE *stream)
{
  unsigned char	res;
  return __stdio_read(1, stream, &res) <= 0 ? EOF : res;
}

int_fast16_t ungetc(int_fast16_t c, FILE *stream)
{
  return stream_fifo_push(&stream->fifo_read, c) ? c : EOF;
}

char *fgets(char *str_, size_t size, FILE *stream)
{
  char	*str = str_;
  char	*ret = NULL;

  while (size-- > 1)
    {
	  error_t res = __stdio_read(1, stream, (uint8_t*)str);

      if (res == 0)
	break;
      else if (res < 0)
	return NULL;

      ret = str_;

      if (*str++ == '\n')
	break;
    }

  *str = 0;

  return ret;
}

/* ************************************************** */

int_fast16_t fputc(unsigned char c, FILE *stream)
{
  if (__stdio_write(1, stream, &c))
    return EOF;

  return (c);
}

error_t	fputs(const char *str, FILE *stream)
{
  return __stdio_write(strlen(str), stream, (uint8_t*)str) ? EOF : 0;
}

/* ************************************************** */

error_t setvbuf(FILE *stream, char *buf, enum stdio_buf_mode_e mode, size_t size)
{
  stream->rwflush(stream);
  stream->buf_mode = mode;
  return 0;
}

/* ************************************************** */

void __stdio_stream_init(FILE *file)
{
  file->rwflush = &__stdio_no_flush;
  stream_fifo_init(&file->fifo_read);
  stream_fifo_init(&file->fifo_write);

  file->pos = 0;
  file->buf_mode = _IONBF;
  file->error = 0;
  file->eof = 0;
}

#if defined(CONFIG_VFS)

static enum vfs_open_flags_e	open_flags(const char *str)
{
  enum vfs_open_flags_e	flags = 0;

  while (*str)
    {
      switch (*str)
	{
	case ('r'):
	  flags |= VFS_OPEN_READ;
	  break;

	case ('w'):
	  flags |= VFS_OPEN_WRITE | VFS_OPEN_CREATE | VFS_OPEN_TRUNCATE;
	  break;

	case ('+'):
	  flags |= VFS_OPEN_READ | VFS_OPEN_WRITE;
	  break;

	case ('a'):
	  flags |= VFS_OPEN_WRITE | VFS_OPEN_APPEND;
	  break;

	case ('b'):
	  break;

	default:
	  return 0;
	}
      str++;
    }

  return (flags);
}

FILE *fopen(const char *path, const char *mode)
{
  enum vfs_open_flags_e flags = open_flags(mode);
  FILE *file = mem_alloc(sizeof(FILE) + sizeof(struct fileops_s), mem_scope_sys);

  if ( file == NULL ) {
	  errno = ENOMEM;
	  goto err;
  }

  struct fileops_s *ops = (struct fileops_s *)(file+1);
  file->ops = ops;

  struct vfs_file_s *hndl;
  error_t error = vfs_open(vfs_get_root(),
						   vfs_get_cwd(),
						   path, flags, &hndl);

  if (error) {
	  errno = -error;
	  goto err_1;
  }
  file->hndl = (void*)hndl;

  ops->read = hndl->read;
  ops->write = hndl->write;
  ops->lseek = hndl->seek;
  ops->close = hndl->close;

  __stdio_stream_init(file);

  file->pos = vfs_file_seek((struct vfs_file_s *)file->hndl,
							0, SEEK_CUR);
  file->buf_mode = _IOFBF;

  return file;

 err_1:
  mem_free(file);
 err:
  return NULL;
}

#endif /* CONFIG_VFS */


/* ************************************************** standard streams */

#ifdef CONFIG_LIBC_STREAM_STD

#include <mutek/console.h>

static error_t	no_flush(FILE *stream)
{
  return 0;
}

static struct file_s stdin_file =
{
  .ops = &console_file_ops,
  .buf_mode = _IOLBF,
  .rwflush = &no_flush,
};

FILE * const stdin = &stdin_file;

static struct file_s stdout_file =
{
  .ops = &console_file_ops,
  .buf_mode = _IOLBF,
  .rwflush = &no_flush,
};

FILE * const stdout = &stdout_file;

static struct file_s stderr_file =
{
  .ops = &console_file_ops,
  .buf_mode = _IONBF,
  .rwflush = &no_flush,
};

FILE * const stderr = &stderr_file;

void perror(const char *reason)
{
  fprintf(stderr, "%s: %s\n", reason, strerror(errno));
}

#endif
