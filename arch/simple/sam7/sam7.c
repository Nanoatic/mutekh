#include <hexo/local.h>
#include <hexo/types.h>
#include <hexo/interrupt.h>

#include <drivers/icu/arm/icu-arm.h>
#include <device/device.h>
#include <device/driver.h>

#include "arch/sam7/at91sam7x256.h"

#ifdef CONFIG_HEXO_IRQ
CPU_LOCAL cpu_interrupt_handler_t  *cpu_interrupt_handler;
CPU_LOCAL cpu_exception_handler_t  *cpu_exception_handler;

__attribute__ ((interrupt ("UNDEF")))
void arm_c_exc_undef()
{
	void *where = __builtin_return_address(0);
	cpu_exception_handler_t *exception_handler = CPU_LOCAL_GET(cpu_exception_handler);
	exception_handler(0, (uintptr_t)where, 0, 0, 0);
}

__attribute__ ((interrupt ("SWI")))
void arm_c_exc_swi()
{
	void *where = __builtin_return_address(0);
	cpu_exception_handler_t *exception_handler = CPU_LOCAL_GET(cpu_exception_handler);
	exception_handler(1, (uintptr_t)where, 0, 0, 0);
}

__attribute__ ((interrupt ("ABORT")))
void arm_c_exc_pabt()
{
	void *where = __builtin_return_address(0);
	cpu_exception_handler_t *exception_handler = CPU_LOCAL_GET(cpu_exception_handler);
	exception_handler(2, (uintptr_t)where, (uintptr_t)AT91C_BASE_MC->MC_AASR, 0, 0);
}

__attribute__ ((interrupt ("ABORT")))
void arm_c_exc_dabt()
{
	void *where = __builtin_return_address(0);
	cpu_exception_handler_t *exception_handler = CPU_LOCAL_GET(cpu_exception_handler);
	exception_handler(3, (uintptr_t)where, (uintptr_t)AT91C_BASE_MC->MC_AASR, 0, 0);
}

# ifdef CONFIG_DRIVER_ICU_ARM
__attribute__ ((interrupt ("IRQ")))
void arm_c_irq_handler()
{
	cpu_interrupt_handler(0);
}

__attribute__ ((interrupt ("FIQ")))
void arm_c_fiq_handler()
{
	cpu_interrupt_handler(0);
}
# endif
#endif

void arch_specific_init()
{
	// Flash write wait delay
	AT91C_BASE_MC->MC_FMR = (AT91C_MC_FMCN & (48 << 16)) | AT91C_MC_FWS_1FWS;

	// Enable external reset
	AT91C_BASE_RSTC->RSTC_RMR = 0xa5003001;

	// Reset external pins
	AT91C_BASE_PIOA->PIO_PPUER = (uint32_t)-1;
	AT91C_BASE_PIOA->PIO_ODR = (uint32_t)-1;
	AT91C_BASE_PIOA->PIO_PER = (uint32_t)-1;
//	AT91C_BASE_PIOA->PIO_PDR = (uint32_t)1;

	AT91C_BASE_PIOB->PIO_PPUER = (uint32_t)-1;
	AT91C_BASE_PIOB->PIO_ODR = (uint32_t)-1;
	AT91C_BASE_PIOB->PIO_PER = (uint32_t)-1;
//	AT91C_BASE_PIOB->PIO_PDR = (uint32_t)-1;

	// Disable watchdog
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	// Enable main oscillator
	AT91C_BASE_PMC->PMC_MOR = (AT91C_CKGR_OSCOUNT & (0x06 << 8)) | AT91C_CKGR_MOSCEN;

	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS));

	// Configure PLLs
	AT91C_BASE_PMC->PMC_PLLR = (
		(AT91C_CKGR_DIV & 0x05)
		| (AT91C_CKGR_PLLCOUNT & (28<<8))
		| (AT91C_CKGR_MUL & (25<<16))
		);

	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCK));
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

	AT91C_BASE_PMC->PMC_PCER = (1<<6) | (1<<1) | (1<<4) | (1<<2) | (1<<30) | (1<<31) | (1<<0);

 	// Selection of Master Clock and Processor Clock
	AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2 | AT91C_PMC_CSS_PLL_CLK;
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
}
