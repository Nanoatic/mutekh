
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "srl.h"
#include "srl_private_types.h"

#include "mjpeg_tasks.h"

pthread_barrier_t start_barrier;

static unsigned char demux_vld_buf[64] ;
static unsigned char huffman_buf[192] ;
static unsigned char idct_libu_buf[128] ;
static unsigned char iqzz_idct_buf[512] ;
static unsigned char libu_ramdac_buf[768] ;
static unsigned char quanti_buf[256]  ;
static unsigned char tg_demux_buf[64] ;
static unsigned char vld_iqzz_buf[256] ;

static srl_mwmr_status_s demux_vld_status = SRL_MWMR_STATUS_INITIALIZER(  32,    2);
static srl_mwmr_status_s huffman_status = SRL_MWMR_STATUS_INITIALIZER(  32,    6);
static srl_mwmr_status_s idct_libu_status = SRL_MWMR_STATUS_INITIALIZER(  64,    2);
static srl_mwmr_status_s iqzz_idct_status = SRL_MWMR_STATUS_INITIALIZER( 256,    2);
static srl_mwmr_status_s libu_ramdac_status = SRL_MWMR_STATUS_INITIALIZER( 384,    2);
static srl_mwmr_status_s quanti_status = SRL_MWMR_STATUS_INITIALIZER(  64,    4);
static srl_mwmr_status_s tg_demux_status = SRL_MWMR_STATUS_INITIALIZER(  32,    2);
static srl_mwmr_status_s vld_iqzz_status = SRL_MWMR_STATUS_INITIALIZER( 128,    2);

srl_mwmr_s demux_vld     = SRL_MWMR_INITIALIZER(  32,    2, demux_vld_buf, &demux_vld_status,
						  "demux_vld", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s huffman       = SRL_MWMR_INITIALIZER(  32,    6, huffman_buf, &huffman_status,
						  "huffman", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s idct_libu     = SRL_MWMR_INITIALIZER(  64,    2, idct_libu_buf, &idct_libu_status,
						  "idct_libu", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s iqzz_idct     = SRL_MWMR_INITIALIZER( 256,    2, iqzz_idct_buf, &iqzz_idct_status,
						 "iqzz_idct", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s libu_ramdac   = SRL_MWMR_INITIALIZER( 384,    2, libu_ramdac_buf, &libu_ramdac_status,
						 "libu_ramdac", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s quanti        = SRL_MWMR_INITIALIZER(  64,    4, quanti_buf, &quanti_status,
						  "quanti", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s tg_demux      = SRL_MWMR_INITIALIZER(  32,    2, tg_demux_buf, &tg_demux_status,
						  "tg_demux", SRL_MWMR_LOCK_INITIALIZER);
srl_mwmr_s vld_iqzz      = SRL_MWMR_INITIALIZER( 128,    2, vld_iqzz_buf, &vld_iqzz_status,
						 "vld_iqzz", SRL_MWMR_LOCK_INITIALIZER);

static const struct _demux_args_t demux_args = {
	.input = (const srl_mwmr_t)&tg_demux,
	.quanti = (const srl_mwmr_t)&quanti,
	.huffman = (const srl_mwmr_t)&huffman,
	.output = (const srl_mwmr_t)&demux_vld,
};
static const struct _idct_args_t idct_args = {
	.input = (const srl_mwmr_t)&iqzz_idct,
	.output = (const srl_mwmr_t)&idct_libu,
};
static const struct _iqzz_args_t iqzz_args = {
	.input = (const srl_mwmr_t)&vld_iqzz,
	.quanti = (const srl_mwmr_t)&quanti,
	.output = (const srl_mwmr_t)&iqzz_idct,
};
static const struct _libu_args_t libu_args = {
	.input = (const srl_mwmr_t)&idct_libu,
	.output = (const srl_mwmr_t)&libu_ramdac,
};
static const struct _ramdac_args_t ramdac_args = {
	.input = (const srl_mwmr_t)&libu_ramdac,
};
static const struct _tg_args_t tg_args = {
	.output = (const srl_mwmr_t)&tg_demux,
};
static const struct _vld_args_t vld_args = {
	.input = (const srl_mwmr_t)&demux_vld,
	.huffman = (const srl_mwmr_t)&huffman,
	.output = (const srl_mwmr_t)&vld_iqzz,
};

static srl_task_s demux  = SRL_TASK_INITIALIZER(0, demux_func_demux, &demux_args, "demux", 0x0, 0);
static srl_task_s idct   = SRL_TASK_INITIALIZER(0, idct_func_idct, &idct_args, "idct", 0x0, 0);
static srl_task_s iqzz   = SRL_TASK_INITIALIZER(0, iqzz_func_iqzz, &iqzz_args, "iqzz", 0x0, 0);
static srl_task_s libu   = SRL_TASK_INITIALIZER(0, libu_func_libu, &libu_args, "libu", 0x0, 0);
static srl_task_s ramdac = SRL_TASK_INITIALIZER(ramdac_func_bootstrap, ramdac_func_ramdac,
						&ramdac_args, "ramdac", 0x0, 0);
static srl_task_s tg     = SRL_TASK_INITIALIZER(tg_func_bootstrap, tg_func_tg, &tg_args, "tg", 0x0, 0);
static srl_task_s vld    = SRL_TASK_INITIALIZER(0, vld_func_vld, &vld_args, "vld", 0x0, 0);

typedef void *(*start_routine_t)(void*);

static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

static void *run_task(srl_task_s *task)
{
  pthread_mutex_lock(&print_lock);
  printk("Starting %s task on %s processor %i"
#ifdef CONFIG_MUTEK_SCHEDULER_MIGRATION
	 " but threads migration is enabled"
#endif
	 "\n", task->name, cpu_type_name(), cpu_id());
  pthread_mutex_unlock(&print_lock);

  if ( task->bootstrap )
    task->bootstrap(task->args);

  pthread_barrier_wait(&start_barrier);

  if ( task->func )
    for(;;)
      task->func(task->args);
  return NULL;
}

static volatile start = 0;

int app_start()
{
  /* every processor execute the app_start function due to CONFIG_MUTEK_SMP_APP_START */

  switch (cpu_id())
    {
    case 0:
      pthread_barrier_init(&start_barrier, NULL, 7);
      pthread_create((pthread_t*)&demux.thread, NULL, (start_routine_t)run_task, (void*)&demux);
      pthread_create((pthread_t*)&libu.thread, NULL, (start_routine_t)run_task, (void*)&libu);
      start = 1234;
      break;

    case 1:
      pthread_create((pthread_t*)&vld.thread, NULL, (start_routine_t)run_task, (void*)&vld);
      pthread_create((pthread_t*)&iqzz.thread, NULL, (start_routine_t)run_task, (void*)&iqzz);
      break;

    case 2:
      pthread_create((pthread_t*)&idct.thread, NULL, (start_routine_t)run_task, (void*)&idct);
      pthread_create((pthread_t*)&tg.thread, NULL, (start_routine_t)run_task, (void*)&tg);
      break;

    case 3:
      pthread_create((pthread_t*)&ramdac.thread, NULL, (start_routine_t)run_task, (void*)&ramdac);
    }

  while (start != 1234)
    ;

  return 0;
}

