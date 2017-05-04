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

#ifndef NETINET_LIBUDP_H
#define NETINET_LIBUDP_H

#ifndef CONFIG_NETWORK_UDP
# warning UDP support is not enabled in configuration file
#endif

#include <netinet/protos.h>
#include <netinet/packet.h>
#include <netinet/udp.h>
#include <netinet/route.h>

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/object_simple.h>

/*
 * Temporary ports boundaries.
 */

#define UDP_TEMP_PORT_BASE	46000
#define UDP_TEMP_PORT_RANGE	4000

/*
 * Types
 */

struct net_udp_desc_s;

/*
 * An UDP address is made of an IP address and a port number.
 */

struct	net_udp_addr_s
{
  struct net_addr_s	address;
  uint_fast16_t		port;
};

/*
 * UDP callback on packet receiving.
 */

#define UDP_CALLBACK(f)	void (f)(struct net_udp_desc_s	*desc,		\
				 struct net_udp_addr_s	*remote,	\
				 const void		*data,		\
				 size_t			size,		\
				 void			*pv)
typedef UDP_CALLBACK(udp_callback_t);

/*
 * UDP callback on error.
 */

#define UDP_ERROR_CALLBACK(f)	void (f)(struct net_udp_desc_s	*desc,	\
					 net_error_id_t		error,	\
					 void			*pv)

typedef UDP_ERROR_CALLBACK(udp_error_callback_t);

/*
 * Connection descriptors container.
 */

OBJECT_TYPE(udp_desc_obj, SIMPLE, struct net_udp_desc_s);

struct					net_udp_desc_s
{
  /* local address of the descriptor */
  net_port_t				port;
  struct net_addr_s			address;
  bool_t				connected;
  bool_t				bound;

  /* error callback */
  udp_error_callback_t			*callback_error;
  void					*pv_error;

  /* used for bound descriptors */
  udp_callback_t			*callback;
  void					*pv;
  /* used for a connected descriptor */
  struct net_udp_addr_s			remote;
  struct net_route_s			*route;

  /* options */
  bool_t				checksum;

  udp_desc_obj_entry_t			obj_entry;
  CONTAINER_ENTRY_TYPE(HASHLIST)	list_entry;
};

OBJECT_CONSTRUCTOR(udp_desc_obj);
OBJECT_DESTRUCTOR(udp_desc_obj);
OBJECT_FUNC(udp_desc_obj, SIMPLE, static inline, udp_desc_obj, obj_entry);

#define CONTAINER_LOCK_udp_desc	HEXO_SPIN
CONTAINER_TYPE(udp_desc, HASHLIST, struct net_udp_desc_s, list_entry, 64);
CONTAINER_KEY_TYPE(udp_desc, PTR, SCALAR, port);

/*
 * Prototypes
 */

error_t	udp_connect(struct net_udp_desc_s	**desc,
		    struct net_udp_addr_s	*remote);
error_t	udp_bind(struct net_udp_desc_s		**desc,
		 struct net_udp_addr_s		*local,
		 udp_callback_t			*callback,
		 void				*pv);
error_t	udp_send(struct net_udp_desc_s		*desc,
		 struct net_udp_addr_s		*remote,
		 const void			*data,
		 size_t				size);
void	udp_close(struct net_udp_desc_s		*desc);

void	libudp_signal(struct net_packet_s	*packet,
		      struct udphdr		*hdr);
void	libudp_destroy(void);
NET_SIGNAL_ERROR(libudp_signal_error);

#endif
