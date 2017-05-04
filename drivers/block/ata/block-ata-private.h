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

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#ifndef BLOCK_ATA_PRIVATE_H_
#define BLOCK_ATA_PRIVATE_H_

#include <hexo/types.h>
#include <hexo/lock.h>
#include <hexo/endian.h>

/*
  first registers block (0x1f0)
*/

#define ATA_REG_DATA			0
#define ATA_REG_ERROR			1
 /* error bits in error register */
#define  ATA_ERROR_BADBLOCK		0x80
#define  ATA_ERROR_UNCORRECTABLE	0x40
#define  ATA_ERROR_MC			0x20
#define  ATA_ERROR_NOTFOUND		0x10
#define  ATA_ERROR_MCR			0x08
#define  ATA_ERROR_ABORT		0x04
#define  ATA_ERROR_TRACK0		0x02
#define  ATA_ERROR_BADDATAMARK		0x01
 /* diagnostic bits in error register */
#define  ATA_DIAG_NOERROR		0x01
#define  ATA_DIAG_FORMATTERERROR	0x02
#define  ATA_DIAG_BUFFERERROR		0x03
#define  ATA_DIAG_ECCERROR		0x04
#define  ATA_DIAG_MICROPROCERROR	0x05
#define  ATA_DIAG_ERRORMASK		0x7e
#define  ATA_DIAG_SLAVEERROR		0x80

#define ATA_REG_FEATURE			1
#define ATA_REG_SECTOR_COUNT		2
#define ATA_REG_SECTOR_NUMBER		3
#define ATA_REG_CYLINDER_LOW		4
# define ATA_ATAPI_LOW_SIGN		0x14
#define ATA_REG_CYLINDER_HIGH		5
# define ATA_ATAPI_HIGH_SIGN		0xeb
#define ATA_REG_DRVHEAD			6
#define  ATA_DRVHEAD_RESERVED_HIGH	0xa0
#define  ATA_DRVHEAD_HEADMASK		0x0f
#define  ATA_DRVHEAD_SLAVE		0x10
#define  ATA_DRVHEAD_LBA		0x40

#define ATA_REG_STATUS			7
#define  ATA_STATUS_BUSY		0x80
#define  ATA_STATUS_READY		0x40
#define  ATA_STATUS_WRITE_FAULT		0x20
#define  ATA_STATUS_NO_SEEK		0x10
#define  ATA_STATUS_DATA_RQ		0x08
#define  ATA_STATUS_CORRECTED_data	0x04
#define  ATA_STATUS_REVOLUTION		0x02
#define  ATA_STATUS_ERROR		0x20

#define ATA_REG_COMMAND			7

#define ATA_CMD_DRIVE_DIAGNOSTICS	0x90
#define ATA_CMD_FORMAT_TRACK		0x50
#define ATA_CMD_IDENTIFY_DRIVE		0xec
#define ATA_CMD_INIT_DRIVE_PARAM	0x91
#define ATA_CMD_NOP			0x0
#define ATA_CMD_READ_BUFFER		0xe4
#define ATA_CMD_READ_DMA_RTY		0xc8
#define ATA_CMD_READ_DMA		0xc9
#define ATA_CMD_READ_LONG_RTY		0x22
#define ATA_CMD_READ_LONG		0x23
#define ATA_CMD_READ_MULTIPLE		0xc4
#define ATA_CMD_READ_SECTORS_RTY	0x20
#define ATA_CMD_READ_SECTORS		0x21
#define ATA_CMD_READ_VERIFY_SECTORS_RTY 0x40
#define ATA_CMD_READ_VERIFY_SECTORS	0x41
#define ATA_CMD_RECALIBRATE		0x10
#define ATA_CMD_SEEK			0x70
#define ATA_CMD_SET_FEATURES		0xef
#define ATA_CMD_SET_MULTIPLE_MODE	0xc6
#define ATA_CMD_WRITE_BUFFER		0xe8
#define ATA_CMD_WRITE_DMA_RTY		0xca
#define ATA_CMD_WRITE_DMA		0xcb
#define ATA_CMD_WRITE_LONG_RTY		0x32
#define ATA_CMD_WRITE_LONG		0x33
#define ATA_CMD_WRITE_MULTIPLE		0xc5
#define ATA_CMD_WRITE_SAME		0xe9
#define ATA_CMD_WRITE_SECTORS_RTY	0x30
#define ATA_CMD_WRITE_SECTORS		0x31
#define ATA_CMD_WRITE_VERIFY		0x3c
#define ATA_CMD_CECK_POWER_MODE		0x98
#define ATA_CMD_IDLE			0x97
#define ATA_CMD_IDLE_IMMEDIATE		0x95
#define ATA_CMD_SLEEP			0x99
#define ATA_CMD_STANDBY			0x96
#define ATA_CMD_STANDBY_IMMEDIATE	0x94

/*
  second registers block (0x3f4)
*/

#define ATA_REG_ALTERNATE_STATUS	2
#define ATA_REG_DEVICE_CONTROL		2
#define  ATA_DEVCTRL_RESERVED_HIGH	0x08
#define  ATA_DEVCTRL_DISABLE_IRQ	0x02
#define  ATA_DEVCTRL_RESET		0x04
#define ATA_REG_DRIVE_ADDRESS		3

struct ata_indent_s
{
  uint16_t	res0[27 - 0];

  uint8_t	model_name[40];

  uint16_t	res47[49 - 47];

  ENDIAN_BITFIELD(uint16_t	res49_1:8,
		  uint16_t	dma_supported:1,
		  uint16_t	lba_supported:1,
		  uint16_t	res49_10:6
		  );

  uint16_t	res60[60 - 50];

  uint16_t	lba_count_low;
  uint16_t	lba_count_high;

  uint16_t	res62[256 - 62];
} __attribute__ ((packed));

struct drive_ata_context_s
{
  struct ata_indent_s	ident;
  struct dev_block_params_s drv_params;
  uint8_t		devhead_reg;
  dev_blk_queue_root_t	queue;
  size_t		ata_sec_count; /* sector count of current ata operation */
};

#define DRIVE_ATA_START_FUNC(n) void (n) (struct device_s *dev, struct dev_block_rq_s *rq)
typedef DRIVE_ATA_START_FUNC(drive_ata_start_func_t);
#define DRIVE_ATA_IRQ_FUNC(n) bool_t (n) (struct device_s *dev, struct dev_block_rq_s *rq)
typedef DRIVE_ATA_IRQ_FUNC(drive_ata_irq_func_t);

struct drive_ata_rq_s
{
  /* this function is called on irq */
  drive_ata_irq_func_t *irq;
  /* this function is called to start operation on idle device */
  drive_ata_start_func_t *start;
};

struct controller_ata_context_s
{
  struct device_s	*drive[2];
};

bool_t controller_ata_waitbusy(struct device_s *dev);
error_t drive_ata_init(struct device_s *dev, bool_t slave);
bool_t drive_ata_try_irq(struct device_s *dev);
DEV_CLEANUP(drive_ata_cleanup);

static inline void
controller_ata_reg_w8(struct device_s *dev, uint8_t reg, uint8_t data)
{
  cpu_io_write_8(dev->addr[0] + reg, data);
}

static inline void
controller_ata_rega_w8(struct device_s *dev, uint8_t reg, uint8_t data)
{
  cpu_io_write_8(dev->addr[1] + reg, data);
}

static inline uint8_t
controller_ata_reg_r8(struct device_s *dev, uint8_t reg)
{
  return cpu_io_read_8(dev->addr[0] + reg);
}

static inline uint16_t
controller_ata_reg_r16(struct device_s *dev, uint8_t reg)
{
  return cpu_io_read_16(dev->addr[0] + reg);
}

static inline void
controller_ata_data_read16(struct device_s *dev, void *data)
{
  uint16_t *d = data;
  uint_fast16_t i;

  for (i = 0; i < 256; i++)
    d[i] = cpu_io_read_16(dev->addr[0] + ATA_REG_DATA);
}

static inline void
controller_ata_data_write16(struct device_s *dev, void *const data)
{
  uint16_t *d = data;
  uint_fast16_t i;

  for (i = 0; i < 256; i++)
    cpu_io_write_16(dev->addr[0] + ATA_REG_DATA, d[i]);
}

static inline void
controller_ata_data_swapread16(struct device_s *dev, void *data)
{
  uint16_t *d = data;
  uint_fast16_t i;

  for (i = 0; i < 256; i++)
    d[i] = endian_swap16(cpu_io_read_16(dev->addr[0] + ATA_REG_DATA));
}



#endif

