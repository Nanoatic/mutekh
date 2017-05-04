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
 * @short SPI bus device driver API
 */

#ifndef __DEVICE_SPI_H__
#define __DEVICE_SPI_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

struct device_s;
struct driver_s;
struct dev_spi_rq_s;

/** Spi device read/write callback */
#define DEVSPI_CALLBACK(n) void (n) (void *priv,					\
									 const struct dev_spi_rq_s *rq,	\
									 bool_t error)

/**
   Spi device callback. This function is called for any finished
   operation, or on wait_byte_value timeout condition.  When the
   callback is called, the driver dequeues the request and stops
   processing it.  The device lock is held when call back function is
   called.

   @param priv pointer to private data
   @param rq pointer to request data.
   @param error whether there was an error

*/
typedef DEVSPI_CALLBACK(devspi_callback_t);

enum devspi_wait_value_answer_e
{
	DEV_SPI_VALUE_FOUND,
	DEV_SPI_VALUE_FAIL,
	DEV_SPI_VALUE_RETRY,
};

#define DEVSPI_WAIT_VALUE_CALLBACK(n)								   \
	enum devspi_wait_value_answer_e									   \
	(n) (struct device_s *dev,										   \
		 const struct dev_spi_rq_s *rq,								   \
		 uint16_t value)

/**
   Spi callback when waiting for a particular value.
 */
typedef DEVSPI_WAIT_VALUE_CALLBACK(devspi_wait_value_callback_t);


enum dev_spi_rq_type_e
{
    DEV_SPI_DESELECT,
    DEV_SPI_R_16,
	DEV_SPI_W_16,
	DEV_SPI_RW_16,
    DEV_SPI_R_8,
	DEV_SPI_W_8,
	DEV_SPI_RW_8,
	DEV_SPI_WAIT_VALUE,
	DEV_SPI_PAD,
    DEV_SPI_PAD_UNSELECTED,
    DEV_SPI_SET_CONSTANT,
};


struct dev_spi_rq_cmd_s
{
	enum dev_spi_rq_type_e	type;
	union {
		struct {
			void *data;
			size_t size;	/* char count */
			uint16_t padding;   /* word stuffed in SDI when reading */
			uint_fast8_t ptr_increment;
		} read;
		struct {
			uint16_t data; /* constant ored with next written data for transfers */
		} constant;
		struct {
			const void *data;
			size_t size;	/* word count */
			uint_fast8_t ptr_increment;
		} write;
		struct {
			const void *wdata;
			void *rdata;
			size_t size;	/* word count */
			uint_fast8_t ptr_increment;
		} read_write;
		struct {
			devspi_wait_value_callback_t *callback;
			size_t timeout;
			uint16_t padding;
		} wait_value;
		struct {
			size_t  size;
			uint16_t padding;
		} pad;
	};
};

#define SPIRQ_DESELECT()										\
	{																	\
		DEV_SPI_DESELECT,												\
	}

#define SPIRQ_SET_CONSTANT(constant_)							\
	{																	\
		DEV_SPI_SET_CONSTANT, { .constant =	{							\
				.data = constant_,										\
			}, },														\
	}

#define SPIRQ_R_16(data_ptr, _size, _pad, increment)					\
	{																	\
		DEV_SPI_R_16, { .read = {										\
				.padding = _pad,										\
				.data = data_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
			}

#define SPIRQ_W_16(data_ptr, _size, increment)							\
	{																	\
		DEV_SPI_W_16, { .write = {										\
				.data = data_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
			}

#define SPIRQ_RW_16(rdata_ptr, wdata_ptr, _size, increment)				\
	{																	\
		DEV_SPI_RW_16, { .read_write = {								\
				.rdata = rdata_ptr,										\
				.wdata = wdata_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
			}

#define SPIRQ_R_8(data_ptr, _size, _pad, increment)						\
	{																	\
		DEV_SPI_R_8, { .read = {										\
				.padding = _pad,										\
				.data = data_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
			}

#define SPIRQ_W_8(data_ptr, _size, increment)							\
	{																	\
		DEV_SPI_W_8, { .write = {										\
				.data = data_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
	}

#define SPIRQ_RW_8(rdata_ptr, wdata_ptr, _size, increment)				\
	{																	\
		DEV_SPI_RW_8, { .read_write = {									\
				.rdata = rdata_ptr,										\
				.wdata = wdata_ptr,										\
				.size = _size,											\
				.ptr_increment = increment,								\
			}, },														\
	}

#define SPIRQ_WAIT_VALUE(_padding, _timeout, _callback)					\
	{																	\
		DEV_SPI_WAIT_VALUE, { .wait_value = {							\
				.padding = _padding,									\
				.timeout = _timeout,									\
				.callback = _callback,									\
			}, },														\
			}

#define SPIRQ_PAD(_padding, _size)									\
	{																	\
		DEV_SPI_PAD, { .pad = {											\
				.padding = _padding,											\
				.size = _size,											\
			}, },														\
			}

#define SPIRQ_PAD_UNSELECTED(_padding, _size)						\
	{																	\
		DEV_SPI_PAD_UNSELECTED, { .pad = {								\
				.padding = _padding,											\
				.size = _size,											\
			}, },														\
			}



CONTAINER_TYPE(dev_spi_queue, CLIST,
struct dev_spi_rq_s
{
	dev_spi_queue_entry_t	queue_entry; /* used by driver to enqueue requests */

	devspi_callback_t		*callback; /* callback function */
	void				*pvdata; /* pv data for callback */

	void				*drvdata; /* driver private data */

	uint_fast8_t device_id;
	struct dev_spi_rq_cmd_s *command;
	size_t command_count;
}, queue_entry);

CONTAINER_FUNC(dev_spi_queue, CLIST, static inline, dev_spi_queue);


/** Spi device class request() function tempate. */
#define DEVSPI_REQUEST(n)	void  (n) (struct device_s *dev, struct dev_spi_rq_s *rq)

/** Spi device class request() methode shortcut */
#define dev_spi_request(dev, ...) (dev)->drv->f.spi.f_request(dev, __VA_ARGS__ )

/**
   Spi device class request() function type. Enqueue a request.

   @param dev pointer to device descriptor
   @param rq pointer to request. callback, command and command_count field must be intialized.
*/
typedef DEVSPI_REQUEST(devspi_request_t);


/** Spi device class set_baudrate() function tempate. */
#define DEVSPI_SET_BAUDRATE(n)	uint32_t  (n) (struct device_s *dev, uint_fast8_t device_id, uint32_t br, uint_fast8_t xfer_delay, uint_fast8_t cs_delay)

/** Spi device class request() methode shortcut */
#define dev_spi_set_baudrate(dev, ...) (dev)->drv->f.spi.f_set_baudrate(dev, __VA_ARGS__ )

/**
   Spi device class set_baudrate() function type. Change clock for device id.

   @param dev pointer to device descriptor
   @param device_id device id in the SPI controller
   @param br new baudrate
   @param xfer_delay delay between transfers
   @param cs_delay delay between chip-select and first transfer
   @return the actual baudrate configured
*/
typedef DEVSPI_SET_BAUDRATE(devspi_set_baudrate_t);


enum spi_mode_e {
	// CPOL, CPHA, what happens
	SPI_MODE_0, // 0, 0, Read on rising clock, then Change on falling
	SPI_MODE_1, // 0, 1, Change on rising, then Read on falling clock
	SPI_MODE_2, // 1, 0, Read on falling clock, then Change on rising
	SPI_MODE_3, // 1, 1, Change on falling, then Read on rising clock
};

/** Spi device class set_baudrate() function tempate. */
#define DEVSPI_SET_DATA_FORMAT(n)	error_t  (n) (						\
		struct device_s *dev,											\
		uint_fast8_t device_id,											\
		uint_fast8_t bits_per_word,										\
		enum spi_mode_e spi_mode,										\
		bool_t keep_cs_active											\
		)

/** Spi device class request() methode shortcut */
#define dev_spi_set_data_format(dev, ...) (dev)->drv->f.spi.f_set_data_format(dev, __VA_ARGS__ )

/**
   Spi device class set_data_format() function type. Change communication protocol.

   @param dev pointer to device descriptor
   @param device_id device id in the SPI controller
   @param bits_per_word bits per SPI word: 8..16
   @param spi_mode SPI_MODE_0..3
   @return 0 if it went well
*/
typedef DEVSPI_SET_DATA_FORMAT(devspi_set_data_format_t);


/** Spi device class methodes */
struct dev_class_spi_s
{
  devspi_request_t		*f_request;
  devspi_set_baudrate_t		*f_set_baudrate;
  devspi_set_data_format_t		*f_set_data_format;
};

#endif

