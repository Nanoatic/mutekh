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
 * User interface to TCP transport layer.
 *
 * TODO
 *  - out-of-segment
 *  - Nagle's (bufferisation)
 *  - Window + Clark's (evitement du SWS)
 *  - Karn's & RTT (retransmission)
 *  - Fast retransmit/Fast recovery
 *  - Van Jacobson's (congestion window)
 *  - Connexion rompues & RST
 *  - lock
 */

#include <hexo/types.h>
#include <mutek/mem_alloc.h>
#include <hexo/cpu.h>

#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/ip.h>
#include <netinet/if.h>
#include <netinet/in.h>
#include <netinet/ether.h>

#include <netinet/tcp.h>
#include <netinet/libtcp.h>

#include <mutek/timer.h>
#include <stdlib.h>

#undef net_debug
#define net_debug printk

/*
 * Session container.
 */

CONTAINER_FUNC(static inline, tcp_session, HASHLIST, tcp_session, NOLOCK, remote);
CONTAINER_KEY_FUNC(static inline, tcp_session, HASHLIST, tcp_session, NOLOCK, remote);

/*
 * Segment queues
 */

CONTAINER_FUNC(static inline, tcp_segment_queue, CLIST, tcp_segment_queue, HEXO_SPIN_IRQ);

/*
 * TCP session list.
 */

static tcp_session_root_t	sessions = CONTAINER_ROOT_INITIALIZER(tcp_session, HASHLIST, NOLOCK);

/*
 * Session objects
 */

OBJECT_CONSTRUCTOR(tcp_session_obj)
{
  struct net_tcp_session_s	*obj;

  if ((obj = mem_alloc(sizeof (struct net_tcp_session_s), (mem_scope_sys))) == NULL)
    return NULL;

  tcp_session_obj_init(obj);

  obj->route = NULL;
  tcp_segment_queue_init(&obj->unacked);
  tcp_segment_queue_init(&obj->unsent);
  tcp_segment_queue_init(&obj->oos);

  return obj;
}

OBJECT_DESTRUCTOR(tcp_session_obj)
{
  if (obj->route != NULL)
    route_obj_refdrop(obj->route);

  mem_free(obj);
}

/*
 * Insert a segment in a queue, sorted by sequence number.
 */

static error_t		insert_segment(tcp_segment_queue_root_t	*root,
				       struct net_tcp_seg_s	*segment)
{
  struct net_tcp_seg_s	*e;
  error_t		err = 0;

  for (e = tcp_segment_queue_head(root);
       e != NULL && segment->seq > e->seq;
       e = tcp_segment_queue_next(root, e))
    ;
  if (e == NULL)
    err = !tcp_segment_queue_pushback(root, segment);
  else
    err = !tcp_segment_queue_insert_pre(root, e, segment);

  return err;
}

/*
 * Close timeout.
 */

static TIMER_CALLBACK(tcp_close_session)
{
  struct net_tcp_session_s	*session = (struct net_tcp_session_s *)pv;

  tcp_session_remove(&sessions, session);
  tcp_session_obj_delete(session);

  net_debug("session deleted\n");

  mem_free(timer);
}

static void	tcp_close_timeout(struct net_tcp_session_s	*session)
{
  struct timer_event_s	*timer;

  /* cancel periodical timer */
  timer_cancel_event(&session->period, 0);

  if ((timer = mem_alloc(sizeof (struct timer_event_s), (mem_scope_sys))) == NULL)
    goto err;
  /* setup a timer */
  timer->callback = tcp_close_session;
  timer->pv = session;
  timer->delay = 2 * TCP_MSL;
  if (timer_add_event(&timer_ms, timer))
    goto err;

  return;

 err:
  /* not enough memory to reserve a timer, delete the session immediately */
  tcp_session_remove(&sessions, session);
  tcp_session_obj_delete(session);
}

/*
 * LibTCP user interface.
 */

/* on error */
static TIMER_CALLBACK(tcp_connect_error)
{
  struct net_tcp_session_s	*session = (struct net_tcp_session_s *)pv;

  /* set error state */
  session->state = TCP_STATE_ERROR;

  /* callback to report the error */
  session->connect(session, session->connect_data);

  tcp_close_timeout(session);
}

/*
 * Open a new TCP connection.
 */

error_t				tcp_open(struct net_tcp_addr_s	*remote,
					 tcp_connect_t		callback,
					 void			*ptr)
{
  struct net_tcp_session_s	*session;
  struct net_route_s		*route;

  /* check for a route */
  if ((route = route_get(&remote->address)) == NULL)
    return -EHOSTUNREACH;

  /* create session instance */
  if ((session = tcp_session_obj_new(NULL)) == NULL)
    return -ENOMEM;
  session->route = route;

  session->connect = callback;
  session->connect_data = ptr;
  session->receive = NULL;
  session->close = NULL;
  session->accept = NULL;
  session->backoff = 0;
  session->srtt = TCP_RTO_MIN;
  session->acked = 0;

  /* choose a local port */
  switch (remote->address.family)
    {
      case addr_ipv4:
	{
	  struct net_pv_ip_s	*pv_ip = (struct net_pv_ip_s *)session->route->addressing->pv;

	  session->local.port = 1024 + (rand() % 32768) ; /* XXX choose me better */
	  /* XXX check availability */
	  IPV4_ADDR_SET(session->local.address, pv_ip->addr);
	}
	break;
      default:
	assert(0); 	/* IPV6 */
	break;
    }

  memcpy(&session->remote, remote, sizeof (struct net_tcp_addr_s));

  session->curr_seq = rand();
  session->send_win = TCP_DFL_WINDOW;
  if ((session->recv_buffer = mem_alloc(session->send_win, (mem_scope_sys))) == NULL)
    goto err2;
  session->recv_offset = 0;
  session->send_mss = TCP_MSS;

  session->recv_mss = route->interface->mtu - TCP_HEADERS_LEN;

  /* enter SYN sent mode, waiting for SYN ACK */
  session->state = TCP_STATE_SYN_SENT;

  /* push the new session into the hashlist */
  if (!tcp_session_push(&sessions, session))
    goto err2;

  /* send the SYN packet */
  tcp_send_controlpkt(session, TCP_SYN);
  net_debug("<- SYN\n");

  /* setup a connection timeout */
  session->period.callback = tcp_connect_error;
  session->period.pv = session;
  session->period.delay = TCP_CONNECTION_TIMEOUT;
  if (timer_add_event(&timer_ms, &session->period))
    goto err;

  return 0;

 err:
  tcp_session_remove(&sessions, session);
 err2:
  tcp_session_obj_delete(session);
  return -ENOMEM;
}

/*
 * Close an opened TCP session.
 */

void			tcp_close(struct net_tcp_session_s	*session)
{
  if (session->state == TCP_STATE_ESTABLISHED || session->state == TCP_STATE_SYN_RCVD)
    {
      /* enter FIN WAIT-1 state, waiting for FIN ACK */
      session->state = TCP_STATE_FIN_WAIT1;

      /* just send a close request */
      tcp_send_controlpkt(session, TCP_FIN);
      session->curr_seq++;
      net_debug("<- FIN\n");
    }
  else if (session->state == TCP_STATE_CLOSE_WAIT)
    {
      /* goto LAST ACK state */
      session->state = TCP_STATE_LAST_ACK;

      /* just send a FIN */
      tcp_send_controlpkt(session, TCP_FIN);
      session->curr_seq++;
      net_debug("<- FIN\n");
    }
  else if (session->state != TCP_STATE_ERROR)
    {
      session->state = TCP_STATE_ERROR;

      if (session->close != NULL)
	session->close(session, session->close_data);

      /* closing on error, just remove the session */
      tcp_close_timeout(session);
    }
}

/*
 * Setup receiving callback.
 */

void	tcp_on_receive(struct net_tcp_session_s	*session,
		       tcp_receive_t		*callback,
		       void			*ptr)
{
  session->receive = callback;
  session->receive_data = ptr;
}

/*
 * Setup close callback.
 */

void	tcp_on_close(struct net_tcp_session_s	*session,
		     tcp_close_t		*callback,
		     void			*ptr)
{
  session->close = callback;
  session->close_data = ptr;
}

/*
 * Setup accept callback.
 */

void	tcp_on_accept(struct net_tcp_session_s	*session,
		      tcp_accept_t		*callback,
		      void			*ptr)
{
  session->accept = callback;
  session->accept_data = ptr;
}

/*
 * Retransmission timeout.
 */

static TIMER_CALLBACK(tcp_retransmission)
{
  struct net_tcp_seg_s		*seg = (struct net_tcp_seg_s *)pv;
  struct net_tcp_session_s	*session = seg->u.send.session;
  uint_fast16_t			size;
  uint_fast16_t			offset;

  if (seg->seq + seg->size <= session->last_ack)
    {
      /* segment has already been ACKed, no need to retransmit */
      mem_free(seg->data);
      mem_free(seg);

      return;
    }

  /* compute the offset and size of the segment to retransmit */
  if (seg->seq < session->last_ack)
    {
      offset = session->last_ack - seg->seq;
      size = seg->size - offset;
    }
  else
    {
      offset = 0;
      size = seg->size;
    }

  /* send the data packet */
  tcp_send_datapkt(session, seg->data + offset, size, TH_PUSH);
  net_debug("<- ACK + Retransmit data (size = %u)\n", size);

  /* setup the timeout again */
  timer_add_event(&timer_ms, &seg->u.send.timeout);
}

/*
 * Enqueue a data segment for sending
 */

static error_t		tcp_enqueue_send_buffer(struct net_tcp_session_s	*session)
{
  struct net_tcp_seg_s	*seg;

  if ((seg = mem_alloc(sizeof (struct net_tcp_seg_s), (mem_scope_sys))) == NULL)
    return -ENOMEM;

  seg->data = session->send_buffer;
  seg->size = session->send_offset;
  seg->u.send.session = session;

  tcp_segment_queue_push(&session->unsent, seg);

  net_debug("queueing a new segment of data (size = %u)\n", seg->size);

  /* allocate a new buffer */
  session->send_offset = 0;
  if ((session->send_buffer = mem_alloc(session->send_mss, (mem_scope_sys))) == NULL)
    return -ENOMEM;

  return 0;
}

/*
 * Send a segment or an ACK if no pending data.
 */

static void		tcp_do_send(struct net_tcp_session_s	*session)
{
  struct net_tcp_seg_s	*seg;
  timer_delay_t		rto;

  /* is there a waiting segment ? */
  if ((seg = tcp_segment_queue_pop(&session->unsent)) == NULL)
    {
      /* look at the send buffer */
      if (session->send_buffer != NULL && session->send_offset != 0)
	{
	  tcp_enqueue_send_buffer(session);
	  seg = tcp_segment_queue_pop(&session->unsent);
	}
      else
	/* no, so just send a control packet (ACK) */
	if (session->to_ack != session->acked)
	  {
	    tcp_send_controlpkt(session, TCP_ACK);
	    net_debug("<- ACK\n");
	    return;
	  }
    }

  /* enough space in receiver's window ? */
  if (session->recv_win < session->recv_mss)
    return;

  /* send the data packet */
  tcp_send_datapkt(session, seg->data, seg->size, TH_PUSH);
  net_debug("<- ACK + data (size = %u)\n", seg->size);

  /* increment the sequence number */
  seg->seq = session->curr_seq;
  session->curr_seq += seg->size;

  /* retransmission timeout */
  rto = TCP_RTO_FACTOR * session->srtt;
  if (rto > TCP_RTO_MAX)
    rto = TCP_RTO_MAX;
  if (rto < TCP_RTO_MIN)
    rto = TCP_RTO_MIN;

  if (session->backoff)
    rto *= TCP_BACKOFF_FACTOR;

  net_debug("  RTO = %u\n", rto);

      /* add timer */
  seg->u.send.timeout.callback = tcp_retransmission;
  seg->u.send.timeout.pv = seg;
  seg->u.send.timeout.delay = rto;
  timer_add_event(&timer_ms, &seg->u.send.timeout);

  /* push into unacked queue */
  insert_segment(&session->unacked, seg);
}

/*
 * TCP periodical poll timer
 */

static TIMER_CALLBACK(tcp_period)
{
  struct net_tcp_session_s	*session = (struct net_tcp_session_s *)pv;

  if (session->state == TCP_STATE_ESTABLISHED)
    tcp_do_send(session);
}

/*
 * Send data using given TCP connection.
 */

error_t			tcp_send(struct net_tcp_session_s	*session,
				 const uint8_t			*data,
				 size_t				size)
{
  uint_fast16_t		i;
  uint_fast16_t		chunksz;

  if (session->state != TCP_STATE_ESTABLISHED)
    return -ENOTCONN;

  /* if no send buffer was allocated */
  if (session->send_buffer == NULL)
    {
      if ((session->send_buffer = mem_alloc(session->send_mss, (mem_scope_sys))) == NULL)
	return -ENOMEM;
    }

  net_debug("tcp_send: %P\n", data, size);

  for (i = 0; i < size; )
    {
      register uint_fast16_t	sz1 = size - i;
      register uint_fast16_t	sz2 = session->send_mss - session->send_offset;

      if (sz1 > sz2)
	chunksz = sz2;
      else
	chunksz = sz1;

      /* fill the send buffer */
      memcpy(session->send_buffer + session->send_offset, data + i, chunksz);
      i += chunksz;
      session->send_offset += chunksz;

      /* segment buffer full ? */
      if (session->send_offset == session->send_mss)
	{
	  /* enqueue the packet in the send queue */
	  tcp_enqueue_send_buffer(session);
	}
    }

  return 0;
}

/*
 * Interface with stack's TCP module.
 */

/*
 * Called on incoming connection.
 */

static void			libtcp_open(struct net_tcp_session_s	*session,
					    struct net_packet_s		*packet,
					    struct tcphdr		*hdr)
{
  if (session->state == TCP_STATE_SYN_SENT)
    {
      tcp_connect_t	*callback = session->connect;
      void		*ptr = session->connect_data;

      /* get mss if present */
      if (hdr->th_off > 5)
	{
	  uint32_t	opt;

	  opt = net_be32_load(*(uint32_t *)(hdr + 1));
	  if (opt & (2 << 24))
	    session->send_mss = opt & 0xffff;
	}

      /* make the transition */
      if (hdr->th_flags & TH_ACK)
	{
	  net_debug("-> SYN ACK\n");

	  /* ok, connection aknowleged */
	  session->state = TCP_STATE_ESTABLISHED;

	  /* cancel timeout */
	  timer_cancel_event(&session->period, 0);

	  /* get the sender seq & win */
	  session->recv_seq = net_be32_load(hdr->th_seq);
	  session->recv_win = net_be16_load(hdr->th_win);
	  session->last_ack = net_be32_load(hdr->th_ack);
	  net_debug("last_ack = %u\n", session->last_ack);
	  session->last_ack_time = timer_get_tick(&timer_ms);
	  session->dup_acks = 1;
	  session->send_buffer = NULL;
	  session->send_offset = 0;

	  /* increment seq and set ack */
	  session->to_ack = session->recv_seq + 1;

	  net_debug("<> ESTABLISHED\n");
	  net_debug("  send MSS = %u\n", session->send_mss);
	  net_debug("  recv MSS = %u\n", session->recv_mss);

	  /* send ACK */
	  net_debug("<- ACK\n");
	  session->curr_seq++;
	  tcp_send_controlpkt(session, TCP_ACK);

	  callback(session, ptr);

	  /* start periodical timeout */
	  session->period.delay = TCP_POLL_PERIOD;
	  session->period.callback = tcp_period;
	  session->period.pv = session;
	  timer_add_event(&timer_ms, &session->period);
	}
      else
	{
	  net_debug("-> SYN\n");

	  /* enter SYN RCVD state  */
	  session->state = TCP_STATE_SYN_RCVD;

	  /* send ACK */
	  net_debug("<- ACK\n");
	  session->curr_seq++;
	  tcp_send_controlpkt(session, TCP_ACK);
	}

    }
  else if (session->state == TCP_STATE_LISTEN) /* incoming connection request */
    {
      struct net_tcp_session_s	*new;

      net_debug("-> SYN\n");

      /* enter SYN RCVD state  */
      session->state = TCP_STATE_SYN_RCVD;

      /* send SYN ACK */
      net_debug("<- SYN ACK\n");
      tcp_send_controlpkt(session, TCP_SYN_ACK);

      /* create a new session */
      if ((new = tcp_session_obj_new(NULL)) == NULL)
	return;

      /* XXX fill me */

      /* push the new session into the hashlist */
      tcp_session_push(&sessions, new);

      /* callback */
      if (session->accept != NULL)
	session->accept(session, new, session->accept_data);
    }
}

/*
 * Called on remote connection closing.
 */

static void			libtcp_close(struct net_tcp_session_s	*session,
					     struct net_packet_s	*packet,
					     struct tcphdr		*hdr)
{
  switch (session->state)
    {
      case TCP_STATE_SYN_SENT: /* error when opening */
      case TCP_STATE_SYN_RCVD:
	net_debug("-> FIN (while connecting)\n");

	tcp_send_controlpkt(session, TCP_FIN);
	net_debug("<- FIN ACK\n");

	/* set error state */
	session->state = TCP_STATE_ERROR;

	/* callback to report the error */
	session->connect(session, session->connect_data);

	tcp_close_timeout(session);
	break;
      case TCP_STATE_FIN_WAIT1: /* it is a FIN acknowlegment */
	/* goto CLOSING */
	session->state = TCP_STATE_CLOSING;

	if (!(hdr->th_flags & TH_ACK))
	  {
	    net_debug("-> FIN\n");

	    /* send a ACK */
	    session->to_ack++;
	    tcp_send_controlpkt(session, TCP_FIN);
	    net_debug("<- ACK\n");
	    break;
	  }
	/* if FIN ACK, directly goto CLOSING state */
	net_debug("-> FIN ACK\n");
      case TCP_STATE_LAST_ACK:
      case TCP_STATE_CLOSING:
      case TCP_STATE_FIN_WAIT2:
	/* send a ACK */
	session->to_ack++;
	tcp_send_controlpkt(session, TCP_FIN);
	net_debug("<- ACK\n");

	session->state = TCP_STATE_CLOSED;

	if (session->close != NULL)
	  session->close(session, session->close_data);

	/* delete session */
	tcp_close_timeout(session);
	break;
      case TCP_STATE_ERROR:
	break;
      default:
	/* otherwise, it is a FIN request */
	net_debug("-> FIN\n");

	/* send a ACK */
	session->to_ack++;
	tcp_send_controlpkt(session, TCP_ACK);
	net_debug("<- ACK\n");

	/* no data remaining, close the connection */
	if (session->send_offset == 0)
	  {
	    net_debug("<- FIN\n");
	    tcp_send_controlpkt(session, TCP_FIN);

	    session->state = TCP_STATE_CLOSED;

	    if (session->close != NULL)
	      session->close(session, session->close_data);

	    tcp_close_timeout(session);
	  }
	else /* otherwise, wait for the remaining operations and change state */
	  session->state = TCP_STATE_CLOSE_WAIT;
    }
}

/*
 * Push some incoming data.
 */

static inline void		tcp_push_data(struct net_tcp_session_s	*session,
					      uint8_t			*data,
					      size_t			length,
					      bool_t			push)
{
  uint_fast16_t			i;
  uint_fast16_t			chunksz;
  register uint_fast16_t	sz1;

  net_debug("-> %u bytes of data\n", length);

  for (i = 0; i < length; )
    {
      /* push the data into the receive buffer */
      sz1 = length - i;
      if (sz1 > session->send_win)
	chunksz = session->send_win;
      else
	chunksz = sz1;
      memcpy(session->recv_buffer + session->recv_offset, data + i, chunksz);
      i += chunksz;
      session->recv_offset += chunksz;
      session->send_win -= chunksz;

      /* deliver data to application */
      if (push || session->send_win == 0)
	{
	  if (push)
	    net_debug("  PUSH flag present\n");
	  else
	    net_debug("  Buffer is full, pushing to application\n");

	  if (session->receive != NULL)
	    session->receive(session, data, length, session->receive_data);

	  /* update window */
	  session->send_win = TCP_DFL_WINDOW; /* XXX SWS avoidment (Clark) */
	  session->recv_offset = 0;
	}
    }
}

/*
 * Acknowledge previously sent TCP segments.
 * Compute Round-Time Trip.
 */

static void			tcp_acknowledge(struct net_tcp_session_s	*session)
{
  struct net_tcp_seg_s		*seg;

  /* look into the unacked queue for segment to acknowledge */
  while ((seg = tcp_segment_queue_head(&session->unacked)) != NULL)
    {
      //      net_debug("%u >= %u + %u\n", session->last_ack, seg->seq, seg->size);

      if (session->last_ack >= seg->seq + seg->size)
	{
	  timer_delay_t	rtt;

	  net_debug("  Acknowledging segment %u\n", seg->seq);

	  /* we can remove it */
	  tcp_segment_queue_pop(&session->unacked);

	  /* compute SRTT XXX not on retransmission */
	  if (!session->backoff)
	    {
	      rtt = timer_get_tick(&timer_ms) - seg->u.send.timeout.start;
	      session->srtt = (1 - TCP_RTT_FACTOR) * session->srtt + TCP_RTT_FACTOR * rtt;
	      net_debug("Adjusting SRTT to %u ms\n", session->srtt);
	    }

	  /* cancel associated timer */
	  timer_cancel_event(&seg->u.send.timeout, 0);

	  /* free memory */
	  mem_free(seg->data);
	  mem_free(seg);
	}
    }
}

/*
 * Called on packet incoming (data or control).
 */

void				libtcp_push(struct net_packet_s	*packet,
					    struct tcphdr	*hdr)
{
  struct net_tcp_session_s	*session;
  struct net_tcp_addr_s		key;
  struct net_header_s		*nethdr;
  struct net_tcp_seg_s		*seg;
  size_t			length;
  uint_fast32_t			seq;
  uint_fast32_t			curr_ack;
  bool_t			force_ack = 0;

  /* look for the corresponding session */
  memcpy(&key.address, &packet->sADDR, sizeof (struct net_addr_s));
  key.port = net_16_load(hdr->th_sport);

  if ((session = tcp_session_lookup(&sessions, (void *)&key)) == NULL ||
      session->state == TCP_STATE_ERROR)
    return ;

  /* get the header */
  nethdr = &packet->header[packet->stage];
  length = nethdr->size - hdr->th_off * 4;

  seq = net_be32_load(hdr->th_seq);

  /* check for out-of-segment packet */
  if (session->state == TCP_STATE_ESTABLISHED && seq > session->to_ack)
    {
      /* the packet is out-of-segment */
      uint8_t			*data = (nethdr->data + hdr->th_off * 4);

      net_debug("-> out-of-segment data (SEQ = %u)\n", seq);

      seg = mem_alloc(sizeof (struct net_tcp_seg_s), (mem_scope_sys)); /* XXX opt malloc size */
      seg->data = data;
      seg->size = length;
      seg->seq = seq;

      /* keep a ref to the packet to avoid copying it */
      seg->u.recv.packet = packet_dup(packet);
      seg->u.recv.push = hdr->th_flags & TH_PUSH;

      insert_segment(&session->oos, seg);

      /* send ack (initiate fast retransmit) */
      tcp_do_send(session);

      return;
    }

  /* update sequence numbers */
  session->recv_seq = seq;
  session->to_ack = session->recv_seq + length;

  /* update ack and dup_acks */
  curr_ack = net_be32_load(hdr->th_ack);

  net_debug("curr_ack = %u\n", curr_ack);

  if (session->state == TCP_STATE_ESTABLISHED && session->last_ack == curr_ack)
    {
      if (++session->dup_acks == 3)
	{
	  /* XXX fast retransmit */
	}
    }
  else
    {
      net_debug("last_ack = %u\n", curr_ack);

      session->last_ack = curr_ack;
      session->dup_acks = 1;
    }
  session->recv_win = net_be16_load(hdr->th_win);

  /* ACK queed packets and compute RTT */
  tcp_acknowledge(session);

  /* check for transition from SYN RCVD to ESTABLISHED */
  if (session->state == TCP_STATE_SYN_RCVD)
    {
      net_debug("-> ACK\n");

      net_debug("<> ESTABLISHED\n");
      net_debug("  send MSS = %u\n", session->send_mss);
      net_debug("  recv MSS = %u\n", session->recv_mss);

      session->state = TCP_STATE_ESTABLISHED;
    }

  /* check for SYN flag */
  if (hdr->th_flags & TH_SYN)
    libtcp_open(session, packet, hdr);

  /* control packet */
  if (nethdr->size == hdr->th_off * 4)
    {
      /* XXX nothing here ?? */
    }
  else /* data packet */
    {
      uint8_t			*data = (nethdr->data + hdr->th_off * 4);

      tcp_push_data(session, data, length, hdr->th_flags & TH_PUSH);

      /* try to pop segments from oos */
      while ((seg = tcp_segment_queue_head(&session->oos)) != NULL)
	{
	  /* can we pop one of the out-of-segment buffers */
	  if (seg->seq <= session->to_ack)
	    {
	      uint_fast16_t	offset;
	      uint_fast16_t	size;

	      net_debug("  Pushing-in an out-of-segment packet (SEQ = %u)\n", seg->seq);

	      /* remove it */
	      tcp_segment_queue_pop(&session->oos);

	      /* if the segment is useless, just ignore it */
	      if (session->to_ack > seg->seq + seg->size)
		goto next;

	      /* determine the amout of data we can get */
	      offset = session->to_ack - seg->seq;
	      size = seg->size - offset;

	      /* pick the data */
	      tcp_push_data(session, seg->data + offset, size, seg->u.recv.push);

	      /* update sequence numbers */
	      session->recv_seq = seg->seq;
	      session->to_ack = seg->seq + size;

	      /* we filled a gap, force an ACK to be sent */
	      force_ack = 1;
	    }
	  else
	    /* no segment is useful at this point */
	    break;

	next:
	  packet_obj_refdrop(seg->u.recv.packet);
	  mem_free(seg);
	}

      /* if needed, send a control packet to acknowledge */
      if (force_ack && !(hdr->th_flags & TH_FIN))
	{
	  tcp_send_controlpkt(session, TCP_ACK);
	  net_debug("<- ACK\n");
	}
    }

  /* check for FIN flag */
  if (hdr->th_flags & TH_FIN)
    libtcp_close(session, packet, hdr);
}

/*
 * Error reception callback.
 */

NET_SIGNAL_ERROR(libtcp_signal_error)
{
  /* XXX -> source quench should start Van Jacobson recovery */
}
