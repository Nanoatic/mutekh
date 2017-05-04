
#include <hexo/init.h>
#include <hexo/types.h>
#include <hexo/endian.h>

#include <hexo/interrupt.h>
#include <hexo/local.h>
#include <hexo/iospace.h>
#include <hexo/lock.h>
#include <hexo/context.h>
#include <hexo/cpu.h>
#include <mutek/scheduler.h>

#include <drivers/char/uart-8250/uart-8250.h>
#include <drivers/char/tty-vga/tty-vga.h>
#include <drivers/icu/8259/icu-8259.h>
#include <drivers/block/sd-mmc/sd-mmc.h>
#include <drivers/timer/8253/timer-8253.h>
#include <drivers/input/8042/input-8042.h>
#include <drivers/input/mt5-f/mt5-f.h>
#include <drivers/fb/vga/fb-vga.h>
#include <drivers/enum/pci/enum-pci.h>
#include <drivers/enum/isapnp/enum-isapnp.h>
#include <drivers/net/ne2000/net-ne2000.h>
#include <drivers/icu/apic/icu-apic.h>

#include <device/device.h>
#include <device/driver.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutek/timer.h>
#include <mutek/printk.h>

#ifdef CONFIG_ARCH_IBMPC_DMA
#include <arch/dma-8237.h>
#endif

#if defined(CONFIG_DRIVER_CHAR_VGATTY) || defined(CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD)
struct device_s tty_dev;
#endif

#if defined(CONFIG_DRIVER_CHAR_UART8250)
struct device_s uart_dev;
#endif

#if defined(CONFIG_DRIVER_FB)
struct device_s fb_dev;
#endif

#if defined(CONFIG_MUTEK_TIMERMS)
extern struct device_s *timerms_dev;
#endif

#if defined(CONFIG_DRIVER_TIMER)
struct device_s timer_dev;
#endif

#if defined(CONFIG_DRIVER_INPUT)
struct device_s keyboard_dev;
#endif

#if defined(CONFIG_DRIVER_ENUM_PCI)
struct device_s enum_pci;
#endif

#if defined(CONFIG_DRIVER_ENUM_ISAPNP)
struct device_s enum_isapnp;
#endif

#if defined(CONFIG_MUTEK_LOGO)
extern const uint8_t mutek_logo_320x200[320*200];
#endif

#if defined(CONFIG_DRIVER_ICU_8259)
struct device_s icu_dev;
#endif

#ifdef CONFIG_MUTEK_CONSOLE
extern struct device_s *console_dev;
#endif

void arch_hw_init()
{
  /********* ICU init ******************************** */

#if defined(CONFIG_DRIVER_ICU_8259)
	device_init(&icu_dev);
	icu_dev.addr[0] = 0x20;
	icu_dev.addr[1] = 0xa0;

# if defined(CONFIG_DRIVER_ICU_APIC)
	icu_dev.icudev = CPU_LOCAL_ADDR(apic_dev);
	icu_dev.irq = 0;
# endif

	icu_8259_init(&icu_dev, NULL);
#endif

	/********* TTY init ******************************** */

#if defined(CONFIG_DRIVER_CHAR_UART8250)
	device_init(&uart_dev);
	uart_dev.addr[UART_8250_ADDR] = 0x03f8;
	uart_dev.irq = 4;
	uart_dev.icudev = &icu_dev;
	uart_8250_init(&uart_dev, NULL);
#endif

#if defined(CONFIG_DRIVER_CHAR_VGATTY) || defined(CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD)
	device_init(&tty_dev);
# if defined(CONFIG_DRIVER_CHAR_VGATTY)
	tty_dev.addr[VGA_TTY_ADDR_BUFFER] = 0x000b8000;
	tty_dev.addr[VGA_TTY_ADDR_CRTC] = 0x03d4;
#  if defined(CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD)
	tty_dev.irq = 1;
	tty_dev.icudev = &icu_dev;
#  endif
	tty_vga_init(&tty_dev, NULL);
# else
#  warning CONFIG_DRIVER_CHAR case not handled in hw_init()
# endif
#endif

	/********* Timer init ******************************** */

#if defined(CONFIG_DRIVER_TIMER)
	device_init(&timer_dev);
# if defined(CONFIG_DRIVER_TIMER_8253)
	timer_dev.addr[0] = 0x0040;
	timer_dev.irq = 0;
	timer_dev.icudev = &icu_dev;
	timer_8253_init(&timer_dev, NULL);
#  if defined(CONFIG_MUTEK_TIMERMS)
	timerms_dev = &timer_dev;
#  endif
# else
#  warning CONFIG_DRIVER_TIMER case not handled in hw_init()
# endif
#endif

#if defined(CONFIG_DRIVER_INPUT)
	device_init(&keyboard_dev);
# if defined(CONFIG_DRIVER_INPUT_8042)
	keyboard_dev.addr[0] = 0x60;
	keyboard_dev.irq = 1;
	keyboard_dev.icudev = &icu_dev;
	input_8042_init(&keyboard_dev, NULL);
# else
#  warning CONFIG_DRIVER_INPUT case not handled in hw_init()
# endif
#endif

	/********* FB init ********************************* */

#if defined(CONFIG_DRIVER_FB)
	device_init(&fb_dev);
# if defined(CONFIG_DRIVER_FB_VGA)
	fb_vga_init(&fb_dev, NULL);
	fb_vga_setmode(&fb_dev, 320, 200, 8, FB_PACK_INDEX);
#  if defined(CONFIG_MUTEK_LOGO)
	uint8_t *p = (void*)fb_vga_getbuffer(&fb_dev, 0);
	memcpy(p, mutek_logo_320x200, 64000);
#  endif
# else
#  warning CONFIG_DRIVER_FB case not handled in hw_init()
# endif
#endif

#if defined(CONFIG_DRIVER_ENUM_PCI)
	device_init(&enum_pci);
	enum_pci.icudev = &icu_dev;
	enum_pci_init(&enum_pci, NULL);
#endif

#if defined(CONFIG_DRIVER_ENUM_ISAPNP)
	device_init(&enum_isapnp);
	enum_usapnp.icudev = &icu_dev;
	enum_isapnp_init(&enum_isapnp, NULL);
#endif

#if defined(CONFIG_DRIVER_NET_NE2000)
#if 0
	/* driver for the D-Link DE200-TP */
	static struct device_s net_dlink_200tp;

	device_init(&net_dlink_200tp);
	net_dlink_200tp.addr[0] = 0x320;
	net_dlink_200tp.irq = 5;
	net_dlink_200tp.icudev = &icu_dev;
	net_ne2000_init(&net_dlink_200tp, NULL);
#endif
#if 0
	/* driver for the UMC9008 */
	static struct device_s net_umc_9008;

	device_init(&net_umc_9008);
	net_umc_9008.addr[0] = 0x300;
	net_umc_9008.irq = 3;
	net_umc_9008.icudev = &icu_dev;
	net_ne2000_init(&net_umc_9008, NULL);
#endif
# endif

#ifdef CONFIG_ARCH_IBMPC_DMA
	dma_8237_init();
#endif

#if defined(CONFIG_MUTEK_CONSOLE)
# if defined(CONFIG_DRIVER_CHAR_VGATTY) || defined(CONFIG_DRIVER_CHAR_VGATTY_KEYBOARD)
	console_dev = &tty_dev;
# elif defined(CONFIG_DRIVER_CHAR_UART8250)
	console_dev = &uart_dev;
# else
#  error I would like an output for my console
# endif
# endif
}
