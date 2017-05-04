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
 * ARP for IPv4
 *
 */

#include <netinet/arp.h>
#include <netinet/ip.h>
#include <netinet/packet.h>
#include <netinet/protos.h>

#include <netinet/if.h>

#include <mutek/timer.h>
#include <mutek/printk.h>

/*
 * ARP table functions.
 */

CONTAINER_FUNC_NOLOCK(arp_table, HASHLIST, static inline, arp_table, ip);
CONTAINER_KEY_FUNC(arp_table, HASHLIST, static inline, arp_table, ip);

/*
 * Structures for declaring the protocol's properties & interface.
 */

const struct net_proto_desc_s	arp_protocol =
  {
    .name = "ARP",
    .id = ETHERTYPE_ARP,
    .pushpkt = arp_pushpkt,
    .preparepkt = arp_preparepkt,
    .initproto = arp_init,
    .destroyproto = arp_destroy,
    .pv_size = sizeof (struct net_pv_arp_s),
  };

/*
 * Init ARP.
 */

NET_INITPROTO(arp_init)
{
  struct net_pv_arp_s	*pv = (struct net_pv_arp_s *)proto->pv;

  if (arp_table_init(&pv->table))
    return -1;
  pv->stale_timeout.callback = arp_stale_timeout;
  pv->stale_timeout.pv = pv;
  pv->stale_timeout.delay = ARP_STALE_TIMEOUT;
  if (timer_add_event(&timer_ms, &pv->stale_timeout))
    {
      arp_table_destroy(&pv->table);
      return -1;
    }
  return 0;
}

/*
 * Destroy ARP.
 */

NET_DESTROYPROTO(arp_destroy)
{
  struct net_pv_arp_s	*pv = (struct net_pv_arp_s *)proto->pv;
  struct arp_entry_s	*to_remove = NULL;

  /* clear timeout */
  timer_cancel_event(&pv->stale_timeout, 0);
  /* remove all items in the arp table */
  CONTAINER_FOREACH(arp_table, HASHLIST, &pv->table,
  {
    /* remove previous item */
    if (to_remove != NULL)
      {
	arp_table_remove(&pv->table, to_remove);
	arp_entry_obj_delete(to_remove);
	to_remove = NULL;
      }
    to_remove = item;
  });

  /* particular case handling */
  if (to_remove != NULL)
    {
      arp_table_remove(&pv->table, to_remove);
      arp_entry_obj_delete(to_remove);
    }
  arp_table_destroy(&pv->table);
}

/*
 * ARP entry constructor.
 */

OBJECT_CONSTRUCTOR(arp_entry_obj)
{
  uint_fast32_t		ip = va_arg(ap, uint_fast32_t);

  assert(ip != 0 && ip != 0xffffffff && ip != 0x7f000001);

  obj->ip = ip;

#ifdef CONFIG_NETWORK_PROFILING
  netobj_new[NETWORK_PROFILING_ARP_ENTRY]++;
#endif

  return 0;
}

/*
 * ARP entry destructor.
 */

OBJECT_DESTRUCTOR(arp_entry_obj)
{
  struct arp_resolution_s	*res = obj->resolution;

  if (res != NULL)
    {
      timer_cancel_event(&res->timeout, 0);
      packet_queue_clear(&res->wait);
      packet_queue_destroy(&res->wait);

      mem_free(obj->resolution);
    }

#ifdef CONFIG_NETWORK_PROFILING
  netobj_del[NETWORK_PROFILING_ARP_ENTRY]++;
#endif
}

/*
 * This function request a MAC address given an IP address.
 */

static inline void	arp_request(struct net_if_s	*interface,
				    struct net_proto_s	*ip,
				    uint_fast32_t	address)
{
  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)ip->pv;
  struct ether_arp	*hdr;
  struct net_packet_s	*packet;
  struct net_header_s	*nethdr;

  if ((packet = packet_obj_new(NULL)) == NULL)
    return ;

  if (arp_preparepkt(interface, packet, 0, 0) == NULL)
    {
      packet_obj_refdrop(packet);
      return ;
    }

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct ether_arp *)nethdr->data;

  /* fill the request */
  net_be16_store(hdr->ea_hdr.ar_hrd, ARPHRD_ETHER);
  net_be16_store(hdr->ea_hdr.ar_pro, ETHERTYPE_IP);
  hdr->ea_hdr.ar_hln = ETH_ALEN;
  hdr->ea_hdr.ar_pln = 4;
  net_be16_store(hdr->ea_hdr.ar_op, ARPOP_REQUEST);
  memcpy(hdr->arp_sha, packet->sMAC, ETH_ALEN);
  net_be32_store(hdr->arp_spa, pv_ip->addr);
  memset(hdr->arp_tha, 0xff, ETH_ALEN);
  net_be32_store(hdr->arp_tpa, address);
  packet->tMAC = hdr->arp_tha;

  /* send the packet to the interface */
  packet->stage--;
  if_sendpkt(interface, packet, ETHERTYPE_ARP);
  packet_obj_refdrop(packet);
}

/*
 * Send an ARP reply
 */

static inline void	arp_reply(struct net_if_s		*interface,
				  struct net_proto_s		*ip,
				  struct net_packet_s		*packet)
{
  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)ip->pv;
  struct ether_arp	*hdr;
  struct net_header_s	*nethdr;

  packet = packet_dup(packet);

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct ether_arp *)nethdr->data;

  /* fill the reply */
  net_be16_store(hdr->ea_hdr.ar_op, ARPOP_REPLY);
  memcpy(hdr->arp_tha, hdr->arp_sha, ETH_ALEN);
  net_32_store(hdr->arp_tpa, net_32_load(hdr->arp_spa));
  memcpy(hdr->arp_sha, interface->mac, ETH_ALEN);
  net_be32_store(hdr->arp_spa, pv_ip->addr);

  packet->sMAC = hdr->arp_sha;
  packet->tMAC = hdr->arp_tha;

  /* send the packet to the interface */
  packet->stage--;
  if_sendpkt(interface, packet, ETHERTYPE_ARP);
  packet_obj_refdrop(packet);
}

/*
 * This function decodes an incoming ARP packet.
 */

NET_PUSHPKT(arp_pushpkt)
{
#ifdef CONFIG_NETWORK_AUTOALIGN
  struct ether_arp	aligned;
#endif
  struct ether_arp	*hdr;
  struct net_header_s	*nethdr;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  hdr = (struct ether_arp *)nethdr->data;

  /* align the packet on 32 bits if necessary */
#ifdef CONFIG_NETWORK_AUTOALIGN
  if (!IS_ALIGNED(hdr, sizeof (uint32_t)))
    {
      memcpy(&aligned, hdr, sizeof (struct ether_arp));
      hdr = &aligned;
    }
#endif

  /* check header */
  if (net_be16_load(hdr->ea_hdr.ar_hrd) != ARPHRD_ETHER ||
      net_be16_load(hdr->ea_hdr.ar_pro) != ETHERTYPE_IP)
    return ;

  /* ARP message */
  switch (net_be16_load(hdr->ea_hdr.ar_op))
    {
      case ARPOP_REQUEST:
	{
	  uint_fast32_t		requested;
	  struct net_addr_s	addr;

	  requested = net_be32_load(hdr->arp_tpa);
	  net_debug("ARP Req %P\n", &requested, 4);
	  IPV4_ADDR_SET(addr, requested);

	  /* loop thru IP modules bound to interface */
	  NET_FOREACH_PROTO(&interface->protocols, ETHERTYPE_IP,
	  {
	    if (item->desc->f.addressing->matchaddr(item, NULL, &addr, NULL))
	      {
		/* force adding the entry */
		arp_update_table(protocol, net_be32_load(hdr->arp_spa),
				 hdr->arp_sha, ARP_TABLE_DEFAULT);
		arp_reply(interface, item, packet);
		requested = 0;
		NET_FOREACH_PROTO_BREAK;
	      }
	  });

	  /* try to update the cache */
	  if (requested)
	    arp_update_table(protocol, net_be32_load(hdr->arp_spa),
			     hdr->arp_sha, ARP_TABLE_NO_UPDATE);

	  break;
	}
      case ARPOP_REPLY:
	net_debug("ARP Reply\n");
	/* try to update the cache */
	arp_update_table(protocol, net_be32_load(hdr->arp_spa), hdr->arp_sha, ARP_TABLE_DEFAULT);
	break;
      default:
	break;
    }
}

/*
 * This function prepares an ARP packet.
 */

NET_PREPAREPKT(arp_preparepkt)
{
  struct net_header_s	*nethdr;
  uint8_t		*next;

#ifdef CONFIG_NETWORK_AUTOALIGN
  if ((next = if_preparepkt(interface, packet, sizeof (struct ether_arp), 4)) == NULL)
    return NULL;
  next = ALIGN_ADDRESS_UP(next, 4);
#else
  if ((next = if_preparepkt(interface, packet, sizeof (struct ether_arp), 0)) == NULL)
    return NULL;
#endif

  nethdr = &packet->header[packet->stage];
  nethdr->data = next;
  nethdr->size = sizeof (struct ether_arp);

  nethdr[1].data = NULL;

  return next + sizeof (struct ether_arp);
}

/*
 * Update table entry.
 */

struct arp_entry_s	*arp_update_table(struct net_proto_s	*arp,
					  uint_fast32_t		ip,
					  uint8_t		*mac,
					  uint_fast8_t		flags)
{
  struct net_pv_arp_s	*pv = (struct net_pv_arp_s *)arp->pv;
  struct arp_entry_s	*arp_entry;

  assert(arp != NULL);
  assert(ip != 0);

  if ((arp_entry = arp_table_lookup(&pv->table, ip)) != NULL)
    {
      /* if we must not update */
      if (flags & ARP_TABLE_NO_UPDATE)
	return NULL;
      /* we are able to update it */
    }
  else
    {
      /* otherwise, allocate a new entry */
      if ((arp_entry = arp_entry_obj_new(NULL, ip)) == NULL)
	return NULL;
      arp_entry->resolution = NULL;
      if (!arp_table_push(&pv->table, arp_entry))
	{
	  arp_entry_obj_delete(arp_entry);
	  return NULL;
	}
    }

  /* fill the significant fields */
  arp_entry->valid = !(flags & ARP_TABLE_IN_PROGRESS);
  if (arp_entry->valid)
    {
      memcpy(arp_entry->mac, mac, ETH_ALEN);
      net_debug("Added entry %P for %P\n", mac, ETH_ALEN, &ip, 4);
      if (arp_entry->resolution != NULL)
	{
	  struct net_packet_s	*waiting;

	  /* cancel timeout */
	  timer_cancel_event(&arp_entry->resolution->timeout, 0);

	  /* send waiting packets */
	  while ((waiting = packet_queue_pop(&arp_entry->resolution->wait)))
	    {
	      waiting->tMAC = arp_entry->mac;
	      waiting->stage--;

	      /* send the packet */
	      if_sendpkt(arp_entry->resolution->interface, waiting, ETHERTYPE_IP);
	      packet_obj_refdrop(waiting);
	    }

	  /* clear the resolution structure */
	  packet_queue_destroy(&arp_entry->resolution->wait);
	  mem_free(arp_entry->resolution);
	  arp_entry->resolution = NULL;
	}
    }

  arp_entry->timestamp = timer_get_tick(&timer_ms);

  return arp_entry;
}

/*
 * Get the MAC address corresponding to an IP.
 *
 * Make an ARP request if needed.
 */

const uint8_t		*arp_get_mac(struct net_proto_s		*addressing,
				     struct net_proto_s		*arp,
				     struct net_packet_s	*packet,
				     uint_fast32_t		ip)
{
  struct net_pv_arp_s	*pv = (struct net_pv_arp_s *)arp->pv;
  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)addressing->pv;
  struct arp_entry_s	*arp_entry;

  if (ip == pv_ip->addr)
    {
      return pv_ip->interface->mac;
    }

  if (ip == 0xffffffff)
    {
      return (uint8_t *)"\xff\xff\xff\xff\xff\xff";
    }

  if ((arp_entry = arp_table_lookup(&pv->table, ip)) != NULL &&
      timer_get_tick(&timer_ms) - arp_entry->timestamp < ARP_ENTRY_TIMEOUT)
    {
      /* is the entry valid ? */
      if (arp_entry->valid)
	return arp_entry->mac;

      /* otherwise, it is validating, so push the packet in the wait queue */
      packet_queue_pushback(&arp_entry->resolution->wait, packet);
    }
  else
    {
      struct arp_resolution_s	*res;
      struct timer_event_s	*event;

      net_debug("Looking for %P\n", &ip, 4);

      if (arp_entry != NULL)
	{
	  net_debug("Refreshing entry %P\n", &arp_entry->ip, 4);
	  arp_entry->valid = 0;
	}
      else
	if ((arp_entry = arp_update_table(arp, ip, NULL, ARP_TABLE_IN_PROGRESS)) == NULL)
	  return NULL;

      /* entry need to be created or refreshed. first create a
	 resolution structure */
      res = arp_entry->resolution = mem_alloc(sizeof (struct arp_resolution_s), (mem_scope_sys));
      if (res == NULL)
	return NULL;

      /* fill the significant fields */
      packet_queue_init(&res->wait);
      packet_queue_push(&res->wait, packet);
      res->interface = pv_ip->interface;
      res->addressing = addressing;
      res->arp = arp;

      /* setup request time out */
      res->retry = 0;
      event = &res->timeout;
      event->callback = arp_timeout;
      event->pv = (void *)arp_entry;
      event->delay = ARP_REQUEST_TIMEOUT;
      timer_add_event(&timer_ms, event);

      /* and send the request */
      net_debug("Emitting request\n");
      arp_request(pv_ip->interface, addressing, ip);
    }

  return NULL;
}

/*
 * Request timeout callback.
 */

TIMER_CALLBACK(arp_timeout)
{
  struct arp_entry_s		*entry = (struct arp_entry_s *)pv;
  struct arp_resolution_s	*res = entry->resolution;
  struct net_packet_s		*waiting;
  struct net_pv_arp_s		*pv_arp = (struct net_pv_arp_s *)res->arp->pv;

  net_debug("ARP timeout\n");

  if (++res->retry < ARP_MAX_RETRIES)
    {
      /* retry */
      arp_request(res->interface, res->addressing, entry->ip);

      /* set another timeout */
      if (!timer_add_event(&timer_ms, timer))
	return;
    }

  /* otherwise, error */
  net_debug("ARP error\n");

  /* delete the entry */
  arp_table_remove(&pv_arp->table, entry);

  /* delete the queued packets and send errors */
  while ((waiting = packet_queue_pop(&res->wait)))
    {
      res->addressing->desc->f.addressing->errormsg(waiting, ERROR_HOST_UNREACHABLE);
      packet_obj_refdrop(waiting);
    }

  /* free the arp entry */
  arp_entry_obj_delete(entry);
}

/*
 * Stale entry timeout.
 */

TIMER_CALLBACK(arp_stale_timeout)
{
  struct net_pv_arp_s	*pv_arp = (struct net_pv_arp_s *)pv;
  timer_delay_t		t = timer_get_tick(&timer_ms);
  struct arp_entry_s	*to_remove = NULL;

  CONTAINER_FOREACH(arp_table, HASHLIST, &pv_arp->table,
  {
    /* remove previously marked item */
    if (to_remove != NULL)
      {
	net_debug("Clearing stale entry %P\n", &to_remove->ip, 4);
	arp_table_remove(&pv_arp->table, to_remove);
	arp_entry_obj_delete(to_remove);
	to_remove = NULL;
      }

    /* look for an item to remove */
    if (item->valid && t - item->timestamp > ARP_ENTRY_TIMEOUT)
      {
	to_remove = item;
      }
  });

  /* particular case handling */
  if (to_remove != NULL)
    {
      net_debug("Clearing stale entry %P\n", &to_remove->ip, 4);
      arp_table_remove(&pv_arp->table, to_remove);
      arp_entry_obj_delete(to_remove);
    }

  /* schedule the timer again */
  timer_add_event(&timer_ms, &pv_arp->stale_timeout);
}
