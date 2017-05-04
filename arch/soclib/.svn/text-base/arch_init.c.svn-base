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

#include <assert.h>

#if defined(CONFIG_ARCH_DEVICE_TREE)
# include <drivers/enum/fdt/enum-fdt.h>
# include <device/enum.h>
# include <device/driver.h>
# include <device/device.h>
# include <mutek/fdt.h>
#endif

#if defined(CONFIG_SOCLIB_MEMCHECK)
# include <arch/mem_checker.h>
#endif

#if defined(CONFIG_SOCLIB_EARLY_CONSOLE)
void soclib_early_console(uintptr_t addr);
#endif

#include <hexo/types.h>
#include <hexo/init.h>
#include <hexo/lock.h>
#include <hexo/cpu.h>
#include <mutek/printk.h>
#include <mutek/scheduler.h>

#include <string.h>

extern __ldscript_symbol_t __bss_start;
extern __ldscript_symbol_t __bss_end;
#ifdef CONFIG_DATA_FROM_ROM
extern __ldscript_symbol_t __data_start;
extern __ldscript_symbol_t __data_load_start;
extern __ldscript_symbol_t __data_load_end;
#endif

#if defined(CONFIG_ARCH_DEVICE_TREE)
struct device_s fdt_enum_dev;
#endif

#ifdef CONFIG_ARCH_SOCLIB_RAMLOCK
extern __ldscript_symbol_t __ramlock_base_start;
uintptr_t __ramlock_base = (uintptr_t)&__ramlock_base_start;
#endif

/* #if defined(CONFIG_MUTEK_SCHEDULER) */
/* void sched_global_init(void); */
/* void sched_cpu_init(void); */
/* #endif */

#ifdef CONFIG_HEXO_MMU
extern __ldscript_symbol_t __system_uncached_heap_start, __system_uncached_heap_end;
#include <hexo/mmu.h>
#endif

#define START_MAGIC 0x67c51264

#ifdef CONFIG_ARCH_SMP
static uint_fast8_t cpu_count = 1;
uint32_t     cpu_init_flag = 0;
uint32_t     cpu_start_flag = 0;
static lock_t       cpu_init_lock;    /* cpu intialization lock */
/* integer atomic operations global spin lock */
lock_t              __atomic_arch_lock;
#endif

/* architecture specific init function */
void arch_init(void *device_tree, void *bootloader_pointer_table)
{
#ifdef CONFIG_ARCH_SMP
    if (cpu_isbootstrap())    /* FIXME */
        /* First CPU */
    {
        lock_init(&__atomic_arch_lock);
        lock_init(&cpu_init_lock);

#endif

#if defined(CONFIG_SOCLIB_MEMCHECK)
        soclib_mem_check_region_status((uint8_t*)&__bss_start,
                                       (uint8_t*)&__bss_end-(uint8_t*)&__bss_start,
                                       SOCLIB_MC_REGION_GLOBAL);
# ifdef CONFIG_DATA_FROM_ROM
        soclib_mem_check_region_status((uint8_t*)&__data_start,
                                       (uint8_t*)&__data_load_end-(uint8_t*)&__data_load_start,
                                       SOCLIB_MC_REGION_GLOBAL);
# endif                                                                                        
#endif
#ifdef CONFIG_DATA_FROM_ROM
        memcpy_from_code((uint8_t*)&__data_start, (uint8_t*)&__data_load_start, (uint8_t*)&__data_load_end-(uint8_t*)&__data_load_start);
#endif
        memset((uint8_t*)&__bss_start, 0, (uint8_t*)&__bss_end-(uint8_t*)&__bss_start);

#if defined(CONFIG_SOCLIB_EARLY_CONSOLE)
        soclib_early_console(CONFIG_SOCLIB_EARLY_CONSOLE_ADDR);
#endif

        /* configure system wide cpu data */
        cpu_global_init();

        mem_init();

        hexo_global_init();

#ifdef CONFIG_HEXO_MMU
        uint32_t t0=(uint32_t)(&__system_uncached_heap_start);
        uint32_t t1=(uint32_t)(&__system_uncached_heap_end);
        t0+=CONFIG_SOCLIB_VMEM_MALLOC_REGION_SIZE;

#ifdef CONFIG_VMEM_PHYS_ALLOC
        vmem_ppage_ops_init(&vmem_ops);
        initial_ppage_region = vmem_ops.ppage_initial_region_get();
        vmem_ops.ppage_region_init(initial_ppage_region, t0, t1);
#endif

#ifdef CONFIG_VMEM_KERNEL_ALLOC
        vmem_vpage_ops_init(&vmem_ops);
        vmem_ops.vpage_init();
#endif

#endif /*CONFIG_HEXO_MMU*/

#ifdef CONFIG_HEXO_MMU
        mmu_cpu_init();
#endif

#if defined(CONFIG_ARCH_DEVICE_TREE)
        device_init(&fdt_enum_dev);
        enum_fdt_init(&fdt_enum_dev, device_tree);
        mutek_parse_fdt(&fdt_enum_dev, device_tree);
        mem_region_init();//TODO: change with mem_parse_fdt when lib topology is done
        //        mem_parse_fdt(device_tree);

#elif defined(CONFIG_ARCH_HW_INIT_USER)
        user_hw_init();
       
#elif defined(CONFIG_ARCH_HW_INIT)
# error CONFIG_ARCH_HW_INIT unsupported for SoCLib platforms
#endif

        /* configure first CPU */
        cpu_init();

#if defined(CONFIG_ARCH_DEVICE_TREE) && defined(CONFIG_HEXO_IRQ)
        struct device_s *icu = enum_fdt_icudev_for_cpuid(&fdt_enum_dev, cpu_id());
        if ( icu )
            cpu_interrupt_sethandler_device(icu);
#endif

#ifdef CONFIG_ARCH_SMP
        /* send reset/init signal to other CPUs */
        cpu_init_flag = START_MAGIC;
#endif

#if defined(CONFIG_MUTEK_SCHEDULER)
        sched_global_init();
        sched_cpu_init();
#endif

#ifdef CONFIG_ARCH_SMP
        uint_fast8_t last_count = 0;
        
        for ( ;; ) {
            order_compiler_mem();

            if ( last_count == cpu_count )
                break;
            last_count = cpu_count;

            if ( last_count == CONFIG_CPU_MAXCOUNT )
                break;
            
            cpu_cycle_wait(100000);
        }

        cpu_init_flag = 0;
#endif

        /* run mutek_start() */
        mutek_start(0, 0);
#ifdef CONFIG_ARCH_SMP
    }
    else
        /* Other CPUs */
    {
        while (cpu_init_flag != START_MAGIC)
            order_compiler_mem();

        assert(cpu_id() < CONFIG_CPU_MAXCOUNT);

        /* configure other CPUs */
        lock_spin(&cpu_init_lock);

#ifdef CONFIG_HEXO_MMU
        mmu_cpu_init();
#endif      
        cpu_init();
        

#if defined(CONFIG_ARCH_DEVICE_TREE) && defined(CONFIG_HEXO_IRQ)
        struct device_s *icu = enum_fdt_icudev_for_cpuid(&fdt_enum_dev, cpu_id());
        if ( icu )
            cpu_interrupt_sethandler_device(icu);
#endif

        ++cpu_count;

        lock_release(&cpu_init_lock);

        /* wait for start signal */

        while (cpu_start_flag != START_MAGIC)
            order_compiler_mem();

        /* run mutek_start_smp() */

#if defined(CONFIG_MUTEK_SCHEDULER)
        sched_cpu_init();
#endif

        mutek_start_smp();
    }
#endif

    while (1)
        ;
}

void arch_start_other_cpu(void)
{
#ifdef CONFIG_ARCH_SMP
    cpu_start_flag = START_MAGIC;
#endif
}

inline cpu_id_t arch_get_cpu_count(void)
{
#ifdef CONFIG_ARCH_SMP
    return cpu_count;
#else
    return 1;
#endif
}

// Local Variables:
// tab-width: 4;
// c-basic-offset: 4;
// c-file-offsets:((innamespace . 0)(inline-open . 0));
// indent-tabs-mode: nil;
// End:
//
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

