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

    Copyright Nicolas Pouillon <nipo@ssji.net> (c) 2009

*/

/**
 * @file
 * @module{Device drivers}
 * @short I2c bus driver API
 */

#ifndef __DEVICE_I2C_H__
#define __DEVICE_I2C_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

struct device_s;
struct driver_s;
struct dev_i2c_rq_s;

/** I2c device read/write callback */
#define DEVI2C_CALLBACK(n) void (n) (void *priv,					\
									 const struct dev_i2c_rq_s *rq,	\
									 bool_t error)

/**
   I2c device callback. This function is called for any finished
   operation, or on error condition.  When the callback is called, the
   driver dequeues the request and stops processing it.  The device
   lock is held when call back function is called.

   @param priv pointer to private data
   @param rq pointer to request data.
   @param error whether there was an error

*/
typedef DEVI2C_CALLBACK(devi2c_callback_t);

enum dev_i2c_rq_type_e
{
    DEV_I2C_WRITE,
    DEV_I2C_READ,
};

#define DEV_I2C_ADDR_10BITS   0x8000

#define DEV_I2C_IADDR_NONE   0x000000
#define DEV_I2C_IADDR_8BITS  0x100000
#define DEV_I2C_IADDR_16BITS 0x200000
#define DEV_I2C_IADDR_24BITS 0x300000
#define DEV_I2C_IADDR_MASK   0x300000

CONTAINER_TYPE(dev_i2c_queue, CLIST,
struct dev_i2c_rq_s
{
	dev_i2c_queue_entry_t	queue_entry; /* used by driver to enqueue requests */

	devi2c_callback_t		*callback; /* callback function */
	void				*pvdata; /* pv data for callback */

	uint32_t internal_address;
	void *data;
	size_t size;	/* char count */
	uint16_t dev_addr;
	enum dev_i2c_rq_type_e	type;
}, queue_entry);

CONTAINER_FUNC(dev_i2c_queue, CLIST, static inline, dev_i2c_queue);


/** I2c device class request() function tempate. */
#define DEVI2C_REQUEST(n)	void  (n) (struct device_s *dev, struct dev_i2c_rq_s *rq)

/** I2c device class request() methode shortcut */
#define dev_i2c_request(dev, ...) (dev)->drv->f.i2c.f_request(dev, __VA_ARGS__ )

/**
   I2c device class request() function type. Enqueue a request.

   @param dev pointer to device descriptor
   @param rq pointer to request. callback, command and command_count field must be intialized.
*/
typedef DEVI2C_REQUEST(devi2c_request_t);


/** I2c device class set_baudrate() function tempate. */
#define DEVI2C_SET_BAUDRATE(n)	uint32_t  (n) (struct device_s *dev, uint32_t br)

/** I2c device class request() methode shortcut */
#define dev_i2c_set_baudrate(dev, ...) (dev)->drv->f.i2c.f_set_baudrate(dev, __VA_ARGS__ )

/**
   I2c device class set_baudrate() function type. Change clock for device id.

   @param dev pointer to device descriptor
   @param br new baudrate
   @return the actual baudrate configured
*/
typedef DEVI2C_SET_BAUDRATE(devi2c_set_baudrate_t);


/** I2c device class methodes */
struct dev_class_i2c_s
{
  devi2c_request_t		*f_request;
  devi2c_set_baudrate_t		*f_set_baudrate;
};


error_t dev_i2c_wait_request(
	struct device_s *dev,
	struct dev_i2c_rq_s *rq);

static inline
error_t dev_i2c_read(struct device_s *dev,
					 uint16_t device_addr,
					 uint32_t internal_addr,
					 uint8_t *data,
					 size_t size)
{
	struct dev_i2c_rq_s rq = {
		.internal_address = internal_addr,
		.data = data,
		.size = size,
		.dev_addr = device_addr,
		.type = DEV_I2C_READ,
	};

	return dev_i2c_wait_request(dev, &rq);
}

static inline
error_t dev_i2c_write(struct device_s *dev,
					  uint16_t device_addr,
					  uint32_t internal_addr,
					  const uint8_t *data,
					  size_t size)
{
	struct dev_i2c_rq_s rq = {
		.internal_address = internal_addr,
		.data = (uint8_t *)data,
		.size = size,
		.dev_addr = device_addr,
		.type = DEV_I2C_WRITE,
	};

	return dev_i2c_wait_request(dev, &rq);
}


#endif

