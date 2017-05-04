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

#ifndef NETINET_PING_H
# define NETINET_PING_H

# ifndef CONFIG_NETWORK_PING
#  warning PING support is not enabled in configuration file
# endif

# include <hexo/types.h>
# include <mutek/timer.h>
# include <netinet/packet.h>

# define PING_INTERVAL	200	// ms
# define PING_TIMEOUT	10000	// ms

struct		ping_s
{
  uint_fast32_t	total;
  uint_fast32_t	lost;
  uint_fast32_t	error;
  timer_delay_t	min;
  timer_delay_t	max;
  timer_delay_t	avg;
};

error_t		ping(struct net_addr_s	*host,
		     uint_fast32_t	count,
		     size_t		size,
		     struct ping_s	*stat);

#endif
