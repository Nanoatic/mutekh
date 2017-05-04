MEMORY
{
	mem_rom (RXAL)   : ORIGIN = CONFIG_ROM_ADDR, LENGTH = CONFIG_ROM_SIZE
    mem_ram (RWA)    : ORIGIN = CONFIG_RAM_ADDR, LENGTH = CONFIG_RAM_SIZE
}

SECTIONS
{
#if CONFIG_CPU_RESET_ADDR == CONFIG_ROM_ADDR
	.boot : {
		KEEP(*(.boot*))
	} > mem_rom
#endif

	.text : {
		*(.init*)
		*(.text*)
		*(.progmem*)
	} > mem_rom

	.data : {
		/* global variables and const data section */
		__data_start = .;
		*(.sdata*)
		*(.data*)
		*(.rodata*)
		global_driver_registry = .;
		KEEP(*(.drivers))
		global_driver_registry_end = .;
		*(.common*)

		/* data depending on cpu architecture (fonction pointer variables, ...) */
		*(.cpuarchdata*)
		__data_end = .;
	} > mem_ram AT> mem_rom

 	.contextdata 0x0 :
	{
		*(.contextdata*)
	} AT> mem_rom

#if CONFIG_CPU_RESET_ADDR != CONFIG_ROM_ADDR
	.boot CONFIG_CPU_RESET_ADDR : {
		KEEP(*(.boot*))
	} > mem_rom
#endif

	.bss __data_end : {
		__bss_start = .;
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		__bss_end = .;
	} > mem_ram

	__system_heap_start = __bss_end;
	__system_heap_end = ORIGIN(mem_ram) + LENGTH(mem_ram) - 4;

	__initial_stack = __system_heap_end;

	__context_data_start = LOADADDR(.contextdata);
	__context_data_end = LOADADDR(.contextdata) + SIZEOF(.contextdata);

	__data_load_start = LOADADDR(.data);
	__data_load_end = LOADADDR(.data) + SIZEOF(.data);
}

ENTRY(arch_init)

