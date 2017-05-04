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
 * Ping function
 */

#include <hexo/error.h>

#include <netinet/if.h>
#include <netinet/in.h>
#include <netinet/icmp.h>
#include <netinet/socket.h>

#include <limits.h>
#include <mutek/timer.h>
#include <mutek/printk.h>

#include <netinet/ping.h>

error_t			ping(struct net_addr_s	*host,
			     uint_fast32_t	count,
			     size_t		size,
			     struct ping_s	*stat)
{
  struct sockaddr_in	addr;
  uint8_t		*buf1;
  uint8_t		*buf2;
  struct icmphdr	*hdr;
  size_t		i;
  socket_t		sock;
  uint_fast32_t		id;
  uint_fast32_t		seq;
  struct sockaddr_in	from;
  socklen_t		len;
  ssize_t		recvd;
  timer_delay_t		*t1;
  timer_delay_t		*t2;
  timer_delay_t		timeout;
  size_t		tot;
  struct icmp_filter	filt;

  /* reset the statistics */
  memset(stat, 0, sizeof (struct ping_s));
  stat->min = UINT_MAX;

  /* allocate and create the message */
  if ((buf1 = mem_alloc(size + sizeof (struct icmphdr), (mem_scope_sys))) == NULL)
    return -1;

  if ((buf2 = mem_alloc(size + sizeof (struct icmphdr), (mem_scope_sys))) == NULL)
    {
      mem_free(buf1);
      return -1;
    }

  /* fill with junk bytes */
  for (i = 0; i < size - sizeof (timer_delay_t); i++)
    buf1[sizeof (struct icmphdr) + sizeof (timer_delay_t) + i] = 32 + i % 96;

  t1 = (timer_delay_t *)&buf1[sizeof (struct icmphdr)];
  t2 = (timer_delay_t *)&buf2[sizeof (struct icmphdr)];

  /* fill icmp header */
  hdr = (struct icmphdr *)buf1;
  id = (uint32_t)hdr;
  id = (id & 0xffff) + (id >> 16);

  hdr->type = 8;
  hdr->code = 0;
  hdr->un.echo.id = htons(id);
  seq = hdr->un.echo.sequence = 0;

  /* open a SOCK_RAW socket and connect it */
  if ((sock = socket(PF_INET, SOCK_RAW, htons(IPPROTO_ICMP))) == NULL)
    {
      mem_free(buf1);
      mem_free(buf2);
      return -1;
    }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(IPV4_ADDR_GET(*host));

  /* filter incoming ICMP responses */
  filt.data = ~(ICMP_ECHOREPLY | ICMP_DEST_UNREACH | ICMP_TIME_EXCEEDED);
  if (setsockopt(sock, SOL_RAW, ICMP_FILTER, &filt, sizeof (struct icmp_filter)) < 0)
    {
      shutdown(sock, SHUT_RDWR);
      mem_free(buf1);
      mem_free(buf2);
      return -1;
    }

  if (connect(sock, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) < 0)
    {
      shutdown(sock, SHUT_RDWR);
      mem_free(buf1);
      mem_free(buf2);
      return -1;
    }

  printk("PING %u.%u.%u.%u %u bytes of data.\n", addr.sin_addr.s_addr & 0xff,
	 (addr.sin_addr.s_addr >> 8) & 0xff, (addr.sin_addr.s_addr >> 16) & 0xff,
	 (addr.sin_addr.s_addr >> 24) & 0xff, size);

  tot = size + sizeof (struct icmphdr);

  /* send successive ping */
  for (seq = 0; seq < count; seq++)
    {
      ssize_t	sent;

      hdr = (struct icmphdr *)buf1;

      /* sequence number */
      hdr->un.echo.sequence = htons(seq);

      /* setup start time */
      *t1 = timer_get_tick(&timer_ms);

      /* compute checksum */
      hdr->checksum = 0;
      hdr->checksum = ~packet_checksum(buf1, tot);

      /* send ! */
      sent = send(sock, buf1, tot, 0);

      if (sent != tot)
	printk("Error (sent = %d)\n", sent);

      /* start timeout */
      timeout = timer_get_tick(&timer_ms);

      /* wait for echo */
      while (1)
	{
	  len = sizeof (struct sockaddr_in);
	  recvd = recvfrom(sock, buf2, tot, MSG_DONTWAIT, (struct sockaddr *)&from, &len);

	  if (recvd > 0)
	    {
	      hdr = (struct icmphdr *)buf2;

	      /* if valid echo reply */
	      if (hdr->type == ICMP_ECHOREPLY && ntohs(hdr->un.echo.id) == id &&
		  from.sin_addr.s_addr == addr.sin_addr.s_addr)
		{
		  timer_delay_t	t = timer_get_tick(&timer_ms) - *t2;

		  /* check size */
		  if (recvd != tot)
		    {
		      printk("Reply %d bytes (instead of %d)\n", recvd, tot);
		      break;
		    }

		  /* check data */
		  for (i = 0; i < size - sizeof (timer_delay_t); i++)
		    if (buf2[sizeof (struct icmphdr) + sizeof (timer_delay_t) + i] != 32 + i % 96)
		      {
			printk("Reply %d bytes with incorrect data\n", recvd);
			break;
		      }

		  if (t < stat->min)
		    stat->min = t;
		  if (t > stat->max)
		    stat->max = t;
		  stat->avg += t;

		  printk("Reply %d bytes from %u.%u.%u.%u: seq=%d time=%u ms\n", recvd,
			 addr.sin_addr.s_addr & 0xff, (addr.sin_addr.s_addr >> 8) & 0xff,
			 (addr.sin_addr.s_addr >> 16) & 0xff, (addr.sin_addr.s_addr >> 24) & 0xff,
			 ntohs(hdr->un.echo.sequence), t);
		  break;
		}
	      else if (hdr->type != 0)
		{
		  /* ICMP error */
		  switch (hdr->type)
		    {
		      case ICMP_DEST_UNREACH:
			switch (hdr->code)
			  {
			    case ICMP_NET_UNREACH:
			      printk("Reply: Network unreachable\n");
			      break;
			    case ICMP_HOST_UNREACH:
			      printk("Reply: Host unreachable\n");
			      break;
			    case ICMP_PROT_UNREACH:
			      printk("Reply: Protocol unreachable\n");
			      break;
			    case ICMP_FRAG_NEEDED:
			      printk("Reply: Cannot fragment (Next HOP MTU = %u)\n",
				     ntohs(hdr->un.frag.mtu));
			      break;
			    default:
			      printk("Reply: Destination unreachable\n");
			      break;
			  }
			break;
		      case ICMP_TIME_EXCEEDED:
			switch (hdr->code)
			  {
			    case ICMP_EXC_TTL:
			      printk("Reply: Timeout (ttl has reached 0)\n");
			      break;
			    case ICMP_EXC_FRAGTIME:
			      printk("Reply: Reassembly timeout\n");
			      break;
			    default:
			      printk("Reply: Timeout\n");
			      break;
			  }
			break;
		      default:
			printk("Reply: unknown error\n");
			break;
		    }
		  stat->error++;
		  break;
		}
	    }

	  /* check for timeout */
	  if ((timer_get_tick(&timer_ms) - timeout) >= PING_TIMEOUT)
	    {
	      stat->lost++;
	      printk("No reply\n");
	      break;
	    }
	}

      /* update statistics */
      stat->total++;
    }

  if (stat->total)
    stat->avg /= stat->total;

  shutdown(sock, SHUT_RDWR);
  mem_free(buf1);
  mem_free(buf2);

  return 0;
}
