/*

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 2 of the License,
    or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    Based on dietlibc lib/__dtostr.c file.
*/

#include <stdlib.h>
//#include <math.h>

/* convert double to string.  Helper for sprintf. */

static ssize_t
copystring (char *buf, ssize_t maxlen, const char *s)
{
  ssize_t i;

  for (i = 0; i < 3 && i < maxlen; ++i)
    buf[i] = s[i];
  if (i < maxlen)
    buf[i++] = 0;
  return i;
}

ssize_t
formatter_dtostr (double d, char *buf, size_t maxlen,
		  size_t prec, size_t prec2, ssize_t g)
{
#if 1
  union {
    uint64_t l;
    double d;
  } u = {
    .d = d
  };
  /* step 1: extract sign, mantissa and exponent */
  int_fast32_t e = ((u.l >> 52) & ((1 << 11) - 1)) - 1023;
#else
# ifdef CONFIG_CPU_ENDIAN_LITTLE
  int_fast32_t e =
    (((((uint_fast32_t *) & d)[1]) >> 20) & ((1 << 11) - 1)) - 1023;
# else
  int_fast32_t e =
    (((*((uint_fast32_t *) & d)) >> 20) & ((1 << 11) - 1)) - 1023;
# endif
#endif
  /*  uint_fast32_t long m=u.l & ((1ull<<52)-1); */
  /* step 2: exponent is base 2, compute exponent for base 10 */
  int_fast32_t e10;
  /* step 3: calculate 10^e10 */
  size_t i;
  double backup = d;
  double tmp;
  char *oldbuf = buf;

  if ((i = (d != d)))
    return copystring (buf, maxlen, i > 0 ? "inf" : "-inf");
  if (__builtin_isnan(d))
    return copystring (buf, maxlen, "nan");
  e10 = 1 + (long) (e * 0.30102999566398119802);	/* log10(2) */
  /* Wir iterieren von Links bis wir bei 0 sind oder maxlen erreicht
   * ist.  Wenn maxlen erreicht ist, machen wir das nochmal in
   * scientific notation.  Wenn dann von prec noch was übrig ist, geben
   * wir einen Dezimalpunkt aus und geben prec2 Nachkommastellen aus.
   * Wenn prec2 Null ist, geben wir so viel Stellen aus, wie von prec
   * noch übrig ist. */
  if (d == 0.0)
    {
      prec2 = prec2 == 0 ? 1 : prec2 + 2;
      prec2 = prec2 > maxlen ? 8 : prec2;
      i = 0;
      if (prec2 && (int64_t) u.l < 0)
	{
	  buf[0] = '-';
	  ++i;
	}
      for (; i < prec2; ++i)
	buf[i] = '0';
      buf[buf[0] == '0' ? 1 : 2] = '.';
      buf[i] = 0;
      return i;
    }

  if (d < 0.0)
    {
      d = -d;
      *buf = '-';
      --maxlen;
      ++buf;
    }

  /*
     Perform rounding. It needs to be done before we generate any
     digits as the carry could propagate through the whole number.
   */

  tmp = 0.5;
  for (i = 0; i < prec2; i++)
    tmp *= 0.1;
  d += tmp;

  if (d < 1.0)
    {
      *buf = '0';
      --maxlen;
      ++buf;
    }
  /*  printf("e=%d e10=%d prec=%d\n",e,e10,prec); */
  if (e10 > 0)
    {
      ssize_t first = 1;	/* are we about to write the first digit? */

      for (tmp = 10.0; i > 10; i -= 10)
        tmp = tmp * 1e10;

      for (i = e10; i > 1; --i)
        tmp = tmp * 10;

      /* the number is greater than 1. Iterate through digits before the
       * decimal point until we reach the decimal point or maxlen is
       * reached (in which case we switch to scientific notation). */
      while (tmp > 0.9)
	{
	  char digit;
	  double fraction = d / tmp;
	  digit = (ssize_t) (fraction);	/* floor() */
	  if (!first || digit)
	    {
	      first = 0;
	      *buf = digit + '0';
	      ++buf;
	      if (!maxlen)
		{
		  /* use scientific notation */
		  ssize_t len = formatter_dtostr(backup / tmp, oldbuf, maxlen, prec, prec2, 0);
		  ssize_t initial = 1;
		  if (len == 0)
		    return 0;
		  maxlen -= len;
		  buf += len;
		  if (maxlen > 0)
		    {
		      *buf = 'e';
		      ++buf;
		    }
		  --maxlen;
		  for (len = 1000; len > 0; len /= 10)
		    {
		      if (e10 >= len || !initial)
			{
			  if (maxlen > 0)
			    {
			      *buf = (e10 / len) + '0';
			      ++buf;
			    }
			  --maxlen;
			  initial = 0;
			  e10 = e10 % len;
			}
		    }
		  if (maxlen > 0)
		    goto fini;
		  return 0;
		}
	      d -= digit * tmp;
	      --maxlen;
	    }
	  tmp /= 10.0;
	}
    }
  else
    {
      tmp = 0.1;
    }

  if (buf == oldbuf)
    {
      if (!maxlen)
	return 0;
      --maxlen;
      *buf = '0';
      ++buf;
    }
  if (prec2 || prec > (size_t) (buf - oldbuf) + 1)
    {				/* more digits wanted */
      if (!maxlen)
	return 0;
      --maxlen;
      *buf = '.';
      ++buf;
      if (g)
	{
	  if (prec2)
	    prec = prec2;
	  prec -= buf - oldbuf - 1;
	}
      else
	{
	  prec -= buf - oldbuf - 1;
	  if (prec2)
	    prec = prec2;
	}
      if (prec > maxlen)
	return 0;
      while (prec > 0)
	{
	  char digit;
	  double fraction = d / tmp;
	  digit = (ssize_t) (fraction);	/* floor() */
	  *buf = digit + '0';
	  ++buf;
	  d -= digit * tmp;
	  tmp /= 10.0;
	  --prec;
	}
    }
fini:
  *buf = 0;
  return buf - oldbuf;
}
