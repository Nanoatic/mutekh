/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

/**
 * @file
 * @module{Device drivers}
 * @short Character device driver API
 */                                                                 

#ifndef __DEVICE_CHAR_H__
#define __DEVICE_CHAR_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

struct device_s;
struct driver_s;
struct dev_char_rq_s;

/** Char device read/write callback */
#define DEVCHAR_CALLBACK(n) bool_t (n) (struct device_s *dev, \
				        const struct dev_char_rq_s *rq, \
				        size_t size)

/**
   Char device read/write callback. This function is called for each
   chunk of available data. It may be called several times, rq->size
   != 0 and rq->error must be used to detect operation end. Operation
   is aborted if call back function return true. The device lock is
   held when call back function is called.

   @param dev pointer to device descriptor
   @param rq pointer to request data. rq->size field is updated to \
          remaining data bytes. rq->data is advanced after callback.\
	  rq->error is updated.
   @param size amount of data bytes read.
*/
typedef DEVCHAR_CALLBACK(devchar_callback_t);

enum dev_char_rq_type_e
  {
    DEV_CHAR_READ, DEV_CHAR_WRITE,
  };

CONTAINER_TYPE(dev_char_queue, CLIST,
struct dev_char_rq_s
{
  enum dev_char_rq_type_e	type;
  size_t			size;	/* char count */
  devchar_callback_t		*callback; /* callback function */
  void				*pvdata; /* pv data for callback */
  uint8_t			*data; /* chars */
  error_t			error; /* error code set by driver */

  void				*drvdata; /* driver private data */
  dev_char_queue_entry_t	queue_entry; /* used by driver to enqueue requests */
}, queue_entry);

CONTAINER_FUNC(dev_char_queue, CLIST, static inline, dev_char_queue);


/** Char device class request() function tempate. */
#define DEVCHAR_REQUEST(n)	void  (n) (struct device_s *dev, struct dev_char_rq_s *rq)

/** Char device class request() methode shortcut */
#define dev_char_request(dev, ...) (dev)->drv->f.chr.f_request(dev, __VA_ARGS__ )

/**
   Char device class request() function type. Enqueue a read or write request.

   @param dev pointer to device descriptor
   @param rq pointer to request. data, size and callback, field must be intialized.
*/
typedef DEVCHAR_REQUEST(devchar_request_t);


/** Char device class methodes */
struct dev_class_char_s
{
  devchar_request_t		*f_request;
};


/** Synchronous helper read function. This function uses the scheduler
    api to put current context in wait state if no data is available
    from device yet. This function spins in a loop waiting for read
    operation to complete when scheduler is disabled.

    @returns processed bytes count or negative error code.
*/
ssize_t dev_char_wait_read(struct device_s *dev, uint8_t *data, size_t size);

/** Synchronous helper read function. This function spins in a loop
    waiting for read operation to complete.

    @returns processed bytes count or negative error code.
*/
ssize_t dev_char_spin_read(struct device_s *dev, uint8_t *data, size_t size);

/** Synchronous helper write function. This function uses the scheduler
    api to put current context in wait state if no data is available
    from device yet. This function spins in a loop waiting for write
    operation to complete when scheduler is disabled.

    @returns processed bytes count or negative error code.
*/
ssize_t dev_char_wait_write(struct device_s *dev, const uint8_t *data, size_t size);

/** Synchronous helper write function. This function spins in a loop
    waiting for write operation to complete.

    @returns processed bytes count or negative error code.
*/
ssize_t dev_char_spin_write(struct device_s *dev, const uint8_t *data, size_t size);

#endif

