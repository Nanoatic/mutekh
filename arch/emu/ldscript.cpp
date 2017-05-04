
PHDRS
{
  mem PT_LOAD ;
}

SECTIONS
   {
	.text 0x40100000 : AT( 0x40100000 ) { *(.text*) } : mem

	.data :
		{
			/* global variables and const data section */
			*(.data*)
			*(.rodata*)
			global_driver_registry = .;
			*(.drivers)
			global_driver_registry_end = .;

			/* data depending on cpu architecture (fonction pointer variables, ...) */
			*(.cpuarchdata*)
		}
        __data_start = LOADADDR(.data);
    __data_end = LOADADDR(.data) + SIZEOF(.data);

	.bss : { *(.bss*) }
    __bss_start = LOADADDR(.bss);
    __bss_end = LOADADDR(.bss) + SIZEOF(.bss);

	/* We do not set VMA to 0 for these sections because it causes some loading
	problems with linux exec loader (when /proc/sys/vm/mmap_min_addr is set).
	Instead we subtract the __context_data_start to the TLS address when used. */

    /* ensure cpudata is in a separate page from data */
    . = ALIGN(CONFIG_ARCH_EMU_PAGESIZE);

	/* CPU local data section */
	.cpudata :
		{
			*(.cpudata*)
		}

	__cpu_data_start = LOADADDR(.cpudata);
	__cpu_data_end = LOADADDR(.cpudata) + SIZEOF(.cpudata);

	/* Task local data section */
 	.contextdata :
		{
			*(.contextdata*)
		}

	__context_data_start = LOADADDR(.contextdata);
	__context_data_end = LOADADDR(.contextdata) + SIZEOF(.contextdata);

	__system_heap_start = __context_data_end;

   }

ENTRY(arch_init)

