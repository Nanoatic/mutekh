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

    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009
*/

#ifndef LIBNETWORK_DISPATCH_H_
#define LIBNETWORK_DISPATCH_H_

#include <hexo/types.h>

struct net_dispatch_s;
struct net_packet_s;
struct net_if_s;

struct net_dispatch_s *network_dispatch_create(struct net_if_s *interface);
void network_dispatch_kill(struct net_dispatch_s *dispatch);
void network_dispatch_packet(struct net_dispatch_s *dispatch,
							 struct net_packet_s *packet);
void network_dispatch_data(struct net_dispatch_s *dispatch,
						   void *data,
						   uint_fast16_t size);

#endif
