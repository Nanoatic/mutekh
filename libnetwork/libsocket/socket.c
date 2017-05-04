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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#include <netinet/socket.h>
#include <netinet/socket_internals.h>
#include <semaphore.h>
#include <mutek/timer.h>

/*
 * Shortcut macro to get an option verifying user arguments.
 */

#define GETOPT(_type_,_code_)									\
  {												\
    _type_	*val;										\
												\
    if (*optlen < sizeof (_type_))								\
      {												\
	fd->error = EINVAL;									\
	return -1;										\
      }												\
												\
    val = optval;										\
    _code_											\
    *optlen = sizeof (_type_);									\
  }

/*
 * Shortcut macro to set an option.
 */

#define SETOPT(_type_,_code_)									\
  {												\
    _type_	*val;										\
												\
    if (optlen < sizeof (_type_))								\
      {												\
	fd->error = EINVAL;									\
	return -1;										\
      }												\
    												\
    val = optval;										\
    _code_											\
  }

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or NULL for errors.  */
socket_t			socket(int_fast32_t domain, int_fast32_t type, int_fast32_t protocol)
{
  const struct socket_api_s	*api;
  socket_t			sock;

  switch (domain)
    {
      /* Internet sockets */
      case PF_INET:
      case PF_INET6:
	switch (type)
	  {
	    case SOCK_DGRAM:
	      switch (protocol)
		{
#ifdef CONFIG_NETWORK_UDP
		  /* UDP is the default DGRAM protocol */
		  case IPPROTO_UDP:
		  case 0:
		    api = &udp_socket_dispatch;
		    break;
#endif
		  default:
		    //return -EPROTONOSUPPORT;
		    return NULL;
		}
	      break;
	    case SOCK_STREAM:
	      switch (protocol)
		{
#ifdef CONFIG_NETWORK_TCP
		  /* UDP is the default STREAM protocol */
		  case IPPROTO_TCP:
		  case 0:
		    api = &tcp_socket_dispatch;
		    break;
#endif
		  default:
		    //return -EPROTONOSUPPORT;
		    return NULL;
		}
	      break;
#ifdef CONFIG_NETWORK_SOCKET_RAW
	    /* Raw packets to a given protocol */
	    case SOCK_RAW:
	      api = &raw_socket_dispatch;
	      break;
#endif
	    default:
	      //return -EPROTONOSUPPORT;
	      return NULL;
	  }
	break;
#ifdef CONFIG_NETWORK_SOCKET_PACKET
      /* Packet sockets, used to write Layer 2 protocols */
      case PF_PACKET:
	api = &packet_socket_dispatch;
	break;
#endif
      default:
	//return -EPFNOSUPPORT;
	return NULL;
    }
  if ((sock = mem_alloc(sizeof (struct socket_s), (mem_scope_sys))) == NULL)
    //return -ENOMEM;
    return NULL;
  /* setup common fields to their defaults */
  sock->error = 0;
  sock->shutdown = -1;
  sock->type = type;
  sock->broadcast = 0;
  sock->keepalive = 0;
  sock->recv_timeout = 0;
  sock->send_timeout = 0;
  sock->linger = 0;
  sock->f = api;
  api->socket(sock, domain, type, protocol);
  return sock;
}
#if 0
/*
 * Send a message.
 */

_SENDMSG(sendmsg)
{
  ssize_t	ret;
  uint8_t	*buf;
  size_t	n = 0;
  size_t	i;

  /* determine the total size */
  for (i = 0; i < message->msg_iovlen; i++)
    n += message->msg_iov[i].iov_len;

  /* allocate a buffer large enough */
  if ((buf = mem_alloc(n, (mem_scope_sys))) == NULL)
    {
      fd->error = ENOMEM;
      return -1;
    }

  /* build the buffer */
  for (i = 0, n = 0; i < message->msg_iovlen; i++)
    {
      struct iovec	*v = &message->msg_iov[i];
      memcpy(buf + n, v->iov_base, v->iov_len);
      n += v->iov_len;
    }

  /* send & free */
  ret = fd->f->sendto(fd, buf, n, flags, message->msg_name, message->msg_namelen, message);
  mem_free(buf);

  return ret;
}

/*
 * Receive a message.
 */

_RECVMSG(recvmsg)
{
  ssize_t	ret;
  uint8_t	*buf;
  size_t	n = 0;
  size_t	i;

  /* determine the total size */
  for (i = 0; i < message->msg_iovlen; i++)
    n += message->msg_iov[i].iov_len;

  /* allocate a buffer large enough */
  if ((buf = mem_alloc(n, (mem_scope_sys))) == NULL)
    {
      fd->error = ENOMEM;
      return -1;
    }

  /* receive the data */
  ret = fd->f->recvfrom(fd, buf, n, flags, message->msg_name, &message->msg_namelen, message);

  /* pack into multiple vectors */
  for (i = 0, n = 0; i < message->msg_iovlen; i++)
    {
      struct iovec	*v = &message->msg_iov[i];

      if (n + v->iov_len > ret)
	memcpy(v->iov_base, buf + n, ret - n);
      else
	memcpy(v->iov_base, buf + n, v->iov_len);
      n += v->iov_len;
    }

  mem_free(buf);

  return ret;
}
#endif
/*
 * Setsock opt, SOL_SOCKET level.
 */

int_fast32_t setsockopt_socket(socket_t		fd,
			       int_fast32_t	optname,
			       const void	*optval,
			       socklen_t	optlen)
{
  switch (optname)
    {
      /* recv timeout */
      case SO_RCVTIMEO:
	SETOPT(const struct timeval,
	{
	  fd->recv_timeout = val->tv_sec * 1000 + val->tv_usec / 1000;
	});
	break;
      /* send timeout */
      case SO_SNDTIMEO:
	SETOPT(const struct timeval,
	{
	  fd->send_timeout = val->tv_sec * 1000 + val->tv_usec / 1000;
	});
	break;
      /* allow sending/receiving broadcast */
      case SO_BROADCAST:
	SETOPT(const bool_t,
	{
	  fd->broadcast = *val;
	});
	break;
      /* allow keepalive packets */
      case SO_KEEPALIVE:
	SETOPT(const bool_t,
	{
	  fd->keepalive = *val;
	});
	break;
      /* linger timeout */
      case SO_LINGER:
	SETOPT(const struct linger,
	{
	  if (val->l_onoff)
	    fd->linger = val->l_linger;
	  else
	    fd->linger = 0;
	});
	break;
      default:
	fd->error = ENOPROTOOPT;
	return -1;
    }

  return 0;
}

/*
 * Getsock opt, SOL_SOCKET level.
 */

int_fast32_t getsockopt_socket(socket_t	fd,
		      int_fast32_t	optname,
		      void	*optval,
		      socklen_t	*optlen)
{
  switch (optname)
    {
      /* recv timeout */
      case SO_RCVTIMEO:
	GETOPT(struct timeval,
	{
	  val->tv_usec = (fd->recv_timeout % 1000) * 1000;
	  val->tv_sec = fd->recv_timeout / 1000;
	});
	break;
      /* send timeout */
      case SO_SNDTIMEO:
	GETOPT(struct timeval,
	{
	  val->tv_usec = (fd->send_timeout % 1000) * 1000;
	  val->tv_sec = fd->send_timeout / 1000;
	});
	break;
      /* broadcast enabled */
      case SO_BROADCAST:
	GETOPT(bool_t,
	{
	  *val = fd->broadcast;
	});
	break;
      /* keepalive enabled */
      case SO_KEEPALIVE:
	GETOPT(bool_t,
	{
	  *val = fd->keepalive;
	});
	break;
      /* socket type */
      case SO_TYPE:
	GETOPT(int_fast32_t,
	{
	  *val = fd->type;
	});
	break;
      /* socket last error */
      case SO_ERROR:
	GETOPT(int_fast32_t,
	{
	  *val = fd->error;
	});
	break;
      /* linger timeout */
      case SO_LINGER:
	GETOPT(struct linger,
	{
	  val->l_onoff = !!fd->linger;
	  val->l_linger = fd->linger;
	});
	break;
      default:
	fd->error = ENOPROTOOPT;
	return -1;
    }

  return 0;
}

/*
 * Shutdown
 */

_SHUTDOWN(shutdown_socket)
{
  if (how != SHUT_RDWR && how != SHUT_RD && how != SHUT_WR)
    {
      fd->error = EINVAL;
      return -1;
    }

  /* check combinations */
  if (how == SHUT_RDWR || (fd->shutdown == SHUT_RD && how == SHUT_WR) ||
      (fd->shutdown == SHUT_WR && how == SHUT_RD))
    fd->shutdown = SHUT_RDWR;
  else
    fd->shutdown = how;

  return 0;
}

/*
 * Setsock opt, SOL_IP level.
 */

int_fast32_t setsockopt_inet(socket_t		fd,
			     int_fast32_t	optname,
			     const void		*optval,
			     socklen_t		optlen)
{
  switch (optname)
    {
      /* XXX SOL_IP */
      default:
	fd->error = ENOPROTOOPT;
	return -1;
    }

  return 0;
}

/*
 * Getsock opt, SOL_SOCKET level.
 */

int_fast32_t getsockopt_inet(socket_t		fd,
			     int_fast32_t	optname,
			     void		*optval,
			     socklen_t		*optlen)
{
  switch (optname)
    {
      /* XXX SOL_IP */
      default:
	fd->error = ENOPROTOOPT;
	return -1;
    }

  return 0;
}

/*
 * Packet grabbing
 */

struct net_packet_s	*socket_grab_packet(socket_t			fd,
					    int_fast32_t		flags,
					    timer_event_callback_t	*recv_timeout,
					    packet_queue_root_t		*recv_q,
					    struct semaphore_s		*recv_sem)
{
  struct net_packet_s		*packet;
  timer_delay_t			start;
  struct timer_event_s		timeout;
  bool_t			timeout_started = 0;

  start = timer_get_tick(&timer_ms);

  /* try to grab a packet */
 again:
  if (fd->shutdown == SHUT_RD || fd->shutdown == SHUT_RDWR)
    {
      fd->error = ESHUTDOWN;
      return NULL;
    }

  if (flags & MSG_PEEK)
    packet = packet_queue_lock_head(recv_q);
  else
    packet = packet_queue_lock_pop(recv_q);

  if (packet == NULL)
    {
      if (flags & MSG_DONTWAIT)
	{
	  fd->error = EAGAIN;
	  return NULL;
	}

      /* if there is a receive timeout, start a timer */
      if (!timeout_started && fd->recv_timeout)
	{
	  timeout.callback = recv_timeout;
	  timeout.delay = fd->recv_timeout;
	  timeout.pv = fd;
	  timeout_started = 1;
	  timer_add_event(&timer_ms, &timeout);
	}

      semaphore_take(recv_sem, 1);

      /* has timeout expired ? */
      if (timeout_started)
	if (timer_get_tick(&timer_ms) - start >= (fd->recv_timeout * 5) / 6)
	  {
	    fd->error = EAGAIN;
	    return NULL;
	  }

      goto again;
    }

  if (timeout_started)
    timer_cancel_event(&timeout, 0);

  return packet;
}

/*
 * Buffer grabbing
 */

struct net_buffer_s	*socket_grab_buffer(socket_t			fd,
					    int_fast32_t		flags,
					    timer_event_callback_t	*recv_timeout,
					    buffer_queue_root_t		*recv_q,
					    struct semaphore_s		*recv_sem)
{
  struct net_buffer_s		*buffer;
  timer_delay_t			start;
  struct timer_event_s		timeout;
  bool_t			timeout_started = 0;

  start = timer_get_tick(&timer_ms);

  /* try to grab a buffer */
 again:
  if (fd->shutdown == SHUT_RD || fd->shutdown == SHUT_RDWR)
    {
      fd->error = ESHUTDOWN;
      return NULL;
    }

  if (flags & MSG_PEEK)
    buffer = buffer_queue_lock_head(recv_q);
  else
    buffer = buffer_queue_lock_pop(recv_q);

  if (buffer == NULL)
    {
      if (flags & MSG_DONTWAIT)
	{
	  fd->error = EAGAIN;
	  return NULL;
	}

      /* if there is a receive timeout, start a timer */
      if (!timeout_started && fd->recv_timeout)
	{
	  timeout.callback = recv_timeout;
	  timeout.delay = fd->recv_timeout;
	  timeout.pv = fd;
	  timeout_started = 1;
	  timer_add_event(&timer_ms, &timeout);
	}

      semaphore_take(recv_sem, 1);

      /* has timeout expired ? */
      if (timeout_started)
	if (timer_get_tick(&timer_ms) - start >= (fd->recv_timeout * 5) / 6)
	  {
	    fd->error = EAGAIN;
	    return NULL;
	  }

      goto again;
    }

  if (timeout_started)
    timer_cancel_event(&timeout, 0);

  return buffer;
}
