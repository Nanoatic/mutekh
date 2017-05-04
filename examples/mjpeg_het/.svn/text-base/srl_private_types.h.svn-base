
#ifndef SRL_PRIVATE_TYPES_H
#define SRL_PRIVATE_TYPES_H

#include <pthread.h>
#include <stdint.h>

#include "srl_public_types.h"

#define SRL_CONST_INITIALIZER(x) x

typedef pthread_mutex_t srl_lock_s;

#define SRL_LOCK_INITIALIZER() PTHREAD_MUTEX_INITIALIZER

typedef struct srl_mwmr_status_s srl_mwmr_status_s;
typedef struct srl_mwmr_lock_s srl_mwmr_lock_t;

struct srl_mwmr_lock_s {
	pthread_mutex_t lock;
	pthread_cond_t nempty;
	pthread_cond_t nfull;
};
#define SRL_MWMR_LOCK_INITIALIZER {		\
    .lock = PTHREAD_MUTEX_INITIALIZER,		\
      .nempty = PTHREAD_COND_INITIALIZER,	\
      .nfull = PTHREAD_COND_INITIALIZER,	\
      }

typedef struct srl_abstract_mwmr_s {
	srl_mwmr_lock_t lock;
	size_t width;
	size_t depth;
	size_t gdepth;
	srl_buffer_t buffer;
	const char *name;
	struct srl_mwmr_status_s *status;
} srl_mwmr_s;

struct srl_mwmr_status_s {
	size_t rptr;
	size_t wptr;
	size_t usage;
};

#define SRL_MWMR_STATUS_INITIALIZER(w,d) {	\
    .rptr = 0,					\
      .wptr = 0,				\
      .usage = 0,				\
      }

#define SRL_MWMR_INITIALIZER(w, d, b, s, n, l)	\
  {						\
    .width = w,					\
      .depth = d,				\
      .gdepth = (w)*(d),			\
      .buffer = (void*)b,			\
      .name = n,				\
      .status = s,				\
      .lock = l,				\
   }

typedef struct srl_abstract_barrier_s {
	size_t count;
	size_t current;
	pthread_mutex_t lock;
	pthread_cond_t ok;
} srl_barrier_s;

#define SRL_BARRIER_INITIALIZER(c)		\
  {						\
    .count = c,					\
      .current = 0,				\
      .lock = PTHREAD_MUTEX_INITIALIZER,	\
      .ok = PTHREAD_COND_INITIALIZER,		\
      }

#define SRL_MEMSPACE_INITIALIZER( b, s ) \
{\
	.buffer = b,\
		 .size = s,\
		 }

typedef void srl_task_func_t( void* );
typedef struct srl_abstract_task_s {
	srl_task_func_t *bootstrap;
	srl_task_func_t *func;
	void *args;
	void *stack;
	size_t stack_size;
	const char *name;
	pthread_t thread;
} srl_task_s;

#define SRL_TASK_INITIALIZER(b, f, a, n, ttya, ttyn)	\
  {								\
    .bootstrap = (srl_task_func_t *)b,				\
      .func = (srl_task_func_t *)f,				\
      .args = (void*)a,						\
      .name = n,						\
      }

#define SRL_CPUSTATE_INITIALIZER()		\
  {						\
    .boo = 0,					\
      }

typedef struct srl_abstract_cpudesc_s srl_cpudesc_s;
struct srl_abstract_cpudesc_s {
	size_t ntasks;
	const srl_task_s *const*task_list;
};

#define SRL_CPUDESC_INITIALIZER(nt, tl, ttya, ttyn)	\
  {							\
    .ntasks = nt,					\
      .task_list = tl,					\
      }

typedef struct srl_abstract_appdesc_s srl_appdesc_s;
struct srl_abstract_appdesc_s {
	const size_t ntasks;
	srl_barrier_s *start;
	const srl_mwmr_s * const *mwmr;
	const srl_cpudesc_s * const *cpu;
	const srl_task_s * const *task;
};

#define SRL_APPDESC_INITIALIZER(nt, cl, ml, tl, sb, ttya, ttyn) \
  {								\
    .ntasks = nt,						\
      .cpu = cl,						\
      .mwmr = ml,						\
      .task = tl,						\
      .start = sb,						\
      }

#endif
