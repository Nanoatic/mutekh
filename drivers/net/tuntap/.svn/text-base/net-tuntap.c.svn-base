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
 * Linux simulation. TUN/TAP driver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hexo/types.h>

#include <device/icu.h>
#include <device/net.h>
#include <device/device.h>
#include <device/driver.h>
#include <hexo/endian.h>

#include <pthread.h>
#include <semaphore.h>

#include <netinet/if.h>

#include <arch/hexo/emu_syscalls.h>

#include "net-tuntap.h"
#include "net-tuntap-private.h"

/*
 * Driver operations vector.
 */

const struct driver_s	net_tuntap_drv =
{
  .class		= device_class_net,
  .id_table		= NULL,
  .f_init		= net_tuntap_init,
  .f_cleanup		= net_tuntap_cleanup,
  .f_irq		= net_tuntap_irq,
  .f.net = {
    .f_preparepkt	= net_tuntap_preparepkt,
    .f_sendpkt		= net_tuntap_sendpkt,
    .f_setopt		= net_tuntap_setopt,
    .f_getopt		= net_tuntap_getopt,
  }
};

static uint32_t	tap_id = 0;

/*
 * Emulated IRQ
 */

DEV_IRQ(net_tuntap_irq)
{
  return 1;
}

/*
 * Packet receive thread.
 */

void	*net_tuntap_recv(void *p)
{
  struct device_s		*dev = (struct device_s *)p;
  struct net_tuntap_context_s	*pv = (struct net_tuntap_context_s *)dev->drv_pv;
  uint8_t			buff[1514];
  ssize_t			size;
  uint8_t			*ptr;

  while (1)
    {
      size = (size_t)emu_do_syscall(EMU_SYSCALL_READ, 3, pv->fd, buff, sizeof (buff));

      if (size > 0)
	{
	  ptr = mem_alloc(size, (mem_scope_sys));
	  memcpy(ptr, buff, size);

	  network_dispatch_data(pv->dispatch, ptr, size);
	}
    }
}

/*
 * Init
 */

DEV_INIT(net_tuntap_init)
{
  struct net_tuntap_context_s	*pv;
  pthread_t			th;
  int_fast32_t			tun;
  struct ifreq			ifr;
  const int			true = 1;

  dev->drv = &net_tuntap_drv;

  printk("tuntap driver init on device %p\n", dev);

  /* driver private data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  dev->drv_pv = pv;

  lock_init(&pv->lock);

  /* create tun */
  memset(&ifr, 0, sizeof(ifr));
  if ((tun = (int_fast32_t)emu_do_syscall(EMU_SYSCALL_OPEN, 2, "/dev/net/tun", O_RDWR)) == -1)
    {
      printk("tuntap: cannot open /dev/net/tun\n");

      net_tuntap_cleanup(dev);

      return -1;
    }
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
  memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
  sprintf(ifr.ifr_name, "tap%d", tap_id++);
  printk(" using %s\n", ifr.ifr_name);
  if((int_fast32_t)emu_do_syscall(EMU_SYSCALL_IOCTL, 3, tun, TUNSETIFF, (void *) &ifr) < 0)
    {
      printk("tuntap: cannot add tap interface\n");

      net_tuntap_cleanup(dev);

      return -1;
    }
  pv->fd = tun;
  ioctl(pv->fd, TUNSETNOCSUM, &true);

  /* get mac */
  if ((tun = (int_fast32_t)emu_do_syscall(EMU_SYSCALL_SOCKET, 3, PF_INET, SOCK_DGRAM, 0)) == -1)
    {
      printk("tuntap: cannot create socket\n");

      net_tuntap_cleanup(dev);

      return -1;
    }
  if ((int_fast32_t)emu_do_syscall(EMU_SYSCALL_IOCTL, 3, tun, SIOCGIFHWADDR, (void *) &ifr) < 0)
    {
      printk("tuntap: cannot get MAC address\n");

      net_tuntap_cleanup(dev);

      return -1;
    }
  memcpy(pv->mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

  printk("HW addr: %P\n", pv->mac, ETH_ALEN);

  /* register as a net device */
  pv->interface = if_register(dev, IF_ETHERNET, pv->mac, ETHERMTU);

  /* start dispatch thread */
  pv->dispatch = network_dispatch_create(pv->interface);
 
  if (pv->dispatch == NULL)
  {
	  printk("tuntap: cannot init dispatch\n");

      net_tuntap_cleanup(dev);

      return -1;
  }

  if (pthread_create(&th, NULL, net_tuntap_recv, dev))
  {
      printk("tuntap: cannot start reading thread\n");
  
      network_dispatch_kill(pv->dispatch);
      net_tuntap_cleanup(dev);

      return -1;
  }

  return 0;
}

/*
 * Cleanup
 */

DEV_CLEANUP(net_tuntap_cleanup)
{
	network_dispatch_kill(pv->dispatch);
}

DEVNET_PREPAREPKT(net_tuntap_preparepkt)
{
  struct net_tuntap_context_s	*pv = dev->drv_pv;
  struct net_header_s		*nethdr;
  uint_fast16_t			total = 0;
  uint8_t			*buff;

  total = sizeof (struct ether_header) + size;

  buff = packet->packet = mem_alloc(total, (mem_scope_context));

  nethdr = &packet->header[0];
  nethdr->data = buff;
  nethdr->size = sizeof (struct ether_header) + size;

  packet->stage = 1;

  packet->sMAC = pv->mac;
  packet->MAClen = ETH_ALEN;

  return buff + sizeof (struct ether_header);
}

/*
 * sending a packet.
 */

DEVNET_SENDPKT(net_tuntap_sendpkt)
{
  struct net_tuntap_context_s	*pv = dev->drv_pv;
  struct ether_header		*hdr;
  struct net_header_s		*nethdr;
  uint_fast8_t			i;
  uint8_t			buff[1514];
  size_t			offs = 0;

  /* get a pointer to the header */
  nethdr = &packet->header[0];
  hdr = (struct ether_header*)nethdr->data;

  /* fill the header */
  memcpy(hdr->ether_shost, packet->sMAC, packet->MAClen);
  memcpy(hdr->ether_dhost, packet->tMAC, packet->MAClen);
  net_be16_store(hdr->ether_type, proto);

  for (i = 0; nethdr[i].data; i++)
    {
      uint_fast16_t	fragsz;

      if (!nethdr[i + 1].data)
	fragsz = nethdr[i].size;
      else
	fragsz = nethdr[i].size - nethdr[i + 1].size;
      /* write each chunk after the previous one */
      memcpy(buff + offs, nethdr[i].data, fragsz);
      offs += fragsz;
    }

  LOCK_SPIN_IRQ(&pv->lock);

  emu_do_syscall(EMU_SYSCALL_WRITE, 3, pv->fd, buff, offs);

  LOCK_RELEASE_IRQ(&pv->lock);

  packet_obj_refdrop(packet);
}

/*
 * Setup driver level options.
 */

DEVNET_SETOPT(net_tuntap_setopt)
{
  struct net_ne2000_context_s	*pv = dev->drv_pv;

  switch (option)
    {
      case DEV_NET_OPT_PROMISC:
	/* XXX promisc */
	break;
      default:
	return -1;
    }

  return 0;
}

/*
 * Get driver level options / info.
 */

DEVNET_GETOPT(net_tuntap_getopt)
{
  switch (option)
    {
      case DEV_NET_OPT_BCAST:
	if (*len < ETH_ALEN)
	  return -1;
	memcpy(value, "\xff\xff\xff\xff\xff\xff", ETH_ALEN);
	*len = ETH_ALEN;
	break;
      default:
	return -1;
    }

  return 0;
}
