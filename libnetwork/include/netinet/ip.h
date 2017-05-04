/* Copyright (C) 1991,92,93,95,96,97,98,99,2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef NETINET_IP_H_
#define NETINET_IP_H_

#include <hexo/types.h>
#include <hexo/endian.h>

/* Type Of Service flags */
#define IPTOS_TOS_MASK		0x1E
#define IPTOS_TOS(tos)		((tos)&IPTOS_TOS_MASK)
#define	IPTOS_LOWDELAY		0x10
#define	IPTOS_THROUGHPUT	0x08
#define	IPTOS_RELIABILITY	0x04
#define	IPTOS_MINCOST		0x02

#define IPTOS_PREC_MASK		0xE0
#define IPTOS_PREC(tos)		((tos)&IPTOS_PREC_MASK)
#define IPTOS_PREC_NETCONTROL           0xe0
#define IPTOS_PREC_INTERNETCONTROL      0xc0
#define IPTOS_PREC_CRITIC_ECP           0xa0
#define IPTOS_PREC_FLASHOVERRIDE        0x80
#define IPTOS_PREC_FLASH                0x60
#define IPTOS_PREC_IMMEDIATE            0x40
#define IPTOS_PREC_PRIORITY             0x20
#define IPTOS_PREC_ROUTINE              0x00

/* IP options */
#define IPOPT_COPY		0x80
#define IPOPT_CLASS_MASK	0x60
#define IPOPT_NUMBER_MASK	0x1f

#define	IPOPT_COPIED(o)		((o)&IPOPT_COPY)
#define	IPOPT_CLASS(o)		((o)&IPOPT_CLASS_MASK)
#define	IPOPT_NUMBER(o)		((o)&IPOPT_NUMBER_MASK)

#define	IPOPT_CONTROL		0x00
#define	IPOPT_RESERVED1		0x20
#define	IPOPT_MEASUREMENT	0x40
#define	IPOPT_RESERVED2		0x60

#define IPOPT_END	(0 |IPOPT_CONTROL)
#define IPOPT_NOOP	(1 |IPOPT_CONTROL)
#define IPOPT_SEC	(2 |IPOPT_CONTROL|IPOPT_COPY)
#define IPOPT_LSRR	(3 |IPOPT_CONTROL|IPOPT_COPY)
#define IPOPT_TIMESTAMP	(4 |IPOPT_MEASUREMENT)
#define IPOPT_RR	(7 |IPOPT_CONTROL)
#define IPOPT_SID	(8 |IPOPT_CONTROL|IPOPT_COPY)
#define IPOPT_SSRR	(9 |IPOPT_CONTROL|IPOPT_COPY)
#define IPOPT_RA	(20|IPOPT_CONTROL|IPOPT_COPY)

#define IPOPT_OPTVAL 0
#define IPOPT_OLEN   1
#define IPOPT_OFFSET 2
#define IPOPT_MINOFF 4
#define MAX_IPOPTLEN 40
#define IPOPT_NOP IPOPT_NOOP
#define IPOPT_EOL IPOPT_END
#define IPOPT_TS  IPOPT_TIMESTAMP

#define	IPOPT_TS_TSONLY		0		/* timestamps only */
#define	IPOPT_TS_TSANDADDR	1		/* timestamps and addresses */
#define	IPOPT_TS_PRESPEC	3		/* specified modules only */

/* Misc */
#define IPVERSION	4
#define MAXTTL		255
#define IPDEFTTL	64

/*
 * IP flags.
 */

#define IP_FLAG_MF	0x2000
#define IP_FLAG_DF	0x4000
#define IP_FLAG_RS	0x8000
#define IP_FRAG_MASK	0x1fff

/* IP packet header */
struct iphdr {
  ENDIAN_BITFIELD(uint8_t	version:4,
		  uint8_t	ihl:4);
  uint8_t	tos;
  uint16_t	tot_len;
  uint16_t	id;
  uint16_t	fragment;
  uint8_t	ttl;
  uint8_t	protocol;
  uint16_t	check;
  uint32_t	saddr;
  uint32_t	daddr;
} __attribute__((packed));

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

#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/route.h>

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/object_simple.h>

#include <mutek/timer.h>

#define IP_DELIVERY_DIRECT	0
#define IP_DELIVERY_INDIRECT	1

/*
 * Reassembly timeout.
 */

#define IP_REASSEMBLY_TIMEOUT	10000; /* 10 seconds */

/*
 * IP pseudo header (for upper layer checksum computation)
 */

struct		ip_pseudoheader_s
{
  uint32_t	source;
  uint32_t	dest;
  uint8_t	zero;
  uint8_t	type;
  uint16_t	size;
} __attribute__((packed));

/*
 * Fragmentation structures.
 */

OBJECT_TYPE(fragment_obj, SIMPLE, struct ip_packet_s);

struct					ip_packet_s
{
  uint8_t				id[6];
  uint_fast16_t				size;
  uint_fast16_t				received;
  struct net_proto_s			*addressing;
  struct timer_event_s			timeout;
  packet_queue_root_t			packets;

  fragment_obj_entry_t			obj_entry;
  CONTAINER_ENTRY_TYPE(HASHLIST)	list_entry;
};

OBJECT_CONSTRUCTOR(fragment_obj);
OBJECT_DESTRUCTOR(fragment_obj);
OBJECT_FUNC(fragment_obj, SIMPLE, static inline, fragment_obj, obj_entry);

/*
 * Fragments list.
 */

#define CONTAINER_LOCK_ip_packet	HEXO_SPIN
CONTAINER_TYPE(ip_packet, HASHLIST, struct ip_packet_s, list_entry, 64);
CONTAINER_KEY_TYPE(ip_packet, PTR, BLOB, id, 6);

/*
 * IP private data.
 */

struct			net_pv_ip_s
{
  struct net_if_s	*interface;
  struct net_proto_s	*arp;
  struct net_proto_s	*icmp;
  uint_fast32_t		addr;
  uint_fast32_t		mask;
  ip_packet_root_t	fragments;
  uint_fast32_t		id_seq;
};

/*
 * IP functions
 */

NET_INITPROTO(ip_init);
NET_DESTROYPROTO(ip_destroy);
NET_PUSHPKT(ip_pushpkt);
NET_PREPAREPKT(ip_preparepkt);
NET_SENDPKT(ip_send);
NET_MATCHADDR(ip_matchaddr);
NET_PSEUDOHEADER_CHECKSUM(ip_pseudoheader_checksum);
void		ip_route(struct net_packet_s	*packet,
			 struct net_route_s	*route);
TIMER_CALLBACK(ip_fragment_timeout);

extern const struct net_proto_desc_s	ip_protocol;

#endif

