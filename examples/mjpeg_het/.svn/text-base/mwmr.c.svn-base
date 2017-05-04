/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2005, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id: mwmr.c 998 2007-06-29 08:27:02Z nipo $
 */
#include <pthread.h>

#ifdef __APPLE__
#define pthread_yield pthread_yield_np
#endif

void pthread_yield();

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "srl.h"
#include "srl_private_types.h"

#define WORD_SIZE 4

void srl_mwmr_read( srl_mwmr_t fifo, void *mem, size_t len )
{
    uint8_t *ptr = (uint8_t *)mem;

    assert ( len % fifo->width == 0 );

    pthread_mutex_lock( &(fifo->lock.lock) );
    while ( len ) {
		assert( fifo->status->rptr < fifo->gdepth );
		assert( fifo->status->wptr < fifo->gdepth );
        while ( ! fifo->status->usage )
            pthread_cond_wait( &(fifo->lock.nempty), &(fifo->lock.lock) );
		assert( fifo->status->rptr + fifo->width <= fifo->gdepth );
        memcpy( ptr, ((uint8_t*)fifo->buffer)+fifo->status->rptr, fifo->width );
        fifo->status->rptr += fifo->width;
        if ( fifo->status->rptr == fifo->gdepth )
            fifo->status->rptr = 0;
        fifo->status->usage -= fifo->width;
        ptr += fifo->width;
		len -= fifo->width;
		pthread_cond_signal( &(fifo->lock.nfull) );
    }
    pthread_mutex_unlock( &(fifo->lock.lock) );
}

void srl_mwmr_write( srl_mwmr_t fifo, void *mem, size_t len )
{
    uint8_t *ptr = (uint8_t *)mem;

    assert ( len % fifo->width == 0 );

    pthread_mutex_lock( &(fifo->lock.lock) );
    while ( len ) {
		assert( fifo->status->rptr < fifo->gdepth );
		assert( fifo->status->wptr < fifo->gdepth );
        while ( fifo->status->usage == fifo->gdepth )
            pthread_cond_wait( &(fifo->lock.nfull), &(fifo->lock.lock) );
		assert( fifo->status->wptr + fifo->width <= fifo->gdepth );
        memcpy( ((uint8_t*)fifo->buffer)+fifo->status->wptr, ptr, fifo->width );
        fifo->status->wptr += fifo->width;
        if ( fifo->status->wptr == fifo->gdepth )
            fifo->status->wptr = 0;
        fifo->status->usage += fifo->width;
        len -= fifo->width;
        ptr += fifo->width;
        pthread_cond_signal( &(fifo->lock.nempty) );
    }
    pthread_mutex_unlock( &(fifo->lock.lock) );
}

ssize_t srl_mwmr_try_read( srl_mwmr_t fifo, void *mem, size_t len )
{
    unsigned int got = 0;
    uint8_t *ptr = (uint8_t *)mem;

    assert ( len % fifo->width == 0 );

    if ( pthread_mutex_trylock( &(fifo->lock.lock) ) ) {
        return 0;
    }
    
    while ( got < len ) {
		assert( fifo->status->rptr < fifo->gdepth );
		assert( fifo->status->wptr < fifo->gdepth );
        if ( ! fifo->status->usage ) {
            pthread_mutex_unlock( &(fifo->lock.lock) );
            pthread_cond_signal( &(fifo->lock.nfull) );
            return got;
        }
		assert( fifo->status->rptr + fifo->width <= fifo->gdepth );
        memcpy( ptr, ((uint8_t*)fifo->buffer)+fifo->status->rptr, fifo->width );
        fifo->status->rptr += fifo->width;
        if ( fifo->status->rptr == fifo->gdepth )
            fifo->status->rptr = 0;
        fifo->status->usage -= fifo->width;
        got += fifo->width;
        ptr += fifo->width;
		pthread_cond_signal( &(fifo->lock.nfull) );
    }
    pthread_mutex_unlock( &(fifo->lock.lock) );
    return got;
}

ssize_t srl_mwmr_try_write( srl_mwmr_t fifo, void *mem, size_t len )
{
    unsigned int put = 0;
    uint8_t *ptr = (uint8_t *)mem;

    assert( len % fifo->width == 0 );

    if ( pthread_mutex_trylock( &(fifo->lock.lock) ) ) {
        return 0;
    }

    while ( put < len ) {
		assert( fifo->status->rptr < fifo->gdepth );
		assert( fifo->status->wptr < fifo->gdepth );
        if ( fifo->status->usage == fifo->gdepth ) {
            pthread_mutex_unlock( &(fifo->lock.lock) );
            pthread_cond_signal( &(fifo->lock.nempty) );
            return put;
        }
		assert( fifo->status->wptr + fifo->width <= fifo->gdepth );
        memcpy( ((uint8_t*)fifo->buffer)+fifo->status->wptr, ptr, fifo->width );
        fifo->status->wptr += fifo->width;
        if ( fifo->status->wptr == fifo->gdepth )
            fifo->status->wptr = 0;
        fifo->status->usage += fifo->width;
        put += fifo->width;
        ptr += fifo->width;
		pthread_cond_signal( &(fifo->lock.nempty) );
    }
    pthread_mutex_unlock( &(fifo->lock.lock) );
    return put;
}

