#ifndef SRL_MEMSPACE_H
#define SRL_MEMSPACE_H

/**
 * @file
 * @module{SRL}
 * @short Memory resources
 */

/**
   @this retrieves the base address of a memspace

   @param memsp The memspace
   @return the base address of the memspace
 */
#define SRL_MEMSPACE_ADDR(memsp) ((memsp)->buffer)

/**
   @this retrieves the size of a memspace

   @param memsp The memspace
   @return the size of the memspace
 */
#define SRL_MEMSPACE_SIZE(memsp) ((memsp)->size)

#endif
