
#ifndef MJPEG_TASKS_H_
#define MJPEG_TASKS_H_

struct _idct_args_t {
  const srl_mwmr_t input;
  const srl_mwmr_t output;
};

void idct_func_idct(struct _idct_args_t *_func_args);

struct _vld_args_t {
  const srl_mwmr_t input;
  const srl_mwmr_t huffman;
  const srl_mwmr_t output;
};

void vld_func_vld(struct _vld_args_t *_func_args);

struct _iqzz_args_t {
  const srl_mwmr_t input;
  const srl_mwmr_t quanti;
  const srl_mwmr_t output;
};

void iqzz_func_iqzz(struct _iqzz_args_t *_func_args);

struct _demux_args_t {
  const srl_mwmr_t input;
  const srl_mwmr_t quanti;
  const srl_mwmr_t huffman;
  const srl_mwmr_t output;
};

void demux_func_demux(struct _demux_args_t *_func_args);

struct _libu_args_t {
  const srl_mwmr_t input;
  const srl_mwmr_t output;
};

void libu_func_libu(struct _libu_args_t *_func_args);

struct _tg_args_t {
  const srl_mwmr_t output;
};

void tg_func_tg(struct _tg_args_t *_func_args);
void tg_func_bootstrap(struct _tg_args_t *_func_args);

struct _ramdac_args_t {
  const srl_mwmr_t input;
};

void ramdac_func_ramdac(struct _ramdac_args_t *_func_args);
void ramdac_func_bootstrap(struct _ramdac_args_t *_func_args);

#endif

