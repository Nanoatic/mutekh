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


#if !defined(TYPES_H_) || defined(ARCH_TYPES_H_)
#error This file can not be included directly
#else

#define ARCH_TYPES_H_

/* architecture specific integer types */

/** boolean value */
typedef int8_t		bool_t;
/** data size integer type */
typedef uintptr_t	size_t;
/** signed data size integer type */
typedef intptr_t	ssize_t;
/** offset integer type */
typedef intptr_t	off_t;
/** biggest unsigned integer type available */
typedef uint64_t	uintmax_t;
/** biggest signed integer type available */
typedef int64_t		intmax_t;
/** cpu integer id */
typedef uint8_t		cpu_id_t;

#endif

