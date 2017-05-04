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
 * Reverse ARP for IPv4
 *
 */

#include <hexo/error.h>
#include <hexo/endian.h>

#include <mutek/printk.h>

#include <netinet/if.h>
#include <netinet/in.h>
#include <netinet/arp.h>
#include <netinet/socket.h>

error_t			rarp_client(const char	*ifname)
{
  struct sockaddr_ll	addr;
  struct ether_arp	arp;
  socket_t		sock;
  struct net_if_s	*interface;
  ssize_t		sz;
  bool_t		one = 1;
  struct timeval	tv;

  /* create a PF_PACKET socket */
  if ((interface = if_get_by_name(ifname)) == NULL)
    return -1;

  if ((sock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_RARP))) == NULL)
    return -1;

  addr.sll_family = AF_PACKET;
  addr.sll_protocol = htons(ETH_P_RARP);
  addr.sll_ifindex = interface->index;

  /* init the socket */
  if (bind(sock, (struct sockaddr *)&addr, sizeof (struct sockaddr_ll)) < 0)
    goto leave;

  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &one, sizeof (bool_t)) < 0)
    goto leave;

  tv.tv_usec = 0;
  tv.tv_sec = RARP_TIMEOUT;

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (struct timeval)) < 0)
    goto leave;


  /* build a RARP request */
  arp.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
  arp.ea_hdr.ar_pro = htons(ETHERTYPE_IP);
  arp.ea_hdr.ar_hln = ETH_ALEN;
  arp.ea_hdr.ar_pln = 4;
  arp.ea_hdr.ar_op = htons(ARPOP_RREQUEST);

  memcpy(arp.arp_sha, interface->mac, ETH_ALEN);
  memcpy(arp.arp_tha, interface->mac, ETH_ALEN);
  endian_be32_na_store(&arp.arp_spa, 0);
  arp.arp_tpa = 0;

  /* send the request */
  addr.sll_family = AF_PACKET;
  addr.sll_ifindex = interface->index;
  addr.sll_halen = ETH_ALEN;
  memcpy(addr.sll_addr, "\xff\xff\xff\xff\xff\xff", ETH_ALEN);

  if (sendto(sock, &arp, sizeof (struct ether_arp), 0, (struct sockaddr *)&addr, sizeof (struct sockaddr_ll)) < sizeof (struct ether_arp))
    goto leave;

  /* wait reply */
  while ((sz = recv(sock, &arp, sizeof (struct ether_arp), 0)) > 0)
    {
      if (sz < 0)
	goto leave;

      /* parse reply */
      if (arp.ea_hdr.ar_hrd != htons(ARPHRD_ETHER) ||
	  arp.ea_hdr.ar_pro != htons(ETHERTYPE_IP))
	continue;

      if (arp.ea_hdr.ar_op == htons(ARPOP_RREPLY) && !memcmp(arp.arp_tha, interface->mac, ETH_ALEN))
	{
	  /* it's our reply */
	  uint_fast32_t		ip = ntohl(arp.arp_tpa);
	  uint_fast32_t		mask;
	  struct net_addr_s	v4_addr;
	  struct net_addr_s	v4_mask;
	  struct net_route_s	*route;
	  struct net_addr_s	target;

	  /* guess netmask */
	  if (IN_CLASSA(ip))
	    mask = IN_CLASSA_NET;
	  else if (IN_CLASSB(ip))
	    mask = IN_CLASSB_NET;
	  else if (IN_CLASSC(ip))
	    mask = IN_CLASSC_NET;

	  /* set the new address */
	  IPV4_ADDR_SET(v4_addr, ip);
	  IPV4_ADDR_SET(v4_mask, mask);
	  if (if_config(interface->index, IF_SET, &v4_addr, &v4_mask))
	    goto leave;
	  /* configure interface route */
	  IPV4_ADDR_SET(target, v4_addr.addr.ipv4 & v4_mask.addr.ipv4);
	  if ((route = route_obj_new(NULL, &target, &v4_mask, interface)) != NULL)
	    {
	      route->is_routed = 0;
	      route_add(route);
	      route_obj_refdrop(route);
	    }
	  printk("Assigned IP: %d.%d.%d.%d, netmask: %d.%d.%d.%d\n", EXTRACT_IPV4(ip), EXTRACT_IPV4(mask));
	  break;
	}
    }

  net_if_obj_refdrop(interface);
  shutdown(sock, SHUT_RDWR);

  return 0;

 leave:
  printk("rarp: error\n");

  net_if_obj_refdrop(interface);
  shutdown(sock, SHUT_RDWR);

  return -1;
}
