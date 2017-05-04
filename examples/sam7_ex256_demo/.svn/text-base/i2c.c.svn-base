
#include <device/gpio.h>
#include <device/i2c.h>
#include <device/driver.h>
#include <device/device.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include <stdio.h>

extern struct device_s dev_gpio_pioa;
extern struct device_s icu_dev;
extern struct device_s i2c_dev;
extern struct device_s spi1_dev;


int cmd_i2c_read(lua_State *st)
{
	size_t nargs = lua_gettop(st);

	if ( nargs < 3 )
		return 0;

	uint8_t addr = lua_tonumber(st, 1);
	uint8_t iaddr = lua_tonumber(st, 2);
	uint32_t size = lua_tonumber(st, 3);
	uint8_t data[size];

	error_t err = dev_i2c_read(&i2c_dev, addr, DEV_I2C_IADDR_8BITS|iaddr, data, size);
	if ( err )
		printf("Read data from %x failed: %d\n", addr, err);
	else
		printf("Read data from %x: %P\n", addr, data, size);

	return 0;
}

int cmd_i2c_write(lua_State *st)
{
	size_t nargs = lua_gettop(st);

	if ( nargs < 3 )
		return 0;

	uint8_t addr = lua_tonumber(st, 1);
	uint8_t iaddr = lua_tonumber(st, 2);
	uint8_t data = lua_tonumber(st, 3);

	error_t err = dev_i2c_write(&i2c_dev, addr, DEV_I2C_IADDR_8BITS|iaddr, &data, 1);
	if ( err )
		printf("Write data to %x failed: %d\n", addr, err);
	else
		printf("Write data to %x OK\n", addr);

	return 0;
}


void term_i2c_init(lua_State *st)
{
	uint_fast16_t i;
	
	dev_gpio_set_value(&dev_gpio_pioa, 21, 1);
	dev_gpio_set_way(&dev_gpio_pioa, 21, GPIO_WAY_OUTPUT);
	dev_gpio_assign_to_peripheral(&dev_gpio_pioa, 21, 0);
	for (i=0; i<100; ++i) asm volatile("nop");
	dev_gpio_set_value(&dev_gpio_pioa, 21, 0);
	for (i=0; i<100; ++i) asm volatile("nop");
	dev_gpio_set_value(&dev_gpio_pioa, 21, 1);

	lua_register(st, "i2c_read", cmd_i2c_read);
	lua_register(st, "i2c_write", cmd_i2c_write);
}
