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
 * @short Sound device driver API
 */

#ifndef __DEVICE_SOUND_H__
#define __DEVICE_SOUND_H__

#ifdef __DRIVER_H__
# error This header must not be included after "device/driver.h"
#endif

#include <hexo/types.h>
#include <hexo/error.h>

struct device_s;
struct driver_s;

#define DEVSOUND_CALLBACK(n)	void (n)(void *priv)
typedef DEVSOUND_CALLBACK(devsound_callback_t);

/** Sound device class read() function tempate. */
#define DEVSOUND_READ(n)	ssize_t  (n) (struct device_s *dev, uint8_t *data, size_t count, \
					      devsound_callback_t *cback, void *priv)

/** Sound device class read() methode shortcut */

#define dev_sound_read(dev, ...) (dev)->drv->f.sound.f_read(dev, __VA_ARGS__ )
/**
   Sound device class read() function type.  Read bytes data from the
   device. Should not block if unable to read more bytes.

   @param dev pointer to device descriptor
   @param data pointer to data buffer
   @param count max sample to read
   @param cback function called when reading done
   @return data bytes count read from the device or negative error code
*/
typedef DEVSOUND_READ(devsound_read_t);




/** Sound device class write() function tempate. */
#define DEVSOUND_WRITE(n)	ssize_t  (n) (struct device_s *dev, const uint8_t *data, size_t count, \
					      devsound_callback_t *cback, void *priv)

/** 
    Sound device class write() function type.  Write bytes data to the
    device. Return number of bytes written. Should not block if unable
    to write more bytes.

    @param dev pointer to device descriptor
    @param data pointer to read only data buffer
    @param count data bytes count
    @return data bytes count written to the device or negative error code
*/
typedef DEVSOUND_WRITE(devsound_write_t);

/** Sound device class write() methode shortcut */
#define dev_sound_write(dev, ...) (dev)->drv->f.sound.f_write(dev, __VA_ARGS__ )




enum dev_sound_mode_e
  {
    dev_sound_8bit_signed,
    dev_sound_8bit_unsigned,
    dev_sound_16bit_signed,
    dev_sound_16bit_unsigned,
  };

/** Sound device class write() function tempate. */
#define DEVSOUND_MODE(n)	error_t  (n) (struct device_s *dev,	\
					      enum dev_sound_mode_e mode, \
					      uint_fast8_t chan_count)

/** 
    Sound device class mode() function type. Set sound samples format.

    @param dev pointer to device descriptor
    @param mode sample format
    @param chan_count number of channels
    @return non zero value on error
*/
typedef DEVSOUND_MODE(devsound_mode_t);

/** Sound device class write() methode shortcut */
#define dev_sound_mode(dev, ...) (dev)->drv->f.sound.f_mode(dev, __VA_ARGS__ )



/** Sound device class methodes */
struct dev_class_sound_s
{
  devsound_read_t		*f_read;
  devsound_write_t		*f_write;
  devsound_mode_t		*f_mode;
};


#endif

