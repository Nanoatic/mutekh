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

#include <hexo/endian.h>
#include <hexo/types.h>

#include <mutek/scheduler.h>
#include <mutek/semaphore.h>
#include <hexo/context.h>
#include <hexo/lock.h>

#include <netinet/packet.h>
#include <netinet/protos.h>
#include <netinet/if.h>

#include <mutek/printk.h>

#define CONFIG_NET_DISPATCH_STACK_SIZE 10240

/*
 * Used to give info to the dispatch thread.
 */

struct net_dispatch_s
{
	struct sched_context_s context;
	struct sched_context_s *killer;
	bool_t                 must_quit;
	uint8_t                stack[CONFIG_NET_DISPATCH_STACK_SIZE];

    struct net_if_s     *interface;

    packet_queue_root_t queue;
	struct semaphore_s sem;
};

/*
 * packet dispatching thread.
 */

static CONTEXT_ENTRY(packet_dispatch_thread)
{
	struct net_dispatch_s *dispatch = (struct net_dispatch_s *)param;
	struct net_packet_s *packet;

	net_if_obj_refnew(dispatch->interface);

	sched_unlock();
	cpu_interrupt_enable();

	net_debug("[%s] In dispatch thread, pv=%p\n", dispatch->interface->name, dispatch);

	while (1)
	{
		/* wait for a packet, or termination */
		net_debug("[%s] dispatch waiting for packet\n", dispatch->interface->name);
		semaphore_take(&dispatch->sem, 1);
		net_debug("[%s] wakeup\n", dispatch->interface->name);

		packet = packet_queue_pop(&dispatch->queue);
		if ( packet != NULL ) {
			net_debug("[%s] handling %p\n", dispatch->interface->name, packet);
			if_pushpkt(dispatch->interface, packet);
			packet_obj_refdrop(packet);
		} else {
			if (dispatch->must_quit) {
				net_if_obj_refdrop(dispatch->interface);
				sched_context_start(dispatch->killer);
				sched_lock();
				sched_context_exit();
			}
		}
	}
}

static inline void dispatch_wakeup(struct net_dispatch_s *dispatch, struct net_packet_s *packet)
{
	net_debug("[%s] pushing %p\n", dispatch->interface->name, packet);

	packet_queue_pushback(&dispatch->queue, packet);
	semaphore_give(&dispatch->sem, 1);
}

struct net_dispatch_s *network_dispatch_create(struct net_if_s *interface)
{
	struct net_dispatch_s *dispatch =
		mem_alloc(sizeof(struct net_dispatch_s), mem_scope_sys);

	if ( dispatch == NULL )
		return NULL;

	dispatch->interface = net_if_obj_refnew(interface);

	net_debug("[%s] Creating dispatch thread, pv=%p\n", dispatch->interface->name, dispatch);

	semaphore_init(&dispatch->sem, 0);
	dispatch->must_quit = 0;
	dispatch->killer = NULL;

	packet_queue_init(&dispatch->queue);

	CPU_INTERRUPT_SAVESTATE_DISABLE;
	context_init( &dispatch->context.context,
				  &dispatch->stack[0],
				  &dispatch->stack[CONFIG_NET_DISPATCH_STACK_SIZE],
				  packet_dispatch_thread,
				  dispatch );
	sched_context_init( &dispatch->context );
	sched_context_start( &dispatch->context );
	CPU_INTERRUPT_RESTORESTATE;

	return dispatch;
}

void network_dispatch_kill(struct net_dispatch_s *dispatch)
{
	/* Signal the thread for termination... */
	dispatch->killer = sched_get_current();
	dispatch->must_quit = 1;

	semaphore_give(&dispatch->sem, 1);

	/* and wait it actually stops */
	CPU_INTERRUPT_SAVESTATE_DISABLE;
	sched_context_stop();
	CPU_INTERRUPT_RESTORESTATE;

	packet_queue_destroy(&dispatch->queue);

	net_if_obj_refdrop(dispatch->interface);

	mem_free(dispatch);
}

void network_dispatch_packet(struct net_dispatch_s *dispatch,
							 struct net_packet_s *packet)
{
	packet_obj_refnew(packet);

	dispatch_wakeup(dispatch, packet);
}

void network_dispatch_data(
	struct net_dispatch_s *dispatch,
	void *data,
	uint_fast16_t size)
{
	struct ether_header *hdr;
	struct net_packet_s *packet;
	struct net_header_s *nethdr;

	/* create a new packet object */
	packet = packet_obj_new(NULL);
	packet->packet = data;

	nethdr = &packet->header[0];
	nethdr->data = data;
	nethdr->size = size;

	/* get the good header */
	hdr = (struct ether_header*)nethdr->data;

	/* fill some info */
	packet->MAClen = sizeof(struct ether_addr);
	packet->sMAC = hdr->ether_shost;
	packet->tMAC = hdr->ether_dhost;
	packet->proto = net_be16_load(hdr->ether_type);

	/* prepare packet for next stage */
	nethdr[1].data = data + sizeof(struct ether_header);
	nethdr[1].size = size - sizeof(struct ether_header);

	packet->stage++;

	dispatch_wakeup(dispatch, packet);
}
