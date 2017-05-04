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

/*
 * User interface to UDP transport layer.
 *
 * XXX bind + connect => cas connect tt court, mettre en listen
 */

#include <hexo/types.h>
#include <mutek/mem_alloc.h>
#include <hexo/cpu.h>

#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/if.h>
#include <netinet/route.h>

#include <netinet/libudp.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>

#include <stdlib.h>

/*
 * The descriptors set.
 */

CONTAINER_FUNC(udp_desc, HASHLIST, static inline, udp_desc, port);
CONTAINER_KEY_FUNC(udp_desc, HASHLIST, static inline, udp_desc, port);

static udp_desc_root_t	descriptors = CONTAINER_ROOT_INITIALIZER(udp_desc, HASHLIST);

/*
 * Descriptors contructor and destructor.
 */

OBJECT_CONSTRUCTOR(udp_desc_obj)
{
  return 0;
}

OBJECT_DESTRUCTOR(udp_desc_obj)
{
}

/*
 * Create or connect a connected UDP descriptor.
 */

error_t			udp_connect(struct net_udp_desc_s	**desc,
				    struct net_udp_addr_s	*remote)
{
  struct net_route_s	*route;

  /* look for a route */
  if ((route = route_get(&remote->address)) == NULL)
    return -EHOSTUNREACH;

  /* allocate a descriptor if needed */
  if (*desc == NULL)
    {
      if ((*desc = udp_desc_obj_new(NULL)) == NULL)
	{
	  route_obj_refdrop(route);
	  return -ENOMEM;
	}
      (*desc)->bound = 0;
      (*desc)->checksum = 1;
      (*desc)->callback_error = NULL;
    }

  /* setup the connection endpoint */
  memcpy(&(*desc)->remote, remote, sizeof (struct net_udp_addr_s));
  (*desc)->route = route;
  (*desc)->connected = 1;

  return 0;
}

/*
 * Create or bind a listening UDP descriptor.
 */

error_t			udp_bind(struct net_udp_desc_s	**desc,
				 struct net_udp_addr_s	*local,
				 udp_callback_t		*callback,
				 void			*pv)
{
  struct net_udp_desc_s	*d;

  /* alloc a descriptor if needed */
  if (*desc == NULL)
    {
      if ((d = udp_desc_obj_new(NULL)) == NULL)
	return -ENOMEM;
      d->connected = 0;
      d->checksum = 1;
      d->callback_error = NULL;
    }
  else
    {
      d = *desc;

      /* if already bound */
      if (d->bound)
	{
	  /* XXX what to do ? */
	}
    }

  /* bind the socket */
  d->bound = 1;
  d->callback = callback;
  d->pv = pv;
  d->port = local->port; /* XXX check availability */
  memcpy(&d->address, &local->address, sizeof (struct net_addr_s));
  if (!udp_desc_push(&descriptors, d))
    {
      udp_desc_obj_delete(d);
      return -ENOMEM;
    }

  *desc = d;

  return 0;
}

/*
 * Send an UDP packet through a given interface.
 */

static inline bool_t udp_send_if(struct net_udp_desc_s	*desc,
				 struct net_udp_addr_s	*remote,
				 struct net_if_s	*interface,
				 struct net_proto_s	*addressing,
				 uint_fast16_t		local_port,
				 const void		*data,
				 size_t			size)
{
  struct net_packet_s	*packet;
  uint8_t		*dest;

  /* prepare the packet */
  if ((packet = packet_obj_new(NULL)) == NULL)
    return 0;
  if ((dest = udp_preparepkt(interface, addressing, packet, size, 0)) == NULL)
    {
      packet_obj_refdrop(packet);

      return 0;
    }

  /* copy data into the packet */
  memcpy(dest, data, size);

  /* setup destination address */
  memcpy(&packet->tADDR, &remote->address, sizeof (struct net_addr_s));

  /* send UDP packet */
  udp_sendpkt(interface, addressing, packet, local_port, remote->port,
	      (desc != NULL ? desc->checksum : 1));
  packet_obj_refdrop(packet);

  return 1;
}

/*
 * Send an UDP packet.
 *
 * If desc is NULL, a temporary descriptor is created, used, and removed.
 * If desc is not NULL, remote can be NULL if the socket is connected.
 */

error_t			udp_send(struct net_udp_desc_s		*desc,
				 struct net_udp_addr_s		*remote,
				 const void			*data,
				 size_t				size)
{
  struct net_if_s	*interface;
  struct net_route_s	*route = NULL;
  uint_fast16_t		local_port;
  bool_t		drop_route = 0;
  bool_t		global_bcast = 0;
  error_t		err = 0;

  /* handle global broadcast */
  if (remote != NULL)
    {
      switch (remote->address.family)
	{
	  case addr_ipv4:
	    global_bcast = (remote->address.addr.ipv4 == 0xffffffff);
	    break;
	  default:
	    assert(0);
	    /* IPV6 */
	}
    }

  /* find a route to the remote host */
  if (!global_bcast)
    {
      if (desc == NULL)
	{
	  if (remote == NULL)
	    return -EDESTADDRREQ;

	  if ((route = route_get(&remote->address)) == NULL)
	    return -EHOSTUNREACH;

	  drop_route = 1;
	}
      else
	{
	  if (remote == NULL)
	    {
	      if (!desc->connected)
		return -EDESTADDRREQ;

	      remote = &desc->remote;
	      route = desc->route;
	    }
	  else
	    {
	      if ((route = route_get(&remote->address)) == NULL)
		return -EHOSTUNREACH;

	      drop_route = 1;
	    }
	}
    }

  /* select a port */
  if (desc != NULL && desc->bound)
    local_port = desc->port;
  else
    {
      local_port = UDP_TEMP_PORT_BASE + (rand() % UDP_TEMP_PORT_RANGE);
    }

  if (global_bcast)
    {
      CONTAINER_FOREACH(net_if, HASHLIST, &net_interfaces,
      {
	interface = item;
	NET_FOREACH_PROTO(&interface->protocols, remote->address.family,
	{
	  udp_send_if(desc, remote, interface, item, local_port, data, size);
	});
      });
    }
  else
    err = !udp_send_if(desc, remote, route->interface, route->addressing, local_port, data, size);

  if (drop_route)
    route_obj_refdrop(route);

  return err;
}

/*
 * Close an UDP descriptor.
 */

void			udp_close(struct net_udp_desc_s		*desc)
{
  if (desc->bound)
    udp_desc_remove(&descriptors, desc);

  if (desc->connected)
    route_obj_refdrop(desc->route);

  udp_desc_obj_delete(desc);
}

/*
 * Signal an incoming packet. This function is called by the UDP
 * protocol module.
 */

void			libudp_signal(struct net_packet_s	*packet,
				      struct udphdr		*hdr)
{
  struct net_udp_desc_s	*desc;
  struct net_udp_addr_s	remote;
  net_port_t		port;

  /* build local address descriptor */
  port = hdr->dest;

  /* do we have a callback to handle the packet */
  for (desc = udp_desc_lookup(&descriptors, port);
       desc != NULL;
       desc = udp_desc_lookup_next(&descriptors, desc, port))
    {
      if (packet->tADDR.family == addr_ipv4)
	{
	  if (packet->tADDR.addr.ipv4 == desc->address.addr.ipv4 ||
	      desc->address.addr.ipv4 == INADDR_ANY)
	    break;
	}
      else
	assert(!"bouh");
    }

  /* port unreachable */
  if (desc == NULL)
    {
      packet->stage -= 2;

      /* this packet is destinated to no one */
      packet->source_addressing->desc->f.addressing->errormsg(packet, ERROR_PORT_UNREACHABLE);
      return;
    }

  memcpy(&remote.address, &packet->sADDR, sizeof (struct net_addr_s));
  remote.port = hdr->source;

  /* callback */
  desc->callback(desc, &remote, packet->header[packet->stage].data,
		 net_be16_load(hdr->len) - sizeof (struct udphdr), desc->pv);
}

/*
 * This function is used to clean the LibUDP.
 */

void		libudp_destroy(void)
{
  struct net_udp_desc_s	*to_remove = NULL;

  /* remove all opened descriptors */
  CONTAINER_FOREACH(udp_desc, HASHLIST, &descriptors,
  {
    /* remove previous item */
    if (to_remove != NULL)
      {
	udp_desc_remove(&descriptors, to_remove);
	udp_desc_obj_delete(to_remove);
	to_remove = NULL;
      }
    to_remove = item;
  });

  /* particular case handling */
  if (to_remove != NULL)
    {
      udp_desc_remove(&descriptors, to_remove);
      udp_desc_obj_delete(to_remove);
    }
}

/*
 * Signal an error on an UDP packet. This function is called by a
 * control protocol such as ICMP.
 */

NET_SIGNAL_ERROR(libudp_signal_error)
{
  struct net_udp_desc_s	*desc;

  /* do we have a callback to handle the error */
  for (desc = udp_desc_lookup(&descriptors, port);
       desc != NULL;
       desc = udp_desc_lookup_next(&descriptors, desc, port))
    {
      if (address->family == addr_ipv4)
	{
	  if (address->addr.ipv4 == desc->address.addr.ipv4 ||
	      desc->address.addr.ipv4 == INADDR_ANY)
	    break;
	}
      else
	assert(!"bouh");
    }

  if (desc != NULL && desc->callback_error != NULL)
    {
      desc->callback_error(desc, error, desc->pv_error);
    }
}

