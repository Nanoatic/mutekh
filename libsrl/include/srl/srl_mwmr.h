/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2006, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id$
 */

#ifndef SRL_MWMR_H_
#define SRL_MWMR_H_

/**
 * @file
 * @module{SRL}
 * @short MWMR channels access
 */

#include <mwmr/mwmr.h>
#include <hexo/endian.h>
#include <assert.h>

/**
   @this reads a given size from a mwmr channel. If the size is not
   available in the channel, the read will block.

   @param mwmr The mwmr channel
   @param buffer The buffer to retrieve data into
   @param size The size (in bytes) of the requested transfer. This has
   to be a multiple of the channel's width.
 */
static inline void srl_mwmr_read( srl_mwmr_t mwmr, void *buffer, size_t size )
{
	mwmr_read( mwmr, buffer, size );
}

/**
   @this writes a given size from a mwmr channel. If the size is not
   free in the channel, the write will block.

   @param mwmr The mwmr channel
   @param buffer The buffer to retrieve data from
   @param size The size (in bytes) of the requested transfer. This has
   to be a multiple of the channel's width.
 */
static inline void srl_mwmr_write( srl_mwmr_t mwmr, const void *buffer, size_t size )
{
	mwmr_write( mwmr, buffer, size );
}

/**
   @this reads a given size from a mwmr channel. If the size is not
   available in the channel, or if the lock is not available, @this will
   return without transfering the whole buffer.

   @param mwmr The mwmr channel
   @param buffer The buffer to retrieve data into
   @param size The size (in bytes) of the requested transfer. This has
   to be a multiple of the channel's width.
   @return the amount of bytes actually transfered
 */
static inline size_t srl_mwmr_try_read( srl_mwmr_t mwmr, void *buffer, size_t size )
{
	return mwmr_try_read( mwmr, buffer, size );
}

/**
   @this writes a given size from a mwmr channel. If the size is not
   free in the channel, or if the lock is not available, @this will
   return without transfering the whole buffer.

   @param mwmr The mwmr channel
   @param buffer The buffer to retrieve data from
   @param size The size (in bytes) of the requested transfer. This has
   to be a multiple of the channel's width.
   @return the amount of bytes actually transfered
 */
static inline size_t srl_mwmr_try_write( srl_mwmr_t mwmr, const void *buffer, size_t size )
{
	return mwmr_try_write( mwmr, buffer, size );
}

# if defined(CONFIG_MWMR_SOCLIB)
/**
   @this retrieves a status word from a mwmr controller.

   @param coproc base address of the controller
   @param no number of the queried status register
   @return the current status word in the mwmr controller
 */
static inline uint32_t srl_mwmr_status( void *coproc, size_t no )
{
	uint32_t *c = coproc;
	assert(no < MWMR_IOREG_MAX);
	return endian_le32(c[no]);
}

/**
   @this sets a configuration word in a mwmr controller.

   @param coproc base address of the controller
   @param no number of the accessed configuration register
   @param value the value to write in the said register
 */
static inline void srl_mwmr_config( void *coproc, size_t no, uint32_t value )
{
	uint32_t *c = coproc;
	assert(no < MWMR_IOREG_MAX);
	c[no] = endian_le32(value);
}
# else
static inline uint32_t srl_mwmr_status( void *coproc, size_t no )
{
	return 0;
}

static inline void srl_mwmr_config( void *coproc, size_t no, uint32_t value )
{
}

# endif

#endif
