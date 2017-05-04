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
 * UDP protocol XXX a merger w/ libudp ?
 *
 */

#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/packet.h>
#include <netinet/protos.h>

#include <netinet/if.h>

#include <netinet/libudp.h>

#include <mutek/printk.h>

/*
 * Structures for declaring the protocol's properties & interface.
 */

const struct net_proto_desc_s	udp_protocol =
  {
    .name = "UDP",
    .id = IPPROTO_UDP,
    .pushpkt = udp_pushpkt,
    .preparepkt = NULL,
    .initproto = NULL,
    .pv_size = 0
  };

/*
 * Receive incoming UDP datagrams.
 */

NET_PUSHPKT(udp_pushpkt)
{
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct udphdr		aligned;
#endif
  struct udphdr		*hdr;
  struct net_header_s	*nethdr;
  uint16_t		check;
  uint_fast16_t		len;
  struct net_proto_s	*addressing = packet->source_addressing;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct udphdr *)nethdr->data;

  /* align the packet on 16 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint16_t)))
    {
      memcpy(&aligned, hdr, sizeof (struct udphdr));
      hdr = &aligned;
    }
#endif

  len = net_be16_load(hdr->len);
  check = net_16_load(hdr->check);

  /* checksum is optional */
  if (check)
    {
      uint32_t		computed_check;

      computed_check = addressing->desc->f.addressing->pseudoheader_checksum(NULL, packet, IPPROTO_UDP, len);
      computed_check += packet_checksum(nethdr->data, len);
      computed_check = (computed_check & 0xffff) + (computed_check >> 16);

      /* incorrect packet */
      if (computed_check != 0xffff)
	{
	  net_debug("UDP: Rejected incorrect packet %x %x\n", check, ~computed_check);
	  return;
	}
    }

  /* next stage */
  if (!nethdr[1].data)
    {
      nethdr[1].data = nethdr->data + sizeof (struct udphdr);
      nethdr[1].size = len - sizeof (struct udphdr);
    }
  packet->stage++;

  /* signal the incoming valid packet to the LibUDP */
  libudp_signal(packet, hdr);
}

/*
 * Prepare UDP packet.
 */

inline uint8_t		*udp_preparepkt(struct net_if_s		*interface,
					struct net_proto_s	*addressing,
					struct net_packet_s	*packet,
					size_t			size,
					size_t			max_padding)
{
  struct net_header_s	*nethdr;
  uint8_t		*next;

#ifdef CONFIG_NETWORK_AUTOALIGN
  if ((next = addressing->desc->preparepkt(interface, packet, sizeof (struct udphdr) + size, 4)) == NULL)
    return NULL;
  next = ALIGN_ADDRESS_UP(next, 4);
#else
  if ((next = addressing->desc->preparepkt(interface, packet, sizeof (struct udphdr) + size, 0)) == NULL)
    return NULL;
#endif

  nethdr = &packet->header[packet->stage];
  nethdr->data = next;
  nethdr->size = sizeof (struct udphdr) + size;

  nethdr[1].data = NULL;

  return next + sizeof (struct udphdr);
}

/*
 * Send a packet.
 */

inline void	udp_sendpkt(struct net_if_s	*interface,
			    struct net_proto_s	*addressing,
			    struct net_packet_s	*packet,
			    uint_fast16_t	source_port,
			    uint_fast16_t	dest_port,
			    bool_t		compute_checksum)
{
  struct udphdr		*hdr;
  struct net_header_s	*nethdr;
  uint_fast32_t		computed_check;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct udphdr *)nethdr->data;

  /* fill the header */
  net_16_store(hdr->source, source_port);
  net_16_store(hdr->dest, dest_port);
  net_be16_store(hdr->len, nethdr->size);

  /* compute checksum */
  net_16_store(hdr->check, 0);
  if (compute_checksum)
    {
      computed_check = addressing->desc->f.addressing->pseudoheader_checksum(addressing, packet, IPPROTO_UDP, nethdr->size);
      computed_check += packet_checksum(nethdr->data, nethdr->size);
      computed_check = (computed_check & 0xffff) + (computed_check >> 16);
      net_16_store(hdr->check, ~computed_check);
    }

  packet->stage--;
  /* send the packet to IP */
  addressing->desc->f.addressing->sendpkt(interface, packet, addressing, IPPROTO_UDP);
}

