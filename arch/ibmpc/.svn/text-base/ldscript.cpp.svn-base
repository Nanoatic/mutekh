MEMORY
{
    mem_rom (RXAL)  : ORIGIN = CONFIG_ROM_ADDR, LENGTH = CONFIG_ROM_SIZE
    mem_ram (RXAL)  : ORIGIN = CONFIG_RAM_ADDR, LENGTH = CONFIG_RAM_SIZE
}

SECTIONS
   {
	/* must be within 8192 first bytes of kernel image */
	.multiboot : { *(.multiboot*) } > mem_rom
    .boot : { KEEP(*(.boot*)) } > mem_rom
	.text : { *(.text*) } > mem_rom
	.rodata : {
			*(.rodata*)
			global_driver_registry = .;
			KEEP(*(.drivers))
			global_driver_registry_end = .;
	} > mem_rom

	.cpudata  0x0 : { *(.cpudata*) } AT> mem_rom
	.contextdata  0x0 : { *(.contextdata*) } AT> mem_rom

	.common : { *(.common*) } > mem_ram
	.scommon : { *(.scommon*) } > mem_ram
	.sdata : { *(.sdata*) } > mem_ram
	.sbss :	 { *(.sbss*) } > mem_ram
	.bss :	 { *(.bss*) } > mem_ram
	.data :	 { *(.data*) } > mem_ram
	.cpuarchdata : { *(.cpuarchdata*) } > mem_ram

	__system_heap_start = ADDR(.cpuarchdata) + SIZEOF(.cpuarchdata);

	__cpu_data_start = LOADADDR(.cpudata);
	__cpu_data_end = LOADADDR(.cpudata) + SIZEOF(.cpudata);

	__context_data_start = LOADADDR(.contextdata);
	__context_data_end = LOADADDR(.contextdata) + SIZEOF(.contextdata);

	__boot_start = LOADADDR(.boot);
	__boot_end = LOADADDR(.boot) + SIZEOF(.boot);

   }

ENTRY(cpu_boot)

