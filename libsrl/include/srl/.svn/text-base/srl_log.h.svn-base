/*
 * This file is part of DSX, development environment for static
 * SoC applications.
 * 
 * This file is distributed under the terms of the GNU General Public
 * License.
 * 
 * Copyright (c) 2006, Nicolas Pouillon, <nipo@ssji.net>
 *     Laboratoire d'informatique de Paris 6 / ASIM, France
 * 
 *  $Id$
 */

#ifndef SRL_LOG_H_
#define SRL_LOG_H_

/**
 * @file
 * @module{SRL}
 * @short Debug messages
 */

/** @internal */
enum __srl_verbosity {
    VERB_NONE,
    VERB_TRACE,
    VERB_DEBUG,
    VERB_MAX,
};

#if defined(CONFIG_SRL_SOCLIB)
void _srl_log(const char *);
void _srl_log_printf(const char *, ...);
void _cpu_printf(const char *, ...);
void srl_console_init_task(void*);
void srl_console_init_cpu(void*);
void srl_console_init(void*);
#else
# include <stdio.h>

# if defined(CONFIG_LIBC_STREAM_STD)
#  define _srl_log(x) printf("%s", x)
#  define _srl_log_printf(x...) printf(x)
#  define _cpu_printf(x...) printk(x)
# elif defined(CONFIG_MUTEK_CONSOLE)
#  define _srl_log(x) printk("%s", x)
#  define _srl_log_printf(x...) printk(x)
#  define _cpu_printf(x...) printk(x)
# else
#  warning No srl_log backend available
#  define _srl_log(x) do{}while(0)
#  define _srl_log_printf(x...) do{}while(0)
#  define _cpu_printf(x...) do{}while(0)
# endif
# define srl_console_init_task(x...)
# define srl_console_init_cpu(x...)
# define srl_console_init(x...)
#endif

#define GET_VERB_(x,y) x##y
#define GET_VERB(x) GET_VERB_(VERB_,x)

/**
   @this prints a message if the current verbosity is sufficient.

   @param l Minimal verbosity of the message
   @param c Message to print
 */
#define srl_log( l, c ) do {										   \
		if (GET_VERB(l) <= GET_VERB(CONFIG_SRL_VERBOSITY)) {		   \
			_srl_log( c );											   \
		}															   \
	} while (0)

/**
   @this prints a message if the current verbosity is sufficient.

   @param l Minimal verbosity of the message
   @param c Message to print, with a printf-like syntax
 */
#define srl_log_printf( l, c... ) do {								   \
		if (GET_VERB(l) <= GET_VERB(CONFIG_SRL_VERBOSITY)) {		   \
			_srl_log_printf( c );									   \
		}															   \
	} while (0)

/**
   @this prints a message on the tty specified for the current
   CPU. There is no verbosity condition.

   @param c Message to print, with a printf-like syntax
 */
#define cpu_printf( c... ) do {					\
		_cpu_printf( c );						\
	} while (0)

/**
   @this is the same as @ref #assert.
 */
#define srl_assert(expr)												\
    do {																\
        if ( ! (expr) ) {												\
            srl_log_printf(NONE, "assertion (%s) failed on %s:%d !\n",  \
						   #expr, __FILE__, __LINE__ );					\
            abort();													\
        }																\
    } while(0)

#endif
