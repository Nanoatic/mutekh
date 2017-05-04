#include <lua/lauxlib.h>
#include <lua/lua.h>

#include <stdlib.h>
#include <string.h>

#include <termui/term.h>
#include <termui/getline.h>

#include <device/block.h>
#include <device/device.h>
#include <device/driver.h>

extern struct device_s bd_dev;

void do_block_hexdump(struct device_s *bd, size_t lba)
{
	const struct dev_block_params_s *params = dev_block_getparams(bd);
	
	uint8_t block[params->blk_size];
	uint8_t *blocks[1] = {block};

	error_t err = dev_block_wait_read(bd, blocks, lba, 1);
	if ( err ) {
		printf("Error reading LBA %x, %d\r\n", lba, err);
	} else {
		size_t i;
		printf("LBA %x, read ok\r\n", lba);
		for ( i=0; i<params->blk_size; i+=16 )
			printf(" %p: %P\r\n", (void*)(uintptr_t)i, &block[i], 16);
	}
}

int cmd_block_hexdump(lua_State *st)
{
	if ( lua_gettop(st) < 1 )
		return 1;

	uint32_t lba = lua_tonumber(st, 1);
	do_block_hexdump(&bd_dev, lba);
	return 0;
}

error_t sd_mmc_rehash(struct device_s *dev);

int cmd_sd_mmc_rehash(lua_State *st)
{
	sd_mmc_rehash(&bd_dev);
	return 0;
}

/* extern uint8_t xfer_delay; */
/* extern uint8_t cs_delay; */
/* extern uint32_t base_br; */
/* extern bool_t cs_bet; */
/* extern uint8_t sd_mode; */

/* int cmd_sd_mmc_delays(lua_State *st) */
/* { */
/* 	xfer_delay = lua_tonumber(st, 1); */
/* 	cs_delay = lua_tonumber(st, 2); */
/* 	base_br = lua_tonumber(st, 3); */
/* 	cs_bet = lua_tonumber(st, 4); */
/* 	sd_mode = lua_tonumber(st, 5); */
/* 	sd_mmc_rehash(&bd_dev); */
/* 	return 0; */
/* } */

void term_block_init(lua_State *st)
{
	lua_register(st, "block_hexdump", cmd_block_hexdump);
	lua_register(st, "sd_mmc_rehash", cmd_sd_mmc_rehash);
/* 	lua_register(st, "sd_mmc_delays", cmd_sd_mmc_delays); */
}
