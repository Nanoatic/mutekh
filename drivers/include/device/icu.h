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
 * @short Interrupt controller driver API
 */

#ifndef __DEVICE_ICU_H__
#define __DEVICE_ICU_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>
#include <device/device.h>

struct device_s;
struct driver_s;

/** ICU device class enable() function template */
#define DEVICU_ENABLE(n)	error_t (n) (struct device_s *dev, uint_fast8_t irq, \
                                             bool_t enable, reg_t flags)

/** ICU device class enable() function type. Enable or Disable
    interrupt line.

    * @param dev pointer to device descriptor
    * @param irq icu interrupt line number
    * @param enable 0 disable interrupt
    * @param flags icu specific interrupt handling flags, default to 0
    */
typedef DEVICU_ENABLE(devicu_enable_t);

/** ICU device class enable() function shortcut */
#define dev_icu_enable(dev, ...) (dev)->drv->f.icu.f_enable(dev, __VA_ARGS__ )



/** ICU device class sethndl() function template */
#define DEVICU_SETHNDL(n)	error_t (n) (struct device_s *dev, uint_fast8_t irq, dev_irq_t *hndl, void *data)
/** ICU device class sethndl() function type. Setup a new interrupt
    handler and associated private data.

    * @param dev pointer to device descriptor
    * @param irq icu interrupt line number
    * @param hndl pointer to handler function
    * @param data pointer to associated private data if any
    * @return negative error code
    */
typedef DEVICU_SETHNDL(devicu_sethndl_t);
/** ICU device class sethndl() function shortcut */
#define dev_icu_sethndl(dev, ...) (dev)->drv->f.icu.f_sethndl(dev, __VA_ARGS__ )

/** bind a device to this icu irq for an already configured device */	\
#define DEV_ICU_BIND(icu_dev, dev, irq, callback)			\
    do {								\
      dev_icu_sethndl((icu_dev), (irq), (callback), (dev));		\
      dev_icu_enable((icu_dev), (irq), 1, 0);				\
    } while(0)




/** ICU device class delhndl() function template */
#define DEVICU_DELHNDL(n)	error_t (n) (struct device_s *dev, uint_fast8_t irq, dev_irq_t *hndl)
/** ICU device class delhndl() function type. Remove interrupt
    handler. Several handlers may be registered when interrupt sharing
    is used.

    * @param dev pointer to device descriptor
    * @param irq icu interrupt line number
    * @param hndl pointer to handler function
    * @return negative error code
    */
typedef DEVICU_DELHNDL(devicu_delhndl_t);
/** ICU device class delhndl() function shortcut */
#define dev_icu_delhndl(dev, ...) (dev)->drv->f.icu.f_delhndl(dev, __VA_ARGS__ )

/** unbind icu irq for a device */
#define DEV_ICU_UNBIND(icu_dev, dev, irq, callback)	\
  do {							\
    dev_icu_enable((icu_dev), (irq), 0, 0);		\
    dev_icu_delhndl((icu_dev), (irq), (callback));	\
  } while(0)



struct ipi_endpoint_s;

/** ICU device class sendipi() function template */
#define DEVICU_SENDIPI(n)	error_t (n) (struct ipi_endpoint_s *endpoint)
/** ICU device class sendipi() function type. send an ipi to specified processor. */
typedef DEVICU_SENDIPI(devicu_sendipi_t);
/** ICU device class sendipi() function shortcut */
#define dev_icu_sendipi(dev, ...) (dev)->drv->f.icu.f_sendipi(__VA_ARGS__ )



/** ICU device class setupipi() function template */
#define DEVICU_SETUP_IPI_EP(n)	error_t (n) (struct device_s *dev, \
					     struct ipi_endpoint_s *endpoint, \
					     uint_fast8_t ipi_no)
/** ICU device class setupipi() function type. setup an ipi endpoint. */
typedef DEVICU_SETUP_IPI_EP(devicu_setup_ipi_ep_t);
/** ICU device class setupipi() function shortcut */
#define dev_icu_setup_ipi_ep(dev, ...) (dev)->drv->f.icu.f_setup_ipi_ep(dev, __VA_ARGS__ )



/** ICU device class methodes */

struct dev_class_icu_s
{
  devicu_enable_t	*f_enable;
  devicu_sethndl_t	*f_sethndl;
  devicu_delhndl_t	*f_delhndl;
  devicu_sendipi_t	*f_sendipi;
  devicu_setup_ipi_ep_t	*f_setup_ipi_ep;
};

#endif

