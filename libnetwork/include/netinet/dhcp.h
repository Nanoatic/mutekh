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

#ifndef NETINET_DHCP_H
#define NETINET_DHCP_H

#ifndef CONFIG_NETWORK_DHCLIENT
# warning DHCP client support is not enabled in configuration file
#endif

#include <hexo/types.h>

/*
 * Ports.
 */

#define BOOTP_SERVER_PORT	67
#define BOOTP_CLIENT_PORT	68

/*
 * BOOTP operations.
 */

#define BOOTREQUEST	1
#define BOOTREPLY	2

/*
 * DHCP options identifiers.
 */

#define DHCP_PAD	0
#define DHCP_NETMASK	1
#define DHCP_ROUTER	3
#define DHCP_HOSTNAME	12
#define DHCP_REQIP	50
#define DHCP_LEASE	51
#define DHCP_MSG	53
#define DHCP_SERVER	54
#define DHCP_REQLIST	55
#define DHCP_ID		61
#define DHCP_END	255

/*
 * DHCP message types.
 */

#define DHCPDISCOVER	1
#define DHCPOFFER	2
#define DHCPREQUEST	3
#define DHCPDECLINE	4
#define DHCPACK		5
#define DHCPNACK	6
#define DHCPRELEASE	7
#define DHCPINFORM	8

/*
 * Misc.
 */

#define DHCP_TIMEOUT	10 /* 10 seconds */
#define DHCP_DFL_LEASE	120000 /* 2 minutes */

#include <netinet/if.h>
#include <mutek/timer.h>
#include <mutek/scheduler.h>
#include <mutek/semaphore.h>

/*
 * DHCP lease info.
 */

struct			dhcp_lease_s
{
  struct sched_context_s context;
  uint8_t stack[2000];
  struct net_if_s	*interface;
  timer_delay_t		delay;
  uint_fast32_t		serv;
  uint_fast32_t		ip;
  struct timer_event_s	*timer;
  struct semaphore_s			sem;
  bool_t		exit;
};

/*
 * DHCP options.
 */

struct		dhcp_opt_s
{
  uint8_t	code;
  uint8_t	len;
  uint8_t	data[1];
} __attribute__((packed));

/*
 * DHCP header.
 */

struct			dhcphdr
{
  uint8_t		op;
  uint8_t		htype;
  uint8_t		hlen;
  uint8_t		hops;
  uint32_t		xid;
  uint16_t		secs;
  uint16_t		flags;
  uint32_t		ciaddr;
  uint32_t		yiaddr;
  uint32_t		siaddr;
  uint32_t		giaddr;
  uint8_t		chaddr[16];
  uint8_t		sname[64];
  uint8_t		file[128];
  uint8_t		magic[4];
} __attribute__((packed));

/*
 * DHCP exported functions
 */

error_t			dhcp_client(const char	*ifname);

#endif

