/*

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@free.fr> (C) 2009

    GCC Atomic operations based spin locks
    http://gcc.gnu.org/onlinedocs/gcc-4.4.1/gcc/Atomic-Builtins.html
*/

#ifndef GPCT_LOCK_GCC_H_
#define GPCT_LOCK_GCC_H_

#if !(((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) || __GNUC__ > 4)
# error GCC atomic operations based locks not supported by the compiler, gcc >= 4.1.0 required
#endif

/** Some architectures require some special code in spin loop */
#if defined(__x86_64__) || defined(__i386__)
# define GPCT_GCC_SPIN_PAUSE() asm volatile ("pause\n");
#else
# define GPCT_GCC_SPIN_PAUSE() asm volatile ("nop\n");
#endif


#endif

