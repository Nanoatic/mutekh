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

#ifndef DRIVERS_TUNTAP_H
#define DRIVERS_TUNTAP_H

#include <device/net.h>
#include <device/device.h>

DEV_IRQ(net_tuntap_irq);
DEV_INIT(net_tuntap_init);
DEV_CLEANUP(net_tuntap_cleanup);
DEVNET_PREPAREPKT(net_tuntap_preparepkt);
DEVNET_SENDPKT(net_tuntap_sendpkt);
DEVNET_SETOPT(net_tuntap_setopt);
DEVNET_GETOPT(net_tuntap_getopt);

extern const struct driver_s	net_tuntap_drv;

#endif

