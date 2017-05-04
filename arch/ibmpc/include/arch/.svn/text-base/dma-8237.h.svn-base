
#ifndef DMA_8237_H_
#define DMA_8237_H_

#ifndef CONFIG_ARCH_IBMPC_DMA
# warning CONFIG_ARCH_IBMPC_DMA configuration token is disabled
#else

/********************* */
#define DMA8237_ADDR_8BITS_REG(c)	(0x00 + (c) * 2)
#define DMA8237_ADDR_16BITS_REG(c)	(0xc0 + (c) * 4 - 16)

#define DMA8237_ADDR_REG(c)		((c) > 3 ? DMA8237_ADDR_16BITS_REG(c) : DMA8237_ADDR_8BITS_REG(c))

/********************* */
#define DMA8237_COUNT_8BITS_REG(c)	(0x01 + (c) * 2)
#define DMA8237_COUNT_16BITS_REG(c)	(0xc2 + (c) * 4 - 16)

#define DMA8237_COUNT_REG(c)		((c) > 3 ? DMA8237_COUNT_16BITS_REG(c) : DMA8237_COUNT_8BITS_REG(c))

/********************* */
#define DMA8237_PAGE_REG(c)		(0x80 | ((0xa9bf2437 >> ((c) * 4)) & 0xf))

/********************* */
#define DMA8237_CMD_8BITS_REG		0x08
#define DMA8237_CMD_16BITS_REG		0xd0

/********************* */
#define DMA8237_REQ_8BITS_REG		0x09
#define DMA8237_REQ_16BITS_REG		0xd2

/********************* */
#define DMA8237_MASK1_8BITS_REG		0x0a
#define DMA8237_MASK1_16BITS_REG	0xd4

#define DMA8237_MASK1_ENABLE		0x00
#define DMA8237_MASK1_DISABLE		0x04

#define DMA8237_MASK1_CHANNEL(c)	((c) & 0x3)

/********************* mode */
#define DMA8237_MODE_8BITS_REG		0x0b
#define DMA8237_MODE_16BITS_REG		0xd6

#define DMA8237_MODE_DEMAND		0x0000
#define DMA8237_MODE_SINGLE		0x4000
#define DMA8237_MODE_BLOCK		0x8000
#define DMA8237_MODE_CASCADE		0xc000

#define DMA8237_MODE_INCREMENT		0x0000
#define DMA8237_MODE_DECREMENT		0x2000

#define DMA8237_MODE_SINGLE_CYCLE	0x00
#define DMA8237_MODE_AUTO_CYCLE		0x10

#define DMA8237_MODE_VERIFY		0x00
#define DMA8237_MODE_WRITE		0x04
#define DMA8237_MODE_READ		0x08

#define DMA8237_MODE_CHANNEL(c)		((c) & 0x3)

/********************* */
#define DMA8237_FLIPFLOP_8BITS_REG	0x0c
#define DMA8237_FLIPFLOP_16BITS_REG	0xd8

/********************* */
#define DMA8237_RESET_8BITS_REG		0x0d
#define DMA8237_RESET_16BITS_REG	0xda

/********************* */
#define DMA8237_MASKRST_8BITS_REG	0x0e
#define DMA8237_MASKRST_16BITS_REG	0xdc

/********************* */
#define DMA8237_MASK2_8BITS_REG		0x0f
#define DMA8237_MASK2_16BITS_REG	0xde


/************************************************************************/

void dma_8237_init(void);

void dma_8237_channel_init(uint_fast8_t channel, uint8_t mode,
			   uintptr_t address, size_t count);

void dma_8237_channel_disable(uint_fast8_t channel);

#endif
#endif

