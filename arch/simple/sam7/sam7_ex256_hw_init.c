#include <hexo/init.h>
#include <hexo/types.h>

#include <drivers/char/uart-us6089c/uart-us6089c.h>
#include <drivers/i2c/twi6061a/i2c-twi6061a.h>
#include <drivers/spi/spi6088d/spi-spi6088d.h>

#include <drivers/char/dbgu-sam7/dbgu-sam7.h>
#include <drivers/icu/sam7/icu-sam7.h>
#include <drivers/gpio/sam7/gpio-sam7.h>

#include <drivers/lcd/s1d15g00/s1d15g00.h>
#include <drivers/input/mt5-f/mt5-f.h>
#include <drivers/block/sd-mmc/sd-mmc.h>

#include <drivers/timer/pitc_6079a/pitc_6079a.h>

#include <device/device.h>
#include <device/driver.h>

#include <mutek/printk.h>

#include "arch/sam7/at91sam7x256.h"

struct device_s dev_gpio_pioa;
struct device_s dev_gpio_piob;
struct device_s uart_dev;
struct device_s icu_dev;
struct device_s bd_dev;
struct device_s spi0_dev;
struct device_s spi1_dev;
struct device_s i2c_dev;
struct device_s lcd_dev;
struct device_s dev_mt5f;
struct device_s pitc_dev;

extern struct device_s *console_dev;

#if defined(CONFIG_MUTEK_TIMERMS)
extern struct device_s *timerms_dev;
#endif

static void set_gpio(
	struct device_s *pio,
	uint_fast8_t gpio,
	uint_fast8_t dev,
	enum devgpio_way_e way)
{
	dev_gpio_set_value(pio, gpio, 1);
	dev_gpio_set_way(pio, gpio, way);
	dev_gpio_assign_to_peripheral(pio, gpio, dev);
}

static inline PRINTF_OUTPUT_FUNC(__printf_out_tty)
{
  while (len > 0)
    {
		ssize_t	res = dev_char_spin_write((struct device_s *)ctx, (uint8_t*)str, len);

      if (res < 0)
	break;
      len -= res;
      str += res;
    }
}

void arch_specific_hw_init()
{
	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_FIQ;
	icu_dev.addr[0] = (uintptr_t)AT91C_BASE_AIC;
#  ifdef CONFIG_DRIVER_ICU_ARM
	icu_dev.irq = 0;
	icu_dev.icudev = CPU_LOCAL_ADDR(cpu_icu_dev);
#  endif
	icu_sam7_init(&icu_dev, NULL);

    cpu_interrupt_enable();

#if defined(CONFIG_DRIVER_CHAR_SAM7DBGU) && defined(CONFIG_MUTEK_CONSOLE)
	sam7_dbgu_init();
#endif

	// GPIO controllers
	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;
	device_init(&dev_gpio_pioa);
	dev_gpio_pioa.addr[0] = (uintptr_t)AT91C_BASE_PIOA;
	dev_gpio_pioa.irq = AT91C_ID_PIOA;
	dev_gpio_pioa.icudev = &icu_dev;
	gpio_sam7_init(&dev_gpio_pioa, NULL);

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOB;
	device_init(&dev_gpio_piob);
	dev_gpio_piob.addr[0] = (uintptr_t)AT91C_BASE_PIOB;
	dev_gpio_pioa.irq = AT91C_ID_PIOB;
	dev_gpio_piob.icudev = &icu_dev;
	gpio_sam7_init(&dev_gpio_piob, NULL);

	
	// UART0
	set_gpio(&dev_gpio_pioa, 0, 1, GPIO_WAY_INPUT); // rxd
	set_gpio(&dev_gpio_pioa, 1, 1, GPIO_WAY_OUTPUT); // txd
	set_gpio(&dev_gpio_pioa, 3, 1, GPIO_WAY_OUTPUT); // rts
	set_gpio(&dev_gpio_pioa, 4, 1, GPIO_WAY_INPUT); // cts

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_US0;
	uart_dev.addr[0] = (uintptr_t)AT91C_BASE_US0;
	uart_dev.irq = AT91C_ID_US0;
	uart_dev.icudev = &icu_dev;
	uart_us6089c_init(&uart_dev, NULL);

	console_dev = &uart_dev;
#if !(defined(CONFIG_DRIVER_CHAR_SAM7DBGU) && defined(CONFIG_MUTEK_CONSOLE)) && !defined(CONFIG_MUTEK_PRINTK_KEEP_EARLY)
	printk_set_output(__printf_out_tty, console_dev);
#endif

	// spi0
	set_gpio(&dev_gpio_pioa, 16, 1, GPIO_WAY_INPUT);  // miso
	set_gpio(&dev_gpio_pioa, 17, 1, GPIO_WAY_OUTPUT); // mosi
	set_gpio(&dev_gpio_pioa, 18, 1, GPIO_WAY_OUTPUT); // sck
	set_gpio(&dev_gpio_pioa, 12, 1, GPIO_WAY_OUTPUT); // cs0
	set_gpio(&dev_gpio_pioa, 13, 1, GPIO_WAY_OUTPUT); // cs1

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI0;
	{
		struct spi_spi6088d_param_s params = { .lun_count = 2, };
		device_init(&spi0_dev);
		spi0_dev.addr[0] = (uintptr_t)AT91C_BASE_SPI0;
		spi0_dev.irq = 4;
		spi0_dev.icudev = &icu_dev;
		spi_spi6088d_init(&spi0_dev, &params);
	}


	// Joystick
	{
		struct dev_mt5f_param_s params = {
			.gpio_dev = &dev_gpio_pioa,
			.a = 7,
			.b = 8,
			.c = 9,
			.d = 14,
			.common = 15,
		};

		device_init(&dev_mt5f);
		dev_mt5f_init(&dev_mt5f, &params);
	}


	// LCD

	// enable reset & backlight
	set_gpio(&dev_gpio_pioa, 2, 0, GPIO_WAY_OUTPUT);
	set_gpio(&dev_gpio_piob, 20, 0, GPIO_WAY_OUTPUT);

	{
		struct s1d15g00_param_s params = {
			.spi = &spi0_dev,
			.spi_lun = 0,
			.set_reset_gpio_dev = &dev_gpio_pioa,
			.set_reset_gpio_id = 2,
		};
		
		lcd_dev.irq = 0;
		lcd_dev.icudev = NULL;
		s1d15g00_init(&lcd_dev, &params);
	}


	// i2c
	set_gpio(&dev_gpio_pioa, 10, 1, GPIO_WAY_OPENDRAIN);  // sda
	set_gpio(&dev_gpio_pioa, 11, 1, GPIO_WAY_OPENDRAIN);  // sck

/* 	set_gpio(&dev_gpio_pioa, 10, 1, GPIO_WAY_OUTPUT);  // sda */
/* 	set_gpio(&dev_gpio_pioa, 11, 1, GPIO_WAY_OUTPUT);  // sck */
/* 	dev_gpio_set_value(&dev_gpio_pioa, 10, 1);  // sda */
/* 	dev_gpio_set_value(&dev_gpio_pioa, 11, 1);  // sda */

//	AT91C_BASE_PIOA->PIO_PPUER = (1 << 10)|(1 << 11);

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_TWI;
	device_init(&i2c_dev);
	i2c_dev.addr[0] = (uintptr_t)AT91C_BASE_TWI;
	i2c_dev.irq = AT91C_ID_TWI;
	i2c_dev.icudev = &icu_dev;
	i2c_twi6061a_init(&i2c_dev, NULL);


	// spi1
	set_gpio(&dev_gpio_pioa, 24, 2, GPIO_WAY_INPUT);  // miso
	set_gpio(&dev_gpio_pioa, 23, 2, GPIO_WAY_OUTPUT); // mosi
	set_gpio(&dev_gpio_pioa, 22, 2, GPIO_WAY_OUTPUT); // sck

	AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI1;
	{
		struct spi_spi6088d_param_s params = { .lun_count = 1, };
		device_init(&spi1_dev);
		spi1_dev.addr[0] = (uintptr_t)AT91C_BASE_SPI1;
		spi1_dev.irq = AT91C_ID_SPI1;
		spi1_dev.icudev = &icu_dev;
		spi_spi6088d_init(&spi1_dev, &params);
	}


	{
		struct sd_mmc_param_s params = {
			.spi = &spi0_dev,
			.spi_lun = 1,
		};
		device_init(&bd_dev);
		sd_mmc_init(&bd_dev, &params);
	}

	// Timer
	device_init(&pitc_dev);
	pitc_dev.addr[0] = (uintptr_t)AT91C_BASE_PITC;
	pitc_dev.irq = ICU_SAM7_ID_PITC;
	pitc_dev.icudev = &icu_dev;
	pitc_6079a_init(&pitc_dev, NULL);

#if defined(CONFIG_MUTEK_TIMERMS)
	timerms_dev = &pitc_dev;
#endif
}
