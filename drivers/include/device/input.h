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
 * @short Event driven input device driver API
 */

#ifndef __DEVICE_INPUT_H__
#define __DEVICE_INPUT_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>

struct device_s;
struct driver_s;

#define DEVINPUT_EVENT_BUTTON_UP	0x01
#define DEVINPUT_EVENT_BUTTON_DOWN	0x02
#define DEVINPUT_EVENT_AXIS_MOVED	0x04

#define DEVINPUT_CTRLID_ALL		((devinput_ctrlid_t)-1)

/** id of a control (button or axe) for an input device */
typedef uint_fast8_t devinput_ctrlid_t;
/** control value type. This can be a simple boolean value for button,
    a position or a move offset for an axis control */
typedef uint_fast16_t devinput_value_t;

struct devinput_info_s
{
  const char	*name;
  size_t	ctrl_button_count;
  size_t	ctrl_axe_count;
};


/** input device class callback function template */
#define DEVINPUT_CALLBACK(n)	void (n) (devinput_ctrlid_t id, devinput_value_t value, void *priv)
/** input device class callback function type */
typedef DEVINPUT_CALLBACK(devinput_callback_t);


/** Input device class info function tempate. */
#define DEVINPUT_INFO(n)	void  (n) (struct device_s *dev,		\
					   struct devinput_info_s *info)

#define dev_input_info(dev, ...) (dev)->drv->f.input.f_info(dev, __VA_ARGS__ )
/**
   Input device class info() function type. This function get
   informations about available controles.

   @param dev pointer to device descriptor
   @param info pointer to information structure to fill in
*/
typedef DEVINPUT_INFO(devinput_info_t);


/** Input device class read function tempate. */
#define DEVINPUT_READ(n)	devinput_value_t (n) (struct device_s *dev,	\
						      devinput_ctrlid_t id)

#define dev_input_read(dev, ...) (dev)->drv->f.input.f_read(dev, __VA_ARGS__ )
/**
   Input device class read() function type. This function read control
   current value.

   @param dev pointer to device descriptor
   @param id id of the controle to read
*/
typedef DEVINPUT_READ(devinput_read_t);



/** Input device class write function tempate. */
#define DEVINPUT_WRITE(n)	error_t (n) (struct device_s *dev,	\
					     devinput_ctrlid_t id,	\
					     devinput_value_t value)

#define dev_input_write(dev, ...) (dev)->drv->f.input.f_write(dev, __VA_ARGS__ )
/**
   Input device class write() function type. This function set control
   current value.

   @param dev pointer to device descriptor
   @param id id of the controle to write
   @param value new status value for the control
*/
typedef DEVINPUT_WRITE(devinput_write_t);



/** Input device class event setcallback function tempate. */
#define DEVINPUT_SETCALLBACK(n)	error_t (n) (struct device_s *dev,		\
					     uint_fast8_t type,			\
					     devinput_ctrlid_t id,		\
					     devinput_callback_t *callback,	\
					     void *priv)

#define dev_input_setcallback(dev, ...) (dev)->drv->f.input.f_setcallback(dev, __VA_ARGS__ )
/**
   Input device class setcallback() function type. This function set
   a new event handler for a control. Special DEVINPUT_CTRLID_ALL
   value can be used to get events for all controls available on this
   device. a NULL function pointer disable callback.

   @param dev pointer to device descriptor
   @param type type mask value for event types to watch
   @param id id of the control to watch
   @param callback new callback function
   @param priv private data passed to callback function
   @return non zero value on error
*/
typedef DEVINPUT_SETCALLBACK(devinput_setcallback_t);


/** Input device class methodes */
struct dev_class_input_s
{
  devinput_info_t		*f_info;
  devinput_read_t		*f_read;
  devinput_write_t		*f_write;
  devinput_setcallback_t	*f_setcallback;
};


#endif

