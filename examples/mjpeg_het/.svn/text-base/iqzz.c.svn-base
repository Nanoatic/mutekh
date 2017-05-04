
#include "srl.h"
#include "mjpeg_tasks.h"

#include "jpeg.h"
#include "jpeg_data.h"

static const uint8_t G_ZZ[64] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

void iqzz_func_iqzz(struct _iqzz_args_t *_func_args)
{
    srl_mwmr_t input = _func_args->input;
    srl_mwmr_t output = _func_args->output;
    srl_mwmr_t quanti = _func_args->quanti;

	int16_t i,j;
    uint8_t QTable[64];
    int16_t in[BLOCK_SIZE];
    int32_t UnZZ[BLOCK_SIZE];

    /*
     * three quantization tables/image for JPEG, only one table for
     * JFIF.  64 elements for JFIF table.
     */

    //srl_log(TRACE, "IQZZ thread is alive!\n");

    srl_mwmr_read( quanti, QTable, 64 );

    for ( j=0; j<NBLOCKS; ++j ) {
        //srl_log_printf(TRACE, "IQZZ processing block %d/%d\n", j, NBLOCKS);

        /* get from vld */
        srl_mwmr_read( input, in, 2*BLOCK_SIZE );
        for ( i=0; i<BLOCK_SIZE; ++i ) {
            int16_t buf;

            buf = in[i];
            /* unquantify & UnZZ block */
            UnZZ[G_ZZ[i]] = buf*QTable[i];
            //srl_log_printf( DEBUG,
            //              "i: %d, G: %d, UG: %d, buf: %d, QTable: %d\n",
            //              i, G_ZZ[i], UnZZ[G_ZZ[i]], buf, QTable[i] );
        }
        srl_mwmr_write( output, UnZZ, 4*BLOCK_SIZE );
    }
}
