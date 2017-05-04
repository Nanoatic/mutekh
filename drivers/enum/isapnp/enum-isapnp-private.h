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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef __ENUM_ISAPNP_PRIVATE_H_
#define __ENUM_ISAPNP_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>
#include <hexo/iospace.h>

#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_bitmap.h>

#define ISAPNP_ADDRESS_REG	0x0279
#define ISAPNP_WRITE_REG	0x0a79
#define ISAPNP_READ_REG		0x0243 /* relocatable from 0x203 to 0x3ff */

#define ISAPNP_SERIALID_LEN	72

#define ISAPNP_REG_RDP_SET	0x00
#define ISAPNP_REG_CTRL		0x02
# define ISAPNP_REG_CTRL_RST	0x01 /* reset device */
# define ISAPNP_REG_CTRL_WFK	0x02 /* enter wait for key state */
# define ISAPNP_REG_CTRL_CSNRST	0x04 /* reset CSN id */
#define ISAPNP_REG_CSN_WAKE	0x03
#define ISAPNP_REG_CSN_SET	0x06

/************************************************************************/

CONTAINER_TYPE(isapnp_id, BITMAP, uint8_t, NOLOCK, NOOBJ, ISAPNP_SERIALID_LEN);
CONTAINER_FUNC(static inline, isapnp_id, BITMAP, isapnp_id, NOLOCK);

struct enum_isapnp_context_s
{
  lock_t		lock;
};

struct enum_pv_isapnp_s
{
  uint32_t		vendor;
  uint16_t		csn;
  isapnp_id_root_t	serialid;
};

#endif

