#ifndef SRL_HW_HELPERS_H
#define SRL_HW_HELPERS_H

/**
 * @file
 * @module{SRL}
 * @short Miscellaneous APIs
 */

#include <hexo/types.h>
#include <hexo/cpu.h>
#include <stdlib.h>

#define BASE_ADDR_OF(id)											   \
	({																   \
		extern __ldscript_symbol_t _dsx_##id##_region_begin;		   \
		(void*)&_dsx_##id##_region_begin;								   \
	})

/**
   Standard API call, expands to nothing for this implementation.
 */
#define srl_busy_cycles(n) do{}while(0)

/**
   @this flushes the cache line containing the address.
 */
#define srl_dcache_flush_addr cpu_dcache_invld

/**
   @this flushes a memory zone from cache.
 */
#define srl_dcache_flush_zone cpu_dcache_invld_buf

/**
   @this waits for at least the given time (in cycles). The actual
   time spent in this call is not predictable.

   @param time Number of cycles to wait for
 */
void srl_sleep_cycles( uint32_t time );

/**
   @this returns the absolute timestamp counter from the
   initialization of the platform.

   @return Cycles from the initialization of the system
 */
static inline uint32_t srl_cycle_count()
{
	return cpu_cycle_count();
}

/**
   @this aborts the current execution. On most systems, @this will
   simply hang.
 */
static inline void srl_abort()
{
	abort();
}

#endif
