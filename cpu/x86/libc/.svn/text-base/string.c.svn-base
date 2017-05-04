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

#include <string.h>

/********************************/

#undef memset

void *
memset(void *dst, int_fast8_t data, size_t size)
{
  uint32_t	size_, dst_;

  __asm__ volatile (
		    "cld\n"
		    "rep stosb\n"
		    : "=&c" (size_), "=&D" (dst_)
		    : "0" (size), "1" (dst), "a" (data)
		    : "memory"
		    );

  return dst;
}

/********************************/

#undef memcpy

void *
memcpy(void *dst, const void *src, size_t size)
{
  uint32_t	size_, dst_, src_;

  __asm__ volatile (
		    "cld\n"
		    "rep movsl\n"
		    "mov %6, %0\n"
		    "rep movsb\n"
		    : "=&c" (size_), "=&D" (dst_), "=&S" (src_)
		    : "0" (size >> 2), "1" (dst), "2" (src), "r" (size & 3)
		    : "memory"
		    );

  return dst;
}

/********************************/

void *
__memcpy_reverse(void *dst, const void *src, size_t size)
{
  uint32_t	size_, dst_, src_;

  __asm__ volatile (
		    "std\n"
		    "rep movsb\n"
		    "mov %6, %0\n"
		    "rep movsl\n"
		    : "=&c" (size_), "=&D" (dst_), "=&S" (src_)
		    : "0" (size & 3), "1" (dst + size - 1), "2" (src + size - 1), "r" (size >> 2)
		    : "memory"
		    );

  return dst;
}

