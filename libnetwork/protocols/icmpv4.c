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
 * ICMP protocol for IPv4
 *
 */

#include <netinet/icmp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/packet.h>
#include <netinet/protos.h>
#ifdef CONFIG_NETWORK_UDP
#include <netinet/libudp.h>
#endif
#ifdef CONFIG_NETWORK_TCP
#include <netinet/libtcp.h>
#include <netinet/tcp.h>
#endif

#include <netinet/if.h>
#include <mutek/printk.h>

/*
 * Structures for declaring the protocol's properties & interface.
 */

const struct net_control_interface_s	icmp_interface =
  {
    .errormsg = icmp_errormsg
  };

const struct net_proto_desc_s	icmp_protocol =
  {
    .name = "ICMP",
    .id = IPPROTO_ICMP,
    .pushpkt = icmp_pushpkt,
    .preparepkt = icmp_preparepkt,
    .initproto = NULL,
    .destroyproto = NULL,
    .f.control = &icmp_interface,
    .pv_size = 0
  };

/*
 * Reply an echo request.
 */

static inline void	icmp_echo(struct net_if_s	*interface,
				  struct net_proto_s	*addressing,
				  struct net_proto_s	*icmp,
				  struct net_packet_s	*packet)
{
  struct icmphdr	*hdr;
  struct net_header_s	*nethdr;
  uint_fast32_t		xchg;

  packet = packet_dup(packet);

  net_debug("Pong\n");

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct icmphdr *)nethdr->data;

  nethdr[1].data = NULL;

  /* fill the echo */
  hdr->type = 0;
  hdr->code = 3;
  net_16_store(hdr->checksum, 0);

  /* compute checksum XXX incremental */
  net_16_store(hdr->checksum, ~packet_checksum(nethdr->data, nethdr->size));

  /* target IP */
  xchg = packet->tADDR.addr.ipv4;
  packet->tADDR.addr.ipv4 = packet->sADDR.addr.ipv4;
  packet->sADDR.addr.ipv4 = xchg;

  packet->stage--;
  /* send the packet to IP */
  addressing->desc->f.addressing->sendpkt(interface, packet, addressing, IPPROTO_ICMP);
  packet_obj_refdrop(packet);
}


/*
 * Receive incoming ICMP packets.
 */

NET_PUSHPKT(icmp_pushpkt)
{
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct icmphdr	aligned;
  struct iphdr		aligned_ip;
#endif
  struct icmphdr	*hdr;
  struct net_header_s	*nethdr;
  uint_fast16_t		computed_check;
  struct net_proto_s	*addressing = packet->source_addressing;
  struct iphdr		*hdr_ip;
  struct net_addr_s	address;
  uint_fast16_t		port;
  net_proto_id_t	proto;
  net_signal_error_t	*signal_error;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct icmphdr *)nethdr->data;

  /* align the packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint32_t)))
    {
      memcpy(&aligned, hdr, sizeof (struct icmphdr));
      hdr = &aligned;
    }
#endif

  /* verify checksum */
  computed_check = packet_checksum(nethdr->data, nethdr->size);

  /* incorrect packet */
  if (computed_check != 0xffff)
    {
      net_debug("ICMP: Rejected incorrect packet %x\n", computed_check);
      return;
    }

  /* special case: the ping */
  if (hdr->type == 8 && hdr->code == 0)
    {
      net_debug("Ping\n");
      icmp_echo(interface, addressing, protocol, packet);
      return ;
    }

  /* isolate the erroneous packet: destination ip/proto/port */
  if (nethdr[1].data == NULL)
    {
      nethdr[1].data = nethdr->data + sizeof (struct icmphdr);
      nethdr[1].size = nethdr->size - sizeof (struct icmphdr);
    }

  hdr_ip = (struct iphdr *)nethdr[1].data;
  assert(hdr_ip != NULL);

  /* align the ip packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr_ip, sizeof (uint32_t)))
    {
      memcpy(&aligned_ip, hdr_ip, sizeof (struct iphdr));
      hdr_ip = &aligned_ip;
    }
#endif

  if (nethdr[2].data == NULL)
    {
      uint_fast8_t	hdr_len = hdr_ip->ihl * 4;

      nethdr[2].data = nethdr[1].data + hdr_len;
      nethdr[2].size = nethdr[1].size - hdr_len;
    }

  /* determine source IP and protocol */
  IPV4_ADDR_SET(address, net_be32_load(hdr_ip->saddr));
  proto = hdr_ip->protocol;

  switch (proto)
    {
#ifdef CONFIG_NETWORK_UDP
      case IPPROTO_UDP:
	{
	  struct udphdr	*hdr_udp;
# ifdef CONFIG_NETWORK_AUTOALIGN
	  struct udphdr	aligned_udp;
# endif

	  hdr_udp = (struct udphdr *)nethdr[2].data;
	  assert(hdr_udp != NULL);

	  /* align on 16 bits if needed */
# ifdef CONFIG_NETWORK_AUTOALIGN
	  if (!IS_ALIGNED(hdr_udp, sizeof (uint16_t)))
	    {
	      memcpy(&aligned_udp, hdr_udp, sizeof (struct udphdr));
	      hdr_udp = &aligned_udp;
	    }
# endif

	  port = net_be16_load(hdr_udp->source);
	  signal_error = libudp_signal_error;
	}
	break;
#endif
#ifdef CONFIG_NETWORK_TCP
      case IPPROTO_TCP:
	{
	  struct tcphdr	*hdr_tcp;
# ifdef CONFIG_NETWORK_AUTOALIGN
	  struct tcphdr	aligned_tcp;
# endif

	  hdr_tcp = (struct tcphdr *)nethdr[2].data;
	  assert(hdr_tcp != NULL);

	  /* align on 32 bits if needed */
# ifdef CONFIG_NETWORK_AUTOALIGN
	  if (!IS_ALIGNED(hdr_tcp, sizeof (uint32_t)))
	    {
	      memcpy(&aligned_tcp, hdr_tcp, sizeof (struct tcphdr));
	      hdr_tcp = &aligned_tcp;
	    }
# endif

	  port = net_be16_load(hdr_tcp->th_sport);
	  signal_error = libtcp_signal_error;
	}
	break;
#endif
      default:
	return; /* unknown protocol, we can't do anything */
    }

  /* at this point, address is the source address of the erroneous
     packet, proto is either UDP or TCP, and port is the source port
     of the connection that caused the error */

  /* analyse the error */
  switch (hdr->type)
    {
      case 3:
	switch (hdr->code)
	  {
	    case 2:	/* protocol unavailable */
	      signal_error(ERROR_PROTO_UNREACHABLE, &address, port);
	      break;
	    case 3:	/* port unreachable */
	      signal_error(ERROR_PORT_UNREACHABLE, &address, port);
	      break;
	    default:	/* other: host unreachable, net unreachable... */
	      signal_error(ERROR_HOST_UNREACHABLE, &address, port);
	      break;
	  }
	break;
      case 4:	/* source quench */
	signal_error(ERROR_CONGESTION, &address, port);
	break;
      case 11:	/* timeout */
	signal_error(ERROR_TIMEOUT, &address, port);
	break;
      default:	/* other kind of error */
	signal_error(ERROR_UNKNOWN, &address, port);
	break;
    }
}

/*
 * Prepare ICMP part of a packet.
 */

NET_PREPAREPKT(icmp_preparepkt)
{
  struct net_header_s	*nethdr;
  uint8_t		*next;

#ifdef CONFIG_NETWORK_AUTOALIGN
  if ((next = ip_preparepkt(interface, packet, sizeof (struct icmphdr) + size, 4)) == NULL)
    return NULL;
  next = ALIGN_ADDRESS_UP(next, 4);
#else
  if ((next = ip_preparepkt(interface, packet, sizeof (struct icmphdr) + size, 0)) == NULL)
    return NULL;
#endif

  nethdr = &packet->header[packet->stage];
  nethdr->data = next;
  nethdr->size = sizeof (struct icmphdr) + size;

  nethdr[1].data = NULL;

  return next + sizeof (struct icmphdr);
}

/*
 * Report error with ICMP.
 */

NET_ERRORMSG(icmp_errormsg)
{
  struct net_proto_s	*addressing = erroneous->source_addressing;
  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)addressing->pv;
  struct net_if_s	*interface = (struct net_if_s *)pv_ip->interface;
  struct net_packet_s	*packet;
  struct icmphdr	*hdr;
  struct iphdr		*hdr_err;
  struct net_header_s	*nethdr;
  uint8_t		*dest;
  uint_fast16_t		offs;
  uint_fast16_t		size;
  va_list		va;

  va_start(va, error);

  net_debug("ICMP error %d\n", error);

  /* get a pointer to the erroneous packet */
  nethdr = &erroneous->header[erroneous->stage];
  hdr_err = (struct iphdr *)nethdr->data;

  /* we must not generate error on ICMP packets (except ping) */
  if (hdr_err->protocol == IPPROTO_ICMP)
    {
      struct icmphdr	*hdr_icmp;

      hdr_icmp = (struct icmphdr *)nethdr[1].data;
      if (hdr_icmp->type != 0 && hdr_icmp->type != 8)
	return ;
    }

  offs = hdr_err->ihl * 4;
  /* next stage */
  if (nethdr[1].data == NULL)
    {
      nethdr[1].data = nethdr->data + offs;
      nethdr[1].size = nethdr->size - offs;
    }

  size = nethdr[1].size;
  if (size > 8)
    size = 8;

  /* prepare the packet */
  if ((packet = packet_obj_new(NULL)) == NULL)
    return ;
  if ((dest = icmp_preparepkt(interface, packet, offs + size, 0)) == NULL)
    {
      packet_obj_refdrop(packet);
      return ;
    }

  memcpy(&packet->tADDR, &erroneous->sADDR, sizeof (struct net_addr_s));

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct icmphdr *)nethdr->data;

  net_32_store(hdr->un.gateway, 0);

  /* fill the type and code */
  switch (error)
    {
      case ERROR_NET_UNREACHABLE:
	hdr->type = 3;
	hdr->code = 0;
	break;
      case ERROR_HOST_UNREACHABLE:
	hdr->type = 3;
	hdr->code = 1;
	break;
      case ERROR_PROTO_UNREACHABLE:
	hdr->type = 3;
	hdr->code = 2;
	break;
      case ERROR_PORT_UNREACHABLE:
	hdr->type = 3;
	hdr->code = 3;
	break;
      case ERROR_CANNOT_FRAGMENT:
	{
	  struct net_route_s	*route = va_arg(va, struct net_route_s *);

	  hdr->type = 3;
	  hdr->code = 4;
	  net_be16_store(hdr->un.frag.mtu, route->interface->mtu);
	}
	break;
      case ERROR_NET_DENIED:
	hdr->type = 3;
	hdr->code = 11;
	break;
      case ERROR_HOST_DENIED:
	hdr->type = 3;
	hdr->code = 10;
	break;
      case ERROR_CONGESTION:
	hdr->type = 4;
	hdr->code = 0;
	break;
      case ERROR_TIMEOUT:
	hdr->type = 11;
	hdr->code = 0;
	break;
      case ERROR_FRAGMENT_TIMEOUT:
	hdr->type = 11;
	hdr->code = 1;
	break;
      case ERROR_BAD_HEADER:
	hdr->type = 12;
	hdr->code = 0;
	break;
      default:
	assert(0);
	break;
    }

  /* copy the head of the packet that caused the error */
  memcpy(dest, hdr_err, offs);
  memcpy(dest + offs, erroneous->header[erroneous->stage + 1].data, size);

  net_16_store(hdr->checksum, 0);
  /* compute checksum */
  net_16_store(hdr->checksum, ~packet_checksum(nethdr->data, nethdr->size));

  packet->stage--;
  /* send the packet to the interface */
  addressing->desc->f.addressing->sendpkt(interface, packet, addressing, IPPROTO_ICMP);
  packet_obj_refdrop(packet);

  va_end(va);
}

