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

#ifndef GPCT_CONT_RING_H_
#define GPCT_CONT_RING_H_

GPCT_INTERNAL gpct_bool_t
gpct_cont_ring_size_ispow2(size_t size)
{
  return !(size & (size - 1)); /* check size is a power of 2 */
}

/* backslash-region-begin */
#define GPCT_CONT_RINGS_FUNC(attr, name, prefix, lockname, algo)

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

  item = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + (gpct_index_t)index)];

  GPCT_CONT_REFNEW(name, item);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETPTR(name, prefix)
{
  return root->data + GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + (gpct_index_t)index);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETINDEX(name, prefix)
{
  gpct_sindex_t      i = (ptr - root->data);
  gpct_sindex_t      n = i - GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first);

  GPCT_ASSERT(!(((char*)ptr - (char*)root->data) % sizeof(*ptr)));
  GPCT_ASSERT(i >= 0);
  GPCT_ASSERT(i < GPCT_CONT_##algo##_SIZE(name, root));

  if (n < 0)
    n += GPCT_CONT_##algo##_SIZE(name, root);

  GPCT_ASSERT(n >= 0 && n < root->count);

  return n;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SET(name, prefix)
{
  gpct_index_t     i;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  i = GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + (gpct_index_t)index);
  GPCT_CONT_REFDROP(name, root->data[i]);
  root->data[i] = GPCT_CONT_REFNEW(name, item);

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
    ? GPCT_CONT_##algo##_SIZE_MOD(name, root, index + 1)
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
    ? GPCT_CONT_##algo##_SIZE_MOD(name, root, index - 1)
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
    ? GPCT_CONT_##algo##_SIZE_MOD(name, root, root->count - 1)
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

GPCT_INTERNAL void
prefix##_xchg_  (name##_item_t *a, name##_item_t *b)
{
  name##_item_t tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

attr GPCT_CONT_PROTO_INSERT_PRE(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  if (gpct_cont_ring_##name##_extend(root, 1))
    {
      gpct_index_t         i;

      GPCT_CONT_REFNEW(name, item);
        /* FIXME replace with 2 memcpy/memmove */
      for (i = index; i < root->count; i++)
        prefix##_xchg_(&item, root->data + GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + i));

      root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + i)] = item;
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

  if (gpct_cont_ring_##name##_extend(root, 1))
    {
      gpct_index_t         i;

      GPCT_CONT_REFNEW(name, item);
        /* FIXME replace with 2 memcpy/memmove */
      for (i = index + 1; i < root->count; i++)
        prefix##_xchg_(&item, root->data + GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + i));

      root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + i)] = item;
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

  for (i = root->first; i != (gpct_index_t)(root->first + root->count); i++)
    GPCT_CONT_REFDROP(name, root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, i)]);

  root->first = 0;
  root->count = 0;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_REMOVE(name, prefix)
{
  gpct_index_t     j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  GPCT_ASSERT((index >= 0) && (index < root->count));

  GPCT_CONT_REFDROP(name, root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + index)]);

        /* FIXME replace with 2 memcpy/memmove */
  for (j = root->first + index;
       j != (gpct_index_t)(root->first + root->count - 1); j++)
    root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, j)]
      = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, j + 1)];

  root->count--;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 0;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  size_t        res = 0;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  if (gpct_cont_ring_##name##_extend(root, 1))
    {
      root->count++;
      root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, --root->first)]
        = GPCT_CONT_REFNEW(name, item);
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

  if (gpct_cont_ring_##name##_extend(root, 1))
    {
      root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + root->count++)]
        = GPCT_CONT_REFNEW(name, item);
      res++;
    }

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
      root->count--;
      res = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first++)];
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
    res = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, root->first + --root->count)];

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = gpct_cont_ring_##name##_extend(root, size);

        /* FIXME replace with 2 memcpy/memmove */
  for (j = root->first;
       j != (gpct_index_t)(root->first - i); )
    root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, --j)]
                   = GPCT_CONT_REFNEW(name, *item++);

  root->count += i;
  root->first -= i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = gpct_cont_ring_##name##_extend(root, size);

        /* FIXME replace with 2 memcpy/memmove */
  for (j = root->first + root->count;
       j != (gpct_index_t)(root->first + root->count + i); j++)
    root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, j)]
                 = GPCT_CONT_REFNEW(name, *item++);

  root->count += i;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_POP_ARRAY(name, prefix)
{
  gpct_index_t     i, j;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  i = size > root->count ? root->count : size;

        /* FIXME replace with 2 memcpy/memmove */
  for (j = root->first;
       j != (gpct_index_t)(root->first + i); j++)
    *item++ = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, j)];

  root->first += i;
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

        /* FIXME replace with 2 memcpy/memmove */
  for (j = root->first + root->count;
       j != (gpct_index_t)(root->first + root->count - i); )
    *item++ = root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, --j)];

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

  for (i = root->first; !res &&
       i != (gpct_index_t)(root->first + root->count); i++)
    {
      va_start(ap, fcn);
      res = fcn(root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, i)], ap);
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

  for (i = root->first + root->count; !res && i != root->first; i--)
    {
      va_start(ap, fcn);
      res = fcn(root->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, i - 1)], ap);
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
#define GPCT_CONT_RINGS_FOREACH_FROM(name, algo, root, idx, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_i;

  GPCT_ASSERT((gpct_sindex_t)idx <= (gpct_sindex_t)(root)->count);
  GPCT_ASSERT((gpct_sindex_t)idx >= -1);

  for (gpct_i = (root)->first + (idx + 1);
       gpct_i != (gpct_index_t)((root)->first + (root)->count);
       gpct_i++)
    {
      GPCT_CONT_LABEL_DECL(gpct_continue);
      GPCT_UNUSED name##_index_t index = gpct_i - (root)->first;
      GPCT_UNUSED name##_item_t item = (root)->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, gpct_i)];

      { __VA_ARGS__ }
      GPCT_CONT_LABEL(gpct_continue);
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */



/* backslash-region-begin */
#define GPCT_CONT_RINGS_FOREACH_REVERSE_FROM(name, algo, root, idx, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  gpct_index_t     gpct_i;

  GPCT_ASSERT((gpct_sindex_t)idx <= (gpct_sindex_t)(root)->count);
  GPCT_ASSERT((gpct_sindex_t)idx >= -1);

  for (gpct_i = (root)->first + (idx);
       gpct_i != (root)->first;
       gpct_i--)
    {
      GPCT_CONT_LABEL_DECL(gpct_continue);
      GPCT_UNUSED name##_index_t index = gpct_i - 1 - (root)->first;
      GPCT_UNUSED name##_item_t item = (root)->data[GPCT_CONT_##algo##_SIZE_MOD(name, root, gpct_i - 1)];

      { __VA_ARGS__ }
      GPCT_CONT_LABEL(gpct_continue);
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */


#endif

