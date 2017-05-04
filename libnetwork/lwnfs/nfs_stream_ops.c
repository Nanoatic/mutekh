
#include <stdio.h>
#include <netinet/nfs.h>

/* descriptor */
struct		hdl_s
{
  nfs_handle_t	handle;
  int		flags;
  int		offs;
  int		size;
};

/* nfs server */
static struct nfs_s	*server = NULL;
/* nfs root */
static nfs_handle_t	root;

/* open a file */
fd_t			open_ptr(const char	*pathname,
				 uint_fast8_t	flags,
				 unsigned int	mode)
{
  struct nfs_attr_s	stat;
  struct hdl_s		*hdl;
  nfs_handle_t		handle;
  error_t		r;
  int			fd;

  r = nfs_lookup(server, pathname, root, handle, &stat);

  /* create if needed */
  if (r)
    {
      if ((flags & O_CREAT))
	{
	  memset(&stat, 0xff, sizeof (struct nfs_attr_s));
	  stat.mode = 0644;

	  if (nfs_creat(server, root, pathname, &stat, handle))
	    goto err;
	}
      else
	goto err;
    }

  if (!(hdl = malloc(sizeof (*hdl))))
    goto err;

  memcpy(hdl->handle, handle, sizeof (nfs_handle_t));
  hdl->flags = flags;

  if (flags & O_APPEND)
    hdl->offs = stat.size;
  else
    hdl->offs = 0;

  /* truncate if needed */
  if (stat.size && (flags & O_TRUNC))
    {
      memset(&stat, 0xff, sizeof (struct nfs_attr_s));
      stat.size = 0;

      nfs_setattr(server, handle, &stat);
      hdl->offs = 0;
    }

  hdl->size = stat.size;

  return hdl;

 err_free:
  free(handle);
 err:
  return NULL;
}

/* read some data */
ssize_t			read_ptr(fd_t	fd,
				 char	*buf,
				 size_t	count)
{
  struct hdl_s	*hdl = (struct hdl_s *)fd;
  ssize_t	rd;
  unsigned long sum = 0;

  /*  if (hdl == NULL)
      mem_guard_check();*/
  assert(hdl != NULL);

  if (hdl == NULL || !((hdl->flags & O_RDONLY) || (hdl->flags & O_RDWR)))
    return -1;

  rd = nfs_read(server, hdl->handle, buf, hdl->offs, count);

  if (rd > 0)
    hdl->offs += rd;

  for (; count > 0; buf++, count--)
    sum += *buf;
  printf("checksum = %lu\n", sum);

  return rd;
}

/* write some data */
ssize_t			write_ptr(fd_t		fd,
				  const void	*buf,
				  size_t	count)
{
  struct hdl_s	*hdl = (struct hdl_s *)fd;
  ssize_t	wr;

  if (hdl == NULL || !((hdl->flags & O_WRONLY) || (hdl->flags & O_RDWR)))
    return -1;

  wr = nfs_write(server, hdl->handle, buf, hdl->offs, count);

  if (wr > 0)
    hdl->offs += wr;

  printf("wrote = %d\n", wr);

  return wr;
}

/* seek a position */
off_t			lseek_ptr(fd_t	fildes,
				  off_t	offset,
				  int	whence)
{
  struct hdl_s	*hdl = (struct hdl_s *)fildes;

  printf("lseek %u from %d\n", offset, whence);

  if (hdl == NULL)
    return -1;

  switch (whence)
    {
      case SEEK_SET:
	hdl->offs = offset;
	break;
      case SEEK_CUR:
	hdl->offs += offset;
	break;
      case SEEK_END:
	hdl->offs = hdl->size + offset;
	break;
      default:
	return -1;
    }

  if (hdl->offs < 0)
    hdl->offs = 0;

  if (hdl->offs > hdl->size)
    hdl->offs = hdl->size;

  assert(whence == SEEK_CUR || whence == SEEK_SET || whence == SEEK_END);
  //  printf("lseek(%d, %lu, %s) = %lu\n", hdl->fd, offset, (whence == SEEK_CUR ? "SEEK_CUR" : (whence == SEEK_SET ? "SEEK_SET" : "SEEK_END")), hdl->offs);

  return hdl->offs;
}

/* only release fd */
int			close_ptr(fd_t	fd)
{
  struct hdl_s	*hdl = (struct hdl_s *)fd;

  return release_handle(hdl->fd);
}

/* permission checking */
bool_t	is_readable(fd_t	fd)
{
  struct hdl_s	*hdl = (struct hdl_s *)fd;

  if (hdl == NULL)
    return 0;

  return (hdl->flags & O_RDONLY) || (hdl->flags & O_RDWR);
}

bool_t	is_writable(fd_t	fd)
{
  struct hdl_s	*hdl = (struct hdl_s *)fd;

  if (hdl == NULL)
    return 0;

  return (hdl->flags & O_WRONLY) || (hdl->flags & O_RDWR);
}

static const struct	stream_ops_s	ops_nfs =
  {
    .open = open_ptr,
    .write = write_ptr,
    .read = read_ptr,
    .close = close_ptr,
    .lseek = lseek_ptr,
    .readable = is_readable,
    .writable = is_writable
  };

