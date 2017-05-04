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

#ifndef NET_NE2000_PRIVATE_H_
#define NET_NE2000_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>

#include <pthread.h>
#include <semaphore.h>

#include <netinet/packet.h>
#include <netinet/ether.h>
#include <netinet/protos.h>
#include <netinet/if.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

/*
 * private data of a ne2000 network device
 */

struct				net_ne2000_context_s
{
  lock_t			lock;

  uint_fast8_t			io_16;
  uint_fast16_t			tx_buf;
  uint_fast16_t			rx_buf;
  uint_fast16_t			mem;

  bool_t			run;
  packet_queue_root_t		sendqueue;
  uint_fast8_t			send_tries;
  struct net_dispatch_s *dispatch;
  struct net_packet_s		*current;
  struct device_s		*icudev;

  uint8_t			mac[ETH_ALEN];
  struct net_if_s		*interface;
};

#endif

