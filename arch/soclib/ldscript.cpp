/*
  We may have aliasing in rom/except/boot segments. If so, we merge
  all we can in mem_rom segment.
 */

/* The first two may only happen if reset handler is present */
#if defined(CONFIG_CPU_RESET_HANDLER)

/* boot + rom */
# if CONFIG_CPU_RESET_ADDR == CONFIG_ROM_ADDR
#  define mem_boot mem_rom
# endif

/* exception + boot
 *
 * boot may already be rom, and we'll have cascading defines */
# if defined(CONFIG_CPU_EXCEPTION_FIXED_ADDRESS) && \
     (CONFIG_CPU_EXCEPTION_FIXED_ADDRESS == CONFIG_CPU_RESET_ADDR)
#  define mem_except mem_boot
# endif

#endif /* end if reset handler */


/* exception + rom */
#if ( (CONFIG_CPU_EXCEPTION_FIXED_ADDRESS == CONFIG_ROM_ADDR) ||   \
	  !defined(CONFIG_CPU_EXCEPTION_FIXED_ADDRESS) ) &&			   \
    !defined(mem_except)
# define mem_except mem_rom
#endif


/*
  Implement .data from rom (copied at boot) by putting all r/w data at
  end of mem_rom. This is used for rom-only bootloaders.
 */
#if defined(CONFIG_DATA_FROM_ROM)
# define __AT_MEM_ROM AT>mem_rom
#else
# define __AT_MEM_ROM
#endif


MEMORY
{
#if defined(CONFIG_CPU_RESET_HANDLER) && !defined(mem_boot)
	mem_boot (RXAL)  : ORIGIN = CONFIG_CPU_RESET_ADDR, LENGTH = CONFIG_CPU_RESET_SIZE
#endif
#if !defined(mem_except)
	mem_except (RXAL)  : ORIGIN = CONFIG_CPU_EXCEPTION_FIXED_ADDRESS, LENGTH = 0x1000
#endif
#ifdef CONFIG_HET_BUILD
    mem_hetrom (RXAL): ORIGIN = CONFIG_HETROM_ADDR, LENGTH = CONFIG_HETROM_SIZE
#else
# define mem_hetrom mem_rom
#endif
    mem_rom (RXAL): ORIGIN = CONFIG_ROM_ADDR, LENGTH = CONFIG_ROM_SIZE
    mem_ram (RWAL): ORIGIN = CONFIG_RAM_ADDR, LENGTH = CONFIG_RAM_SIZE
}

SECTIONS
{
#if defined(CONFIG_CPU_RESET_HANDLER)
	.boot : {
		KEEP(*(.boot*))
	} > mem_boot
#endif

	.text : {
		*(.init*)
		*(.text*)
		*(.glue*)
		*(.got2)
	} > mem_hetrom

	.rodata : {
			*(.rodata*)
            . = ALIGN(4);
			global_driver_registry = .;
			KEEP(*(.drivers))
			global_driver_registry_end = .;
	} > mem_rom

	.excep : {
#if !defined(CONFIG_CPU_EXCEPTION_FIXED_ADDRESS)
		/* On some architectures, exception vector is freely
		 * relocatable up to a given alignment.
		 *
		 * We must set the correct pointer ASAP in the boot sequence,
		 * dont forget reset vector is optional...
		 */
		. = ALIGN(CONFIG_CPU_EXCEPTION_ALIGN);
#endif
		__exception_base_ptr = .;
		KEEP(*(.excep*))
	} > mem_except


	/* TLS/CLS are templates for newly allocated contexts/cpu's
	 * private data. They are always read-only.
	 *
	 * On a non-smp machine, cpudata is read-write, but does not fall
	 * in the cpudata section (it is normal global data), so we can
	 * keep on linking this as r/o.
	 */

	/* CPU local data section */
	.cpudata  0x0 : { *(.cpudata*) } AT> mem_rom

	__cpu_data_start = LOADADDR(.cpudata);
	__cpu_data_end = LOADADDR(.cpudata) + SIZEOF(.cpudata);

	/* Task local data section */
	.contextdata  0x0 : { *(.contextdata*) } AT> mem_rom

	__context_data_start = LOADADDR(.contextdata);
	__context_data_end = LOADADDR(.contextdata) + SIZEOF(.contextdata);

	.data :	{
		__data_start = ABSOLUTE(.);
		*(.sdata*)
		*(.data*)
		*(.cpuarchdata*)
	} > mem_ram __AT_MEM_ROM

	__data_load_start = LOADADDR(.data);
	__data_load_end = LOADADDR(.data) + SIZEOF(.data);

// #if defined(CONFIG_HET_BUILD)
//     /DISCARD/ : {
// #else
    .bss : {
		__bss_start = ABSOLUTE(.);
// #endif
		*(.sbss*)
		*(COMMON)
		*(.common*)
		*(.scommon*)
		*(.bss*)
// #if !defined(CONFIG_HET_BUILD)
		__bss_end = ABSOLUTE(.);
	} > mem_ram
// #else
//     }

//     __bss_end = 0;
//     __bss_start = 0;
// #endif

	__system_uncached_heap_start = .;
	__system_uncached_heap_end = ORIGIN(mem_ram) + LENGTH(mem_ram);

	. = ALIGN(CONFIG_HEXO_STACK_ALIGN);
	__initial_stack = __system_uncached_heap_end;

	/* GOT section */
 	/DISCARD/ : { *(.eh_frame) }

	ASSERT(__system_uncached_heap_start == __bss_end, "Unlinked sections found, please report a bug")
}

ENTRY(arch_init)
