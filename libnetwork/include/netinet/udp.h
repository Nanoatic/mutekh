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

#ifndef NETINET_UDP_H_
#define NETINET_UDP_H_

#ifndef CONFIG_NETWORK_UDP
# warning UDP support is not enabled in configuration file
#endif

#include <hexo/types.h>

/* The UDP packet header */
struct		udphdr
{
  uint16_t	source;
  uint16_t	dest;
  uint16_t	len;
  uint16_t	check;
} __attribute__ ((packed));


#include <netinet/packet.h>
#include <netinet/protos.h>

/*
 * UDP functions
 */

NET_PUSHPKT(udp_pushpkt);
inline uint8_t	*udp_preparepkt(struct net_if_s		*interface,
				struct net_proto_s	*addressing,
				struct net_packet_s	*packet,
				size_t			size,
				size_t			max_padding);
inline void	udp_sendpkt(struct net_if_s	*interface,
			    struct net_proto_s	*addressing,
			    struct net_packet_s	*packet,
			    uint_fast16_t	source_port,
			    uint_fast16_t	dest_port,
			    bool_t		compute_checksum);

/*
 * UDP protocol descriptor.
 */

extern const struct net_proto_desc_s	udp_protocol;

#endif

