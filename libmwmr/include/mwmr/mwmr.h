/*
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) UPMC / Lip6
 *     2005-2008, Nicolas Pouillon, <nipo@ssji.net>
 */

#ifndef MWMR_H_
#define MWMR_H_

#if defined(CONFIG_MWMR_PTHREAD)

#include <pthread.h>

struct mwmr_status_s
{
	pthread_mutex_t lock;
	pthread_cond_t nempty;
	pthread_cond_t nfull;
	size_t rptr, wptr;
	size_t usage;
};

struct mwmr_s {
	struct mwmr_status_s *status;
	size_t width;
	size_t depth;
	size_t gdepth;
	uint8_t *buffer;
	const char *const name;
};

typedef struct {} srl_mwmr_lock_t;
#define MWMR_LOCK_INITIALIZER {}

#define MWMR_STATUS_INITIALIZER(x,y)							\
	{															\
		.lock = PTHREAD_MUTEX_INITIALIZER,						\
		.nempty = PTHREAD_COND_INITIALIZER,						\
		.nfull = PTHREAD_COND_INITIALIZER,						\
		.rptr = 0,      										\
		.wptr = 0,      										\
		.usage = 0,												\
	}

#define MWMR_INITIALIZER(w, d, b, st, n, l)					   \
	{														   \
		.width = w,											   \
		.depth = d,											   \
		.gdepth = (w)*(d),									   \
		.status = st,									   	   \
		.buffer = (void*)b,									   \
		.name = n,											   \
	}

#elif defined CONFIG_MWMR_SOCLIB

# ifdef CONFIG_MWMR_LOCKFREE

enum SoclibMwmrRegisters {
    MWMR_IOREG_MAX = 16,
    MWMR_RESET = MWMR_IOREG_MAX,
    MWMR_CONFIG_FIFO_WAY,
    MWMR_CONFIG_FIFO_NO,
    MWMR_CONFIG_STATUS_ADDR,
    MWMR_CONFIG_DEPTH, // bytes
    MWMR_CONFIG_BUFFER_ADDR,
    MWMR_CONFIG_RUNNING,
    MWMR_CONFIG_WIDTH, // bytes
    MWMR_CONFIG_ENDIANNESS, // Write 0x11223344 here
    MWMR_FIFO_FILL_STATUS,
};

enum SoclibMwmrWay {
    MWMR_TO_COPROC,
    MWMR_FROM_COPROC,
};

struct mwmr_status_s
{
	uint32_t free_tail; // bytes
	uint32_t free_head; // bytes
	uint32_t free_size; // bytes

	uint32_t data_tail; // bytes
	uint32_t data_head; // bytes
	uint32_t data_size; // bytes
};

#define MWMR_STATUS_INITIALIZER(w, d) {0,0,(w*d),0,0,0}

# else /* not CONFIG_MWMR_LOCKFREE */

enum SoclibMwmrRegisters {
    MWMR_IOREG_MAX = 16,
    MWMR_RESET = MWMR_IOREG_MAX,
    MWMR_CONFIG_FIFO_WAY,
    MWMR_CONFIG_FIFO_NO,
    MWMR_CONFIG_STATUS_ADDR,
    MWMR_CONFIG_DEPTH,
    MWMR_CONFIG_BUFFER_ADDR,
    MWMR_CONFIG_LOCK_ADDR,
    MWMR_CONFIG_RUNNING,
    MWMR_CONFIG_WIDTH,
    MWMR_FIFO_FILL_STATUS,
};

enum SoclibMwmrWay {
    MWMR_TO_COPROC,
    MWMR_FROM_COPROC,
};

struct mwmr_status_s
{
	uint32_t rptr;
	uint32_t wptr;
	uint32_t usage;
	uint32_t lock;
};

#define MWMR_STATUS_INITIALIZER(w,d) {0,0,0,0}

# endif /* CONFIG_MWMR_LOCKFREE */

#ifdef CONFIG_MWMR_USE_RAMLOCKS
#define MWMR_USE_SEPARATE_LOCKS
typedef uint32_t srl_mwmr_lock_t;
#define MWMR_LOCK_INITIALIZER 0
#endif

struct mwmr_s {
	size_t width;
	size_t depth;
	size_t gdepth;
	void *buffer;
	struct mwmr_status_s *status;
	const char *const name;
#ifdef CONFIG_MWMR_INSTRUMENTATION
	uint32_t n_read;
	uint32_t n_write;
	uint32_t time_read;
	uint32_t time_write;
#endif
#ifdef CONFIG_MWMR_USE_RAMLOCKS
	srl_mwmr_lock_t *lock;
#endif
};

void mwmr_hw_init( void *coproc, enum SoclibMwmrWay way,
				   size_t no, const struct mwmr_s* mwmr );

#ifdef CONFIG_MWMR_USE_RAMLOCKS

# define MWMR_INITIALIZER(w, d, b, st, n, l)				   \
	{														   \
		.width = w,											   \
		.depth = d,											   \
		.gdepth = (w)*(d),									   \
		.buffer = (void*)b,									   \
		.status = st,									   	   \
		.name = n,											   \
		.lock = l,											   \
	}
#else

typedef struct {} srl_mwmr_lock_t;
#define MWMR_LOCK_INITIALIZER {}

# define MWMR_INITIALIZER(w, d, b, st, n, l)				   \
	{														   \
		.width = w,											   \
		.depth = d,											   \
		.gdepth = (w)*(d),									   \
		.buffer = (void*)b,									   \
		.status = st,									   	   \
		.name = n,											   \
	}
#endif

#ifdef CONFIG_MWMR_INSTRUMENTATION
void mwmr_dump_stats( const struct mwmr_s *mwmr );
void mwmr_clear_stats( struct mwmr_s *mwmr );
#endif

#else
# error No valid MWMR implementation
#endif

void mwmr_init( struct mwmr_s* );

void mwmr_read( struct mwmr_s*, void *, size_t );
void mwmr_write( struct mwmr_s*, const void *, size_t );

size_t mwmr_try_read( struct mwmr_s*, void *, size_t );
size_t mwmr_try_write( struct mwmr_s*, const void *, size_t );

#endif
