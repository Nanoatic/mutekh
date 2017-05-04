
#include "srl.h"
#include "mjpeg_tasks.h"

#include "jpeg_data.h"
#include "jpeg.h"

void libu_func_libu(struct _libu_args_t *_func_args)
{
  srl_mwmr_t input =  _func_args->input;
  srl_mwmr_t output = _func_args->output;
  uint8_t input_buffer[BLOCK_SIZE];
  uint8_t output_buffer[WIDTH*BLOCK_HEIGHT];

  //srl_log(TRACE, "LIBU thread is alive !\n");

  int16_t line, column;

  for ( line=0; line<BLOCKS_H; ++line ) {
    //srl_log_printf(TRACE, "LIBU processing stripe %d/%d\n", line, BLOCKS_H);

    for ( column=0; column<BLOCKS_W; ++column ) {
      char i;
      srl_mwmr_read( input, input_buffer, BLOCK_SIZE );

      for ( i=0; i<BLOCK_HEIGHT; ++i ) {
	memcpy( &output_buffer[i*WIDTH+BLOCK_WIDTH*column],
		&input_buffer[i*BLOCK_WIDTH],
		BLOCK_WIDTH );
      }
    }
    srl_mwmr_write( output, output_buffer, WIDTH*BLOCK_HEIGHT );
  }
}
