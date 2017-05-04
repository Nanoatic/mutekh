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
 * @short Timer device driver API
 */

#ifndef __DEVICE_TIMER_H__
#define __DEVICE_TIMER_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>

struct device_s;
struct driver_s;



/** timer device class callback function template */
#define DEVTIMER_CALLBACK(n)	void (n) (void *priv)
/** timer device class callback function type */
typedef DEVTIMER_CALLBACK(devtimer_callback_t);



/** TIMER device class setcallback() function template */
#define DEVTIMER_SETCALLBACK(n)	error_t (n) (struct device_s *dev, uint_fast8_t id, devtimer_callback_t *callback, void *priv)
/** TIMER device class setcallback() function type. Change current
    timer/counter callback. a NULL pointer may be used to disable
    timer callback.

    * @param dev pointer to device descriptor
    * @param id timer id
    * @param callback new timer callback
    * @param priv private data passed to callback function
    */
typedef DEVTIMER_SETCALLBACK(devtimer_setcallback_t);
/** TIMER device class setcallback() function shortcut */
#define dev_timer_setcallback(dev, ...) (dev)->drv->f.timer.f_setcallback(dev, __VA_ARGS__ )



/** TIMER device class setperiod() function template */
#define DEVTIMER_SETPERIOD(n)	error_t (n) (struct device_s *dev, uint_fast8_t id, uintmax_t period)
/** TIMER device class setperiod() function type. Change timer/counter
    period. Period can be the max value for incremening counters or
    the start value for decrmenting counters. A value of 0 may disable
    timer depending on hardware capabilites.

    * @param dev pointer to device descriptor
    * @param id timer id
    * @param period timer period
    */
typedef DEVTIMER_SETPERIOD(devtimer_setperiod_t);
/** TIMER device class setperiod() function shortcut */
#define dev_timer_setperiod(dev, ...) (dev)->drv->f.timer.f_setperiod(dev, __VA_ARGS__ )



/** TIMER device class setvalue() function template */
#define DEVTIMER_SETVALUE(n)	error_t (n) (struct device_s *dev, uint_fast8_t id, uintmax_t value)
/** TIMER device class setvalue() function type. Change current
    timer/counter value. May only be used to reset timer depending on
    hardware capabilities.

    * @param dev pointer to device descriptor
    * @param id timer id
    * @param value new timer value
    */
typedef DEVTIMER_SETVALUE(devtimer_setvalue_t);
/** TIMER device class setvalue() function shortcut */
#define dev_timer_setvalue(dev, ...) (dev)->drv->f.timer.f_setvalue(dev, __VA_ARGS__ )



/** TIMER device class getvalue() function template */
#define DEVTIMER_GETVALUE(n)	uintmax_t (n) (struct device_s *dev, uint_fast8_t id)
/** TIMER device class getvalue() function type. Get the current timer
    value. May return 0 or truncated timer value depending on hardware
    capabilities.

    * @param dev pointer to device descriptor
    * @param id timer id
    * @return current timer value
    */
typedef DEVTIMER_GETVALUE(devtimer_getvalue_t);
/** TIMER device class getvalue() function shortcut */
#define dev_timer_getvalue(dev, ...) (dev)->drv->f.timer.f_getvalue(dev, __VA_ARGS__ )



/** TIMER device class methodes */

struct dev_class_timer_s
{
  devtimer_setperiod_t			*f_setperiod;
  devtimer_getvalue_t			*f_getvalue;
  devtimer_setcallback_t		*f_setcallback;
  devtimer_setvalue_t			*f_setvalue;
};


#endif

