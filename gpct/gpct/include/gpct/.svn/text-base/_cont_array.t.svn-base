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

#ifndef GPCT_CONT_ARRAY_H_
#define GPCT_CONT_ARRAY_H_

/* backslash-region-begin */
#define GPCT_CONT_ARRAYS_FUNC(attr, name, prefix, lockname, algo)

attr GPCT_CONT_PROTO_ISNULL(name, prefix)
{
  return index == name##_gpct_null_index;
}

attr GPCT_CONT_PROTO_ISVALID(name, prefix)
{
  return ((index >= 0) && (index < root->count));
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISEMPTY(name, prefix)
{
  return root->count == 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GET(name, prefix)
{
  name##_item_t         item;

  GPCT_ASSERT((index >= 0) && (index < root->count));

  item = root->data[index];

  GPCT_CONT_REFNEW(name, item);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETPTR(name, prefix)
{
  return root->data + index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETINDEX(name, prefix)
{
  gpct_sindex_t      i = (ptr - root->data);

  GPCT_ASSERT(i >= 0);
  GPCT_ASSERT(i < GPCT_CONT_##algo##_SIZE(name, root));
  GPCT_ASSERT(i < root->count);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  GPCT_CONT_REFDROP(name, root->data[index]);
  root->data[index] = GPCT_CONT_REFNEW(name, item);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_NEXT(name, prefix)
{
  name##_index_t                res;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  res = index < root->count - 1
    ? index + 1
    : name##_gpct_null_index;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PREV(name, prefix)
{
  name##_index_t                res;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  res = index > 0
    ? index - 1
    : name##_gpct_null_index;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_HEAD(name, prefix)
{
  name##_index_t                res;

  res = root->count ? 0 : name##_gpct_null_index;

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_TAIL(name, prefix)
{
  name##_index_t                res;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  res = root->count
    ? root->count - 1
    : name##_gpct_null_index;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_COUNT(name, prefix)
{
  return root->count;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SIZE(name, prefix)
{
  return GPCT_CONT_##algo##_SIZE(name, root);
}

attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  if (gpct_cont_array_##name##_extend(root, 1))
    {
      GPCT_CONT_REFNEW(name, item);
      memmove(root->data + index + 1,
              root->data + index,
              (root->count - index) * sizeof(name##_item_t));
      root->data[index] = item;
      root->count++;
      res++;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_INSERT_POST(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  if (gpct_cont_array_##name##_extend(root, 1))
    {
      GPCT_CONT_REFNEW(name, item);
      memmove(root->data + index + 2,
              root->data + index + 1,
              (root->count - index - 1) * sizeof(name##_item_t));
      root->data[index + 1] = item;
      root->count++;
      res++;
     }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  gpct_index_t     i;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  for (i = 0; i < root->count; i++)
    GPCT_CONT_REFDROP(name, root->data[i]);

  root->count = 0;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  GPCT_CONT_REFDROP(name, root->data[index]);

  memmove(root->data + index,
          root->data + index + 1,
          (root->count - index - 1) * sizeof(name##_item_t));

  root->count--;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (gpct_cont_array_##name##_extend(root, 1))
    {
      memmove(root->data + 1, root->data,
              root->count * sizeof(name##_item_t));
      root->data[0] = item;
      root->count++;
      res++;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (gpct_cont_array_##name##_extend(root, 1))
    {
      root->data[root->count++] = GPCT_CONT_REFNEW(name, item);
      res++;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ALLOC(name, prefix)
{
  name##_index_t        res = name##_gpct_null_index;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (gpct_cont_array_##name##_extend(root, 1))
    res = root->count++;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP(name, prefix)
{
  name##_item_t res = name##_gpct_null_item;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->count > 0)
    {
      res = root->data[0];
      memmove(root->data, root->data + 1,
              (root->count - 1) * sizeof(name##_item_t));
      root->count--;
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POPBACK(name, prefix)
{
  name##_item_t res = name##_gpct_null_item;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (root->count > 0)
    res = root->data[--root->count];

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = gpct_cont_array_##name##_extend(root, size);

  memmove(root->data + i, root->data,
          root->count * sizeof(name##_item_t));
  for (j = 0; j < i; j++)
    root->data[i - j - 1] = GPCT_CONT_REFNEW(name, item[j]);

  root->count += i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = gpct_cont_array_##name##_extend(root, size);

  for (j = 0; j < i; j++)
    root->data[root->count + j] = GPCT_CONT_REFNEW(name, item[j]);

  root->count += i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP_ARRAY(name, prefix)
{
  gpct_index_t     i;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = size > root->count ? root->count : size;

  memcpy(item,
         root->data,
         i * sizeof(name##_item_t));
  memmove(root->data,
          root->data + i,
          (root->count - i) * sizeof(name##_item_t));

  root->count -= i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = size > root->count ? root->count : size;

  for (j = 0; j < i; j++)
    item[j] = root->data[i - j - 1];
  memmove(root->data,
          root->data + i,
          i * sizeof(name##_item_t));

  root->count -= i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH(name, prefix)
{
  gpct_index_t     i;
  intptr_t      res = 0;
  va_list       ap;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = 0; !res && i < root->count; i++)
    {
      va_start(ap, fcn);
      res = fcn(root->data[i], ap);
      va_end(ap);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_FOREACH_REVERSE(name, prefix)
{
  gpct_index_t     i;
  intptr_t      res = 0;
  va_list       ap;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (i = root->count; !res && i > 0; i--)
    {
      va_start(ap, fcn);
      res = fcn(root->data[i - 1], ap);
      va_end(ap);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */




/***********************************************************************
 *      iteration macros
 */


/* backslash-region-begin */
#define GPCT_CONT_ARRAYS_FOREACH_FROM(name, algo, root, idx, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_i;

  GPCT_ASSERT((gpct_sindex_t)idx <= (gpct_sindex_t)(root)->count);
  GPCT_ASSERT((gpct_sindex_t)idx >= -1);

  for (gpct_i = (idx + 1);
       gpct_i < (root)->count;
       gpct_i++)
    {
      GPCT_CONT_LABEL_DECL(gpct_continue);
      GPCT_UNUSED name##_index_t index = gpct_i;
      GPCT_UNUSED name##_item_t item = (root)->data[gpct_i];

      { __VA_ARGS__ }
      GPCT_CONT_LABEL(gpct_continue);
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */



/* backslash-region-begin */
#define GPCT_CONT_ARRAYS_FOREACH_REVERSE_FROM(name, algo, root, idx, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_i;

  GPCT_ASSERT((gpct_sindex_t)idx <= (gpct_sindex_t)(root)->count);
  GPCT_ASSERT((gpct_sindex_t)idx >= -1);

  for (gpct_i = (idx);
       gpct_i > 0;
       gpct_i--)
    {
      GPCT_CONT_LABEL_DECL(gpct_continue);
      GPCT_UNUSED name##_index_t index = gpct_i - 1;
      GPCT_UNUSED name##_item_t item = (root)->data[gpct_i - 1];

      { __VA_ARGS__ }
      GPCT_CONT_LABEL(gpct_continue);
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */


#endif

