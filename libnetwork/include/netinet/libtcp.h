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

#ifndef NETINET_LIBTCP_H
#define NETINET_LIBTCP_H

#ifndef CONFIG_NETWORK_TCP
# warning TCP support is not enabled in configuration file
#endif

#include <netinet/protos.h>
#include <netinet/packet.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_hashlist.h>
#include <gpct/cont_clist.h>
#include <gpct/object_simple.h>
#include <mutek/timer.h>

/*
 * A few constants
 */

#define TCP_CONNECTION_TIMEOUT	10000	/* milliseconds */
#define TCP_POLL_PERIOD		200	/* milliseconds */
#define TCP_RTO_FACTOR		1.5f	/* RTO computation factor */
#define TCP_BACKOFF_FACTOR	2	/* Karn's backoff factor */
#define TCP_RTT_FACTOR		0.125f	/* RTT weight factor */
#define TCP_RTO_MIN		1000	/* milliseconds */
#define TCP_RTO_MAX		10000	/* milliseconds */

/*
 * Forward decls.
 */

struct tcphdr;
struct net_tcp_session_s;

/*
 * A TCP endpoint
 */

struct	net_tcp_addr_s
{
  struct net_addr_s	address;
  uint_fast16_t		port;
};

/*
 * Callbacks
 */

#define TCP_CONNECT(f)	void (f)(struct net_tcp_session_s	*session,	\
				 void				*ptr)

typedef TCP_CONNECT(tcp_connect_t);

#define TCP_RECEIVE(f)	void (f)(struct net_tcp_session_s	*session,	\
				 const void			*data,		\
				 size_t				size,		\
				 void				*ptr)

typedef TCP_RECEIVE(tcp_receive_t);

#define TCP_CLOSE(f)	void (f)(struct net_tcp_session_s	*session,\
				 void				*ptr)

typedef TCP_CLOSE(tcp_close_t);

#define TCP_ACCEPT(f)	void (f)(struct net_tcp_session_s *session,	\
				 struct net_tcp_session_s *client,	\
				 void			*ptr)

typedef TCP_ACCEPT(tcp_accept_t);

/*
 * TCP segments and segment queues.
 */

struct					net_tcp_seg_s
{
  void					*data;
  size_t				size;
  uint_fast32_t				seq;

  CONTAINER_ENTRY_TYPE(CLIST)		list_entry;

  union
  {
    struct
    {
      struct net_tcp_session_s		*session;
      struct timer_event_s		timeout;
    }					send;
    struct
    {
      struct net_packet_s		*packet;
      bool_t				push;
    }					recv;
  }					u;
};

CONTAINER_TYPE(tcp_segment_queue, CLIST, struct net_tcp_seg_s, HEXO_SPIN_IRQ, NOOBJ, list_entry);

/*
 * This structure defines a TCP session.
 */

OBJECT_TYPE(tcp_session_obj, SIMPLE, struct net_tcp_session_s);

struct					net_tcp_session_s
{
  struct net_route_s			*route;
  struct net_tcp_addr_s			local;
  struct net_tcp_addr_s			remote;

  /* sequence, acks and windows */
  uint_fast32_t				curr_seq;
  uint_fast32_t				to_ack;
  uint_fast32_t				acked;
  uint_fast16_t				send_win;
  uint_fast16_t				send_mss;
  uint_fast32_t				recv_seq;
  uint_fast16_t				recv_win;
  uint_fast16_t				recv_mss;

  /* send & receive buffer */
  tcp_segment_queue_root_t		oos;		/* out-of-segment queue */
  tcp_segment_queue_root_t		unacked;	/* send but unacked queue */
  tcp_segment_queue_root_t		unsent;		/* unsent segments queue */
  uint8_t				*recv_buffer;
  uint_fast16_t				recv_offset;
  uint8_t				*send_buffer;
  uint_fast16_t				send_offset;
  struct timer_event_s			period;

  /* callbacks */
  tcp_connect_t				*connect;
  void					*connect_data;
  tcp_receive_t				*receive;
  void					*receive_data;
  tcp_close_t				*close;
  void					*close_data;
  tcp_accept_t				*accept;
  void					*accept_data;

  /* rtt & other variables */
  timer_delay_t				srtt;		/* smoothed round-trip time */
  bool_t				backoff;	/* Karn's backoff enabled */
  timer_delay_t				last_ack_time;	/* last ACK time */
  uint_fast32_t				last_ack;	/* last ACK value */
  uint_fast8_t				dup_acks;	/* number of duplicate ACKs */

  uint_fast8_t				state;

  tcp_session_obj_entry_t		obj_entry;
  CONTAINER_ENTRY_TYPE(HASHLIST)	list_entry;
};

OBJECT_CONSTRUCTOR(tcp_session_obj);
OBJECT_DESTRUCTOR(tcp_session_obj);
OBJECT_FUNC(static inline, tcp_session_obj, SIMPLE, tcp_session_obj, obj_entry);

/*
 * Container types for tcp session list.
 */

CONTAINER_TYPE(tcp_session, HASHLIST, struct net_tcp_session_s, NOLOCK, NOOBJ, list_entry, 64);
CONTAINER_KEY_TYPE(tcp_session, PTR, AGGREGATE, remote);

/*
 * Prototypes
 */

error_t	tcp_open(struct net_tcp_addr_s	*remote,
		 tcp_connect_t		callback,
		 void			*ptr);

void	tcp_close(struct net_tcp_session_s	*session);

void	tcp_on_receive(struct net_tcp_session_s	*session,
		       tcp_receive_t		*callback,
		       void			*ptr);

void	tcp_on_close(struct net_tcp_session_s	*session,
		     tcp_close_t		*callback,
		     void			*ptr);

void	tcp_on_accept(struct net_tcp_session_s	*session,
		      tcp_accept_t		*callback,
		      void			*ptr);

error_t	tcp_send(struct net_tcp_session_s	*session,
		 const uint8_t			*data,
		 size_t				size);

void	libtcp_push(struct net_packet_s	*packet,
		    struct tcphdr	*hdr);

NET_SIGNAL_ERROR(libtcp_signal_error);

#endif

