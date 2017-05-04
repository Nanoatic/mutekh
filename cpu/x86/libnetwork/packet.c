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

    Copyright Matthieu Bucchianeri <matthieu.bucchianeri@epita.fr> (c) 2006

*/

#include <hexo/types.h>
#include <assert.h>

/*
 * Compute Internet checksum.
 */

uint16_t		packet_checksum(const void	*data,
					size_t		size)
{
  uint32_t		result = 0, tmp = 0, acc;

  asm (

       /* handle additional byte */
       "	shrl $1, %1					\n"
       "	jnc 1f						\n"
       "	movzbl (%2,%1,0x2), %3				\n"
       "1:							\n"

       /* handle additional 16 bits word */
       "	shrl $1, %1					\n"
       "	cmovcw (%2,%1,0x4), %w0				\n"

       /* compute 32 bits sum */
       "	cld						\n"
       "	clc						\n"
       "1:							\n"
       "	adcl (%2), %0 					\n"
       "	lea 4(%2), %2					\n"
       "	decl %1						\n"
       "	jnz 1b						\n"
       "	adcl %3, %0					\n"
       "	adcl $0, %0					\n"

       /* fold 32 bits sum */
       "	movl %0, %3					\n"
       "	shr $16, %3					\n"
       "	addw %w3, %w0					\n"
       "	adcw $0, %w0					\n"
       "	not %0						\n"

       : "=r" (result), "=c" (size), "=S" (data), "=r" (tmp)
       : "0" (result), "1" (size), "2" (data), "3" (tmp)
       );

  /*
  result = (result >> 16) + (result & 0xffff);
  result = (result >> 16) + result;
  */

  return result;
}

/*
 * Memcpy and compute Internet checksum.
 */

uint_fast16_t		packet_memcpy(void		*dst,
				      const void	*src,
				      size_t		size)
{
  uint32_t		result;
  uint32_t		right = 0;
  size_t		len;
  size_t		diff;

  len = size >> 2;

  asm ("cld\n\t"
       "xorl %0, %0\n\t"
       "1:\n\t"
       "lodsl\n\t"
       "adcl %%eax, %0\n\t"
       "stosl\n\t"
       "loopnz 1b\n\t"
       : "=b" (result)
       : "c" (size), "S" (src), "D" (dst)
       : "%eax");

  if ((diff = (size - len)))
    {
      switch (diff)
	{
	  case 1:
	    right = *(uint8_t *)(src + len);
	    *(uint8_t *)(dst + len) = right;
	    break;
	  case 2:
	    right = *(uint16_t *)(src + len);
	    *(uint16_t *)(dst + len) = right;
	    break;
	  case 3:
	    right = *(uint16_t *)(src + len) + *(uint8_t *)(src + len + 2);
	    *(uint16_t *)(dst + len) = *(uint16_t *)(src + len);
	    *(uint8_t *)(dst + len + 2) = *(uint8_t *)(src + len + 2);
	    break;
	}
      result += right;
    }

  result = (result >> 16) + (result & 0xffff);
  result = (result >> 16) + result;

  return (~result) & 0xffff;
}
