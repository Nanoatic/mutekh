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

#ifndef NETINET_SOCKET_INTERNALS_H
# define NETINET_SOCKET_INTERNALS_H

#ifndef NETINET_SOCKET_H
# error "You must include <netinet/socket.h> before including this file."
#endif

/*
 * Buffers
 */

struct				net_buffer_s
{
  void				*data;
  size_t			size;
  struct net_addr_s		address;
  net_port_t			port;

  CONTAINER_ENTRY_TYPE(DLIST)	list_entry;
};

#define CONTAINER_LOCK_buffer_queue	HEXO_SPIN_IRQ
CONTAINER_TYPE(buffer_queue, DLIST, struct net_buffer_s, list_entry);
CONTAINER_FUNC(buffer_queue, DLIST, static inline, buffer_queue_lock);

/*
 * Common operations.
 */

int_fast32_t getsockopt_socket(socket_t		fd,
			       int_fast32_t	optname,
			       void		*optval,
			       socklen_t	*optlen);
int_fast32_t setsockopt_socket(socket_t		fd,
			       int_fast32_t	optname,
			       const void	*optval,
			       socklen_t	optlen);
int_fast32_t setsockopt_inet(socket_t		fd,
			     int_fast32_t	optname,
			     const void		*optval,
			     socklen_t		optlen);
int_fast32_t getsockopt_inet(socket_t		fd,
			     int_fast32_t	optname,
			     void		*optval,
			     socklen_t		*optlen);

struct net_packet_s	*socket_grab_packet(socket_t			fd,
					    int_fast32_t		flags,
					    timer_event_callback_t	*recv_timeout,
					    packet_queue_root_t		*recv_q,
					    struct semaphore_s			*recv_sem);

struct net_buffer_s	*socket_grab_buffer(socket_t			fd,
					    int_fast32_t		flags,
					    timer_event_callback_t	*recv_timeout,
					    buffer_queue_root_t		*recv_q,
					    struct semaphore_s			*recv_sem);

_SHUTDOWN(shutdown_socket);

/*
 * Dispatch structure instances.
 */

#ifdef CONFIG_NETWORK_UDP
extern const struct socket_api_s	udp_socket_dispatch;
#endif
#ifdef CONFIG_NETWORK_TCP
extern const struct socket_api_s	tcp_socket_dispatch;
#endif
#ifdef CONFIG_NETWORK_SOCKET_PACKET
extern const struct socket_api_s	packet_socket_dispatch;
#endif
#ifdef CONFIG_NETWORK_SOCKET_RAW
extern const struct socket_api_s	raw_socket_dispatch;
#endif

/*
 * Address conversion.
 */

static inline error_t	socket_in_addr(struct socket_s		*fd,
				       struct net_addr_s	*a,
				       struct sockaddr		*addr,
				       socklen_t		len,
				       uint_fast16_t		*port)
{
  switch (addr->sa_family)
    {
      case AF_INET:
	{
	  struct sockaddr_in	*in;

	  in = (struct sockaddr_in *)addr;

	  if (len < sizeof (struct sockaddr_in))
	    {
	      fd->error = EINVAL;
	      return -1;
	    }

	  IPV4_ADDR_SET(*a, ntohl(in->sin_addr.s_addr));
	  if (port != NULL)
	    *port = ntohs(in->sin_port);
	}
	break;
      case AF_INET6:
	/* IPV6 */
      default:
	fd->error = EAFNOSUPPORT;
	return -1;
    }

  return 0;
}

static inline error_t	socket_addr_in(struct socket_s		*fd,
				       struct net_addr_s	*a,
				       struct sockaddr		*addr,
				       socklen_t		*len,
				       uint_fast16_t		port)
{
  switch (a->family)
    {
      case addr_ipv4:
	{
	  struct sockaddr_in	*in = (struct sockaddr_in *)addr;

	  if (*len < sizeof (struct sockaddr_in))
	    {
	      fd->error = EINVAL;
	      return -1;
	    }

	  /* fill the address structure */
	  in->sin_family = AF_INET;
	  in->sin_port = port;
	  in->sin_addr.s_addr = htonl(IPV4_ADDR_GET(*a));

	  *len = sizeof (struct sockaddr_in);
	}
	break;
      default:
	fd->error = EAFNOSUPPORT;
	return -1;
    }

  return 0;
}

#endif
