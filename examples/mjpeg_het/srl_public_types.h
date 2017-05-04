
#ifndef SRL_PUBLIC_TYPES_H
#define SRL_PUBLIC_TYPES_H

#include <stdint.h>
#if defined SRL_POSIX
#include <sys/types.h>
# define SRL_MWMR_USE_SEPARATE_LOCKS
#elif defined SRL_OVER_HEXO
#include <hexo/types.h>
#endif

typedef uint32_t srl_const_t;

struct srl_abstract_lock_s;
typedef struct srl_abstract_lock_s *srl_lock_t;

typedef void *srl_buffer_t;

typedef struct srl_memspace_s {
	srl_buffer_t buffer;
	uint32_t size;
} srl_memspace_s;
typedef srl_memspace_s *srl_memspace_t;

struct srl_abstract_mwmr_s;
typedef struct srl_abstract_mwmr_s *srl_mwmr_t;

struct srl_abstract_barrier_s;
typedef struct srl_abstract_barrier_s *srl_barrier_t;

#endif
