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

/**
 * @file
 * @module{Device drivers}
 * @short Network device driver API
 */                                                                 

#ifndef __DEVICE_NET_H__
#define __DEVICE_NET_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <device/device.h>

struct device_s;
struct driver_s;
struct net_packet_s;

/* network device options */
#define DEV_NET_OPT_PROMISC	1
#define DEV_NET_OPT_BCAST	2

/** Network device class packet creation function tempate. */
#define DEVNET_PREPAREPKT(n)	uint8_t  *(n) (struct device_s *dev, struct net_packet_s *packet, size_t size, size_t max_padding)

/**
    Network device class preparepkt() function type.
    Create a buffer of given size into a packet.

    @param dev pointer to device descriptor
    @param packet pointer to the packet
    @param size the size of the level 2 subpacket (eg: ARP, IP...)
    @param max_padding the additionnal size to allocate to permit the upper layer to pad the data
    @return pointer to the layer 2 subpacket
*/
typedef DEVNET_PREPAREPKT(devnet_preparepkt_t);

/** Network device class preparepkt() method shortcut */
#define dev_net_preparepkt(dev, ...) (dev)->drv->f.net.f_preparepkt(dev, __VA_ARGS__ )


/** Network device class packet sending function tempate. */
#define DEVNET_SENDPKT(n)	void  (n) (struct device_s *dev, struct net_packet_s *packet, uint_fast16_t proto)

/**
    Network device class sendpkt() function type.
    Send a packet.

    @param dev pointer to device descriptor
    @param packet pointer to the packet
    @param proto the level 2 protocol identifier
*/
typedef DEVNET_SENDPKT(devnet_sendpkt_t);

/** Network device class sendpkt() method shortcut */
#define dev_net_sendpkt(dev, ...) (dev)->drv->f.net.f_sendpkt(dev, __VA_ARGS__ )


/** Network device class device set option function tempate. */
#define DEVNET_SETOPT(n)	error_t (n) (struct device_s *dev, uint_fast32_t option, void *value, size_t len)

/**
    Network device class setopt() function type.
    Set an option.

    @param dev pointer to device descriptor
    @param option option to set
    @param value value for the option
    @param len length of value
    @return error code
*/
typedef DEVNET_SETOPT(devnet_setopt_t);

/** Network device class setopt() method shortcut */
#define dev_net_setopt(dev, ...) (dev)->drv->f.net.f_setopt(dev, __VA_ARGS__ )


/** Network device class device get option function tempate. */
#define DEVNET_GETOPT(n)	error_t (n) (struct device_s *dev, uint_fast32_t option, void *value, size_t *len)

/**
    Network device class getopt() function type.
    Get an option or constant.

    @param dev pointer to device descriptor
    @param option to get
    @param value value for the option
    @param len length of value
    @return error code
*/
typedef DEVNET_GETOPT(devnet_getopt_t);

/** Network device class getopt() method shortcut */
#define dev_net_getopt(dev, ...) (dev)->drv->f.net.f_getopt(dev, __VA_ARGS__ )


/** Net device class methodes */
struct dev_class_net_s
{
  devnet_preparepkt_t		*f_preparepkt;
  devnet_sendpkt_t		*f_sendpkt;
  devnet_setopt_t		*f_setopt;
  devnet_getopt_t		*f_getopt;
};

#endif

