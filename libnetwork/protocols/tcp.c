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
 * Transmission Control Protocol
 */

#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/packet.h>
#include <netinet/protos.h>

#include <netinet/if.h>

#include <netinet/libtcp.h>

#include <stdio.h>

/*
 * Structures for declaring the protocol's properties & interface.
 */

const struct net_proto_desc_s	tcp_protocol =
  {
    .name = "TCP",
    .id = IPPROTO_TCP,
    .pushpkt = tcp_pushpkt,
    .preparepkt = NULL,
    .initproto = NULL,
    .pv_size = 0
  };

/*
 * Receive incoming TCP packets.
 */

NET_PUSHPKT(tcp_pushpkt)
{
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct tcphdr		aligned;
#endif
  struct tcphdr		*hdr;
  struct net_header_s	*nethdr;
  uint_fast8_t		flags;
  uint32_t		computed_check;
  struct net_proto_s	*addressing = packet->source_addressing;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct tcphdr *)nethdr->data;

  /* align the packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint32_t)))
    {
      memcpy(&aligned, hdr, hdr->th_off * 4);
      hdr = &aligned;
    }
#endif

  /* verify checksum */
  computed_check = addressing->desc->f.addressing->pseudoheader_checksum(NULL, packet, IPPROTO_TCP, nethdr->size);
  computed_check += packet_checksum(nethdr->data, nethdr->size);
  computed_check = (computed_check & 0xffff) + (computed_check >> 16);

  /* incorrect packet */
  if (computed_check != 0xffff)
    {
      net_debug("TCP: Rejected incorrect packet %x\n", computed_check);
      return;
    }

  flags = hdr->th_flags;

  libtcp_push(packet, hdr);
}

/*
 * Prepare a TCP packet.
 */

uint8_t			*tcp_preparepkt(struct net_if_s		*interface,
					struct net_proto_s	*addressing,
					struct net_packet_s	*packet,
					size_t			size,
					size_t			max_padding)
{
  struct net_header_s	*nethdr;
  uint8_t		*next;

#ifdef CONFIG_NETWORK_AUTOALIGN
  if ((next = addressing->desc->preparepkt(interface, packet, sizeof (struct tcphdr) + size, 2)) == NULL)
    return NULL;
  next = ALIGN_ADDRESS_UP(next, 4);
#else
  if ((next = addressing->desc->preparepkt(interface, packet, sizeof (struct tcphdr) + size, 0)) == NULL)
    return NULL;
#endif

  nethdr = &packet->header[packet->stage];
  nethdr->data = next;
  nethdr->size = sizeof (struct tcphdr) + size;

  nethdr[1].data = NULL;

  return next + sizeof (struct tcphdr);
}

/*
 * Send a TCP control packet (without data).
 */

void	tcp_send_controlpkt(struct net_tcp_session_s	*session,
			    uint_fast8_t		operation)
{
  struct net_packet_s	*packet;
  struct net_proto_s	*addressing = session->route->addressing;
  struct net_if_s	*interface = session->route->interface;
  struct net_header_s	*nethdr;
  struct tcphdr		*hdr;
  uint_fast32_t		check;

  if ((packet = packet_obj_new(NULL)) == NULL)
    return;

  /* prepare the packet */
  if (tcp_preparepkt(interface, addressing, packet, (operation == TCP_SYN || operation == TCP_SYN_ACK) ? 4 : 0, 0) == NULL)
    {
      /* out of memory */
      packet_obj_refdrop(packet);
      return;
    }
  nethdr = &packet->header[packet->stage];
  hdr = (struct tcphdr *)nethdr->data;
  hdr->th_x2 = 0;

  /* setup the targeted address */
  memcpy(&packet->tADDR, &session->remote.address,
	 sizeof (struct net_addr_s));
  net_16_store(hdr->th_sport, session->local.port);
  net_16_store(hdr->th_dport, session->remote.port);

  /* fill the packet header */
  switch (operation)
    {
      /* connection opening */
      case TCP_SYN:
	{
	  uint32_t	*mss;

	  hdr->th_flags = TH_SYN;
	  net_be32_store(hdr->th_seq, session->curr_seq);
	  net_be32_store(hdr->th_ack, 0);
	  net_be16_store(hdr->th_win, session->send_win);
	  hdr->th_urp = 0;
	  hdr->th_off = 6;
	  /* add MSS option */
	  mss = (uint32_t *)(hdr + 1);
	  net_be32_store(*mss, (2 << 24) | (4 << 16) | session->recv_mss);
	}
	break;
      /* acknowledgment of connection opening */
      case TCP_SYN_ACK:
	{
	  uint32_t	*mss;

	  hdr->th_flags = TH_SYN | TH_ACK;
	  net_be32_store(hdr->th_seq, session->curr_seq);
	  net_be32_store(hdr->th_ack, session->to_ack);
	  net_be16_store(hdr->th_win, session->send_win);
	  hdr->th_urp = 0;
	  hdr->th_off = 6;
	  /* add MSS option */
	  mss = (uint32_t *)(hdr + 1);
	  net_be32_store(*mss, (2 << 24) | (4 << 16) | session->recv_mss);
	}
	break;
      /* simple acknowlegment of received data when no data to send */
      case TCP_ACK:
	hdr->th_flags = TH_ACK;
	net_be32_store(hdr->th_seq, session->curr_seq);
	net_be32_store(hdr->th_ack, session->to_ack);
	net_be16_store(hdr->th_win, session->send_win);
	hdr->th_urp = 0;
	hdr->th_off = 5;
	break;
      /* request for closing connection */
      case TCP_FIN:
	hdr->th_flags = TH_FIN | TH_ACK;
	net_be32_store(hdr->th_seq, session->curr_seq);
	net_be32_store(hdr->th_ack, session->to_ack);
	net_be16_store(hdr->th_win, session->send_win);
	hdr->th_urp = 0;
	hdr->th_off = 5;
	break;
      default:
	assert(0);
	break;
    }

  /* checksum */
  check = addressing->desc->f.addressing->pseudoheader_checksum(addressing, packet, IPPROTO_TCP, hdr->th_off * 4);
  net_16_store(hdr->th_sum, 0);
  check += packet_checksum((uint8_t *)hdr, hdr->th_off * 4);
  check = check + (check >> 16);
  net_16_store(hdr->th_sum, ~check);

  packet->stage--;
  session->acked = session->to_ack;
  /* send the packet to IP */
  addressing->desc->f.addressing->sendpkt(interface, packet, addressing, IPPROTO_TCP);
  packet_obj_refdrop(packet);
}

/*
 * Send a TCP data packet.
 */

void	tcp_send_datapkt(struct net_tcp_session_s	*session,
			 void				*data,
			 size_t				size,
			 uint_fast8_t			flags)
{
  struct net_packet_s	*packet;
  struct net_proto_s	*addressing = session->route->addressing;
  struct net_if_s	*interface = session->route->interface;
  uint8_t		*dest;
  struct net_header_s	*nethdr;
  struct tcphdr		*hdr;
  uint_fast32_t		check;

  if ((packet = packet_obj_new(NULL)) == NULL)
    return;

  /* prepare the packet */
  if ((dest = tcp_preparepkt(interface, addressing, packet, size, 0)) == NULL)
    {
      /* no more memory */
      packet_obj_refdrop(packet);
      return;
    }
  nethdr = &packet->header[packet->stage];
  hdr = (struct tcphdr *)nethdr->data;
  hdr->th_x2 = 0;

  /* setup the targeted address */
  memcpy(&packet->tADDR, &session->remote.address, sizeof (struct net_addr_s));
  net_16_store(hdr->th_sport, session->local.port);
  net_16_store(hdr->th_dport, session->remote.port);

  /* fill the packet header */
  hdr->th_flags = TH_ACK | flags;
  net_be32_store(hdr->th_seq, session->curr_seq);
  net_be32_store(hdr->th_ack, session->to_ack);
  net_be16_store(hdr->th_win, session->send_win);
  hdr->th_urp = 0;
  hdr->th_off = 5;

  /* copy the data */
  memcpy(dest, data, size);

  /* checksum */
  check = addressing->desc->f.addressing->pseudoheader_checksum(addressing, packet, IPPROTO_TCP, nethdr->size);
  net_16_store(hdr->th_sum, 0);
  check += packet_checksum((uint8_t *)hdr, nethdr->size);
  check = check + (check >> 16);
  net_16_store(hdr->th_sum, ~check);

  packet->stage--;
  session->acked = session->to_ack;
  /* send the packet to IP */
  addressing->desc->f.addressing->sendpkt(interface, packet, addressing, IPPROTO_TCP);
  packet_obj_refdrop(packet);
}

