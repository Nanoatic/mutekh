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
 * IP protocol version 4
 *
 */

#include <hexo/endian.h>

#include <netinet/ip.h>
#include <netinet/icmp.h>
#include <netinet/arp.h>
#include <netinet/packet.h>
#include <netinet/ether.h>
#include <netinet/protos.h>
#include <netinet/in.h>

#include <netinet/if.h>
#include <netinet/route.h>
#include <netinet/libsocket.h>

#include <stdlib.h>
#include <mutek/timer.h>
#include <mutek/printk.h>

/*
 * Fragment lists.
 */

CONTAINER_FUNC_NOLOCK(ip_packet, HASHLIST, static inline, ip_packet, id);
CONTAINER_KEY_FUNC(ip_packet, HASHLIST, static inline, ip_packet, id);

/*
 * Structures for declaring the protocol's properties & interface.
 */

const struct net_addressing_interface_s	ip_interface =
  {
    .sendpkt = ip_send,
    .matchaddr = ip_matchaddr,
    .pseudoheader_checksum = ip_pseudoheader_checksum,
    .errormsg = icmp_errormsg,
  };

const struct net_proto_desc_s	ip_protocol =
  {
    .name = "IP",
    .id = ETHERTYPE_IP,
    .pushpkt = ip_pushpkt,
    .preparepkt = ip_preparepkt,
    .initproto = ip_init,
    .destroyproto = ip_destroy,
    .f.addressing = &ip_interface,
    .pv_size = sizeof (struct net_pv_ip_s)
  };

/*
 * Initialize private data of IP module.
 */

NET_INITPROTO(ip_init)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)proto->pv;
  struct net_proto_s	*arp = va_arg(va, struct net_proto_s *);
  struct net_proto_s	*icmp = va_arg(va, struct net_proto_s *);
  uint_fast32_t		ip = va_arg(va, uint_fast32_t);
  uint_fast32_t		mask = va_arg(va, uint_fast32_t);

  assert(interface != NULL);
  assert(arp != NULL);
  assert(icmp != NULL);

  pv->interface = interface;
  pv->arp = arp;
  pv->icmp = icmp;
  pv->addr = ip;
  pv->mask = mask;
  if (ip_packet_init(&pv->fragments))
    return -1;
  pv->id_seq = 1;

  return 0;
}

/*
 * Clear IP module.
 */

NET_DESTROYPROTO(ip_destroy)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)proto->pv;
  struct ip_packet_s	*to_remove = NULL;

  /* remove all items in the reassembly table */
  CONTAINER_FOREACH(ip_packet, HASHLIST, &pv->fragments,
  {
    /* remove previous item */
    if (to_remove != NULL)
      {
	ip_packet_remove(&pv->fragments, to_remove);
	fragment_obj_delete(to_remove);
	to_remove = NULL;
      }
    to_remove = item;
  });

  /* particular case handling */
  if (to_remove != NULL)
    {
      ip_packet_remove(&pv->fragments, to_remove);
      fragment_obj_delete(to_remove);
    }

  ip_packet_destroy(&pv->fragments);
}

/*
 * Fragment object constructor.
 */

OBJECT_CONSTRUCTOR(fragment_obj)
{
  struct net_proto_s	*addressing = va_arg(ap, struct net_proto_s *);
  uint8_t		*id = va_arg(ap, uint8_t *);

  assert(addressing != NULL);
  assert(id != NULL);

  /* setup critical fields */
  obj->size = 0;
  obj->received = 0;
  obj->addressing = addressing;
  memcpy(obj->id, id, 6);
  if (packet_queue_init(&obj->packets))
    return -1;

  /* start timeout timer */
  obj->timeout.callback = ip_fragment_timeout;
  obj->timeout.pv = (void *)obj;
  obj->timeout.delay = IP_REASSEMBLY_TIMEOUT;
  if (timer_add_event(&timer_ms, &obj->timeout))
    {
      packet_queue_destroy(&obj->packets);

      return -1;
    }

#ifdef CONFIG_NETWORK_PROFILING
  netobj_new[NETWORK_PROFILING_FRAGMENT]++;
#endif

  return 0;
}

/*
 * Fragment object destructor.
 */

OBJECT_DESTRUCTOR(fragment_obj)
{
  timer_cancel_event(&obj->timeout, 0);

  packet_queue_clear(&obj->packets);
  packet_queue_destroy(&obj->packets);

#ifdef CONFIG_NETWORK_PROFILING
  netobj_del[NETWORK_PROFILING_FRAGMENT]++;
#endif
}

/*
 * Does an address need to be routed ?
 */

static inline	uint_fast8_t	ip_delivery(struct net_if_s	*interface,
					    struct net_proto_s	*ip,
					    uint32_t		addr)
{
  struct net_pv_ip_s		*pv = (struct net_pv_ip_s *)ip->pv;

  /* masked destination address must be equal to masked local address */
  if ((addr & pv->mask) == (pv->addr & pv->mask) || addr == 0xffffffff)
    return IP_DELIVERY_DIRECT;
  else
    return IP_DELIVERY_INDIRECT;
}

/*
 * Receive fragments and try to reassemble a packet.
 */

static inline bool_t	ip_fragment_pushpkt(struct net_proto_s	*ip,
					    struct net_packet_s	*packet,
					    struct iphdr	*hdr)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)ip->pv;
  struct ip_packet_s	*p;
  struct net_header_s	*nethdr;
  uint8_t		id[6];
  uint_fast16_t		offs;
  uint_fast16_t		fragment;
  uint_fast16_t		datasz;
  uint_fast16_t		total;

  /* the unique identifier of the packet is the concatenation of the
     source address and the packet id */
  memcpy(id, &packet->sADDR.addr.ipv4, 4);
  memcpy(id + 4, &hdr->id, 2);

  /* extract some useful fields */
  fragment = net_be16_load(hdr->fragment);
  offs = (fragment & IP_FRAG_MASK) * 8;
  datasz = net_be16_load(hdr->tot_len) - hdr->ihl * 4;

  net_debug("fragment %d offs %d size %d\n", hdr->id, offs, datasz);

  /* do we already received packet with same id ? */
  if ((p = ip_packet_lookup(&pv->fragments, id)) == NULL)
    {
      /* initialize the reassembly structure */
      if ((p = fragment_obj_new(NULL, ip, id)) == NULL)
	return 0;

      if (!ip_packet_push(&pv->fragments, p))
	{
	  fragment_obj_delete(p);
	  return 0;
	}
    }

  p->received += datasz;

  /* try to determine the total size */
  if (!(fragment & IP_FLAG_MF))
    p->size = offs + datasz;

  total = p->size;

  if (total)
    net_debug("received %d out of %d\n", p->received, total);

  if (total && total == p->received)
    {
      struct net_packet_s	*frag;
      uint_fast16_t		headers_len;
      uint8_t			*data;
      uint8_t			*ptr;
      size_t			sizes[NETWORK_MAX_STAGES];
      uint_fast8_t		i;

      /* disable timeout & remove the fragment structure */
      ip_packet_remove(&pv->fragments, p);
      timer_cancel_event(&p->timeout, 0);

      /* we received the whole packet, reassemble now */
      nethdr = &packet->header[packet->stage];
      headers_len = (nethdr->data - packet->header[0].data);

      /* allocate a packet large enough */
      if ((data = mem_alloc(total + headers_len + 3, (mem_scope_sys))) == NULL)
	{
	  /* memory exhausted, clear the packet */
	  fragment_obj_delete(p);

	  return 0;
	}

      /* copy previous headers (ethernet, ip, etc.) */
      net_debug("copying headers : %d-%d\n", 0, headers_len);
      memcpy(data, packet->packet, headers_len);

      /* update final packet flags & total length */
      net_16_store(((struct iphdr *)nethdr[-1].data)->fragment, 0);
      net_16_store(((struct iphdr *)nethdr[-1].data)->tot_len, total);

      /* copy current packet to its position */
#ifdef CONFIG_NETWORK_AUTOALIGN
      ptr = (uint8_t *)ALIGN_VALUE_UP((uintptr_t)(data + headers_len), 4);
#else
      ptr = data + headers_len;
#endif

      /* copy the first part of the packet */
      net_debug("copying packet : %d-%d\n", offs, offs + datasz);
      memcpy(ptr + offs, nethdr->data, datasz);

      /* release current packet data */
      mem_free(packet->packet);

      /* replace by reassembling packet */
      packet->packet = data;
      data = ptr;

      /* loop through previously received packets and reassemble them */
      while ((frag = packet_queue_pop(&p->packets)))
	{
	  /* determine destination offset */
	  nethdr = &frag->header[packet->stage];
	  offs = (net_be16_load(((struct iphdr *)nethdr[-1].data)->fragment) & IP_FRAG_MASK) * 8;

	  net_debug("copying packet : %d-%d\n", offs, offs + nethdr->size);

	  /* check for overflow */
	  if (offs >= total)
	    {
	      /* XXX error on fragment must be reported by 1st fragment */
	      /* report the error */
	      pv->icmp->desc->f.control->errormsg(frag, ERROR_BAD_HEADER);

	      /* delete all the packets */
	      packet_obj_refdrop(frag);
	      fragment_obj_delete(p);

	      return 0;
	    }

	  /* copy data to their final place */
	  memcpy(data + offs, nethdr->data, nethdr->size);

	  /* release our reference to the packet */
	  packet_obj_refdrop(frag);
	}

      /* release memory */
      fragment_obj_delete(p);

      /* update nethdr. first, we need to determine the real size of
	 each chunks. then we must update the pointers to the
	 different headers into the reassembled packets. to finish, we
	 update the size of each subpackets. */
      nethdr = packet->header;
      for (i = 0; i < packet->stage; i++)
	sizes[i] = nethdr[i + 1].data - nethdr[i].data;
      nethdr[0].data = packet->packet;
      for (i = 1; i < packet->stage; i++)
	nethdr[i].data = nethdr[i - 1].data + sizes[i - 1];
      for (i = 0; i < packet->stage; i++)
	nethdr[i].size = nethdr[i].size - datasz + total;
      nethdr[i].data = data;
      nethdr[i].size = total;

      return 1;
    }

  /* otherwise, this is just a fragment */
  packet_queue_push(&p->packets, packet);
  return 0;
}

/*
 * Fragment reassembly timeout.
 */

TIMER_CALLBACK(ip_fragment_timeout)
{
  struct ip_packet_s	*p = (struct ip_packet_s *)pv;
  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)p->addressing->pv;
  struct net_packet_s	*packet;

  /* remove the fragment structure from the waiting list */
  ip_packet_remove(&pv_ip->fragments, p);

  /* report the error */
  packet = packet_queue_pop(&p->packets);
  if (packet != NULL)
    {
      packet->stage--;
      pv_ip->icmp->desc->f.control->errormsg(packet, ERROR_FRAGMENT_TIMEOUT);
      packet_obj_refdrop(packet);
    }

  /* delete all the fragments */
  fragment_obj_delete(p);
}

/*
 * Receive incoming IP packets.
 */

NET_PUSHPKT(ip_pushpkt)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)protocol->pv;
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct iphdr		aligned;
#endif
  struct iphdr		*hdr;
  struct net_header_s	*nethdr;
  net_proto_id_t	proto;
  struct net_proto_s	*p;
  uint_fast16_t		computed_check;
  uint_fast16_t		fragment;
  uint_fast16_t		tot_len;
  bool_t		on_subnet;
  bool_t		is_broadcast;

  assert(interface != NULL);
  assert(packet != NULL);
  assert(protocol != NULL);

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct iphdr *)nethdr->data;

  assert(hdr != NULL);

  /* align the packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint32_t)))
    {
      memcpy(&aligned, hdr, sizeof (struct iphdr));
      hdr = &aligned;
    }
#endif

  /* check IP version */
  if (hdr->version != 4)
    return;

  /* update packet info */
  IPV4_ADDR_SET(packet->sADDR, net_be32_load(hdr->saddr));
  IPV4_ADDR_SET(packet->tADDR, net_be32_load(hdr->daddr));
  packet->source_addressing = protocol;

  net_debug("%s: incoming for %P\n", interface->name, &packet->tADDR.addr.ipv4, 4);

  /* determine target info */
  on_subnet = (packet->tADDR.addr.ipv4 & pv->mask) == (pv->addr & pv->mask);
  is_broadcast = (packet->tADDR.addr.ipv4 & ~pv->mask) == ~pv->mask;

  /* is the packet really for me ? */
  if (packet->tADDR.addr.ipv4 != pv->addr)
    {
#ifdef CONFIG_NETWORK_FORWARDING
      /* if the packet is not on the same subnet (and is not broadcast) */
      if (!on_subnet && packet->tADDR.addr.ipv4 != 0xffffffff)
	{
	  struct net_route_s	*route_entry = NULL;

	  if (is_broadcast)
	    return ;

	  /* is there a route for this address ? */
	  if ((route_entry = route_get(&packet->tADDR)) != NULL)
	    {
	      /* route the packet */
	      net_debug("routing to host %P\n", &packet->tADDR.addr.ipv4, 4);
	      ip_route(packet, route_entry);
	      route_obj_refdrop(route_entry);
	    }
	  else
	    {
	      /* network unreachable */
	      net_debug("no route to host %P\n", &packet->tADDR.addr.ipv4, 4);
	      pv->icmp->desc->f.control->errormsg(packet, ERROR_NET_UNREACHABLE);
	    }

	  return ;
	}
#endif
      if (!is_broadcast)
	return ;
    }

  /* verify checksum */
  computed_check = packet_checksum((uint8_t *)hdr, hdr->ihl * 4);

  /* incorrect packet */
  if (computed_check != 0xffff)
    {
      net_debug("IP: Rejected incorrect packet %x\n", computed_check);
      return;
    }

  /* fix the packet size (if the interface has a minimum transfert
     unit).  this must be done because some other modules in the
     protocol stack use the size specified in the header array, and if
     the network device set a bad size, we must adjust it */
  tot_len = net_be16_load(hdr->tot_len);
  if (nethdr->size != tot_len)
    {
      int_fast8_t	i;
      uint_fast16_t	delta = nethdr->size - tot_len;

      for (i = packet->stage; i >= 0; i--)
	packet->header[i].size -= delta;
    }

  /* next stage */
  if (!nethdr[1].data)
    {
      uint_fast8_t	hdr_len = hdr->ihl * 4;

      nethdr[1].data = nethdr->data + hdr_len;
      nethdr[1].size = nethdr->size - hdr_len;
    }

  /* increment stage */
  packet->stage++;

  /* is the packet fragmented ? */
  fragment = net_be16_load(hdr->fragment);
  if ((fragment & IP_FLAG_MF) || (fragment & IP_FRAG_MASK))
    {
      /* add fragment */
      if (!ip_fragment_pushpkt(protocol, packet, hdr))
	return;	/* abord the packet, the last fragment will unblock it */

      /* once the packet is reassembled, update its header pointer */
      nethdr = &packet->header[packet->stage - 1];
      hdr = (struct iphdr *)nethdr->data;
    }

  /* dispatch to the matching protocol */
  proto = hdr->protocol;
#ifdef CONFIG_NETWORK_SOCKET_RAW
  sock_raw_signal(interface, protocol, packet, proto);
#endif
  if ((p = net_protos_lookup(&interface->protocols, proto)))
    {
      p->desc->pushpkt(interface, packet, p);
      net_proto_obj_refdrop(p);
    }
  else
    pv->icmp->desc->f.control->errormsg(packet, ERROR_PROTO_UNREACHABLE);
}

/*
 * Prepare a new IP packet.
 */

NET_PREPAREPKT(ip_preparepkt)
{
  struct net_header_s	*nethdr;
  uint8_t		*next;

  assert(interface != NULL);
  assert(packet != NULL);

#ifdef CONFIG_NETWORK_AUTOALIGN
  if ((next = if_preparepkt(interface, packet, 20 + size, 4 + max_padding - 1)) == NULL)
    return NULL;
  next = ALIGN_ADDRESS_UP(next, 4);
#else
  if ((next = if_preparepkt(interface, packet, 20 + size, 0)) == NULL)
    return NULL;
#endif

  nethdr = &packet->header[packet->stage];
  nethdr->data = next;
  nethdr->size = 20 + size;

  packet->stage++;

  return next + 20;
}

/*
 * Fragment sending.
 */

static inline bool_t	 ip_send_fragment(struct net_proto_s	*ip,
					  struct net_if_s	*interface,
					  struct iphdr		*hdr,
					  struct net_packet_s	*packet,
					  struct net_route_s	*route_entry,
					  uint_fast16_t		shift,
					  uint_fast16_t		offs,
					  size_t		fragsz,
					  uint_fast8_t		last)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)ip->pv;
  struct net_packet_s	*frag;
  struct net_header_s	*nethdr;
  struct iphdr		*hdr_frag;
  uint8_t		*dest;
  uint_fast8_t		i;
  uint_fast32_t		check;

  /* prepare a new (child) IP packet */
  if ((frag = packet_obj_new(NULL)) == NULL)
    return 0;
  packet_obj_refnew(packet);
  frag->parent = packet;
  frag->header[frag->stage + 1].data = NULL;
  if ((dest = ip_preparepkt(interface, frag, 0, 0)) == NULL)
    {
      packet_obj_refdrop(frag);
      return 0;
    }

  net_debug("sending fragment %d-%d\n", shift + offs, shift + offs + fragsz);

  /* fill the data */
  frag->header[frag->stage].data = packet->header[packet->stage + 1].data + offs;
  frag->header[frag->stage].size = fragsz;

  /* and update the size fields */
  for (i = 0; i < frag->stage; i++)
    {
      frag->header[i].size += fragsz;
    }
  frag->stage--;

  /* copy IP header */
  nethdr = &frag->header[frag->stage];
  hdr_frag = (struct iphdr *)nethdr->data;
  memcpy(hdr_frag, hdr, hdr->ihl * 4);

  /* setup fragment specific fields */
  net_be16_store(hdr_frag->fragment, (last ? 0 : IP_FLAG_MF) | ((shift + offs) / 8));
  net_be16_store(hdr_frag->tot_len, nethdr->size);
  /* finalize checksum computation */
  check = net_16_load(hdr_frag->check);
  check += net_16_load(hdr_frag->fragment);
  check += net_16_load(hdr_frag->tot_len);
  check = check + (check >> 16);
  net_16_store(hdr_frag->check, ~check);

  /* fill the source and destination address fields */
  IPV4_ADDR_SET(frag->sADDR, net_be32_load(hdr_frag->saddr));
  IPV4_ADDR_SET(frag->tADDR, net_be32_load(hdr_frag->daddr));

  /* need to route ? */
  if (route_entry != NULL)
    {
#ifdef CONFIG_NETWORK_FORWARDING
      ip_route(frag, route_entry);
      return 1;
#else
      assert(route_entry == NULL);
#endif
    }
  else
    {
      /* no route: IP -> MAC translation */
      if (!(frag->tMAC = arp_get_mac(ip, pv->arp, frag, frag->tADDR.addr.ipv4)))
	return 1;
    }

  /* send the packet to the driver */
  frag->stage--;
  if_sendpkt(interface, frag, ETHERTYPE_IP);
  packet_obj_refdrop(frag);

  return 1;
}

/*
 * Send an IP packet.
 */

NET_SENDPKT(ip_send)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)protocol->pv;
  struct iphdr		*hdr;
  struct net_header_s	*nethdr;
  struct net_route_s	*route_entry;
  uint_fast16_t		total;

  assert(interface != NULL);
  assert(packet != NULL);
  assert(protocol != NULL);

  packet->source_addressing = protocol;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct iphdr *)nethdr->data;
  assert(hdr != NULL);

  /* start filling common IP header fields */
  hdr->version = 4;
  hdr->ihl = 5;
  hdr->tos = 0;
  hdr->ttl = IPDEFTTL;
  hdr->protocol = proto;
  net_be32_store(hdr->saddr, pv->addr);
  net_be32_store(hdr->daddr, IPV4_ADDR_GET(packet->tADDR));

  net_debug("%s: outgoing from %P\n", interface->name, &pv->addr, 4);

  /* need fragmentation ? */
  total = nethdr[1].size;
  if (total > interface->mtu - 20)
    {
      uint_fast16_t		offs;
      uint_fast16_t		fragsz;
      uint8_t			*data;
      bool_t			error = 0;

      data = nethdr[1].data;
      offs = 0;
      fragsz = (interface->mtu - 20) & ~7;
      /* choose a random identifier */
      net_be16_store(hdr->id, pv->id_seq++);
      /* compute the (partial) checksum of the header. as the header will be
	 completed during next step, the checksum will be incrementaly
	 updated */
      net_16_store(hdr->tot_len, 0);
      net_16_store(hdr->fragment, 0);
      net_16_store(hdr->check, 0);
      net_16_store(hdr->check, packet_checksum((uint8_t *)hdr, 20));

      /* need to route ? */
      IPV4_ADDR_SET(packet->sADDR, pv->addr);
      if (ip_delivery(interface, protocol, packet->tADDR.addr.ipv4) == IP_DELIVERY_INDIRECT)
	{
#ifdef CONFIG_NETWORK_FORWARDING
	  if ((route_entry = route_get(&packet->tADDR)) == NULL)
	    {
#endif
	      /* network unreachable */
	      pv->icmp->desc->f.control->errormsg(packet, ERROR_NET_UNREACHABLE);

	      return ;
#ifdef CONFIG_NETWORK_FORWARDING
	    }
#endif
	}
      else
	route_entry = NULL;

      /* send the middle fragments */
      while (!error && offs + fragsz < total)
	{
	  error = !ip_send_fragment(protocol, interface, hdr, packet, route_entry, 0, offs, fragsz, 0);

	  offs += fragsz;
	}

      /* last fragment */
      if (!error)
	ip_send_fragment(protocol, interface, hdr, packet, route_entry, 0, offs, total - offs, 1);

#ifdef CONFIG_NETWORK_FORWARDING
      if (route_entry != NULL)
	route_obj_refdrop(route_entry);
#endif

      return ;
    }

  /* for the non fragmented packets */
  /* finish the IP header */
  net_be16_store(hdr->tot_len, nethdr->size);
  net_16_store(hdr->id, pv->id_seq++);
  net_16_store(hdr->fragment, 0);
  net_16_store(hdr->check, 0);
  /* checksum */
  net_16_store(hdr->check, ~packet_checksum((uint8_t *)hdr, hdr->ihl * 4));

  /* need to route ? */
  IPV4_ADDR_SET(packet->sADDR, pv->addr);
  if (ip_delivery(interface, protocol, packet->tADDR.addr.ipv4) == IP_DELIVERY_INDIRECT)
    {
#ifdef CONFIG_NETWORK_FORWARDING
      if ((route_entry = route_get(&packet->tADDR)) != NULL)
	{
	  ip_route(packet, route_entry);
	  route_obj_refdrop(route_entry);
	}
      else
	{
#endif
	  /* network unreachable */
	  pv->icmp->desc->f.control->errormsg(packet, ERROR_NET_UNREACHABLE);
#ifdef CONFIG_NETWORK_FORWARDING
	}
#endif

      return ;
    }
  else
    {
      /* no route: IP -> MAC translation */
      if (!(packet->tMAC = arp_get_mac(protocol, pv->arp, packet, packet->tADDR.addr.ipv4)))
	return ;
    }

  /* send the packet to the driver */
  packet->stage--;
  if_sendpkt(interface, packet, ETHERTYPE_IP);
}

#ifdef CONFIG_NETWORK_FORWARDING
/*
 * Route a packet.
 */

void		ip_route(struct net_packet_s	*packet,
			 struct net_route_s	*route)
{
  struct net_pv_ip_s	*pv;
  struct iphdr		*hdr;
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct iphdr		aligned;
#endif
  struct net_header_s	*nethdr;
  struct net_if_s	*interface;
  uint_fast32_t		router;
  uint_fast16_t		total;
  uint_fast16_t		check;

  interface = route->interface;
  pv = (struct net_pv_ip_s *)route->addressing->pv;

  /* get the packet header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct iphdr *)nethdr->data;

  /* check for timeout */
  if (hdr->ttl == 1)
    {
      pv->icmp->desc->f.control->errormsg(packet, ERROR_TIMEOUT);

      return ;
    }

  /* align the packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint32_t)))
    {
      memcpy(&aligned, hdr, sizeof (struct iphdr));
      hdr = &aligned;
    }
#endif

  if (!nethdr[1].data)
    {
      uint_fast8_t	hdr_len = hdr->ihl * 4;
      nethdr[1].data = nethdr->data + hdr_len;
      nethdr[1].size = net_be16_load(hdr->tot_len) - hdr_len;
    }

  /* check for fragmentation */
  total = nethdr[1].size;
  if (total > interface->mtu - hdr->ihl * 4)
    {
      uint_fast16_t		offs;
      uint_fast16_t		shift;
      uint_fast16_t		fragsz;
      uint8_t			*data;
      uint_fast16_t		fragment;
      bool_t			error = 0;

      net_debug("routing with fragmentation\n");

      /* if the Don't Fragment flag is set, destroy the packet */
      fragment = net_be16_load(hdr->fragment);
      if (fragment & IP_FLAG_DF)
	{
	  /* report the error */
	  pv->icmp->desc->f.control->errormsg(packet, ERROR_CANNOT_FRAGMENT, route);

	  return;
	}

      data = nethdr[1].data;
      offs = 0;
      shift = (fragment & IP_FRAG_MASK) * 8;
      fragsz = (interface->mtu - 20) & ~7;

      /* adjust packet checksum, erasing the fields tot_len & fragment */
      check = ~net_16_load(hdr->check) - net_16_load(hdr->tot_len) - net_16_load(hdr->fragment) + 1;
      net_16_store(hdr->check, check + (check >> 16));

      /* send the middle fragments */
      while (!error && offs + fragsz < total)
	{
	  error = !ip_send_fragment(route->addressing, interface, hdr, packet, route, shift, offs, fragsz, 0);

	  offs += fragsz;
	}

      /* last fragment */
      if (!error)
	ip_send_fragment(route->addressing, interface, hdr, packet, route, shift, offs, total - offs, !(fragment & IP_FLAG_MF));

      return ;
    }

  /* recompute checksum (in fact, incrementally adjust it) */
  hdr->ttl--;
  check = net_16_load(hdr->check) + 1;
  net_16_store(hdr->check, check + (check >> 16));

  /* direct or indirect delivery */
  if (route->is_routed)
    {
      net_debug("local delivery on %s\n", interface->name);

      if (!(packet->tMAC = arp_get_mac(route->addressing , pv->arp, packet, packet->tADDR.addr.ipv4)))
	return ;
    }
  else
    {
      /* get router address */
      router = IPV4_ADDR_GET(route->router);
      net_debug("remote delivery thru %P on %s\n", &router, 4, interface->name);

      if (!(packet->tMAC = arp_get_mac(route->addressing, pv->arp, packet, router)))
	return ;
    }

  /* send the packet */
  packet->stage--;
  if_sendpkt(interface, packet, ETHERTYPE_IP);
}
#endif

/*
 * Address matching function.
 */

NET_MATCHADDR(ip_matchaddr)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)protocol->pv;
  uint_fast32_t		A;
  uint_fast32_t		B;
  uint_fast32_t		M;

  if (a != NULL)
    A = IPV4_ADDR_GET(*a);
  else
    A = pv->addr;

  if (b != NULL)
    B = IPV4_ADDR_GET(*b);
  else
    B = pv->addr;

  if (mask != NULL)
    M = IPV4_ADDR_GET(*mask);
  else /* no mask set, equality test */
    return A == B;

  /* masked equality test */
  return (A & M) == (B & M);
}

/*
 * Compute checksum of the IP pseudo-header (needed by higher protocols).
 */

NET_PSEUDOHEADER_CHECKSUM(ip_pseudoheader_checksum)
{
  struct ip_pseudoheader_s	hdr;

  if (addressing == NULL)
    hdr.source = endian_be32(IPV4_ADDR_GET(packet->sADDR));
  else
    hdr.source = endian_be32(((struct net_pv_ip_s *)addressing->pv)->addr);
  hdr.dest = endian_be32(IPV4_ADDR_GET(packet->tADDR));
  hdr.zero = 0;
  hdr.type = proto;
  hdr.size = endian_be16(size);

  return packet_checksum(&hdr, sizeof (hdr));
}
