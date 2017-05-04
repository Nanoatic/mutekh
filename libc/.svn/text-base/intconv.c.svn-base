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

#include <hexo/types.h>

#include <ctype.h>
#include <stdlib.h>

#define __INTCONV_STRTOTYPE_POST(name, type)			\
								\
static inline type name##_post(const char *nptr,		\
			       char **endptr, int_fast8_t base)	\
{								\
  type		res = 0;					\
								\
  if (*nptr == '0')						\
    {								\
      *endptr = (char*)++nptr;					\
								\
      if ((!base || base == 16) && *nptr == 'x')		\
        {							\
	  nptr++;						\
	  base = 16;						\
	}							\
      else							\
        if (!base)						\
	  base = 8;						\
    }								\
								\
  if (!base)							\
    base = 10;							\
								\
  while (*nptr)							\
    {								\
      uint8_t	n;						\
								\
      if ((n = ( *nptr         - '0')) > 9u &&			\
	  (n = ((*nptr | 0x20) - 'a' + 10)) >= base)		\
	break;							\
								\
      res = res * base + n;					\
								\
      *endptr = (char*)++nptr;					\
    }								\
								\
  return res;							\
}

#define __INTCONV_STRTOTYPE_SIGNED(name, type, postname)		\
									\
inline type name(const char *nptr, char **endptr, int_fast8_t base)		\
{									\
  char	*dummy_end;							\
									\
  if (!endptr)								\
    endptr = &dummy_end;						\
									\
  *endptr = (char*)nptr;						\
									\
  while (isspace(*nptr))						\
    nptr++;								\
									\
  switch (*nptr)							\
    {									\
    case ('-'):								\
      nptr++;								\
      return -postname##_post(nptr, endptr, base);			\
									\
    case ('+'):								\
      nptr++;								\
									\
    default:								\
      return postname##_post(nptr, endptr, base);			\
    }									\
}

#define __INTCONV_STRTOTYPE_UNSIGNED(name, type, postname)		\
									\
inline type name(const char *nptr, char **endptr, int_fast8_t base)		\
{									\
  char	*dummy_end;							\
									\
  if (!endptr)								\
    endptr = &dummy_end;						\
									\
  *endptr = (char*)nptr;						\
									\
  while (isspace(*nptr))						\
    nptr++;								\
									\
  return postname##_post(nptr, endptr, base);				\
}

#define __INTCONV_ATOTYPE(name, type, postname)				\
									\
type name(const char *nptr)						\
{									\
  char	*dummy_end;							\
									\
  while (isspace(*nptr))						\
    nptr++;								\
									\
  switch (*nptr)							\
    {									\
    case ('-'):								\
      nptr++;								\
      return -postname##_post(nptr, &dummy_end, 10);			\
									\
    case ('+'):								\
      nptr++;								\
									\
    default:								\
      return postname##_post(nptr, &dummy_end, 10);			\
    }									\
}

#if 0
__INTCONV_STRTOTYPE_POST	(strto_uintl8, uint_fast8_t);
__INTCONV_STRTOTYPE_UNSIGNED	(strto_uintl8, uint_fast8_t, strto_uintl8);
__INTCONV_STRTOTYPE_SIGNED	(strto_intl8,  int_fast8_t,  strto_uintl8);
__INTCONV_ATOTYPE		(ato_intl8,    int_fast8_t,  strto_uintl8);
#endif

__INTCONV_STRTOTYPE_POST	(strto_uintl16, uint_fast16_t);
__INTCONV_STRTOTYPE_UNSIGNED	(strto_uintl16, uint_fast16_t, strto_uintl16);
__INTCONV_STRTOTYPE_SIGNED	(strto_intl16,  int_fast16_t,  strto_uintl16);
__INTCONV_ATOTYPE		(ato_intl16,    int_fast16_t,  strto_uintl16);

__INTCONV_STRTOTYPE_POST	(strto_uintl32, uint_fast32_t);
__INTCONV_STRTOTYPE_UNSIGNED	(strto_uintl32, uint_fast32_t, strto_uintl32);
__INTCONV_STRTOTYPE_SIGNED	(strto_intl32,  int_fast32_t,  strto_uintl32);
__INTCONV_ATOTYPE		(ato_intl32,    int_fast32_t,  strto_uintl32);

__INTCONV_STRTOTYPE_POST	(strto_uintl64, uint_fast64_t);
__INTCONV_STRTOTYPE_UNSIGNED	(strto_uintl64, uint_fast64_t, strto_uintl64);
__INTCONV_STRTOTYPE_SIGNED	(strto_intl64,  int_fast64_t,  strto_uintl64);
__INTCONV_ATOTYPE		(ato_intl64,    int_fast64_t,  strto_uintl64);

