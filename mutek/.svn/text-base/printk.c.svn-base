/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2009-2010
 */

#ifdef CONFIG_MUTEK_PRINTK_LOCK
# include <hexo/lock.h>
#endif

#include <stdio.h>
#include <mutek/printk.h>

static printf_output_func_t *printk_output = NULL;
static void *printk_output_arg;

#ifdef CONFIG_COMPILE_INSTRUMENT
bool_t mutek_instrument_trace(bool_t state);
#endif

void printk_set_output(printf_output_func_t *f, void *priv)
{
	printk_output = f;
	printk_output_arg = priv;
}

#ifdef CONFIG_MUTEK_PRINTK_LOCK
static lock_t printk_lock = LOCK_INITIALIZER;
#endif

inline ssize_t vprintk(const char *format, va_list ap)
{
#ifdef CONFIG_COMPILE_INSTRUMENT
	bool_t old = mutek_instrument_trace(0);
#endif
	error_t err = EIO;

#if defined(CONFIG_MUTEK_CONSOLE) || defined(CONFIG_MUTEK_EARLY_CONSOLE)
	if ( printk_output ) {
#ifdef CONFIG_MUTEK_PRINTK_LOCK
                lock_spin(&printk_lock);
#endif
		err = formatter_printf(printk_output_arg, printk_output, format, ap);
#ifdef CONFIG_MUTEK_PRINTK_LOCK
                lock_release(&printk_lock);
#endif
        }
#endif

#ifdef CONFIG_COMPILE_INSTRUMENT
	mutek_instrument_trace(old);
#endif

	return err;
}

ssize_t printk(const char *format, ...)
{
  ssize_t	res;
  va_list	ap;

  va_start(ap, format);
  res = vprintk(format, ap);
  va_end(ap);

  return res;
}

void writek(const char *data, size_t len)
{
  if ( printk_output ) {
#ifdef CONFIG_MUTEK_PRINTK_LOCK
    lock_spin(&printk_lock);
#endif
    printk_output(printk_output_arg, data, 0, len);
#ifdef CONFIG_MUTEK_PRINTK_LOCK
    lock_release(&printk_lock);
#endif
  }
}

#ifdef CONFIG_MUTEK_PRINTK_HEXDUMP

void hexdumpk(uintptr_t address, const void *data, size_t len)
{
#ifdef CONFIG_MUTEK_PRINTK_LOCK
  lock_spin(&printk_lock);
#endif
  formatter_hexdump(printk_output_arg, printk_output, address, data, len);
#ifdef CONFIG_MUTEK_PRINTK_LOCK
  lock_release(&printk_lock);
#endif
}

#endif

