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
#include <netinet/if.h>

#ifdef CONFIG_NETWORK_IPV4
# include <netinet/ip.h>
# include <netinet/arp.h>
# include <netinet/icmp.h>
#endif
#ifdef CONFIG_NETWORK_UDP
# include <netinet/udp.h>
#endif
#ifdef CONFIG_NETWORK_TCP
# include <netinet/tcp.h>
#endif
#ifdef CONFIG_NETWORK_SOCKET
# include <netinet/libsocket.h>
#endif

#include <device/net.h>
#include <device/device.h>
#include <device/driver.h>

#include <stdio.h>
#include <mutek/printk.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>

/*
 * Functions for the interface container.
 */

CONTAINER_FUNC(net_if, HASHLIST, static inline, net_if, name);
CONTAINER_KEY_FUNC(net_if, HASHLIST, static inline, net_if, name);

/*
 * Some local variables.
 */

net_if_root_t		net_interfaces = CONTAINER_ROOT_INITIALIZER(net_if, HASHLIST);
static uint_fast8_t	ifid = 0;
static uint_fast8_t	ethid = 0;

/*
 * Interface object constructor.
 */

OBJECT_CONSTRUCTOR(net_if_obj)
{
  struct device_s	*dev = va_arg(ap, struct device_s *);
  net_if_type_t		type = va_arg(ap, net_if_type_t);
  uint8_t		*mac = va_arg(ap, uint8_t *);
  uint_fast16_t		mtu = va_arg(ap, uint_fast16_t);

  /* initialize the object */
  obj->dev = dev;
  obj->mac = mac;
  obj->mtu = mtu;
  obj->type = type;
  obj->rx_bytes = obj->rx_packets = obj->tx_bytes = obj->tx_packets = 0;
  obj->state = NET_IF_STATE_DOWN;
  if (net_protos_init(&obj->protocols))
    return -1;

  /* choose a funky name for the interface */
  if (type == IF_ETHERNET)
    sprintf(obj->name, "eth%d", ethid++);
  else
    sprintf(obj->name, "if%d", ifid);
  ifid++;
  obj->index = ifid;

#ifdef CONFIG_NETWORK_PROFILING
  netobj_new[NETWORK_PROFILING_IF]++;
#endif

  return 0;
}

/*
 * Interface object destructor
 */

OBJECT_DESTRUCTOR(net_if_obj)
{
  net_protos_clear(&obj->protocols);
  net_protos_destroy(&obj->protocols);

#ifdef CONFIG_NETWORK_PROFILING
  netobj_del[NETWORK_PROFILING_IF]++;
#endif
}

/*
 * Register a new interface.
 */

struct net_if_s	*if_register(struct device_s	*dev,
			     net_if_type_t	type,
			     uint8_t		*mac,
			     uint_fast16_t	mtu)
{
#ifdef CONFIG_NETWORK_UDP
  struct net_proto_s				*udp;
#endif
#ifdef CONFIG_NETWORK_TCP
  struct net_proto_s				*tcp;
#endif
  struct net_if_s				*interface;

  /* create new device node */
  if ((interface = net_if_obj_new(NULL, dev, type, mac, mtu)) == NULL)
    return NULL;

  /* initialize standard protocols for the device */
#ifdef CONFIG_NETWORK_UDP
  if ((udp = net_proto_obj_new(NULL, &udp_protocol)) != NULL)
    if_register_proto(interface, udp);
  net_proto_obj_refdrop(udp);
#endif
#ifdef CONFIG_NETWORK_TCP
  if ((tcp = net_proto_obj_new(NULL, &tcp_protocol)) != NULL)
    if_register_proto(interface, tcp);
  net_proto_obj_refdrop(tcp);
#endif

  /* add to the interface list */
  if (!net_if_push(&net_interfaces, interface))
    {
#ifdef CONFIG_NETWORK_UDP
      if (udp != NULL)
	net_protos_remove(&interface->protocols, udp);
#endif
#ifdef CONFIG_NETWORK_TCP
      if (tcp != NULL)
	net_protos_remove(&interface->protocols, tcp);
#endif
      net_if_obj_refdrop(interface);
      return NULL;
    }

  printk("Registered new interface %s (MTU = %u)\n", interface->name, interface->mtu);

  return interface;
}

/*
 * Unregister a net interface.
 */

void			if_unregister(struct net_if_s	*interface)
{
  route_flush(interface);
  net_if_remove(&net_interfaces, interface);
  net_if_obj_refdrop(interface);
}

/*
 * Bring an interface up.
 */

void			if_up(char*		name)
{
  struct net_if_s	*interface;

  if ((interface = net_if_lookup(&net_interfaces, name)))
    {
      printk("Bringing up interface %s\n", name);

      interface->state = NET_IF_STATE_UP;

      net_if_obj_refdrop(interface);
    }
}

/*
 * Bring an interface down.
 */

void			if_down(char*		name)
{
  struct net_if_s	*interface;

  if ((interface = net_if_lookup(&net_interfaces, name)))
    {
      interface->state = NET_IF_STATE_DOWN;

      net_if_obj_refdrop(interface);
    }
}

#ifdef CONFIG_NETWORK_IPV4
/*
 * Cleanup an IPv4 protocol set (includes ARP & ICMPv4).
 */

static void		if_destroy_ipv4(net_protos_root_t	*protos,
					struct net_proto_s	*ipv4)
{
  struct net_pv_ip_s	*pv = (struct net_pv_ip_s *)ipv4->pv;

  net_protos_remove(protos, pv->arp);
  net_protos_remove(protos, pv->icmp);
  net_protos_remove(protos, ipv4);
}
#endif

/*
 * Configure an interface.
 */

error_t			if_config(int_fast32_t		ifindex,
				  uint_fast8_t		action,
				  struct net_addr_s	*address,
				  struct net_addr_s	*mask)
{
  struct net_if_s	*interface = if_get_by_index(ifindex);
  struct net_proto_s	*ip;
  struct net_proto_s	*arp;
  struct net_proto_s	*icmp;
  error_t		err = -1;

  if (interface == NULL)
    return -1;

  if (interface->state != NET_IF_STATE_UP)
    goto leave;

#ifdef CONFIG_NETWORK_IPV4
  if (address->family == addr_ipv4)
    {
      if (action == IF_SET)
	{
	  struct net_proto_s	*prev = NULL;

	  /* remove all other IPv4 modules */
	  NET_FOREACH_PROTO(&interface->protocols, ETHERTYPE_IP,
	  {
	    if (prev != NULL)
	      if_destroy_ipv4(&interface->protocols, prev);
	    prev = item;
	  });
	  if (prev != NULL)
	    if_destroy_ipv4(&interface->protocols, prev);
	}

      if (action == IF_SET || action == IF_ADD)
	{
	  /* add new set of protocols for IPv4 */
	  if ((ip = net_proto_obj_new(NULL, &ip_protocol)) == NULL)
	    {
	      net_if_obj_refdrop(interface);
	      return err;
	    }
	  if ((arp = net_proto_obj_new(NULL, &arp_protocol)) == NULL)
	    {
	      net_if_obj_refdrop(interface);
	      net_proto_obj_refdrop(ip);
	      return err;
	    }
	  if ((icmp = net_proto_obj_new(NULL, &icmp_protocol)) == NULL)
	    {
	      net_if_obj_refdrop(interface);
	      net_proto_obj_refdrop(ip);
	      net_proto_obj_refdrop(arp);
	      return err;
	    }

	  err = 0;
	  /* if one of these fails, the refdrop will clean the memory */
	  if (if_register_proto(interface, arp))
	    err = -1;
	  else
	    if (if_register_proto(interface, icmp))
	      err = -1;
	    else
	      err = if_register_proto(interface, ip, arp, icmp, IPV4_ADDR_GET(*address),
				      IPV4_ADDR_GET(*mask));

	  net_proto_obj_refdrop(ip);
	  net_proto_obj_refdrop(arp);
	  net_proto_obj_refdrop(icmp);
	}
      else
	{
	  /* look for the protocol to remove and remove it */
	  NET_FOREACH_PROTO(&interface->protocols, ETHERTYPE_IP,
	  {
	    if (item->desc->f.addressing->matchaddr(item, address, NULL, NULL))
	      {
		if_destroy_ipv4(&interface->protocols, item);
		NET_FOREACH_PROTO_BREAK;
	      }
	  });
	}
    }
#endif

 leave:

  net_if_obj_refdrop(interface);

  return err;
}

/*
 * Register a new protocol.
 */

error_t			if_register_proto(struct net_if_s	*interface,
					  struct net_proto_s	*proto,
					  ...)
{
  va_list		va;

  va_start(va, proto);

  /* call the protocol constructor */
  if (proto->desc->initproto != NULL)
    {
      if (proto->desc->initproto(interface, proto, va))
	{
	  va_end(va);

	  return -1;
	}
      proto->initialized = 1;
    }

  va_end(va);

  /* insert in the protocol list */
  if (!net_protos_push(&interface->protocols, proto))
    return -1;

  return 0;
}

/*
 * Push a packet.
 */

void			if_pushpkt(struct net_if_s	*interface,
				   struct net_packet_s	*packet)
{
  if (interface->state != NET_IF_STATE_UP)
    return;

  interface->rx_bytes += packet->header[0].size;
  interface->rx_packets++;

  packet->interface = interface;

#ifdef CONFIG_NETWORK_SOCKET_PACKET
  pf_packet_signal(interface, packet, packet->proto);
#endif

  /* lookup to all modules matching the protocol  */
  NET_FOREACH_PROTO(&interface->protocols, packet->proto,
  {
    item->desc->pushpkt(interface, packet, item);
  });
}

/*
 * Prepare a packet.
 */

inline uint8_t		*if_preparepkt(struct net_if_s		*interface,
				       struct net_packet_s	*packet,
				       size_t			size,
				       size_t			max_padding)
{
  return dev_net_preparepkt(interface->dev, packet, size, max_padding);
}

/*
 * Send a packet.
 */

void			if_sendpkt(struct net_if_s	*interface,
				   struct net_packet_s	*packet,
				   net_proto_id_t	proto)
{
  if (interface->state != NET_IF_STATE_UP)
    return;

  interface->tx_bytes += packet->header[0].size;
  interface->tx_packets++;

  if (!memcmp(interface->mac, packet->tMAC, packet->MAClen))
    {
      /* XXX c'est mal poli on passe devant tout le monde */
      packet->proto = proto;
      packet->stage++;
      if_pushpkt(interface, packet);
    }
  else
    dev_net_sendpkt(interface->dev, packet, proto);
}

static void		spc(uint_fast8_t	i)
{
  for (; i > 0; i--)
    printk(" ");
}

/*
 * Dump interface(s)
 */

void			if_dump(const char	*name)
{
  struct net_if_s	*interface;
  uint_fast8_t		i;

  if ((interface = net_if_lookup(&net_interfaces, name)) && interface->state == NET_IF_STATE_UP)
    {
      i = printk("%s", name);
      spc(6 - i);
      printk("HWaddr %02x:%02x:%02x:%02x:%02x:%02x\n", interface->mac[0], interface->mac[1],
	     interface->mac[2], interface->mac[3], interface->mac[4], interface->mac[5]);
#ifdef CONFIG_NETWORK_IPV4
	  NET_FOREACH_PROTO(&interface->protocols, ETHERTYPE_IP,
	  {
	    struct net_pv_ip_s	*ipv4 = (struct net_pv_ip_s *)item->pv;

	    spc(6);
	    printk("inet addr %u.%u.%u.%u mask %u.%u.%u.%u broadcast %u.%u.%u.%u\n",
		   EXTRACT_IPV4(ipv4->addr), EXTRACT_IPV4(ipv4->mask),
		   EXTRACT_IPV4(ipv4->addr | (0xffffffff & ~ipv4->mask)));
	  });
#endif
      spc(6);
      printk("%u bytes sent (%u packets), %u bytes received (%u packets)\n",
	     interface->tx_bytes, interface->tx_packets,
	     interface->rx_bytes, interface->rx_packets);

      net_if_obj_refdrop(interface);
    }
}

/*
 * Get interface from name.
 */

struct net_if_s	*if_get_by_name(const char	*name)
{
  return net_if_lookup(&net_interfaces, name);
}

/*
 * Get interface from name.
 */

struct net_if_s	*if_get_by_index(int_fast32_t	index)
{
  char	name[10];

  sprintf(name, "eth%d", index - 1); /* XXX un peu l'arrache quand meme :-) */
  return if_get_by_name(name);
}

