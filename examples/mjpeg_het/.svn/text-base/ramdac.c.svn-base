
#include <mutek/printk.h>
#include "srl.h"
#include "mjpeg_tasks.h"

#include "jpeg_data.h"
#include "jpeg.h"

static uint32_t chksum = 0;

void ramdac_func_bootstrap(struct _ramdac_args_t *_func_args)
{
}

void ramdac_func_ramdac(struct _ramdac_args_t *_func_args)
{
  srl_mwmr_t input = _func_args->input;
  int32_t i, j;

  for (i = 0; i < MAX_HEIGHT; i += 8) {
    uint8_t row[MAX_WIDTH * 8];

    srl_mwmr_read( input, row, 8 * MAX_WIDTH );

    for (j = 0; j < MAX_WIDTH * 8; j++)
      chksum = (chksum << 1) + row[j];
  }

  printk("Image sum: %08x\n", chksum);
}

