
#include "srl.h"

#include "mjpeg_tasks.h"
#include "jpeg_data.h"

#include "jpeg.h"

static uint8_t *data_ptr, *data_end;

void tg_func_bootstrap(struct _tg_args_t *_func_args)
{
  data_ptr = jpeg_data;
  data_end = data_ptr + sizeof(jpeg_data);
}

void tg_func_tg(struct _tg_args_t *_func_args)
{
  srl_mwmr_t output = _func_args->output;
  size_t s, len = 32;
  uint8_t e_[32], *e = e_;

  while (len--)
    {
      *e++ = *data_ptr++;
      if (data_ptr == data_end)
	data_ptr = jpeg_data;
    }

  srl_mwmr_write(output,e_,32);
}

