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


#include <mutek/mem_alloc.h>
#include <string.h>
#include <hexo/interrupt.h>
#include <hexo/init.h>
#include <hexo/iospace.h>
#include <hexo/lock.h>
#include <hexo/segment.h>
#include <hexo/cpu.h>

#include <cpu/hexo/pmode.h>
#include <cpu/hexo/msr.h>

/** pointer to cpu local storage itself */
CPU_LOCAL void *__cpu_data_base;
/** pointer to context local storage itself */
CONTEXT_LOCAL void *__context_data_base;

/* CPU Local Descriptor structure */

static struct cpu_x86_gatedesc_s cpu_idt[CPU_MAX_INTERRUPTS];

#ifdef CONFIG_ARCH_SMP
void * cpu_local_storage[CONFIG_CPU_MAXCOUNT];
cpu_x86_segsel_t cpu_local_storage_seg[CONFIG_CPU_MAXCOUNT];
#endif

#ifdef CONFIG_HEXO_USERMODE
volatile CPU_LOCAL struct cpu_x86_tss_s cpu_tss;
#endif

/** gdt table lock */
static lock_t		gdt_lock;

/** CPU Global descriptor table */
union cpu_x86_desc_s gdt[ARCH_GDT_SIZE];

error_t
cpu_global_init(void)
{
  uint_fast16_t	i;

  lock_init(&gdt_lock);

  cpu_x86_seg_setup(&gdt[ARCH_GDT_CODE_INDEX].seg, 0,
		    0xffffffff, CPU_X86_SEG_EXEC_NC_R, 0, 1);

#ifdef CONFIG_HEXO_USERMODE
  cpu_x86_seg_setup(&gdt[ARCH_GDT_USER_CODE_INDEX].seg, 0,
		    0xffffffff, CPU_X86_SEG_EXEC_NC_R, 3, 1);
#endif

  cpu_x86_seg_setup(&gdt[ARCH_GDT_DATA_INDEX].seg, 0,
		    0xffffffff, CPU_X86_SEG_DATA_UP_RW, 0, 1);

#ifdef CONFIG_HEXO_USERMODE
  cpu_x86_seg_setup(&gdt[ARCH_GDT_USER_DATA_INDEX].seg, 0,
		    0xffffffff, CPU_X86_SEG_DATA_UP_RW, 3, 1);
#endif

  /* mark all other GDT entries available */
  for (i = ARCH_GDT_FIRST_ALLOC; i < ARCH_GDT_SIZE; i++)
    gdt[i].seg.available = 1;

  /* fill IDT with exceptions entry points */

  for (i = 0; i < CPU_EXCEPT_VECTOR_COUNT; i++)
    {
      uintptr_t	entry = ((uintptr_t)&x86_interrupt_ex_entry) + i * CPU_INTERRUPT_ENTRY_ALIGN;

      cpu_x86_gate_setup(cpu_idt + i + CPU_EXCEPT_VECTOR,
			 ARCH_GDT_CODE_INDEX, entry,
			 CPU_X86_GATE_INT32, 0, 0);
    }

#ifdef CONFIG_HEXO_IRQ
  /* fill IDT with hardware interrupts entry points */

  for (i = 0; i < CPU_HWINT_VECTOR_COUNT; i++)
    {
      uintptr_t	entry = ((uintptr_t)&x86_interrupt_hw_entry) + i * CPU_INTERRUPT_ENTRY_ALIGN;

      cpu_x86_gate_setup(cpu_idt + i + CPU_HWINT_VECTOR,
			 ARCH_GDT_CODE_INDEX, entry,
			 CPU_X86_GATE_INT32, 0, 0);
    }
#endif

  /* fill IDT with syscall entry points */

  for (i = 0; i < CPU_SYSCALL_VECTOR_COUNT; i++)
    {
      uintptr_t	entry = ((uintptr_t)&x86_interrupt_sys_entry) + i * CPU_INTERRUPT_ENTRY_ALIGN;

      cpu_x86_gate_setup(cpu_idt + i + CPU_SYSCALL_VECTOR,
			 ARCH_GDT_CODE_INDEX, entry,
			 CPU_X86_GATE_INT32, 3, 0);
    }

#ifdef CONFIG_ARCH_SMP
  /* copy boot section below 1Mb for slave CPUs bootup */
  memcpy((void*)CONFIG_CPU_X86_SMP_BOOT_ADDR,
         (char*)&__boot_start, (char*)&__boot_end - (char*)&__boot_start);
#endif

  return 0;
}

static cpu_x86_segsel_t
cpu_x86_gdt_desc_alloc(void)
{
  cpu_x86_segsel_t	sel = 0;
  uint_fast16_t		i;

  for (i = ARCH_GDT_FIRST_ALLOC; i < ARCH_GDT_SIZE; i++)
    {
      if (gdt[i].seg.available)
	{
	  sel = i;
	  break;
	}
    }

  return sel;
}

cpu_x86_segsel_t
cpu_x86_segment_alloc(uintptr_t addr, uint32_t size, uint_fast8_t type)
{
  cpu_x86_segsel_t	sel;

  lock_spin(&gdt_lock);

  if ((sel = cpu_x86_gdt_desc_alloc()))
    {
      union cpu_x86_desc_s	*desc = gdt + sel;

      cpu_x86_seg_setup(&desc->seg, addr, size, type, 0, 1);
    }

  lock_release(&gdt_lock);

  return sel;
}

void
cpu_x86_segdesc_free(cpu_x86_segsel_t sel)
{
  lock_spin(&gdt_lock);

  /* FIXME could use atomic op here */
  assert(!gdt[sel].seg.available);
  gdt[sel].seg.available = 1;

  lock_release(&gdt_lock);  
}

void cpu_init(void)
{
  uint_fast16_t		id = cpu_id();

  /* set GDT pointer */
  cpu_x86_set_gdt(gdt, ARCH_GDT_SIZE);

  /* set IDT pointer */
  cpu_x86_set_idt(cpu_idt, CPU_MAX_INTERRUPTS);

  /* activate defined segments */
  cpu_x86_dataseg_use(ARCH_GDT_DATA_INDEX, 0);
  cpu_x86_stackseg_use(ARCH_GDT_DATA_INDEX, 0);
  cpu_x86_codeseg_use(ARCH_GDT_CODE_INDEX, 0);

#ifdef CONFIG_ARCH_SMP

  /* setup cpu local storage */
  void			*cls;
  uint16_t		cls_sel;

  if (!(cls = arch_cpudata_alloc()))
    goto err_cls;

  cpu_local_storage[id] = cls;

  if (!(cls_sel = cpu_x86_segment_alloc((uintptr_t)cls,
					arch_cpudata_size(),
					CPU_X86_SEG_DATA_UP_RW)))
    goto err_cls_seg;

  cpu_local_storage_seg[id] = cls_sel;
  cpu_x86_datasegfs_use(cls_sel, 0);
  CPU_LOCAL_SET(__cpu_data_base, cls);

#endif

#ifdef CONFIG_HEXO_USERMODE

  uint16_t		tss_sel;
  struct cpu_x86_tss_s *tss = CPU_LOCAL_ADDR(cpu_tss);

  memset(tss, 0, sizeof(struct cpu_x86_tss_s));

  tss->ss0 = ARCH_GDT_DATA_INDEX << 3;

  if (!(tss_sel = cpu_x86_segment_alloc((uintptr_t)tss, sizeof(*tss), CPU_X86_SEG_CONTEXT32)))
    goto err_tss_seg;

  cpu_x86_taskseg_use(tss_sel);

# ifdef CONFIG_CPU_X86_SYSENTER
  cpu_x86_write_msr(SYSENTER_CS_MSR, CPU_X86_SEG_SEL(ARCH_GDT_CODE_INDEX, 0));
  cpu_x86_write_msr(SYSENTER_EIP_MSR, x86_interrupt_sys_enter);
# endif

#endif

  return;

#ifdef CONFIG_HEXO_USERMODE
 err_tss_seg:
#endif
#ifdef CONFIG_ARCH_SMP
  cpu_x86_segdesc_free(cls_sel);
 err_cls_seg:
  mem_free(cls);
 err_cls:
#endif
  ;
}

