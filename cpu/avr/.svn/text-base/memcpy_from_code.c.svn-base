/*
   Based on code from avr libgcc.S

   Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.
   Contributed by Denis Chertykov <denisc@overta.ru>

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

#include <string.h>

void *
memcpy_from_code (void *dst, const void *src, size_t n)
{

  while (n--)
    asm volatile (
#if defined (__AVR_ENHANCED__)
		  "lpm	r0, Z+			\n"
#else
		  "lpm				\n"
		  "adiw	r30, 1			\n"
#endif
		  "st	X+, r0			\n"

		  : "=z" (src)
		  , "=x" (dst)

		  : "0" (src)
		  , "1" (dst)
		  );

  return dst;
}

