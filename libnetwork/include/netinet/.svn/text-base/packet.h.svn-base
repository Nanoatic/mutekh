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

#ifndef NETINET_PACKET_H_
#define NETINET_PACKET_H_

#if defined (CONFIG_NETWORK_PACKET_ASM)
# include <cpu/packet.h>
#endif

#include <hexo/types.h>
#include <mutek/mem_alloc.h>
#include <hexo/endian.h>
#include <hexo/lock.h>
#include <string.h>

/*
 * Memory access macros.
 */

#ifdef CONFIG_NETWORK_AUTOALIGN
/* when auto-align is activated, non-aligned accesses are useless */
# define net_le16_load(a)		endian_le16(a)
# define net_le32_load(a)		endian_le32(a)
# define net_le64_load(a)		endian_le64(a)
# define net_be16_load(a)		endian_be16(a)
# define net_be32_load(a)		endian_be32(a)
# define net_be64_load(a)		endian_be64(a)
# define net_le16_store(a, v)		(a = endian_le16(v))
# define net_le32_store(a, v)		(a = endian_le32(v))
# define net_le64_store(a, v)		(a = endian_le64(v))
# define net_be16_store(a, v)		(a = endian_be16(v))
# define net_be32_store(a, v)		(a = endian_be32(v))
# define net_be64_store(a, v)		(a = endian_be64(v))
# define net_16_load(a)			(a)
# define net_32_load(a)			(a)
# define net_64_load(a)			(a)
# define net_16_store(a, v)		(a = (v))
# define net_32_store(a, v)		(a = (v))
# define net_64_store(a, v)		(a = (v))
#else
/* otherwise, use non-aligned accesses */
# define net_le16_load(a)		endian_le16_na_load(&a)
# define net_le32_load(a)		endian_le32_na_load(&a)
# define net_le64_load(a)		endian_le64_na_load(&a)
# define net_be16_load(a)		endian_be16_na_load(&a)
# define net_be32_load(a)		endian_be32_na_load(&a)
# define net_be64_load(a)		endian_be64_na_load(&a)
# define net_le16_store(a, v)		endian_le16_na_store(&a, v)
# define net_le32_store(a, v)		endian_le32_na_store(&a, v)
# define net_le64_store(a, v)		endian_le64_na_store(&a, v)
# define net_be16_store(a, v)		endian_be16_na_store(&a, v)
# define net_be32_store(a, v)		endian_be32_na_store(&a, v)
# define net_be64_store(a, v)		endian_be64_na_store(&a, v)
# define net_16_load(a)			endian_16_na_load(&a)
# define net_32_load(a)			endian_32_na_load(&a)
# define net_64_load(a)			endian_64_na_load(&a)
# define net_16_store(a, v)		endian_16_na_store(&a, v)
# define net_32_store(a, v)		endian_32_na_store(&a, v)
# define net_64_store(a, v)		endian_64_na_store(&a, v)
#endif

#define net_16_na_load(a)		endian_16_na_load(&a)
#define net_32_na_load(a)		endian_32_na_load(&a)
#define net_64_na_load(a)		endian_64_na_load(&a)
#define net_16_na_store(a, v)		endian_16_na_store(&a, v)
#define net_32_na_store(a, v)		endian_32_na_store(&a, v)
#define net_64_na_store(a, v)		endian_64_na_store(&a, v)

/*
 * Maximum number of stages in our stack.
 */

#define NETWORK_MAX_STAGES	5

/*
 * Each packet is divided into one or more parts. The following
 * structure describes a subpacket.
 */

struct		net_header_s
{
  uint8_t	*data;	/* pointers to headers */
  uint_fast16_t	size;	/* size of subpackets */
};


#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>
#include <gpct/object_refcount.h>
#include <gpct/cont_dlist.h>

#include <netinet/protos.h>
#include <netinet/ether.h>

#include <mutek/semaphore.h>


/*
 * Address and mask structures and macros.
 */

enum	net_addr_e
  {
	addr_ipv4 = ETHERTYPE_IP
  };

struct			net_addr_s
{
  enum net_addr_e	family;
  union
  {
    uint_fast32_t	ipv4;
  } addr;
};

/*
 * IPv4 addresses manipulation macros.
 */

#define IPV4_ADDR_SET(_addr_,_ip_)					\
  do {									\
    (_addr_).family = addr_ipv4;					\
    (_addr_).addr.ipv4 = (_ip_);					\
  } while (0)

#define IPV4_ADDR_GET(_addr_)						\
  ({									\
    assert((_addr_).family == addr_ipv4);				\
    (_addr_).addr.ipv4;							\
  })

#define EXTRACT_IPV4(Addr)	\
  ((Addr) >> 24) & 0xff,	\
  ((Addr) >> 16) & 0xff,	\
  ((Addr) >> 8) & 0xff,		\
  ((Addr) >> 0) & 0xff

/*
 * This structure defines a packet.
 */

OBJECT_TYPE(packet_obj, REFCOUNT, struct net_packet_s);

struct				net_packet_s
{
  struct net_packet_s		*parent;
  struct net_header_s		header[NETWORK_MAX_STAGES];
  int_fast8_t			stage;		/* current stage */
  uint8_t			*packet;	/* raw packet */
  uint8_t			*sMAC;		/* source MAC address */
  const uint8_t			*tMAC;		/* target MAC address */
  struct net_if_s		*interface;
  struct net_proto_s		*source_addressing;
  struct net_addr_s		sADDR;		/* source protocol address */
  struct net_addr_s		tADDR;		/* target protocol address */
  uint_fast8_t			MAClen;		/* length of MAC addresses */
  uint_fast16_t			proto;		/* level 2 protocol id */

  packet_obj_entry_t		obj_entry;
  CONTAINER_ENTRY_TYPE(DLIST)	queue_entry;
};

OBJECT_CONSTRUCTOR(packet_obj);
OBJECT_DESTRUCTOR(packet_obj);
OBJECT_FUNC(packet_obj, REFCOUNT, static inline, packet_obj, obj_entry);

/*
 * Packet list.
 */

#define CONTAINER_OBJ_packet_queue	packet_obj
#define CONTAINER_LOCK_packet_queue	HEXO_SPIN_IRQ
CONTAINER_TYPE(packet_queue, DLIST, struct net_packet_s, queue_entry);

/*
 * The packet object.
 */

struct net_packet_s	*packet_dup(struct net_packet_s	*orig);
uint16_t		packet_checksum(const void	*data,
					size_t		size);
uint16_t		packet_memcpy(void		*dst,
				      const void	*src,
				      size_t		size);
void			*packet_dispatch(void	*data);

CONTAINER_PROTOTYPE(packet_queue, inline, packet_queue);
CONTAINER_PROTOTYPE(packet_queue, inline, packet_queue_lock);

/*
 * Profiling info.
 */

#ifdef CONFIG_NETWORK_PROFILING
#define NETWORK_PROFILING_IF		0
#define NETWORK_PROFILING_PROTO		1
#define NETWORK_PROFILING_ROUTE		2
#define NETWORK_PROFILING_PACKET	3
#define NETWORK_PROFILING_ARP_ENTRY	4
#define NETWORK_PROFILING_FRAGMENT	5
#define NETWORK_PROFILING_NB_OBJS	6

extern uint_fast32_t	netobj_new[NETWORK_PROFILING_NB_OBJS];
extern uint_fast32_t	netobj_del[NETWORK_PROFILING_NB_OBJS];

void	netprofile_show(void);
#endif

/*
 * Disable log messages. Replace by a printk.
 */

#if 0
# define net_debug(...) printk(__VA_ARGS__)
#else
# define net_debug(...)
#endif

#endif

