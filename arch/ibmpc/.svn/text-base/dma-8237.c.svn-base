
#include <hexo/types.h>
#include <hexo/iospace.h>

#include <arch/dma-8237.h>

#include <assert.h>

void dma_8237_init(void)
{
}

void dma_8237_channel_init(uint_fast8_t channel, uint8_t mode,
			      uintptr_t address, size_t count)
{
  uint16_t	offset = address & 0xffff;
  uint16_t	page = address >> 16;
  uint16_t	len = count - 1;

  assert(page < 0x0100);
  assert(count < 0x00010000);
  assert(count + offset < 0x00010000);

  /* disable DMA channel */
  cpu_io_write_8(channel > 3 ? DMA8237_MASK1_16BITS_REG : DMA8237_MASK1_8BITS_REG,
		 DMA8237_MASK1_DISABLE | DMA8237_MASK1_CHANNEL(channel));

  /* set DMA mode */
  cpu_io_write_8(channel > 3 ? DMA8237_MODE_16BITS_REG : DMA8237_MODE_8BITS_REG,
		 mode);

  /* clear flipflop */
  cpu_io_write_8(channel > 3 ? DMA8237_FLIPFLOP_16BITS_REG : DMA8237_FLIPFLOP_8BITS_REG,
		 0);

  /* set size */
  cpu_io_write_8(DMA8237_COUNT_REG(channel), len & 0xff);
  cpu_io_write_8(DMA8237_COUNT_REG(channel), len >> 8);

  /* set page */
  cpu_io_write_8(DMA8237_PAGE_REG(channel), page);

  /* set offset */
  cpu_io_write_8(DMA8237_ADDR_REG(channel), offset);

  /* enable DMA channel */
  cpu_io_write_8(channel > 3 ? DMA8237_MASK1_16BITS_REG : DMA8237_MASK1_8BITS_REG,
		 DMA8237_MASK1_ENABLE | DMA8237_MASK1_CHANNEL(channel));
}

void dma_8237_channel_disable(uint_fast8_t channel)
{
  /* disable DMA channel */
  cpu_io_write_8(channel > 3 ? DMA8237_MASK1_16BITS_REG : DMA8237_MASK1_8BITS_REG,
		 DMA8237_MASK1_DISABLE | DMA8237_MASK1_CHANNEL(channel));
}

