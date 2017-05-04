
#ifndef SRL_PUBLIC_TYPES_H
#define SRL_PUBLIC_TYPES_H

/**
 * @file
 * @module{SRL}
 * @short Abstract types definitions
 */

#include <stdint.h>
#include <hexo/types.h>
#include <hexo/lock.h>
#include <mwmr/mwmr.h>

/**
   A numeric constant holder
 */
typedef uint32_t srl_const_t;

/**
   @internal
 */
typedef struct srl_memspace_s {
	void *buffer;
	uint32_t size;
} srl_memspace_s;

/**
   The memspace abstract type.
 */
typedef struct srl_memspace_s *srl_memspace_t;

/**
   The mwmr abstract type.
 */
typedef struct mwmr_s *srl_mwmr_t;

#ifdef CONFIG_PTHREAD
#include <pthread.h>

/**
   The barrier abstract type.
 */
typedef pthread_barrier_t *srl_barrier_t;
/**
   The lock abstract type.
 */
typedef pthread_mutex_t *srl_lock_t;
#else
/** @internal */
struct srl_barrier_s;
typedef struct srl_barrier_s *srl_barrier_t;
typedef lock_t *srl_lock_t;
#endif

#endif
