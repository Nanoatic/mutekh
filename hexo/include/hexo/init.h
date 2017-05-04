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
 * @module{Hexo}
 * @short System initialization and statup functions
 */

#ifndef INIT_H_
#define INIT_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include "types.h"
#include "error.h"

/** boot address, (located in cpu/current/boot.S) */
void cpu_boot();

/** plateform dependant entry point. (located in arch/current/arch_init.c) */
void arch_init();

#if defined(CONFIG_ARCH_HW_INIT_USER)
/** user-provided hardware initialization function, called by arch_init() */
void user_hw_init();
#elif defined(CONFIG_ARCH_HW_INIT)
/** arch-provided hardware initialization function, called by arch_init() */
void arch_hw_init();
#endif

/** Memory initialization. initialize boot strap cpu's memory subsystem. (located in arch/current/mem_init.c) */
void mem_init();

/** Memory initialization. initialize memory subsystem for non boot strap cpus. (located in arch/current/mem_init.c) */
void mem_region_init();

/** Global initialization of Hexo, must be called after memory
 * initialization, before device initalization
 */
void hexo_global_init();

/** MutekH main function (located in main/main.c) */
int_fast8_t mutek_start(int_fast8_t argc, char **argv);

/** MutekH main function for non first CPU (located in main/main.c) */
void mutek_start_smp(void);

/** User application entry point */
void app_start();

C_HEADER_END

#endif

