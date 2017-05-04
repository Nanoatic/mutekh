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

#ifndef NETINET_SOCKET_UDP_H
#define NETINET_SOCKET_UDP_H

#ifndef CONFIG_NETWORK_UDP
# warning UDP support is not enabled in configuration file
#endif

#include <netinet/libudp.h>
#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/socket.h>
#include <netinet/socket_internals.h>

#include <semaphore.h>

struct			socket_udp_pv_s
{
  struct net_udp_desc_s	*desc;
  uint_fast32_t		family;

  net_port_t		recv_port;
  buffer_queue_root_t	recv_q;
  struct semaphore_s			recv_sem;
};

#endif
