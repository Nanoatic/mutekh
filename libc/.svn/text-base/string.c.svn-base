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

    Based on dietlibc-0.29 http://www.fefe.de/dietlibc/

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* FIXME */

#define reg_t_log2_m1 (sizeof(reg_t)-1)

/********************************/

#ifndef HAS_CPU_MEMSET
#undef memset
inline void * memset(void *dst, int_fast8_t _s, size_t count)
{
  int8_t s = _s;
  const reg_t v = (uint8_t)s * (reg_t)0x0101010101010101LL;
  int8_t *a = dst;
  reg_t *r;

  /* align */
  while ( ((uintptr_t)a & reg_t_log2_m1) && count )
    count--, *a++ = s;

  size_t ucount = count & reg_t_log2_m1;
  count &= ~reg_t_log2_m1;

  for (r = (reg_t*)a; count; count -= sizeof(reg_t))
      *r++ = v;

  for (a = (int8_t*)r; ucount; ucount--)
      *a++ = s;

  return dst;
}
#endif

/********************************/

#ifndef HAS_CPU_MEMCPY
#undef memcpy
void *memcpy( void *_dst, const void *_src, size_t size )
{
	reg_t *dst = _dst;
	const reg_t *src = _src;
	if ( ! ((uintptr_t)dst & reg_t_log2_m1) && ! ((uintptr_t)src & reg_t_log2_m1) )
		while (size >= sizeof(reg_t)) {
			*dst++ = *src++;
			size -= sizeof(reg_t);
		}

	uint8_t *cdst = (uint8_t*)dst;
	uint8_t *csrc = (uint8_t*)src;

	while (size--) {
		*cdst++ = *csrc++;
	}
	return _dst;
}
#endif

/********************************/

#ifndef HAS_CPU_MEMCMP
#undef memcmp
inline int_fast8_t memcmp(const void *dst, const void *src, size_t count)
{
  const uint8_t	*d = dst;
  const uint8_t	*s = src;
  int_fast8_t	r;

  count++;

  while (__builtin_expect(--count, 1))
    {
      if (__builtin_expect(r = (*d - *s), 0))
	return r;
      ++d;
      ++s;
  }

  return 0;
}
#endif

/********************************/

#ifndef HAS_CPU_MEMCPY_REVERSE
inline void *
__memcpy_reverse(void *dst, const void *src, size_t size)
{
  uint8_t	*dst_ = (uint8_t*)dst + size;
  uint8_t	*src_ = (uint8_t*)src + size;

  while (size--)
    *--dst_ = *--src_;
  return dst;
}
#endif

inline void *
memmove(void *dst, const void *src, size_t size)
{
  if (dst > src)
    return __memcpy_reverse(dst, src, size);
  else
    return memcpy(dst, src, size);
}

/********************************/

#ifndef HAS_CPU_MEMCPY_FROM_CODE
inline void * memcpy_from_code (void *dst, const void *src, size_t n)
{
  return memcpy(dst, src, n);
}
#endif

/********************************/

#ifndef HAS_CPU_STRLEN
#undef strlen
inline size_t strlen(const char *s)
{
  const char	*s_ = s;

  while (*s)
    s++;

  return s - s_;
}
#endif

/********************************/

#ifndef HAS_CPU_STRCAT
#undef strcat
inline char* strcat(char *s, const char *t)
{
  char	*dest = s;

  s += strlen(s);

  while (1)
    {
      if (!(*s = *t))
	break;
      s++;
      t++;
    }

  return dest;
}
#endif

/********************************/

#ifndef HAS_CPU_STRNCAT
#undef strncat
inline char *strncat(char *dst, const char *src, size_t n)
{
  /* source freebsd libc */
  if (n != 0) {
    char *d = dst;
    const char *s = src;

    while (*d != 0)
      d++;
    do {
      if ((*d = *s++) == 0)
	break;
      d++;
    } while (--n != 0);
    *d = 0;
  }
  return (dst);
}
#endif

/********************************/

#ifndef HAS_CPU_STRCHR
#undef strchr
inline char *strchr(const char *t, int_fast8_t c)
{
  char	ch = c;

  while (1)
    {
      if (__builtin_expect(*t == ch, 0))
	break;

      if (__builtin_expect(!*t, 0))
	return 0;

      t++;
    }

  return (char*)t;
}
#endif
/********************************/


#ifndef HAS_CPU_STRCHR
#undef strrchr
inline char *strrchr(const char *t, int_fast8_t c)
{
  char  ch = c;
  char *last = (char *)0;

  while (1)
    {
      if (__builtin_expect(*t == ch, 0))
	last = (char *)t;
      if (__builtin_expect(!*t, 0))
        break;
      t++;
    }

  return last;
}
#endif

/********************************/

#ifndef HAS_CPU_STRCMP
#undef strcmp
inline int_fast8_t strcmp (const char *s1, const char *s2)
{
  while (*s1 && *s1 == *s2)
    s1++, s2++;

  return (*s1 - *s2);
}
#endif

/********************************/

#ifndef HAS_CPU_STRCPY
#undef strcpy
inline char * strcpy (char *s1, const char *s2)
{
  char	*res = s1;

  while ((*s1++ = *s2++))
    ;

  return (res);
}
#endif

/********************************/

#ifndef HAS_CPU_STRDUP
#undef strdup
inline char *strdup(const char *s)
{
  size_t	len = strlen(s);
  char		*tmp;

  if ((tmp = malloc(len + 1)))
    {
      memcpy(tmp, s, len);
      tmp[len] = 0;
    }

  return tmp;
}
#endif

/********************************/

#ifndef HAS_CPU_STRSTR
#undef strstr
inline char *strstr(const char *haystack, const char *needle)
{
  size_t	nl = strlen(needle);
  size_t	hl = strlen(haystack);
  size_t	i;

  if (!nl)
    return (char*)haystack;

  if (nl > hl)
    return 0;

  for (i = hl - nl + 1; __builtin_expect(i, 1); i--)
    {
      if ((*haystack == *needle) && !memcmp(haystack, needle, nl))
	return (char*)haystack;

      haystack++;
    }

  return 0;
}
#endif

/********************************/

#ifndef HAS_CPU_MEMCHR
#undef memchr
inline void * memchr(const void *s, int_fast8_t c, size_t n)
{
  const unsigned char *pc = (unsigned char *)s;

  for (; n--; pc++)
    if (*pc == c)
      return ((void *)pc);

  return 0;
}
#endif

/********************************/

#define __unlikely(foo) (foo)
#define __likely(foo) (foo)

#ifndef HAS_CPU_STRCASECMP
#undef strcasecmp
inline int_fast8_t strcasecmp(const char* s1, const char* s2)
{
  uint_fast8_t  x2;
  uint_fast8_t  x1;

    while (1) {
        x2 = *s2 - 'A'; if (__unlikely(x2 < 26u)) x2 += 32;
        x1 = *s1 - 'A'; if (__unlikely(x1 < 26u)) x1 += 32;
	s1++; s2++;
        if ( __unlikely(x2 != x1) )
            break;
        if ( __unlikely(x1 == (uint32_t)-'A') )
            break;
    }

    return x1 - x2;
}
#endif

/********************************/

#ifndef HAS_CPU_STRNCASECMP
#undef strncasecmp
inline int_fast8_t strncasecmp(const char* s1, const char* s2, size_t len)
{
  uint_fast8_t  x2;
  uint_fast8_t  x1;
    register const char*   end = s1 + len;

    while (1) {
        if ( __unlikely(s1 >= end) )
            return 0;
        x2 = *s2 - 'A'; if (__unlikely(x2 < 26u)) x2 += 32;
        x1 = *s1 - 'A'; if (__unlikely(x1 < 26u)) x1 += 32;
	s1++; s2++;
        if ( __unlikely(x2 != x1) )
            break;
        if ( __unlikely(x1 == (uint32_t)-'A') )
            break;
    }

    return x1 - x2;
}
#endif

/********************************/

void *memccpy(void *dst, const void *src, char c, size_t count)
{
  char *a = dst;
  const char *b = src;
  while (count--)
  {
    *a++ = *b;
    if (*b==c)
      return (void *)a;
    b++;
  }
  return 0;
}

#ifndef HAS_CPU_STRNCPY
#undef strncpy
inline char *strncpy(char *dest, const char *src, size_t n)
{
  char *tmp;

  tmp = dest;

  while (n && *src)
    {
      *dest++ = *src++;
      n--;
    }

  while (n--)
    *dest++ = 0;

  return (tmp);
#if 0
  memset(dest,0,n);
  memccpy(dest,src,0,n);
  return dest;
#endif
}
#endif

/********************************/

#ifndef HAS_CPU_STRNCMP
#undef strncmp
inline int_fast8_t strncmp(const char *s1, const char *s2, size_t n)
{
  register const unsigned char* a = (const unsigned char *)s1;
  register const unsigned char* b = (const unsigned char *)s2;
  register const unsigned char* fini = a + n;

  while (a < fini)
    {
      register int_fast8_t res = *a - *b;

      if (res)
	return res;

      if (!*a)
	return 0;

      ++a;
      ++b;
    }

  return 0;
}
#endif

/********************************/

#ifndef HAS_CPU_STRTOK_R
#undef strtok_r
char *strtok_r(char *s, const char *delim, char **ptrptr)
{
  char *tmp = 0;

  if (s == 0)
    s = *ptrptr;

  s += strspn(s, delim);

  if (__likely(*s))
    {
      tmp = s;
      s += strcspn(s, delim);
      if (__likely(*s))
	*s++ = 0;
    }

  *ptrptr = s;
  return tmp;
}
#endif

/********************************/

#ifndef HAS_CPU_STRSPN
#undef strspn
size_t strspn(const char *s, const char *accept)
{
  size_t l = 0;
  size_t a = 1, i, al = strlen(accept);

  while((a) && (*s))
    {
      for(a = i = 0; (!a) && (i < al); i++)
	if (*s == accept[i])
	  a = 1;

      if (a)
	l++;

      s++;
    }

  return l;
}
#endif

/********************************/

#ifndef HAS_CPU_STRCSPN
#undef strcspn
size_t strcspn(const char *s, const char *reject)
{
  size_t l = 0;
  size_t a = 1, i, al = strlen(reject);

  while((a) && (*s))
    {
      for(i = 0; (a) && (i < al); i++)
	if (*s == reject[i])
	  a = 0;

      if (a)
	l++;
      s++;
    }

  return l;
}
#endif

/********************************/

#ifndef HAS_CPU_STRPBRK
#undef strpbrk
char *strpbrk(const char *s1, const char *s2)
{
  const char *scanp;
  size_t c, sc;

  while ((c = *s1++) != 0) {
    for (scanp = s2; (sc = *scanp++);)
      if (sc == c)
	return ((char *)(s1 - 1));
  }
  return NULL;
}

#endif

int_fast8_t memcstcmp(const void *s1, int_fast8_t _v, size_t n)
{
    const int8_t *data = s1;
    size_t i;
    const int8_t v = _v;

    for ( i=0; i<n; ++i ) {
        if ( data[i] != v )
            return data[i] - v;
    }
    return 0;
}

