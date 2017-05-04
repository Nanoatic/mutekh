/* -*- c -*-

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

#ifndef GPCT_CONT_BITMAP_H_
#define GPCT_CONT_BITMAP_H_

#define GPCT_CONT_BITMAP_ALIGN(x, b)    ((((x) - 1) | ((b) - 1)) + 1)

#define GPCT_CONT_BITMAP_ALIGN_SIZE(size, type)                 \
 (GPCT_CONT_BITMAP_ALIGN(size, sizeof(type) * 8) / (sizeof(type) * 8))

#define GPCT_CONT_BITMAP_WORDSIZE(name) (sizeof(name##_entry_t) * 8)
#define GPCT_CONT_BITMAP_WSDIV(name, x) (((gpct_index_t)x) / GPCT_CONT_BITMAP_WORDSIZE(name))
#define GPCT_CONT_BITMAP_WSMOD(name, x) (((gpct_index_t)x) % GPCT_CONT_BITMAP_WORDSIZE(name))
#define GPCT_CONT_BITMAP_WSMUL(name, x) (((gpct_index_t)x) * GPCT_CONT_BITMAP_WORDSIZE(name))

/* backslash-region-begin */
#define GPCT_CONT_BITMAPS_FUNC(attr, name, prefix,
                               lockname, algo, ...)

attr GPCT_CONT_PROTO_ISNULL(name, prefix)
{
  return index == name##_gpct_null_index;
}

attr GPCT_CONT_PROTO_ISVALID(name, prefix)
{
  return index >= 0 && index < GPCT_CONT_##algo##_BITCOUNT(root, name);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISEMPTY(name, prefix)
{
  name##_index_t        i;
  gpct_bool_t           res = 1;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; res && i < GPCT_CONT_##algo##_WORDCOUNT(root, name); i++)
    if (root->data[i])
      {
	res = 0;
	break;
      }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GET(name, prefix)
{
  gpct_bool_t           res;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  res = GPCT_BIT_TEST(name##_entry_t,
                      root->data[GPCT_CONT_BITMAP_WSDIV(name, index)],
                      GPCT_CONT_BITMAP_WSMOD(name, index));

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  gpct_bool_t           res;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  res = GPCT_BIT_TEST(name##_entry_t,
                      root->data[GPCT_CONT_BITMAP_WSDIV(name, index)],
                      GPCT_CONT_BITMAP_WSMOD(name, index));

  GPCT_BIT_SETVALUE(name##_entry_t,
                    root->data[GPCT_CONT_BITMAP_WSDIV(name, index)],
                    GPCT_CONT_BITMAP_WSMOD(name, index), item);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_NEXT(name, prefix)
{
  return index < GPCT_CONT_##algo##_BITCOUNT(root, name)
    ? index + 1 : name##_gpct_null_index;
}

attr GPCT_CONT_PROTO_PREV(name, prefix)
{
  return index > 0
    ? index - 1 : name##_gpct_null_index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_HEAD(name, prefix)
{
  return 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_TAIL(name, prefix)
{
  return GPCT_CONT_##algo##_BITCOUNT(root, name) - 1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_COUNT(name, prefix)
{
  name##_index_t        i;
  size_t                res = 0;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; i < GPCT_CONT_##algo##_WORDCOUNT(root, name); i++)
    res += GPCT_POPCOUNT(root->data[i]);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_SIZE(name, prefix)
{
  return GPCT_CONT_##algo##_BITCOUNT(root, name);
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
{
  gpct_index_t     i, j;
  gpct_sindex_t      res = 0;
  va_list       ap;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; !res && i < GPCT_CONT_##algo##_WORDCOUNT(root, name); i++)
    {
      name##_entry_t    word;

      for (word = root->data[i];
           (j = GPCT_BIT_FFS(word));
           GPCT_BIT_CLR(name##_entry_t, word, j - 1))
        {
          va_start(ap, fcn);
          res = fcn(GPCT_CONT_BITMAP_WSMUL(name, i) + j - 1, ap);
          va_end(ap);
        }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  gpct_index_t     i;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  for (i = 0; i < GPCT_CONT_##algo##_WORDCOUNT(root, name); i++)
    root->data[i] = 0;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}

/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_KEY_BITMAPS_FUNC(attr, name, prefix,
                                  lockname, algo, ...)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP(name, prefix, keyfield)
{
  gpct_index_t     i, j;
  gpct_sindex_t      res = name##_gpct_null_index;;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; i < GPCT_CONT_##algo##_WORDCOUNT(root, name); i++)
    {
      name##_entry_t word = root->data[i];

      if (!value)
        word = ~word;

      if ((j = GPCT_BIT_FFS(word)))
        {
          res = GPCT_CONT_BITMAP_WSMUL(name, i) + j - 1;
          break;
        }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_LOOKUP_NEXT(name, prefix, keyfield)
{
  gpct_sindex_t res = name##_gpct_null_index;;
  name##_entry_t word;
  gpct_index_t i, j;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  if (++index < GPCT_CONT_##algo##_BITCOUNT(root, name))
    {
      name##_entry_t mask = ((name##_entry_t)1 << (GPCT_CONT_BITMAP_WSMOD(name, index))) - 1;
      i = GPCT_CONT_BITMAP_WSDIV(name, index);
      word = value ? root->data[i] & ~mask : root->data[i] | mask;

      while (1)
        {
          if (!value)
            word = ~word;

          if ((j = GPCT_BIT_FFS(word)))
            {
              res = GPCT_CONT_BITMAP_WSMUL(name, i) + j - 1;
              break;
            }

          if (++i == GPCT_CONT_##algo##_WORDCOUNT(root, name))
            break;

          word = root->data[i];
        }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

/* backslash-region-end */




/* backslash-region-begin */
#define GPCT_CONT_BITMAPS_FOREACH(name, root, algo, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_i;
  gpct_index_t     gpct_j;

  for (gpct_i = 0;
       gpct_i < GPCT_CONT_##algo##_WORDCOUNT(root, name);
       gpct_i++)
    {
      name##_entry_t    gpct_word;

      for (gpct_word = (root)->data[gpct_i];
           (gpct_j = GPCT_BIT_FFS(gpct_word));
           GPCT_BIT_CLR(name##_entry_t, gpct_word, gpct_j - 1))
        {
          GPCT_CONT_LABEL_DECL(gpct_continue);
          name##_index_t index = GPCT_CONT_BITMAP_WSMUL(name, gpct_i) + gpct_j - 1;
          GPCT_DEPRECATED GPCT_UNUSED name##_index_t item = index;

          { __VA_ARGS__ }
          GPCT_CONT_LABEL(gpct_continue);
        }
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BITMAPS_FOREACH_REVERSE(name, root, algo, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_j;
  gpct_sindex_t    gpct_i;

  for (gpct_i = GPCT_CONT_##algo##_WORDCOUNT(root, name) - 1;
       gpct_i >= 0;
       gpct_i--)
    {
      name##_entry_t    gpct_word;

      for (gpct_word = (root)->data[gpct_i];
           (gpct_j = GPCT_BIT_FFSR(gpct_word));
           GPCT_BIT_CLR(name##_entry_t, gpct_word, gpct_j - 1))
        {
          GPCT_CONT_LABEL_DECL(gpct_continue);
          name##_index_t index = GPCT_CONT_BITMAP_WSMUL(name, gpct_i) + gpct_j - 1;
          GPCT_DEPRECATED GPCT_UNUSED name##_index_t item = index;

          { __VA_ARGS__ }
          GPCT_CONT_LABEL(gpct_continue);
        }
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */


#endif


