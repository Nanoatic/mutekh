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

    Copyright (c) Nicolas Pouillon <nipo@ssji.net>, 2009

*/

/**
 * @file
 * @module{Device drivers}
 * @short General purpose IO drievr API
 */

#ifndef __DEVICE_GPIO_H__
#define __DEVICE_GPIO_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/gpct_platform_hexo.h>
#include <gpct/cont_clist.h>

struct device_s;
struct driver_s;

typedef uint_fast8_t devgpio_id_t;

enum devgpio_way_e
{
	GPIO_WAY_INPUT,
	GPIO_WAY_INPUT_FILTERED,
	GPIO_WAY_OUTPUT,
	GPIO_WAY_OPENDRAIN,
};

/** Gpio device class set_way() function tempate. */
#define DEVGPIO_SET_WAY(n)	error_t  (n) (struct device_s *dev,			\
										  devgpio_id_t gpio,				\
										  enum devgpio_way_e way)

/**
    GPIO device class set_way() function type.  Sets the I/O way of
    the pin.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
	@param way way of pin
    @return error level

 */
typedef DEVGPIO_SET_WAY(devgpio_set_way_t);

/** Gpio device class set_way() methode shortcut */
#define dev_gpio_set_way(dev, ...) (dev)->drv->f.gpio.f_set_way(dev, __VA_ARGS__ )


/** Gpio device class set_value() function tempate. */
#define DEVGPIO_SET_VALUE(n)	error_t  (n) (struct device_s *dev,		\
											  devgpio_id_t gpio,			\
											  bool_t value)

/**
    GPIO device class set_value() function type.  Sets the I/O value
    of the pin.  This is only valid for output pins.  Chaging the
    output value for a pin configured as input may have undesirable
    side-effects that may not be detected nor reported.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
	@param value way of pin
    @return error level
 */
typedef DEVGPIO_SET_VALUE(devgpio_set_value_t);

/** Gpio device class set_value() methode shortcut */
#define dev_gpio_set_value(dev, ...) (dev)->drv->f.gpio.f_set_value(dev, __VA_ARGS__ )


/** Gpio device class set_pullup() function tempate. */
#define DEVGPIO_SET_PULLUP(n)	error_t  (n) (struct device_s *dev,		\
											  devgpio_id_t gpio,			\
											  bool_t pullup)

/**
    GPIO device class set_pullup() function type.  Sets the I/O pullup
    of the pin.  This is only valid for input pins.  Chaging the
    pullup value for a pin configured as output may have undesirable
    side-effects that may not be detected nor reported.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
	@param value way of pullup
    @return error level
 */
typedef DEVGPIO_SET_PULLUP(devgpio_set_pullup_t);

/** Gpio device class set_pullup() methode shortcut */
#define dev_gpio_set_pullup(dev, ...) (dev)->drv->f.gpio.f_set_pullup(dev, __VA_ARGS__ )


/** Gpio device class assign_to_peripheral() function tempate. */
#define DEVGPIO_ASSIGN_TO_PERIPHERAL(n)	error_t  (n) (struct device_s *dev,	\
													  devgpio_id_t gpio,	\
													  uint_fast8_t device_id)

/**
    GPIO device class assign_to_peripheral() function type.  Assign
    the pin to another embedded peripheral controller.  This method
    only makes sense where there is GPIO controllers multiplexed with
    other peripherals.  This is mostly the case for microcontrollers.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
	@param device_id device id GPIO is assigned to. This is
           device-dependant.
    @return error level

 */
typedef DEVGPIO_ASSIGN_TO_PERIPHERAL(devgpio_assign_to_peripheral_t);

/** Gpio device class assign_to_peripheral() methode shortcut */
#define dev_gpio_assign_to_peripheral(dev, ...) (dev)->drv->f.gpio.f_assign_to_peripheral(dev, __VA_ARGS__ )


/** Gpio device class get_value() function tempate. */
#define DEVGPIO_GET_VALUE(n)	bool_t  (n) (struct device_s *dev,	   \
											 devgpio_id_t gpio)

/**
    GPIO device class get_value() function type.  Gets the I/O value
    of the pin.  This is only valid for input pins, although it may
    work with output pins.  This behaviour should not be relied upon.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
    @return value of pin
 */
typedef DEVGPIO_GET_VALUE(devgpio_get_value_t);

/** Gpio device class get_value() methode shortcut */
#define dev_gpio_get_value(dev, ...) (dev)->drv->f.gpio.f_get_value(dev, __VA_ARGS__ )


enum devgpio_event_e
{
	GPIO_EDGE_RAISING,
	GPIO_EDGE_FALLING,
	GPIO_VALUE_UP,
	GPIO_VALUE_DOWN,
	GPIO_EVENT_ALL,
	GPIO_NONE,
};

/** Gpio device class IRQ callback function tempate. */
#define DEVGPIO_IRQ(x) void (x)(struct device_s *dev,				   \
								devgpio_id_t gpio,					   \
								enum devgpio_event_e event,			   \
								void *priv)

/**
   GPIO device IRQ callback function declaration

   @param dev gpio device
   @param gpio the gpio triggerring the IRQ
   @param event event that triggerred the IRQ
   @param priv private callback data
 */
typedef DEVGPIO_IRQ(devgpio_irq_t);

/** Gpio device class register_irq() function tempate. */
#define DEVGPIO_REGISTER_IRQ(n)	error_t (n) (struct device_s *dev,		\
											 devgpio_id_t gpio,			\
											 enum devgpio_event_e event, \
											 devgpio_irq_t *callback,	\
											 void *private_data)

/**
    GPIO device class register_irq() function type.  Registers a
    callback on a specific event coming from a pin.  Having more than
    one callback registered may not be supported.  In order to
    unregister a callback, one should call register_irq with GPIO_NONE
    event type.

    @param dev pointer to device descriptor
	@param gpio descriptor to GPIO pin
	@param event event type
	@param callback function to call on event
	@param private_data private data to pass to the callback
    @return whether the action was successful
 */
typedef DEVGPIO_REGISTER_IRQ(devgpio_register_irq_t);

/** Gpio device class register_irq() methode shortcut */
#define dev_gpio_register_irq(dev, ...) (dev)->drv->f.gpio.f_register_irq(dev, __VA_ARGS__ )



/** Gpio device class methodes */
struct dev_class_gpio_s
{
  devgpio_set_way_t	*f_set_way;
  devgpio_set_value_t	*f_set_value;
  devgpio_set_pullup_t	*f_set_pullup;
  devgpio_assign_to_peripheral_t	*f_assign_to_peripheral;
  devgpio_get_value_t	*f_get_value;
  devgpio_register_irq_t	*f_register_irq;
};

#endif

