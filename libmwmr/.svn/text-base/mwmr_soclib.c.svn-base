/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 */

#include <mutek/scheduler.h>
#include <hexo/types.h>
#include <hexo/atomic.h>
#include <hexo/iospace.h>
#include <hexo/endian.h>
#include <hexo/interrupt.h>
#include <string.h>
#include <mwmr/mwmr.h>

#if defined(CONFIG_SRL) && !defined(CONFIG_PTHREAD)
# include <srl/srl_sched_wait.h>
# include <srl/srl_log.h>
# ifndef SRL_VERBOSITY
#  define SRL_VERBOSITY VERB_DEBUG
# endif
#elif defined(CONFIG_PTHREAD)
# include <pthread.h>
#endif

static inline size_t min(size_t a, size_t b)
{
	if ( a < b )
		return a;
	else
		return b;
}

void
mwmr_hw_init( void *coproc, enum SoclibMwmrWay way,
			  size_t no, const struct mwmr_s* mwmr )
{
	uintptr_t c = (uintptr_t)coproc;
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_FIFO_WAY, endian_le32(way));
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_FIFO_NO, endian_le32(no));
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_STATUS_ADDR, endian_le32((uintptr_t)mwmr->status));
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_DEPTH, endian_le32(mwmr->gdepth));
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_BUFFER_ADDR, endian_le32((uintptr_t)mwmr->buffer));
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_WIDTH, endian_le32((uintptr_t)mwmr->width));
#ifdef CONFIG_MWMR_USE_RAMLOCKS
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_LOCK_ADDR, endian_le32((uintptr_t)mwmr->lock));
#endif
	cpu_mem_write_32( c + sizeof(uint32_t) * MWMR_CONFIG_RUNNING, endian_le32(1));
}

void mwmr_config( void *coproc, size_t no, const uint32_t val )
{
	uintptr_t c = (uintptr_t)coproc;
	cpu_mem_write_32( c + sizeof(uint32_t) * no, val);
}

uint32_t mwmr_status( void *coproc, size_t no )
{
	uintptr_t c = (uintptr_t)coproc;
	return cpu_mem_read_32( c + sizeof(uint32_t) * no );
}

static inline void mwmr_lock( struct mwmr_s *fifo )
{
#ifdef CONFIG_MWMR_USE_RAMLOCKS
	while (*((uint32_t *)fifo->lock) != 0) {
# if defined(CONFIG_PTHREAD)
		pthread_yield();
# else
		cpu_interrupt_disable();
		sched_context_switch();
		cpu_interrupt_enable();
# endif
	}
#else
# if defined(CONFIG_SRL) && !defined(CONFIG_PTHREAD)
	while (cpu_atomic_bit_testset((atomic_int_t*)&fifo->status->lock, 0)) {
/* 		cpu_interrupt_disable(); */
/* 		sched_context_switch(); */
/* 		cpu_interrupt_enable(); */
		srl_sched_wait_eq_le(&fifo->status->lock, 0);
	}
# elif defined(CONFIG_PTHREAD)
	while (cpu_atomic_bit_testset((atomic_int_t*)&fifo->status->lock, 0)) {
		pthread_yield();
	}
# else
	cpu_atomic_bit_waitset((atomic_int_t*)&fifo->status->lock, 0);
# endif
#endif
}

static inline uint32_t mwmr_try_lock( struct mwmr_s *fifo )
{
#ifdef CONFIG_MWMR_USE_RAMLOCKS
	return !!cpu_mem_read_32((uintptr_t)fifo->lock);
#else
	return cpu_atomic_bit_testset((atomic_int_t*)&fifo->status->lock, 0);
#endif
}

static inline void mwmr_unlock( struct mwmr_s *fifo )
{
#ifdef CONFIG_MWMR_USE_RAMLOCKS
	cpu_mem_write_32((uintptr_t)fifo->lock, 0);
#else
    cpu_mem_write_32((uintptr_t)&fifo->status->lock, 0);
#endif
}

typedef struct {
	uint32_t usage, wptr, rptr, modified;
} local_mwmr_status_t;

static inline void rehash_status( struct mwmr_s *fifo, local_mwmr_status_t *status )
{
	struct mwmr_status_s *fstatus = fifo->status;
	cpu_dcache_invld_buf((void*)fstatus, sizeof(*fstatus));
	status->usage = endian_le32(cpu_mem_read_32( (uintptr_t)&fstatus->usage ));
    status->wptr =  endian_le32(cpu_mem_read_32( (uintptr_t)&fstatus->wptr ));
    status->rptr =  endian_le32(cpu_mem_read_32( (uintptr_t)&fstatus->rptr ));
	status->modified = 0;
//	srl_log_printf(NONE,"%s %d %d %d/%d\n", fifo->name, status->rptr, status->wptr, status->usage, fifo->gdepth);
}

static inline void writeback_status( struct mwmr_s *fifo, local_mwmr_status_t *status )
{
    struct mwmr_status_s *fstatus = fifo->status;
	if ( !status->modified )
		return;
	cpu_mem_write_32( (uintptr_t)&fstatus->usage, endian_le32(status->usage) );
    cpu_mem_write_32( (uintptr_t)&fstatus->wptr, endian_le32(status->wptr) );
	cpu_mem_write_32( (uintptr_t)&fstatus->rptr, endian_le32(status->rptr) );
}

void mwmr_read( struct mwmr_s *fifo, void *_ptr, size_t lensw )
{
	uint8_t *ptr = _ptr;
	local_mwmr_status_t status;

#ifdef CONFIG_MWMR_INSTRUMENTATION
	size_t tot = lensw/fifo->width;
	uint32_t access_begin = cpu_cycle_count();
#endif

	mwmr_lock( fifo );
	rehash_status( fifo, &status );
    while ( lensw ) {
        size_t len;
		while ( status.usage < fifo->width ) {
			writeback_status( fifo, &status );
            mwmr_unlock( fifo );
#if defined(CONFIG_SRL) && !defined(CONFIG_PTHREAD)
			srl_sched_wait_ge_le(&fifo->status->usage, fifo->width);
#elif defined(CONFIG_PTHREAD)
			pthread_yield();
#else
			cpu_interrupt_disable();
			sched_context_switch();
			cpu_interrupt_enable();
#endif
            mwmr_lock( fifo );
			rehash_status( fifo, &status );
        }
        while ( lensw && status.usage >= fifo->width ) {
			void *sptr;

            if ( status.rptr < status.wptr )
                len = status.usage;
            else
                len = (fifo->gdepth - status.rptr);
            len = min(len, lensw);
			sptr = &((uint8_t*)fifo->buffer)[status.rptr];
			cpu_dcache_invld_buf(sptr, len);
            memcpy( ptr, sptr, len );
            status.rptr += len;
            if ( status.rptr == fifo->gdepth )
                status.rptr = 0;
            ptr += len;
            status.usage -= len;
            lensw -= len;
			status.modified = 1;
        }
    }
	writeback_status( fifo, &status );

#ifdef CONFIG_MWMR_INSTRUMENTATION
	cpu_dcache_invld_buf(fifo, sizeof(*fifo));
	fifo->n_read += tot;
	fifo->time_read += cpu_cycle_count()-access_begin;
#endif

	mwmr_unlock( fifo );
}

void mwmr_write( struct mwmr_s *fifo, const void *_ptr, size_t lensw )
{
	const uint8_t *ptr = _ptr;
    local_mwmr_status_t status;

#ifdef CONFIG_MWMR_INSTRUMENTATION
	size_t tot = lensw/fifo->width;
	uint32_t access_begin = cpu_cycle_count();
#endif

	mwmr_lock( fifo );
	rehash_status( fifo, &status );
    while ( lensw ) {
        size_t len;
        while (status.usage >= fifo->gdepth) {
			writeback_status( fifo, &status );
            mwmr_unlock( fifo );
#if defined(CONFIG_SRL) && !defined(CONFIG_PTHREAD)
			srl_sched_wait_le_le(&fifo->status->usage, fifo->gdepth-fifo->width);
#elif defined(CONFIG_PTHREAD)
			pthread_yield();
#else
			cpu_interrupt_disable();
			sched_context_switch();
			cpu_interrupt_enable();
#endif
            mwmr_lock( fifo );
			rehash_status( fifo, &status );
        }
        while ( lensw && status.usage < fifo->gdepth ) {
			void *dptr;

            if ( status.rptr <= status.wptr )
                len = (fifo->gdepth - status.wptr);
            else
                len = fifo->gdepth - status.usage;
            len = min(len, lensw);
			dptr = &((uint8_t*)fifo->buffer)[status.wptr];
            memcpy( dptr, ptr, len );
            status.wptr += len;
            if ( status.wptr == fifo->gdepth )
                status.wptr = 0;
            ptr += len;
            status.usage += len;
            lensw -= len;
			status.modified = 1;
        }
    }
	writeback_status( fifo, &status );

#ifdef CONFIG_MWMR_INSTRUMENTATION
	cpu_dcache_invld_buf(fifo, sizeof(*fifo));
	fifo->n_write += tot;
	fifo->time_write += cpu_cycle_count()-access_begin;
#endif

	mwmr_unlock( fifo );
}

size_t mwmr_try_read( struct mwmr_s *fifo, void *_ptr, size_t lensw )
{
	uint8_t *ptr = _ptr;
	size_t done = 0;
    local_mwmr_status_t status;
#ifdef CONFIG_MWMR_INSTRUMENTATION
	uint32_t access_begin = cpu_cycle_count();
#endif

	if ( mwmr_try_lock( fifo ) )
		return done;
	rehash_status( fifo, &status );
	while ( lensw && status.usage >= fifo->width ) {
        size_t len;
		void *sptr;

		if ( status.rptr < status.wptr )
			len = status.usage;
		else
			len = (fifo->gdepth - status.rptr);
		len = min(len, lensw);
		sptr = &((uint8_t*)fifo->buffer)[status.rptr];
		cpu_dcache_invld_buf(sptr, len);
		memcpy( ptr, sptr, len );
		status.rptr += len;
		if ( status.rptr == fifo->gdepth )
			status.rptr = 0;
		ptr += len;
		status.usage -= len;
		lensw -= len;
		done += len;
		status.modified = 1;
	}
	writeback_status( fifo, &status );
	mwmr_unlock( fifo );
#ifdef CONFIG_MWMR_INSTRUMENTATION
	cpu_dcache_invld_buf(fifo, sizeof(*fifo));
	fifo->n_read += done/fifo->width;
	fifo->time_read += cpu_cycle_count()-access_begin;
#endif
	return done;
}

size_t mwmr_try_write( struct mwmr_s *fifo, const void *_ptr, size_t lensw )
{
	const uint8_t *ptr = _ptr;
	size_t done = 0;
    local_mwmr_status_t status;
#ifdef CONFIG_MWMR_INSTRUMENTATION
	uint32_t access_begin = cpu_cycle_count();
#endif

	if ( mwmr_try_lock( fifo ) )
		return done;
	rehash_status( fifo, &status );
	while ( lensw && status.usage < fifo->gdepth ) {
        size_t len;
		void *dptr;

		if ( status.rptr <= status.wptr )
			len = (fifo->gdepth - status.wptr);
		else
			len = fifo->gdepth - status.usage;
		len = min(len, lensw);
		dptr = &((uint8_t*)fifo->buffer)[status.wptr];
		memcpy( dptr, ptr, len );
		status.wptr += len;
		if ( status.wptr == fifo->gdepth )
			status.wptr = 0;
		ptr += len;
		status.usage += len;
		lensw -= len;
		done += len;
		status.modified = 1;
    }
	writeback_status( fifo, &status );
#ifdef CONFIG_MWMR_INSTRUMENTATION
	cpu_dcache_invld_buf(fifo, sizeof(*fifo));
	fifo->n_write += done/fifo->width;
	fifo->time_write += cpu_cycle_count()-access_begin;
#endif
	mwmr_unlock( fifo );
	return done;
}

#ifdef CONFIG_MWMR_INSTRUMENTATION
void mwmr_dump_stats( const struct mwmr_s *mwmr )
{
	cpu_dcache_invld_buf(mwmr, sizeof(*mwmr));
	if ( mwmr->n_read )
		srl_log_printf(NONE, "read,%s,%d,%d,%d\n",
					   mwmr->name, cpu_cycle_count(),
					   mwmr->time_read, mwmr->n_read );
	if ( mwmr->n_write )
		srl_log_printf(NONE, "write,%s,%d,%d,%d\n",
					   mwmr->name, cpu_cycle_count(),
					   mwmr->time_write, mwmr->n_write );
}

void mwmr_clear_stats( struct mwmr_s *mwmr )
{
	cpu_dcache_invld_buf(mwmr, sizeof(*mwmr));
	mwmr->time_read =
		mwmr->n_read =
		mwmr->time_write =
		mwmr->n_write = 0;
}
#endif
