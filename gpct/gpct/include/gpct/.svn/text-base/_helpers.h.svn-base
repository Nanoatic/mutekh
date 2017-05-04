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

*/

#ifndef GPCT_HELPERS_H_
#define GPCT_HELPERS_H_

#include <string.h>
#include <stdlib.h>

#define gpct_malloc(s) malloc(s)
#define gpct_realloc(p, s) realloc(p, s)

GPCT_INTERNAL void
gpct_free(void *p)
{
  free(p);
}

GPCT_INTERNAL void
gpct_memcpy(void *dst, intptr_t dst_offset,
            const void *src,
            intptr_t begin, intptr_t end,
            size_t esize)
{
  if (begin < end)
    {
      begin *= esize;
      dst_offset *= esize;
      memcpy((uint8_t*)dst + dst_offset,
             (uint8_t*)src + begin,
             end * esize - begin);
    }
}

#endif

