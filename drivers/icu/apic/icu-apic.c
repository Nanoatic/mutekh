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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2010

*/


#include <hexo/types.h>
#include <hexo/local.h>
#include <hexo/cpu.h>
#include <hexo/ipi.h>
#include <hexo/interrupt.h>
#include <cpu/hexo/msr.h>

#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <hexo/iospace.h>
#include <mutek/mem_alloc.h>
#include <hexo/interrupt.h>
#include <hexo/error.h>

#include <mutek/printk.h>

#ifdef CONFIG_DRIVER_ICU_8259
# include <drivers/icu/8259/icu-8259.h>
#endif

#include "icu-apic.h"
#include "icu-apic-private.h"
#include "apic.h"

CPU_LOCAL struct device_s apic_dev;

#ifdef CONFIG_HEXO_IRQ

# ifdef CONFIG_HEXO_IPI
DEVICU_SETUP_IPI_EP(icu_apic_setup_ipi_ep)
{
  endpoint->icu_dev = dev;
  endpoint->priv = (void*)ipi_no;

  return 0;
}

DEVICU_SENDIPI(icu_apic_sendipi)
{
  uint32_t icr_low;
  uint32_t dst_id = (uint32_t)endpoint->priv;

  /* use current processor APIC */
  struct device_s *dev = CPU_LOCAL_ADDR(apic_dev);

  /* no need to take device lock once irq are disabled as we can not
     access APIC from an other processor. */
  CPU_INTERRUPT_SAVESTATE_DISABLE;

  /* wait for local APIC to be ready to send IPI */
  do {
    icr_low = cpu_mem_read_32(dev->addr[0] + APIC_REG_ICR_0_31);
  } while (icr_low & APIC_ICR_SEND_PENDING);

  /* set destination processor id */
  cpu_mem_write_32(dev->addr[0] + APIC_REG_ICR_32_63, dst_id << 24);

  ipi_queue_wrlock(&endpoint->ipi_fifo);

  /* select quick return vector if no request ipi pending */
  icr_low = ipi_queue_nolock_isempty(&endpoint->ipi_fifo)
    ? APIC_IPI_VECTOR : APIC_IPI_RQ_VECTOR;
  icr_low += CPU_HWINT_VECTOR;

  /* send ipi */
  cpu_mem_write_32(dev->addr[0] + APIC_REG_ICR_0_31,
                   APIC_ICR_DELIVERY_FIXED | APIC_ICR_LEVEL_ASSERT |
                   APIC_ICR_DST_NORMAL | icr_low);

  ipi_queue_unlock(&endpoint->ipi_fifo);
  CPU_INTERRUPT_RESTORESTATE;

  return 0;
}
#endif

DEVICU_ENABLE(icu_apic_enable)
{
#ifdef CONFIG_DRIVER_ICU_8259
  if (irq < ICU_8259_MAX_LINES)
    return 0;
#endif

  if (dev != CPU_LOCAL_ADDR(apic_dev))
    PRINTK_RET(-EINVAL, "APIC: can not enable irq from other processor");

  /* FIXME should handle IO-APIC here */
  return -EINVAL;
}

DEVICU_SETHNDL(icu_apic_sethndl)
{
  struct icu_apic_private_s	*pv = dev->drv_pv;
  struct icu_apic_handler_s	*h = pv->table + irq;

  if (irq >= CPU_HWINT_VECTOR_COUNT)
    return -EINVAL;

  if (h->hndl)
    return -ENOMEM;

  h->hndl = hndl;
  h->data = data;

  return 0;
}

DEVICU_DELHNDL(icu_apic_delhndl)
{
  struct icu_apic_private_s	*pv = dev->drv_pv;
  struct icu_apic_handler_s	*h = pv->table + irq;

  if (irq >= CPU_HWINT_VECTOR_COUNT)
    return -EINVAL;

  if (h->hndl != hndl)
    return -EINVAL;

  h->hndl = NULL;

  return 0;
}

static CPU_INTERRUPT_HANDLER(icu_apic_cpu_handler)
{
  struct device_s *dev = CPU_LOCAL_ADDR(apic_dev);
  struct icu_apic_private_s *pv = dev->drv_pv;

  assert(irq < CPU_HWINT_VECTOR_COUNT);

  switch (irq)
    {
#ifdef CONFIG_HEXO_IPI
    case APIC_IPI_RQ_VECTOR:
      /* call ipi processing */
      ipi_process_rq();
    case APIC_IPI_VECTOR:
      break;
#endif

    default: {
      struct icu_apic_handler_s *h = pv->table + irq;

      /* call interrupt handler */
      if (h->hndl)
        h->hndl(h->data);
      else
        printk("APIC: lost interrupt %i\n", irq);
    }

    }

#ifdef CONFIG_DRIVER_ICU_8259
  if (irq >= ICU_8259_MAX_LINES)
#endif
  cpu_mem_write_32(dev->addr[0] + APIC_REG_EOI, 0);
}

#endif  /* CONFIG_HEXO_IRQ */

#ifdef CONFIG_ARCH_IBMPC_SMP
static void icu_apic_wake_others(struct device_s *dev)
{
  assert((CONFIG_CPU_X86_SMP_BOOT_ADDR & 0xfff00fff) == 0);

  /* broadcast an INIT IPI to other CPUs */
  cpu_mem_write_32(dev->addr[0] + APIC_REG_ICR_0_31,
                   APIC_ICR_DELIVERY_INIT |
                   APIC_ICR_LEVEL_ASSERT |
                   APIC_ICR_DST_ALL_NOSELF);

  /* 10 ms delay (at 4Ghz) */
  cpu_cycle_wait(40000000);

  cpu_mem_write_32(dev->addr[0] + APIC_REG_ICR_0_31,
                   APIC_ICR_DELIVERY_START |
                   APIC_ICR_LEVEL_ASSERT |
                   APIC_ICR_DST_ALL_NOSELF |
                   (CONFIG_CPU_X86_SMP_BOOT_ADDR >> 12));

  /* 200 us delay (at 4Ghz) */
  cpu_cycle_wait(800000);

  cpu_mem_write_32(dev->addr[0] + APIC_REG_ICR_0_31,
                   APIC_ICR_DELIVERY_START |
                   APIC_ICR_LEVEL_ASSERT |
                   APIC_ICR_DST_ALL_NOSELF |
                   (CONFIG_CPU_X86_SMP_BOOT_ADDR >> 12));
}
#endif

static void icu_apic_setup(struct device_s *dev)
{
  uint32_t x;

  /* relocate APIC address */
  x = cpu_x86_read_msr(IA32_APIC_BASE_MSR);
  cpu_x86_write_msr(IA32_APIC_BASE_MSR, (x & 0xfff) | dev->addr[0]);

#ifdef CONFIG_DRIVER_ICU_8259
  if (cpu_isbootstrap())
    cpu_mem_write_32(dev->addr[0] + APIC_REG_LVT_LINT0,
                     APIC_LVT_TRIG_LEVEL | APIC_LVT_DELIVERY_EXT);
  else
#else
  cpu_mem_write_32(dev->addr[0] + APIC_REG_LVT_LINT0, APIC_LVT_MASKED);
#endif

  cpu_mem_write_32(dev->addr[0] + APIC_REG_LVT_LINT1, APIC_LVT_MASKED);

  /* enable CPU local APIC */
  cpu_mem_write_32(dev->addr[0] + APIC_REG_SPURIOUS_INT, APIC_SPUR_APIC_ENABLE);
}

const struct driver_s	icu_apic_drv =
{
  .class		= device_class_icu,
  .f_init		= icu_apic_init,
  .f_cleanup		= icu_apic_cleanup,
  .f.icu = {
#ifdef CONFIG_HEXO_IRQ
    .f_enable		= icu_apic_enable,
    .f_sethndl		= icu_apic_sethndl,
    .f_delhndl		= icu_apic_delhndl,
#endif
#ifdef CONFIG_HEXO_IPI
    .f_sendipi          = icu_apic_sendipi,
    .f_setup_ipi_ep     = icu_apic_setup_ipi_ep,
#endif
  }
};

DEV_CLEANUP(icu_apic_cleanup)
{
  struct icu_apic_private_s	*pv = dev->drv_pv;

  /* disable CPU local APIC */
  cpu_mem_write_32(dev->addr[0] + APIC_REG_SPURIOUS_INT, 0);

  mem_free(pv);
}

DEV_INIT(icu_apic_init)
{
  struct icu_apic_private_s	*pv;
  uint32_t detect;

  asm ("mov $1, %%eax; cpuid;"
       : "=d" (detect)
       : : "%ebx", "%ecx", "%eax");

  if (!(detect & (1 << 9)))
    PRINTK_RET(-ENOTSUP, "APIC: Local APIC not available");

  if (dev->addr[0] & 0xfff)
    return -EINVAL;

  if (!(pv = mem_alloc(sizeof(*pv), mem_scope_sys)))
    return -ENOMEM;

  dev->drv_pv = pv;
  memset(pv, 0, sizeof(*pv));

  device_mem_map(dev ,1);

#ifdef CONFIG_HEXO_IRQ
  /* we are directly bound to cpu */
  cpu_interrupt_sethandler(icu_apic_cpu_handler);
#endif

  icu_apic_setup(dev);

  dev->drv = &icu_apic_drv;

#ifdef CONFIG_ARCH_IBMPC_SMP
  /* wake up other processors if we are bootstrap */
  if (cpu_isbootstrap())
    icu_apic_wake_others(dev);
#endif

  return 0;
}

