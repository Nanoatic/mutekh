/* Definitions for Address Resolution Protocol.
   Copyright (C) 1997, 1999, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

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

/* Based on the 4.4BSD and Linux version of this file.  */

#ifndef NETINET_ARP_H_
#define NETINET_ARP_H_

#include <hexo/types.h>
#include <netinet/ether.h>

/* ARP protocol opcodes. */
#define	ARPOP_REQUEST	1		/* ARP request.  */
#define	ARPOP_REPLY	2		/* ARP reply.  */
#define	ARPOP_RREQUEST	3		/* RARP request.  */
#define	ARPOP_RREPLY	4		/* RARP reply.  */
#define	ARPOP_InREQUEST	8		/* InARP request.  */
#define	ARPOP_InREPLY	9		/* InARP reply.  */
#define	ARPOP_NAK	10		/* (ATM)ARP NAK.  */

/* This structure defines an ethernet arp header.  */
struct		arphdr
{
  uint16_t	ar_hrd;		/* Format of hardware address.  */
  uint16_t	ar_pro;		/* Format of protocol address.  */
  uint8_t	ar_hln;		/* Length of hardware address.  */
  uint8_t	ar_pln;		/* Length of protocol address.  */
  uint16_t	ar_op;		/* ARP opcode (command).  */
} __attribute__ ((packed));

/* ARP protocol HARDWARE identifiers. */
#define ARPHRD_NETROM	0		/* From KA9Q: NET/ROM pseudo. */
#define ARPHRD_ETHER 	1		/* Ethernet 10/100Mbps.  */
#define	ARPHRD_EETHER	2		/* Experimental Ethernet.  */
#define	ARPHRD_AX25	3		/* AX.25 Level 2.  */
#define	ARPHRD_PRONET	4		/* PROnet token ring.  */
#define	ARPHRD_CHAOS	5		/* Chaosnet.  */
#define	ARPHRD_IEEE802	6		/* IEEE 802.2 Ethernet/TR/TB.  */
#define	ARPHRD_ARCNET	7		/* ARCnet.  */
#define	ARPHRD_APPLETLK	8		/* APPLEtalk.  */
#define	ARPHRD_DLCI	15		/* Frame Relay DLCI.  */
#define	ARPHRD_ATM	19		/* ATM.  */
#define	ARPHRD_METRICOM	23		/* Metricom STRIP (new IANA id).  */
#define ARPHRD_IEEE1394	24		/* IEEE 1394 IPv4 - RFC 2734.  */
#define ARPHRD_EUI64		27		/* EUI-64.  */
#define ARPHRD_INFINIBAND	32		/* InfiniBand.  */

/*
 * Ethernet Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving internet addresses.  Field names used correspond to
 * RFC 826.
 */
struct		ether_arp
{
  struct arphdr	ea_hdr;		/* fixed-size header */
  uint8_t	arp_sha[ETH_ALEN];	/* sender hardware address */
  uint32_t	arp_spa;		/* sender protocol address */
  uint8_t	arp_tha[ETH_ALEN];	/* target hardware address */
  uint32_t	arp_tpa;		/* target protocol address */
} __attribute__ ((packed));

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

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/object_simple.h>

#include <mutek/timer.h>

/*
 * Request timeout and retry count
 */

#define ARP_REQUEST_TIMEOUT	2000 /* 2 seconds */
#define ARP_ENTRY_TIMEOUT	120000 /* 2 minutes */
#define ARP_STALE_TIMEOUT	240000 /* 4 minutes */
#define ARP_MAX_RETRIES		3
#define RARP_TIMEOUT		30 /* 30 seconds */

/*
 * Flags
 */

#define ARP_TABLE_DEFAULT	0
#define ARP_TABLE_IN_PROGRESS	1
#define ARP_TABLE_NO_UPDATE	2

/*
 * Resolution structure.
 */

struct					arp_resolution_s
{
  packet_queue_root_t			wait;
  uint_fast8_t				retry;
  struct net_if_s			*interface;
  struct net_proto_s			*addressing;
  struct net_proto_s			*arp;
  struct timer_event_s			timeout;
};

/*
 * ARP table entry.
 */

OBJECT_TYPE(arp_entry_obj, SIMPLE, struct arp_entry_s);

struct					arp_entry_s
{
  uint_fast32_t				ip;
  uint8_t				mac[ETH_ALEN];
  bool_t				valid;
  timer_delay_t				timestamp;
  struct arp_resolution_s		*resolution;

  arp_entry_obj_entry_t			obj_entry;
  CONTAINER_ENTRY_TYPE(HASHLIST)	list_entry;
};

OBJECT_CONSTRUCTOR(arp_entry_obj);
OBJECT_DESTRUCTOR(arp_entry_obj);
OBJECT_FUNC(arp_entry_obj, SIMPLE, static inline, arp_entry_obj, obj_entry);

/*
 * ARP table types.
 */

#define CONTAINER_LOCK_arp_table	HEXO_SPIN
CONTAINER_TYPE(arp_table, HASHLIST, struct arp_entry_s, list_entry, 64);
CONTAINER_KEY_TYPE(arp_table, PTR, SCALAR, ip);

/*
 * ARP private data.
 */

struct			net_pv_arp_s
{
  arp_table_root_t	table;
  struct timer_event_s	stale_timeout;
};

/*
 * RARP private data.
 */

struct			net_pv_rarp_s
{
  struct net_proto_s	*ip;
};

/*
 * Prototypes of ARP and RARP functions.
 */

NET_INITPROTO(arp_init);
NET_DESTROYPROTO(arp_destroy);
NET_PUSHPKT(arp_pushpkt);
NET_PREPAREPKT(arp_preparepkt);
struct arp_entry_s	*arp_update_table(struct net_proto_s	*arp,
					  uint32_t		ip,
					  uint8_t		*mac,
					  uint_fast8_t		flags);
const uint8_t		*arp_get_mac(struct net_proto_s		*addressing,
				     struct net_proto_s		*arp,
				     struct net_packet_s	*packet,
				     uint_fast32_t		ip);


NET_INITPROTO(rarp_init);
NET_PUSHPKT(rarp_pushpkt);
NET_PREPAREPKT(rarp_preparepkt);
void	rarp_request(struct net_if_s	*interface,
		     struct net_proto_s	*rarp,
		     uint8_t		*mac);
TIMER_CALLBACK(arp_timeout);
TIMER_CALLBACK(arp_stale_timeout);

/*
 * ARP & RARP protocol descriptors.
 */

extern const struct net_proto_desc_s	arp_protocol;
extern const struct net_proto_desc_s	rarp_protocol;

#endif

