/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright (c) Nicolas Pouillon <nipo@ssji.net> 2009

*/

#ifndef SD_MMC_PRIVATE_H_
#define SD_MMC_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>

#define SD_MMC_MAX_RQ_COUNT 64

enum card_mode_e
{
	MODE_SD,
	MODE_MMC,
};

struct sdmmc_csd_s
{
	ENDIAN_BITFIELD(
		uint32_t csd_structure:2,
		uint32_t reserved_0:6,
		uint32_t taac_reserved:1,
		uint32_t taac_value:4,
		uint32_t taac_exponent:3,
		uint32_t nsac:8,
		uint32_t tran_speed_reserved:1,
		uint32_t tran_speed_mantissa:4,
		uint32_t tran_speed_exp:3,
		);
	ENDIAN_BITFIELD(
		uint32_t ccc:12,
		uint32_t read_bl_len:4,
		uint32_t read_bl_partial:1,
		uint32_t write_blk_misalign:1,
		uint32_t read_blk_misalign:1,
		uint32_t dsr_imp:1,
		uint32_t reserved_1:2,
		uint32_t c_size_high10:10,
		);
	ENDIAN_BITFIELD(
		uint32_t c_size_low2:2,
		uint32_t vdd_r_curr_min:3,
		uint32_t vdd_r_curr_max:3,
		uint32_t vdd_w_curr_min:3,
		uint32_t vdd_w_curr_max:3,
		uint32_t c_size_mult:3,
		uint32_t erase_blk_en:1,
		uint32_t sector_size:7,
		uint32_t wp_grp_size:7,
		);
	ENDIAN_BITFIELD(
		uint32_t wp_grp_enable:1,
		uint32_t reserved_2:2,
		uint32_t r2w_factor:3,
		uint32_t write_bl_len:4,
		uint32_t write_bl_partial:1,
		uint32_t reserved_3:5,
		uint32_t file_format_grp:1,
		uint32_t copy:1,
		uint32_t perm_write_protect:1,
		uint32_t tmp_write_protect:1,
		uint32_t file_format:2,
		uint32_t reserved_4:2,
		uint32_t crc:7,
		uint32_t always_one:1,
		);
} __attribute__((packed));


struct cmd_state_s
{
	bool_t done;
	bool_t error;
	uint8_t data;
};

struct rw_cmd_state_s
{
	struct cmd_state_s state;
	struct device_s *dev;
	struct dev_block_rq_s *blockdev_rq;
	uint32_t count;
};

struct sd_mmc_rw_command_buffer_s
{
	struct rw_cmd_state_s state;
	uint8_t command_value[6];
	uint8_t crc[2];
	uint8_t tmp[10];
	struct dev_spi_rq_cmd_s spi_commands[8];
	struct dev_spi_rq_s spi_request;
};


struct sd_mmc_context_s
{
	struct dev_block_params_s params;
	size_t access_timeout;
	size_t bus_freq;
	struct sdmmc_csd_s csd;
	struct device_s *spi;
	struct sd_mmc_rw_command_buffer_s rw_cmd_buffer;
	dev_blk_queue_root_t queue;
	enum card_mode_e mode;
	uint_fast8_t spi_lun;
	bool_t usable;
};

#define SDMMC_CMD_GO_IDLE_STATE      0
#define SDMMC_CMD_ACMD               55
#define SDMMC_CMD_SEND_OP_COND       1
#define SDMMC_ACMD_SEND_OP_COND      41
#define SDMMC_CMD_SEND_CSD           9
#define SDMMC_CMD_READ_SINGLE_BLOCK  17
#define SDMMC_CMD_WRITE_SINGLE_BLOCK 24

#define SDMMC_R1_IDLE        (1<<0)
#define SDMMC_R1_ERASE_RESET (1<<1)
#define SDMMC_R1_ILLEGAL     (1<<2)
#define SDMMC_R1_CRC_ERROR   (1<<3)
#define SDMMC_R1_ERASE_ERROR (1<<4)
#define SDMMC_R1_ADDR_ERROR  (1<<5)
#define SDMMC_R1_PARAM_ERROR (1<<6)

#define SDMMC_TOKEN_DATA_START 0xfe
#define SDMMC_TOKEN_MULT_WRITE_DATA_START 0xfc
#define SDMMC_TOKEN_MULT_WRITE_DATA_STOP 0xfd

#endif
