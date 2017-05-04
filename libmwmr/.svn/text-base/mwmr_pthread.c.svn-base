/*
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) UPMC / Lip6
 *     2005-2008, Nicolas Pouillon, <nipo@ssji.net>
 */
#include <pthread.h>
#include <string.h>
#include <assert.h>

#include <hexo/types.h>
#include <mwmr/mwmr.h>

void mwmr_read( struct mwmr_s *fifo, void *mem, size_t len )
{
	struct mwmr_status_s *state = fifo->status;
    size_t got = 0;
    uint8_t *ptr = (uint8_t *)mem;

//  mutek_instrument_trace(0);

    assert ( len % fifo->width == 0 );

    pthread_mutex_lock( &(state->lock) );
    while ( got < len ) {
        while ( ! state->usage ) {
            pthread_cond_wait( &(state->nempty), &(state->lock) );
        }
        memcpy( ptr, fifo->buffer + state->rptr, fifo->width );
        state->rptr += fifo->width;
        if ( state->rptr == fifo->gdepth )
            state->rptr = 0;
        state->usage -= fifo->width;
		assert( state->rptr < fifo->gdepth );
		assert( state->usage <= fifo->gdepth );
        pthread_cond_signal( &(state->nfull) );
        got += fifo->width;
        ptr += fifo->width;
    }
    pthread_mutex_unlock( &(state->lock) );
    pthread_cond_signal( &(state->nfull) );
    pthread_yield();

//  mutek_instrument_trace(1);
}

void mwmr_write( struct mwmr_s *fifo, const void *mem, size_t len )
{
	struct mwmr_status_s *state = fifo->status;
    size_t put = 0;
    uint8_t *ptr = (uint8_t *)mem;

//  mutek_instrument_trace(0);

    assert ( len % fifo->width == 0 );

    pthread_mutex_lock( &(state->lock) );
    while ( put < len ) {
        while ( state->usage == fifo->gdepth ) {
            pthread_cond_wait( &(state->nfull), &(state->lock) );
        }
        memcpy( fifo->buffer + state->wptr, ptr, fifo->width );
        state->wptr += fifo->width;
        if ( state->wptr == fifo->gdepth )
            state->wptr = 0;
        state->usage += fifo->width;
		assert( state->wptr < fifo->gdepth );
		assert( state->usage <= fifo->gdepth );
        pthread_cond_signal( &(state->nempty) );
        put += fifo->width;
        ptr += fifo->width;
    }
    pthread_mutex_unlock( &(state->lock) );
    pthread_cond_signal( &(state->nempty) );

    pthread_yield();
//	mutek_instrument_trace(1);
}

size_t mwmr_try_read( struct mwmr_s *fifo, void *mem, size_t len )
{
	struct mwmr_status_s *state = fifo->status;
    size_t got = 0;
    uint8_t *ptr = (uint8_t *)mem;

    assert ( len % fifo->width == 0 );

    if ( pthread_mutex_trylock( &(state->lock) ) ) {
        return 0;
    }
    
    while ( got < len ) {
        if ( ! state->usage ) {
            pthread_mutex_unlock( &(state->lock) );
            pthread_cond_signal( &(state->nfull) );
            return got;
        }
        memcpy( ptr, fifo->buffer + state->rptr, fifo->width );
        state->rptr += fifo->width;
        if ( state->rptr == fifo->gdepth )
            state->rptr = 0;
        state->usage -= fifo->width;
        got += fifo->width;
        ptr += fifo->width;
    }
    pthread_mutex_unlock( &(state->lock) );
    pthread_cond_signal( &(state->nfull) );

    pthread_yield();

    return got;
}

size_t mwmr_try_write( struct mwmr_s *fifo, const void *mem, size_t len )
{
	struct mwmr_status_s *state = fifo->status;
    size_t put = 0;
    uint8_t *ptr = (uint8_t *)mem;

    assert( len % fifo->width == 0 );

    if ( pthread_mutex_trylock( &(state->lock) ) ) {
        return 0;
    }

    while ( put < len ) {
        if ( state->usage == fifo->gdepth ) {
            pthread_mutex_unlock( &(state->lock) );
            pthread_cond_signal( &(state->nempty) );
            return put;
        }
        memcpy( fifo->buffer + state->wptr, ptr, fifo->width );
        state->wptr += fifo->width;
        if ( state->wptr == fifo->gdepth )
            state->wptr = 0;
        state->usage += fifo->width;
        put += fifo->width;
        ptr += fifo->width;
    }
    pthread_mutex_unlock( &(state->lock) );
    pthread_cond_signal( &(state->nempty) );

    pthread_yield();

    return put;
}

void mwmr_init( struct mwmr_s *fifo )
{
	struct mwmr_status_s *state = fifo->status;
	pthread_cond_init(&state->nempty, NULL);
	pthread_cond_init(&state->nfull, NULL);
	pthread_mutex_init(&state->lock, NULL);
}
