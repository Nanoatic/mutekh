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


#include <hexo/types.h>

#include <device/enum.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/lock.h>
#include <hexo/interrupt.h>
#include <stdlib.h>

#include "enum-isapnp.h"
#include "enum-isapnp-private.h"

/**************************************************************/

static void enum_isapnp_register_all(struct device_s *dev)
{
  struct enum_isapnp_context_s	*pv = dev->drv_pv;

  /* walk through all devices */
  CONTAINER_FOREACH(device_list, CLIST, HEXO_SPIN, &dev->children,
  {
    struct enum_pv_isapnp_s		*enum_pv = item->enum_pv;
    uint_fast8_t			i;

    /* ignore already configured devices */
    if (item->drv != NULL)
      continue;

	struct driver_s *drv = driver_get_matching_isa(
		enum_pv->vendor);

	if (!drv)
		CONTAINER_FOREACH_CONTINUE;

	item->icudev = dev->icudev;

	/* call driver device init function, use same icu as ISA
	   enumerator parent device */
	if (!drv->f_init(item))
		CONTAINER_FOREACH_CONTINUE;
  });
}

/*
 * device close operation
 */

DEV_CLEANUP(enum_isapnp_cleanup)
{
  struct enum_isapnp_context_s	*pv = dev->drv_pv;

  mem_free(pv);
}

static void udelay(uint_fast16_t delay)
{
  uint32_t	i;

  for (i = 0; i < (uint64_t)delay * 1000; i++)
    {
      asm volatile("nop");
    }
}

/* send ISA PnP initialization key to all cards */
static void
isapnp_init_key(void)
{
  uint_fast8_t	i;
  uint8_t	key[32] =
    {
      0x6a, 0xb5, 0xda, 0xed, 0xf6, 0xfb, 0x7d, 0xbe,
      0xdf, 0x6f, 0x37, 0x1b, 0x0d, 0x86, 0xc3, 0x61,
      0xb0, 0x58, 0x2c, 0x16, 0x8b, 0x45, 0xa2, 0xd1,
      0xe8, 0x74, 0x3a, 0x9d, 0xce, 0xe7, 0x73, 0x39,
    };

  udelay(1000);
  /* reset lfsr */
  cpu_io_write_8(ISAPNP_ADDRESS_REG, 0);
  cpu_io_write_8(ISAPNP_ADDRESS_REG, 0);

  /* send init key */
  for (i = 0; i < 32; i++)
    cpu_io_write_8(ISAPNP_ADDRESS_REG, key[i]);
}

static void isapnp_write(uint8_t addr, uint8_t data)
{
  cpu_io_write_8(ISAPNP_ADDRESS_REG, addr);
  udelay(20);
  cpu_io_write_8(ISAPNP_WRITE_REG, data);
}

static error_t
isapnp_enum_probe(struct device_s *dev)
{
  uint_fast8_t		i, count;
  isapnp_id_root_t	serialid;

  /* enter wait for key */
  isapnp_write(ISAPNP_REG_CTRL, ISAPNP_REG_CTRL_WFK);
  isapnp_init_key();
  /* reset */
  isapnp_write(ISAPNP_REG_CTRL, ISAPNP_REG_CTRL_RST | ISAPNP_REG_CTRL_CSNRST);

  udelay(2000);

  /* enter wait for key */
  isapnp_write(ISAPNP_REG_CTRL, ISAPNP_REG_CTRL_WFK);
  isapnp_init_key();

  for (count = 0; count < 256; count++)
    {
      uint8_t		chksum = 0x6a;

      isapnp_write(ISAPNP_REG_CSN_WAKE, 0);	/* wake csn 0 */

      isapnp_write(ISAPNP_REG_RDP_SET, ISAPNP_READ_REG >> 2);
      udelay(1000);

      cpu_io_write_8(ISAPNP_ADDRESS_REG, 0x01);
      udelay(1000);

      for (i = 0; i < ISAPNP_SERIALID_LEN; i++)
	{
	  uint8_t	seq1, seq2;
	  bool_t	bit;

	  /* read pair */
	  seq1 = cpu_io_read_8(ISAPNP_READ_REG);
	  udelay(250);
	  seq2 = cpu_io_read_8(ISAPNP_READ_REG);
	  udelay(250);

	  /* get bit value */
	  bit = ((seq1 == 0x55) && (seq2 == 0xaa)) ? 1 : 0;
	  isapnp_id_set(&serialid, i, bit);

	  if (i < 64)
	    {
	      /* construct chksum */
	      chksum = ((((chksum ^ (chksum >> 1)) & 0x01) ^ bit) << 7) | (chksum >> 1);
	    }
	  else
	    {
	      /* verify chksum */
	      if ((chksum ^ bit) & 1)
		goto end;
	      chksum >>= 1;
	    }

	}

      isapnp_write(ISAPNP_REG_CSN_SET, count);
      udelay(250);

      printk("ISA PnP device found: %P\n", serialid.data, sizeof(serialid.data));

    }

 end:

  printk("last invalid ISA PnP  id: %P\n", serialid.data, sizeof(serialid.data));

  return 0;
}


/*
 * device open operation
 */

const struct driver_s	enum_isapnp_drv =
{
  .class		= device_class_enum,
  .f_init		= enum_isapnp_init,
  .f_cleanup		= enum_isapnp_cleanup,
  .f.denum = {
		.f_lookup = enum_isapnp_lookup,
  }
};

DEV_INIT(enum_isapnp_init)
{
  struct enum_isapnp_context_s	*pv;

  dev->drv = &enum_isapnp_drv;

  /* allocate private driver data */
  pv = mem_alloc(sizeof(*pv), (mem_scope_sys));

  if (!pv)
    return -1;

  lock_init(&pv->lock);

  dev->drv_pv = pv;

  isapnp_enum_probe(dev);
  enum_isapnp_register_all(dev);

  return 0;
}

DEVENUM_LOOKUP(enum_isapnp_lookup)
{
	return NULL;
}
