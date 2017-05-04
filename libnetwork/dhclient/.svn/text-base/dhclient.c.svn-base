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
 * DHCPv4 client
 *
 */

#include <hexo/error.h>
#include <hexo/endian.h>

#include <netinet/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/dhcp.h>
#include <netinet/arp.h>
#include <netinet/udp.h>
#include <netinet/route.h>
#include <netinet/socket.h>
#include <netinet/packet.h>

#include <mutek/printk.h>

#include <stdlib.h>
#include <mutek/timer.h>
#include <mutek/scheduler.h>
#include <mutek/semaphore.h>

/*
 * This function broadcasts an ARP request to ensure an IP address is
 * not already assigned.
 */

static bool_t		dhcp_ip_is_free(struct net_if_s	*interface,
					uint_fast32_t	ip)
{
  socket_t		sock;
  struct sockaddr_ll	addr_sll;
  struct ether_arp	arp;
  bool_t		one = 1;
  struct timeval	tv;
  timer_delay_t		t;

  /* create a PF_PACKET socket */
  if ((sock = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP))) == NULL)
    return 0;

  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &one, sizeof (bool_t)) < 0)
    goto leave;

  tv.tv_usec = 0;
  tv.tv_sec = ARP_REQUEST_TIMEOUT / 1000;

  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (struct timeval)) < 0)
    goto leave;

  addr_sll.sll_family = AF_PACKET;
  addr_sll.sll_protocol = htons(ETH_P_ARP);
  addr_sll.sll_ifindex = interface->index;

  /* bind it to the interface */
  if (bind(sock, (struct sockaddr *)&addr_sll, sizeof (struct sockaddr_ll)) < 0)
    goto leave;

  /* build an ARP request */
  arp.ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
  arp.ea_hdr.ar_pro = htons(ETHERTYPE_IP);
  arp.ea_hdr.ar_hln = ETH_ALEN;
  arp.ea_hdr.ar_pln = 4;
  arp.ea_hdr.ar_op = htons(ARPOP_REQUEST);
  memcpy(arp.arp_sha, interface->mac, ETH_ALEN);
  endian_be32_na_store(&arp.arp_spa, 0);
  arp.arp_tpa = htonl(ip);

  /* send the request */
  addr_sll.sll_halen = ETH_ALEN;
  memcpy(addr_sll.sll_addr, "\xff\xff\xff\xff\xff\xff", ETH_ALEN);

  if (sendto(sock, &arp, sizeof (struct ether_arp), 0, (struct sockaddr *)&addr_sll, sizeof (struct sockaddr_ll)) < 0)
    goto leave;

  t = timer_get_tick(&timer_ms);

  /* wait reply */
  while (recv(sock, &arp, sizeof (struct ether_arp), 0) > 0)
    {
      if (arp.ea_hdr.ar_hrd == htons(ARPHRD_ETHER) && arp.ea_hdr.ar_pro == htons(ETHERTYPE_IP) &&
	  arp.ea_hdr.ar_hln == ETH_ALEN && arp.ea_hdr.ar_pln == 4 &&
	  arp.ea_hdr.ar_op == htons(ARPOP_REPLY))
	{
	  /* positive reply, the address is in use */
	  if (arp.arp_spa == ip)
	    goto leave;
	}

      if (timer_get_tick(&timer_ms) - t > ARP_REQUEST_TIMEOUT)
	break;
    }

  shutdown(sock, SHUT_RDWR);

  return 1;

 leave:

  shutdown(sock, SHUT_RDWR);

  return 0;
}

/*
 * This function reads a raw packet to detect if it is a DHCP reply
 * destinated to us.
 */

static struct dhcphdr	*dhcp_is_for_me(struct net_if_s	*interface,
					uint8_t		*packet,
					socket_t	sock)
{
  struct iphdr		*ip;
  struct udphdr		*udp;
  struct dhcphdr	*dhcp;

  /* get IP header */
  ip = (struct iphdr *)packet;
  if (ip->protocol != IPPROTO_UDP || ntohs(ip->tot_len) < sizeof (struct dhcphdr) + sizeof (struct udphdr))
    return NULL;

  /* get UDP header */
  udp = (struct udphdr *)((uint8_t*)packet + ip->ihl * 4);
  if (ntohs(udp->dest) != BOOTP_CLIENT_PORT || ntohs(udp->len) < sizeof (struct dhcphdr))
    return NULL;

  /* get DHCP header */
  dhcp = (struct dhcphdr *)(udp + 1);
  if (dhcp->op == BOOTREPLY && dhcp->xid == (uintptr_t)sock &&
      !memcmp(dhcp->magic, "\x63\x82\x53\x63", 4))
    {
      if (!memcmp(dhcp->chaddr, interface->mac, ETH_ALEN))
	return dhcp;
    }

  return NULL;
}

/*
 * This function browses the DHCP options to find a given value.
 */

static struct dhcp_opt_s	*dhcp_get_opt(struct dhcphdr	*dhcp,
					      uint8_t		*endptr,
					      uint8_t		opt)
{
  struct dhcp_opt_s		*p = (struct dhcp_opt_s *)(dhcp + 1);

  while (p->code != DHCP_END && p->code != opt)
    {
      if ((uint8_t *)p >= endptr || (uint8_t *)&p->len >= endptr)
	return NULL;
      p = (struct dhcp_opt_s *)((uint8_t *)p + 2 + p->len);
    }

  return p->code == DHCP_END ? NULL : p;
}

/*
 * This function creates the sockets used for sending and receiving
 * packets.
 */

static error_t		dhcp_init(struct net_if_s	*interface,
				  socket_t		*sock,
				  socket_t		*sock_packet)
{
  struct sockaddr_in	addr;
  struct sockaddr_ll	addr_sll;
  bool_t		one = 1;
  struct timeval	tv;

  /* create an UDP socket */
  if ((*sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == NULL)
    return -1;

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(BOOTP_CLIENT_PORT);

  /* bind it to the client port */
  if (bind(*sock, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) < 0)
    goto leave;

  if (setsockopt(*sock, SOL_SOCKET, SO_BROADCAST, &one, sizeof (bool_t)) < 0)
    goto leave;

  /* create a PF_PACKET socket */
  if ((*sock_packet = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) == NULL)
    goto leave;

  addr_sll.sll_family = AF_PACKET;
  addr_sll.sll_protocol = htons(ETH_P_IP);
  addr_sll.sll_ifindex = interface->index;

  /* bind it to the interface */
  if (bind(*sock_packet, (struct sockaddr *)&addr_sll, sizeof (struct sockaddr_ll)) < 0)
    goto leave;

  tv.tv_usec = 0;
  tv.tv_sec = DHCP_TIMEOUT;

  if (setsockopt(*sock_packet, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (struct timeval)) < 0)
    goto leave;

  return 0;

 leave:
  return -1;
}

/*
 * This function emits a DHCP discover message.
 */

static error_t		dhcp_packet(struct net_if_s	*interface,
				    uint8_t		type,
				    uint_fast32_t	ip,
				    uint_fast32_t	serv,
				    socket_t		sock)
{
  struct sockaddr_in	broadcast;
  struct dhcphdr	*packet = NULL;
  struct dhcp_opt_s	*opt;
  uint8_t		*raw;
  size_t		packet_len;
  char			*str;

  /* init and send a DHCPDISCOVER */
  broadcast.sin_family = AF_INET;
  broadcast.sin_addr.s_addr = htonl(serv);
  broadcast.sin_port = htons(BOOTP_SERVER_PORT);

  if ((packet = malloc(interface->mtu)) == NULL)
    return -1;

  memset(packet, 0, sizeof (struct dhcphdr));

  /* setup DHCP header */
  packet->op = BOOTREQUEST;
  packet->htype = ARPHRD_ETHER;
  packet->hlen = ETH_ALEN;
  packet->xid = (uintptr_t)sock;
  memcpy(packet->magic, "\x63\x82\x53\x63", 4);

  memcpy(packet->chaddr, interface->mac, ETH_ALEN);

  /* fill DHCP options */
  raw = (void *)(packet + 1);
  opt = (struct dhcp_opt_s *)raw;
  opt->code = DHCP_MSG;
  opt->len = 1;
  opt->data[0] = type;
  raw += 3;

  opt = (void *)raw;
  opt->code = DHCP_ID;
  opt->len = 1 + ETH_ALEN;
  opt->data[0] = ARPHRD_ETHER;
  memcpy(&opt->data[1], interface->mac, ETH_ALEN);
  raw += (3 + ETH_ALEN);

  if (type != DHCPDISCOVER)
    {
      /* if not a discovery packet, include requested ip and destination server */
      packet->siaddr = htonl(serv);
      packet->yiaddr = htonl(ip);

      opt = (void *)raw;
      opt->code = DHCP_REQIP;
      opt->len = 4;
      endian_be32_na_store(opt->data, ip);
      raw += 6;

      opt = (void *)raw;
      opt->code = DHCP_SERVER;
      opt->len = 4;
      endian_be32_na_store(opt->data, serv);
      raw += 6;
    }

  opt = (void *)raw;
  opt->code = DHCP_REQLIST;
  opt->len = 3;
  opt->data[0] = DHCP_NETMASK;
  opt->data[1] = DHCP_HOSTNAME;
  opt->data[2] = DHCP_ROUTER;
  raw += 5;

  opt = (void *)raw;
  opt->code = DHCP_END;

  packet_len = (uint8_t *)opt - (uint8_t *)packet;

  switch (type)
    {
      case DHCPDISCOVER:
	str = "DHCPDISCOVER";
	break;
      case DHCPREQUEST:
	str = "DHCPREQUEST";
	break;
      case DHCPDECLINE:
	str = "DHCPDECLINE";
	break;
      default:
	str = NULL;
	break;
    }

  if (str != NULL)
    printk("dhclient: sending %s...\n", str);

  if (sendto(sock, packet, packet_len, 0, (struct sockaddr *)&broadcast,
	     sizeof (struct sockaddr_in)) < 0)
    {
      free(packet);
      return -1;
    }

  free(packet);
  return 0;
}

/*
 * This function waits for DHCP offers and reply to them.
 */

static error_t		dhcp_request(struct net_if_s	*interface,
				     socket_t		sock,
				     socket_t		sock_packet,
				     struct dhcp_lease_s *lease)
{
  ssize_t		sz;
  uint8_t		*packet = NULL;
  struct dhcphdr	*dhcp;
  struct dhcp_opt_s	*opt;
  bool_t		requested = 0;
  timer_delay_t		t;
  uint8_t		*endptr;

  if ((packet = malloc(interface->mtu)) == NULL)
    return -1;

  t = timer_get_tick(&timer_ms);

  /* receive DHCPOFFER & acks */
  while ((sz = recv(sock_packet, packet, interface->mtu, 0)) >= 0)
    {
      if (timer_get_tick(&timer_ms) - t > DHCP_TIMEOUT * 1000)
	break;

      endptr = packet + sz;

      if ((dhcp = dhcp_is_for_me(interface, packet, sock)) != NULL)
	{
	  if ((opt = dhcp_get_opt(dhcp, endptr, DHCP_MSG)) == NULL)
	    continue;

	  switch (opt->data[0])
	    {
	      case DHCPOFFER:
		{
		  uint_fast32_t	yaddr;
		  uint_fast32_t	saddr;

		  saddr = htonl(dhcp->siaddr);
		  yaddr = htonl(dhcp->yiaddr);

		  printk("dhclient: received DHCPOFFER from %u.%u.%u.%u\n",
			 EXTRACT_IPV4(saddr));
		  printk("dhclient: offered %u.%u.%u.%u ... ",
			 EXTRACT_IPV4(yaddr));

		  /* if the IP is free, take the offer */
		  if (!requested && dhcp_ip_is_free(interface, yaddr))
		    {
		      printk("is free. Accepting.\n");

		      /* send DHCPREQUEST */
		      requested = !dhcp_packet(interface, DHCPREQUEST, yaddr, saddr, sock);
		    }
		  else /* otherwise, decline the offer */
		    {
		      printk("is not free. Declining.\n");

		      /* send DHCPDECLINE */
		      dhcp_packet(interface, DHCPDECLINE, yaddr, saddr, sock);
		    }
		}
		break;
	      case DHCPACK:
		{
		  struct net_route_s	*route;
		  struct net_addr_s	addr;
		  struct net_addr_s	mask;
		  struct net_addr_s	target;

		  /* is it address renewal */
		  if (lease->delay)
		    {
		      free(packet);
		      return 0;
		    }

		  printk("dhclient: end of negociation.\n");

		  lease->serv = ntohl(dhcp->siaddr);
		  lease->ip = ntohl(dhcp->yiaddr);

		  /* compute lease time */
		  opt = dhcp_get_opt(dhcp, endptr, DHCP_LEASE);
		  if (opt != NULL)
		    lease->delay = (endian_be32_na_load(opt->data) / 2) * 1000;
		  else
		    lease->delay = DHCP_DFL_LEASE;

		  /* configure IP */
		  IPV4_ADDR_SET(addr, lease->ip);

		  /* if netmask is present, use it, otherwise guess it */
		  opt = dhcp_get_opt(dhcp, endptr, DHCP_NETMASK);
		  if (opt != NULL)
		    IPV4_ADDR_SET(mask, endian_be32_na_load(opt->data));
		  else
		    {
		      if (IN_CLASSA(lease->ip))
			IPV4_ADDR_SET(mask, IN_CLASSA_NET);
		      else if (IN_CLASSB(lease->ip))
			IPV4_ADDR_SET(mask, IN_CLASSB_NET);
		      else if (IN_CLASSC(lease->ip))
			IPV4_ADDR_SET(mask, IN_CLASSC_NET);
		    }
		  printk("dhclient:\n  assigned %u.%u.%u.%u netmask %u.%u.%u.%u\n",
			 EXTRACT_IPV4(addr.addr.ipv4), EXTRACT_IPV4(mask.addr.ipv4));
		  if_config(interface->index, IF_SET, &addr, &mask);
		  route_flush(interface);

		  printk("  lease time: %u seconds\n", lease->delay / 1000);
		  if ((opt = dhcp_get_opt(dhcp, endptr, DHCP_HOSTNAME)) != NULL)
		    {
		      char	name[opt->len + 1];

		      memcpy(name, opt->data, opt->len);
		      name[opt->len] = 0;
		      printk("  hostname: %s\n", name);
		    }

		  if ((opt = dhcp_get_opt(dhcp, endptr, DHCP_ROUTER)) != NULL)
		    {
		      struct net_route_s	*def;
		      struct net_addr_s		null;
		      uint32_t			gateway;

		      gateway = endian_be32_na_load(opt->data);

		      printk("  gateway: %u.%u.%u.%u\n",
			     EXTRACT_IPV4(gateway));

		      /* configure default route */
		      IPV4_ADDR_SET(null, 0x0);
		      if ((def = route_obj_new(NULL, &null, &null, interface)) != NULL)
			{
			  def->is_routed = 1;
			  IPV4_ADDR_SET(def->router, gateway);
			  route_add(def);
			  route_obj_refdrop(def);
			}
		    }

		  /* configure interface route */
		  IPV4_ADDR_SET(target, addr.addr.ipv4 & mask.addr.ipv4);
		  if ((route = route_obj_new(NULL, &target, &mask, interface)) != NULL)
		    {
		      route->is_routed = 0;
		      route_add(route);
		      route_obj_refdrop(route);
		    }

		  /* we've got an address :-)) */
		  free(packet);
		  return 0;
		}
		break;
	      case DHCPNACK:
		/* error during attribution */
		printk("dhclient: error.\n");
		requested = 0;
		break;
	      default:
		break;
	    }
	}
    }

  free(packet);

  return -1;
}

/*
 * DHCP renew thread.
 */

static TIMER_CALLBACK(dhcp_renew)
{
  struct dhcp_lease_s	*lease = (struct dhcp_lease_s *)pv;

  /* initiate renewal */
  semaphore_give(&lease->sem, 1);
}

static CONTEXT_ENTRY(dhcp_renew_th)
{
  struct dhcp_lease_s	*lease = (struct dhcp_lease_s *)param;
  socket_t		sock = NULL;
  socket_t		sock_packet = NULL;
  uint_fast8_t		i;
  struct net_addr_s	null;

  sched_unlock();
  cpu_interrupt_enable();

  while (1)
    {
      semaphore_take(&lease->sem, 1);

      if (lease->exit)
	break;

      printk("dhclient: initiating renewal ...\n");

      /* create sockets */
      if (dhcp_init(lease->interface, &sock, &sock_packet))
	goto leave;

      /* try to renew (12 times is about 2 min) */
      for (i = 0; i < 12; i++)
	{
	  /* send a DHCPREQUEST */
	  dhcp_packet(lease->interface, DHCPREQUEST, lease->ip, lease->serv, sock);

	  /* wait answer */
	  if (!dhcp_request(lease->interface, sock, sock_packet, lease))
	    {
	      printk("dhclient: renewal succeeded.\n");

	      /* start the timer again */
	      if (timer_add_event(&timer_ms, lease->timer))
		goto err;

	      break;
	    }
	}

      /* critical error, panic */
      if (i == 12)
	{
	  /* best solution is to restart from the beginning... */
	  printk("dhclient: renewal failed.\n");

	  /* discover DHCP servers */
	  if (dhcp_packet(lease->interface, DHCPDISCOVER, 0, INADDR_BROADCAST, sock))
	    goto leave;

	  /* answer DHCP offers */
	  lease->delay = 0;
	  if (dhcp_request(lease->interface, sock, sock_packet, lease))
	    goto leave;

	  /* start the timer again */
	  if (timer_add_event(&timer_ms, lease->timer))
	    goto err;

	  if_dump("eth0");
	  route_dump();
	}

    leave:

      if (sock != NULL)
	shutdown(sock, SHUT_RDWR);
      if (sock_packet != NULL)
	shutdown(sock_packet, SHUT_RDWR);
    }

  printk("dhclient: exiting.\n");

  goto exit;

 err:
  printk("dhclient: unknown error, exiting.\n");

 exit:

  /* bring interface down */
  IPV4_ADDR_SET(null, 0);
  if_config(lease->interface->index, IF_SET, &null, &null);
  route_flush(lease->interface);
  if_down("eth0");

  /* release objects */
  timer_cancel_event(lease->timer, 0);
  mem_free(lease->timer);
  semaphore_destroy(&lease->sem);
  mem_free(lease);

  sched_lock();
  sched_context_exit();
}

/*
 * DHCP client entry function. Request an address for the given
 * interface.
 */

error_t			dhcp_client(const char	*ifname)
{
  struct timer_event_s	*timer;
  struct dhcp_lease_s	*lease;
  socket_t		sock = NULL;
  socket_t		sock_packet = NULL;
  struct net_if_s	*interface;
  struct net_addr_s	null;
  struct net_route_s	*route = NULL;

  if ((interface = if_get_by_name(ifname)) == NULL)
    return -1;

  if ((lease = malloc(sizeof (struct dhcp_lease_s))) == NULL)
    return -1;

  lease->interface = interface;
  lease->delay = 0;

  /* ifconfig 0.0.0.0 */
  IPV4_ADDR_SET(null, 0);
  if_config(interface->index, IF_SET, &null, &null);
  if ((route = route_obj_new(NULL, &null, &null, interface)) == NULL)
    goto leave;
  route_add(route);
  route_obj_refdrop(route);

  /* create sockets */
  if (dhcp_init(interface, &sock, &sock_packet))
    goto leave;

  /* discover DHCP servers */
  if (dhcp_packet(interface, DHCPDISCOVER, 0, INADDR_BROADCAST, sock))
    goto leave;

  /* answer DHCP offers */
  if (dhcp_request(interface, sock, sock_packet, lease))
    goto leave;

  /* start DHCP renew thread */
  lease->exit = 0;
  semaphore_init(&lease->sem, 0);

  context_init( &lease->context.context,
				lease->stack,
				lease->stack + sizeof(lease->stack),
				dhcp_renew_th,
				lease );
  sched_context_init( &lease->context );
  CPU_INTERRUPT_SAVESTATE_DISABLE;
  sched_context_start( &lease->context );
  CPU_INTERRUPT_RESTORESTATE;
  
  /* start DHCP renew timer */
  if ((timer = malloc(sizeof (struct timer_event_s))) == NULL)
    {
      lease->exit = 1;
      semaphore_give(&lease->sem, 1);
    }
  else
    {
      lease->timer = timer;
      timer->callback = dhcp_renew;
      timer->delay = lease->delay;
      timer->pv = lease;
      if (timer_add_event(&timer_ms, timer))
	{
	  lease->exit = 1;
	  semaphore_give(&lease->sem, 1);
	}
    }

  shutdown(sock, SHUT_RDWR);
  shutdown(sock_packet, SHUT_RDWR);

  return 0;

 leave:
  printk("dhclient: error, leaving\n");

  free(lease);

  if (route != NULL)
    route_del(route);
  if_config(interface->index, IF_DEL, &null, NULL);

  if (sock != NULL)
    shutdown(sock, SHUT_RDWR);
  if (sock_packet != NULL)
    shutdown(sock_packet, SHUT_RDWR);

  return -1;
}
