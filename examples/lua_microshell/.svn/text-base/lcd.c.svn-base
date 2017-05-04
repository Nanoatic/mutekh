#include <hexo/types.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include <device/lcd.h>
#include <device/gpio.h>
#include <device/block.h>

#include <device/device.h>
#include <device/driver.h>


extern struct device_s lcd_dev;
extern struct device_s bd_dev;
extern struct device_s dev_gpio_piob;

int cmd_lcd_blit_block(lua_State *st)
{
	uint8_t block[512];
	uint8_t *blocks[] = {block};

	dev_block_lba_t lba = lua_tonumber(st, 1);
	lcd_coord_t x = lua_tonumber(st, 2);
	lcd_coord_t y = lua_tonumber(st, 3);

	printf("Reading block... \n");
	dev_block_wait_read(&bd_dev, blocks, lba, 1);
	printf("ok\n");
	printf("Blitting LCD... \n");
	dev_lcd_blit(&lcd_dev, x, x+18, y, y+18, block);
	printf("ok\n");
	return 0;
}

int cmd_backlight(lua_State *st)
{
	bool_t val = lua_tonumber(st, 1);

	dev_gpio_set_value(&dev_gpio_piob, 20, val);

	return 0;
}

int cmd_lcd_blit(lua_State *st)
{
  size_t nargs = lua_gettop(st);
  if ( nargs < 5 )
	  return 0;

  dev_lcd_blit(&lcd_dev,
			   lua_tonumber(st, 1), lua_tonumber(st, 2),
			   lua_tonumber(st, 3), lua_tonumber(st, 4),
			   lua_tostring(st, 5));
  return 0;
}

int cmd_lcd_invert(lua_State *st)
{
	uint_fast8_t flags = 0;
	if (lua_tonumber(st, 1))
		flags |= LCD_INVERT;

	dev_lcd_setmode(&lcd_dev, 12, LCD_PACK_RGB, flags);
	return 0;
}

int cmd_lcd_set_contrast(lua_State *st)
{
	dev_lcd_setcontrast(&lcd_dev, lua_tonumber(st, 1));
	return 0;
}


void init_lcd_shell(lua_State *st)
{
	lua_register(st, "backlight", cmd_backlight);
	lua_register(st, "lcd_blit", cmd_lcd_blit);
	lua_register(st, "lcd_blit_block", cmd_lcd_blit_block);
	lua_register(st, "lcd_invert", cmd_lcd_invert);
	lua_register(st, "lcd_set_contrast", cmd_lcd_set_contrast);
}
