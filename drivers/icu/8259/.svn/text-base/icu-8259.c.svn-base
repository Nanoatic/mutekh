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

#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/interrupt.h>
#include <hexo/error.h>

#include <mutek/printk.h>

#include "icu-8259.h"

#include "icu-8259-private.h"

#include "8259.h"

static inline void
pic_8259_setmask(uintptr_t pic, uint8_t mask)
{
  cpu_io_write_8(pic + 1, mask);
}

static inline void
pic_8259_enable(uintptr_t pic, uint_fast8_t line)
{
  uint8_t	x = cpu_io_read_8(pic + 1);
  cpu_io_write_8(pic + 1, x & ~(1 << line));
}

static inline void
pic_8259_disable(uintptr_t pic, uint_fast8_t line)
{
  uint8_t	x = cpu_io_read_8(pic + 1);
  cpu_io_write_8(pic + 1, x | (1 << line));
}

static inline void
pic_8259_irqend(uintptr_t pic, uint_fast8_t line)
{
  cpu_io_write_8(pic, PIC_OCW2_EOI | PIC_OCW2_PRIO_NOP | line);
}

static inline void
pic_8259_irqend_slave(uintptr_t master, uintptr_t slave, uint_fast8_t line)
{
  pic_8259_irqend(slave, line);
  pic_8259_irqend(master, PIC_SLAVE_CHAINED_IRQ);
}

static inline void
pic_8259_irqend_master(uintptr_t master, uint_fast8_t line)
{
  pic_8259_irqend(master, line);
}

static inline int_fast8_t
pic_8259_irq(uintptr_t master, uintptr_t slave)
{
  uint8_t isr = cpu_io_read_8(master);

  if (isr & (1 << PIC_SLAVE_CHAINED_IRQ))
    return __builtin_ffs(cpu_io_read_8(slave)) - 1 + 8;

  return __builtin_ffs(isr) - 1;
}

static void
pic_8259_init(uintptr_t master, uintptr_t slave, uint8_t base)
{
  /* master init */
  cpu_io_write_8(master    , PIC_ICW1_INIT | PIC_ICW1_HAS_ICW4);
  cpu_io_write_8(master + 1, base);
  cpu_io_write_8(master + 1, 1 << PIC_SLAVE_CHAINED_IRQ);
  cpu_io_write_8(master + 1, PIC_ICW4_X86_MODE);

  cpu_io_write_8(master, PIC_OCW3_GET_ISR);

  pic_8259_setmask(master, ~(1 << PIC_SLAVE_CHAINED_IRQ));
  pic_8259_irqend_master(master, PIC_SLAVE_CHAINED_IRQ);

  /* slave init */
  cpu_io_write_8(slave    , PIC_ICW1_INIT | PIC_ICW1_HAS_ICW4);
  cpu_io_write_8(slave + 1, base + 8);
  cpu_io_write_8(slave + 1, PIC_SLAVE_CHAINED_IRQ);
  cpu_io_write_8(slave + 1, PIC_ICW4_X86_MODE);

  cpu_io_write_8(slave, PIC_OCW3_GET_ISR);

  pic_8259_setmask(slave, 0xff);
}

DEVICU_ENABLE(icu_8259_enable)
{
  uintptr_t addr = irq < 8 ? 0x20 : 0xa0;

  if (enable)
    pic_8259_enable(addr, irq);
  else
    pic_8259_disable(addr, irq);

  return 0;
}

DEVICU_SETHNDL(icu_8259_sethndl)
{
  struct icu_8259_private_s	*pv = dev->drv_pv;
  struct icu_8259_handler_s	*h = pv->table + irq;

  /* FIXME handle multiple handlers for irq sharing */
  if (irq >= ICU_8259_MAX_LINES)
    return -EINVAL;

  if (h->hndl)
    return -ENOMEM;

  h->hndl = hndl;
  h->data = data;

  device_obj_refnew(dev);
  return 0;
}

DEVICU_DELHNDL(icu_8259_delhndl)
{
  struct icu_8259_private_s	*pv = dev->drv_pv;
  struct icu_8259_handler_s	*h = pv->table + irq;

  if (irq >= ICU_8259_MAX_LINES)
    return -EINVAL;

  if (h->hndl != hndl)
    return -EINVAL;

  h->hndl = NULL;

  device_obj_refdrop(dev);
  return 0;
}

static DEV_IRQ(icu_8259_handler)
{
  struct icu_8259_private_s *pv = dev->drv_pv;
  bool_t res = 0;

  do {
    int_fast8_t irq = pic_8259_irq(0x20, 0xa0);
    struct icu_8259_handler_s *h = pv->table + irq;

    if (irq < 0)
      break;
    res = 1;

    assert(irq < ICU_8259_MAX_LINES);

    /* call interrupt handler */
    if (h->hndl)
      h->hndl(h->data);
    else
      printk("8259: lost interrupt %i\n", irq);

    /* reset interrupt line status on icu */
    if (irq < 8)
      pic_8259_irqend_master(0x20, irq);
    else
      pic_8259_irqend_slave(0x20, 0xa0, irq - 8);

  } while (1);                  /* FIXME 1 */

  return res;
}

#ifndef CONFIG_DRIVER_ICU_APIC
static CPU_INTERRUPT_HANDLER(icu_8259_cpu_handler)
{
  /* ignore irq number, will read from pic register */
  icu_8259_handler(CPU_LOCAL_GET(cpu_interrupt_handler_dev));
}
#endif

DEV_CLEANUP(icu_8259_cleanup)
{
  struct icu_8259_private_s	*pv = dev->drv_pv;

  mem_free(pv);
}


const struct driver_s	icu_8259_drv =
{
  .class		= device_class_icu,
  .f_init		= icu_8259_init,
#ifdef CONFIG_DRIVER_ICU_APIC
  .f_irq                = icu_8259_handler,
#else
  .f_irq                = (dev_irq_t *)icu_8259_cpu_handler,
#endif
  .f_cleanup		= icu_8259_cleanup,
  .f.icu = {
    .f_enable		= icu_8259_enable,
    .f_sethndl		= icu_8259_sethndl,
    .f_delhndl		= icu_8259_delhndl,
  }
};

DEV_INIT(icu_8259_init)
{
  struct icu_8259_private_s	*pv;

  if (dev->addr[0] != 0x20 ||
      dev->addr[1] != 0xa0)
    return -EINVAL;

  if (!(pv = mem_alloc(sizeof (*pv), (mem_scope_sys)))) /* FIXME allocation scope ? */
    return -ENOMEM;

  memset(pv, 0, sizeof(*pv));
  dev->drv_pv = pv;
  dev->drv = &icu_8259_drv;

#ifdef CONFIG_DRIVER_ICU_APIC
  {
    uint_fast8_t i;

    for (i = 0; i < ICU_8259_MAX_LINES; i++)
      DEV_ICU_BIND(dev->icudev, dev, i, icu_8259_handler);
  }
#else
  cpu_interrupt_sethandler_device(dev);
#endif

  pic_8259_init(0x20, 0xa0, CPU_HWINT_VECTOR);

  return 0;
}

