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

#ifndef NETINET_SOCKET_PACKET_H
#define NETINET_SOCKET_PACKET_H

#ifndef CONFIG_NETWORK_SOCKET_PACKET
# warning Socket support is not enabled in configuration file
#endif

#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/if.h>
#include <netinet/socket.h>

#include <semaphore.h>

struct				socket_packet_pv_s
{
  net_proto_id_t		proto;
  bool_t			header;
  uint_fast32_t			interface;
  error_t			error;

  packet_queue_root_t		recv_q;
  struct semaphore_s				recv_sem;
};

#endif
