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

#include <device/device.h>
#include <device/net.h>
#include <device/driver.h>

#include <netinet/packet.h>
#include <netinet/socket.h>
#include <netinet/socket_internals.h>
#include <netinet/socket_packet.h>
#include <netinet/if.h>
#include <netinet/arp.h>

#include <mutek/mem_alloc.h>
#include <mutek/printk.h>

#include <mutek/semaphore.h>
#include <mutek/timer.h>

socket_table_root_t	pf_packet = CONTAINER_ROOT_INITIALIZER(socket_table, DLIST);

/*
 * Receive timeout callback.
 */

static TIMER_CALLBACK(recv_timeout)
{
  socket_t			fd = (socket_t)pv;
  struct socket_packet_pv_s	*pv_packet = (struct socket_packet_pv_s *)fd->pv;

  semaphore_give(&pv_packet->recv_sem, 1);
}

/*
 * Create a PACKET socket. Allocate private data.
 */

static _SOCKET(socket_packet)
{
  struct socket_packet_pv_s	*pv;

  if ((pv = fd->pv = mem_alloc(sizeof (struct socket_packet_pv_s), (mem_scope_sys))) == NULL)
    return -ENOMEM;

  /* setup private data */
  pv->proto = ntohs(protocol);
  pv->interface = 0;
  semaphore_init(&pv->recv_sem, 0);
  packet_queue_lock_init(&pv->recv_q);
  pv->header = (type == SOCK_RAW);
  if (!socket_table_push(&pf_packet, fd))
    {
      semaphore_destroy(&pv->recv_sem);
      packet_queue_lock_destroy(&pv->recv_q);
      mem_free(pv);
      return -ENOMEM;
    }

  return 0;
}

/*
 * Set a PACKET socket to listen on a given local address (or interface).
 */

static _BIND(bind_packet)
{
  struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)fd->pv;
  struct sockaddr_ll		*sll = (struct sockaddr_ll *)addr;
  struct net_if_s		*interface;

  if (len < sizeof (struct sockaddr_ll) || sll->sll_family != AF_PACKET)
    {
      fd->error = EINVAL;
      return -1;
    }

  /* bind to the given interface and protocol */
  if ((interface = if_get_by_index(sll->sll_ifindex)) == NULL)
    {
      fd->error = EADDRNOTAVAIL;
      return -1;
    }

  pv->interface = interface->index;

  net_if_obj_refdrop(interface);

  pv->proto = ntohs(sll->sll_protocol);

  return 0;
}

/*
 * Get the socket local address.
 */

static _GETSOCKNAME(getsockname_packet)
{
  struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)fd->pv;
  struct sockaddr_ll		*sll = (struct sockaddr_ll *)addr;

  if (*len < sizeof (struct sockaddr_ll))
    {
      fd->error = EINVAL;
      return -1;
    }

  /* not bound... */
  if (pv->interface == 0)
    {
      fd->error = EADDRNOTAVAIL;
      return -1;
    }

  /* fill socket name */
  sll->sll_family = AF_PACKET;
  sll->sll_ifindex = pv->interface;
  sll->sll_protocol = htons(pv->proto);

  *len = sizeof (struct sockaddr_ll);

  return 0;
}

/*
 * Send a message
 */

static _SENDMSG(sendmsg_packet)
{
  struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)fd->pv;
  struct sockaddr_ll		*sll;
  struct net_packet_s		*packet;
  struct net_if_s		*interface;
  struct net_header_s		*nethdr;
  size_t			n;
  size_t			i;

  if (flags & (MSG_OOB | MSG_EOR | MSG_DONTROUTE | MSG_CONFIRM))
    {
      fd->error = EOPNOTSUPP;
      return -1;
    }

  if (fd->shutdown == SHUT_WR || fd->shutdown == SHUT_RDWR)
    {
      fd->error = ESHUTDOWN;
      return -1;
    }

  if (message == NULL)
    {
      fd->error = EINVAL;
      return -1;
    }

  /* check provided address */
  sll = (struct sockaddr_ll *)message->msg_name;

  if ((sll == NULL && !pv->header) || message->msg_namelen < sizeof (struct sockaddr_ll) ||
      sll->sll_family != AF_PACKET)
    {
      fd->error = (sll == NULL ? EDESTADDRREQ : EINVAL);
      return -1;
    }

  if ((packet = packet_obj_new(NULL)) == NULL)
    {
      fd->error = ENOMEM;
      return -1;
    }

  /* retrieve the interface from the if index */
  if ((interface = if_get_by_index(sll->sll_ifindex)) == NULL)
    {
      fd->error = EINVAL;
      return -1;
    }

  /* compute packet total size */
  for (i = 0, n = 0; i < message->msg_iovlen; i++)
    n += message->msg_iov[i].iov_len;

  if (pv->header)
    {
      /* alloc a buffer to copy the packet content */
      if ((packet->packet = mem_alloc(n, (mem_scope_sys))) == NULL)
	{
	  net_if_obj_refdrop(interface);
	  packet_obj_refdrop(packet);
	  fd->error = ENOMEM;
	  return -1;
	}

      /* set the packet content */
      for (i = 0, n = 0; i < message->msg_iovlen; n += message->msg_iov[i].iov_len, i++)
	memcpy(packet->packet + n, message->msg_iov[i].iov_base, message->msg_iov[i].iov_len);
      nethdr = &packet->header[0];
      nethdr->data = packet->packet;
      nethdr->size = n;
      nethdr[1].data = NULL;
      packet->stage = -1;
      /* send it to the driver */
      if_sendpkt(interface, packet, ntohs(sll->sll_protocol));
    }
  else
    {
      uint8_t	*next;
      uint8_t	bcast[8];
      size_t	maclen = 8;

      assert(!dev_net_getopt(interface->dev, DEV_NET_OPT_BCAST, bcast, &maclen));

      /* is broadcast allowed */
      if (!fd->broadcast && !memcmp(bcast, sll->sll_addr, maclen))
	{
	  net_if_obj_refdrop(interface);
	  packet_obj_refdrop(packet);
	  fd->error = EINVAL;
	  return -1;
	}

      /* prepare the packet */
      if ((next = if_preparepkt(interface, packet, n, 0)) == NULL)
	{
	  net_if_obj_refdrop(interface);
	  packet_obj_refdrop(packet);
	  fd->error = ENOMEM;
	  return -1;
	}

      /* set the packet content */
      nethdr = &packet->header[packet->stage];
      nethdr->data = next;
      nethdr->size = n;
      nethdr[1].data = NULL;
      for (i = 0, n = 0; i < message->msg_iovlen; n += message->msg_iov[i].iov_len, i++)
	memcpy(next + n, message->msg_iov[i].iov_base, message->msg_iov[i].iov_len);
      packet->header[packet->stage + 1].data = NULL;
      packet->MAClen = sll->sll_halen;
      packet->tMAC = sll->sll_addr;
      packet->stage--;
      /* send to the driver */
      if_sendpkt(interface, packet, ntohs(sll->sll_protocol));
    }

  packet_obj_refdrop(packet);
  net_if_obj_refdrop(interface);

  return n;
}

/*
 * Receive some data as a message.
 */

static _RECVMSG(recvmsg_packet)
{
  struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)fd->pv;
  struct net_packet_s		*packet;
  struct sockaddr_ll		*sll;
  ssize_t			sz;
  struct net_header_s		*nethdr;

  if (flags & (MSG_OOB | MSG_ERRQUEUE))
    {
      fd->error = EOPNOTSUPP;
      return -1;
    }

  if (message == NULL)
    {
      fd->error = EINVAL;
      return -1;
    }

  /* grab a packet */
  if ((packet = socket_grab_packet(fd, flags, recv_timeout, &pv->recv_q, &pv->recv_sem)) == NULL)
    return -1;

  sll = (struct sockaddr_ll *)message->msg_name;

  /* fill the address if required */
  if (sll != NULL)
    {
      uint8_t	bcast[8];
      size_t	maclen = 8;

      if (message->msg_namelen < sizeof (struct sockaddr_ll))
	{
	  fd->error = ENOMEM;
	  packet_obj_refdrop(packet);
	  return -1;
	}

      sll->sll_family = AF_PACKET;
      sll->sll_protocol = htons(packet->proto);
      sll->sll_ifindex = packet->interface->index;
      sll->sll_hatype = packet->interface->type;
      sll->sll_halen = packet->MAClen;
      memcpy(sll->sll_addr, packet->sMAC, packet->MAClen);
      if (!memcmp(packet->interface->mac, packet->tMAC, packet->MAClen))
	sll->sll_pkttype = PACKET_HOST;
      else if (!dev_net_getopt(packet->interface->dev, DEV_NET_OPT_BCAST, bcast, &maclen) &&
	       !memcmp(bcast, packet->tMAC, maclen))
	sll->sll_pkttype = PACKET_BROADCAST;
      else
	sll->sll_pkttype = PACKET_OTHERHOST;

      message->msg_namelen = sizeof (struct sockaddr_ll);
    }

  /* copy the data */
  if (pv->header)
    {
      /* XXX code this */
      assert(0);
    }
  else
    {
      size_t	i;
      size_t	chunksz;

      nethdr = &packet->header[1];
      for (i = 0, sz = 0; i < message->msg_iovlen; i++, sz += chunksz)
	{
	  chunksz = message->msg_iov[i].iov_len;
	  if (sz + chunksz > nethdr->size)
	    {
	      chunksz = nethdr->size - sz;
	      memcpy(message->msg_iov[i].iov_base, nethdr->data + sz, chunksz);
	      sz += chunksz;
	      break;
	    }
	  else
	    memcpy(message->msg_iov[i].iov_base, nethdr->data + sz, chunksz);
	}

      if (flags & MSG_TRUNC)
	sz = nethdr->size;
    }

  /* drop the packet */
  packet_obj_refdrop(packet);

  return sz;
}

/*
 * Set a socket option value.
 */

static _SETSOCKOPT(setsockopt_packet)
{
  struct packet_mreq		*req;
  struct net_if_s		*interface;

  switch (level)
    {
      /* PF_PACKET options */
      case SOL_PACKET:
	if (optname != PACKET_ADD_MEMBERSHIP && optname != PACKET_DROP_MEMBERSHIP)
	  {
	    fd->error = ENOPROTOOPT;
	    return -1;
	  }

	if (optlen < sizeof (struct packet_mreq))
	  {
	    fd->error = EINVAL;
	    return -1;
	  }

	req = (struct packet_mreq *)optval;

	switch (req->mr_type)
	  {
	    /* enable or disable promiscuous mode */
	    case PACKET_MR_PROMISC:
	      {
		bool_t	enabled = optname == PACKET_ADD_MEMBERSHIP;

		if ((interface = if_get_by_index(req->mr_ifindex)) == NULL)
		  {
		    fd->error = EADDRNOTAVAIL;
		    return -1;
		  }
		dev_net_setopt(interface->dev, DEV_NET_OPT_PROMISC, &enabled, sizeof (bool_t));
		net_if_obj_refdrop(interface);
	      }
	      break;
	    /* other options not supported (multicast) */
	    default:
	      fd->error = ENOPROTOOPT;
	      return -1;
	  }
	break;
      /* socket options */
      case SOL_SOCKET:
	return setsockopt_socket(fd, optname, optval, optlen);
      default:
	fd->error = ENOPROTOOPT;
	return -1;
    }

  return 0;
}

/*
 * Get a socket option value.
 */

static _GETSOCKOPT(getsockopt_packet)
{
  if (level != SOL_SOCKET)
    {
      fd->error = ENOPROTOOPT;
      return -1;
    }

  return getsockopt_socket(fd, optname, optval, optlen);
}


/*
 * Stop dataflow on a socket.
 */

static _SHUTDOWN(shutdown_packet)
{
  struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)fd->pv;

  if (shutdown_socket(fd, how))
    return -1;

  /* end all the recv with errors */
  if (fd->shutdown == SHUT_RDWR || fd->shutdown == SHUT_RD)
    {
      semaphore_count_t		val;

      /* drop all waiting packets */
      packet_queue_lock_clear(&pv->recv_q);

      val = semaphore_value(&pv->recv_sem);
      while (val < 0)
	{
	  semaphore_give(&pv->recv_sem, 1);
	  val = semaphore_value(&pv->recv_sem);
	}

      if (fd->shutdown == SHUT_RDWR)
	{
	  socket_table_remove(&pf_packet, fd);
	  semaphore_destroy(&pv->recv_sem);
	  packet_queue_lock_destroy(&pv->recv_q);

	  /* free the socket */
	  mem_free(pv);
	  mem_free(fd);
	}
    }

  return 0;
}

/*
 * Following operations are not supported with PACKET sockets.
 */

static _LISTEN(listen_packet) { fd->error = EOPNOTSUPP; return -1; }
static _ACCEPT(accept_packet) { fd->error = EOPNOTSUPP; return -1; }
static _CONNECT(connect_packet) { fd->error = EOPNOTSUPP; return -1; }
static _GETPEERNAME(getpeername_packet) { fd->error = EOPNOTSUPP; return -1; }

/*
 * Socket API for PACKET sockets.
 */

const struct socket_api_s	packet_socket_dispatch =
  {
    .socket = socket_packet,
    .bind = bind_packet,
    .getsockname = getsockname_packet,
    .connect = connect_packet,
    .getpeername = getpeername_packet,
    .sendmsg = sendmsg_packet,
    .recvmsg = recvmsg_packet,
    .getsockopt = getsockopt_packet,
    .setsockopt = setsockopt_packet,
    .listen = listen_packet,
    .accept = accept_packet,
    .shutdown = shutdown_packet
  };

/*
 * Signal an incoming packet at level 2 layer.
 */

void		pf_packet_signal(struct net_if_s	*interface,
				 struct net_packet_s	*packet,
				 net_proto_id_t		protocol)
{
  uint8_t	bcast[packet->MAClen];
  bool_t	is_bcast;
  size_t	maclen = packet->MAClen;

  assert(!dev_net_getopt(interface->dev, DEV_NET_OPT_BCAST, bcast, &maclen));
  assert(maclen == packet->MAClen);
  is_bcast = !memcmp(packet->tMAC, bcast, maclen);

  /* deliver packet to all sockets matching interface and protocol id */
  CONTAINER_FOREACH(socket_table, DLIST, &pf_packet,
  {
    struct socket_packet_pv_s	*pv = (struct socket_packet_pv_s *)item->pv;

    if (item->shutdown == SHUT_RD || item->shutdown == SHUT_RDWR)
      CONTAINER_FOREACH_CONTINUE;

    if (pv->interface == 0 || pv->interface == interface->index)
      {
	if (pv->proto == protocol || pv->proto == ETH_P_ALL)
	  {
	    if (!item->broadcast && is_bcast)
	      CONTAINER_FOREACH_CONTINUE;

	    if (packet_queue_lock_pushback(&pv->recv_q, packet))
	      semaphore_give(&pv->recv_sem, 1);
	  }
      }
  });
}
