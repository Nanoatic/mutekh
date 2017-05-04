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

    Binary trees common function and macro templates
*/

#ifndef GPCT_CONT_BINTREES_H_
#define GPCT_CONT_BINTREES_H_

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_SYMMETRIC_OPS(n, code)
  {
    { const uintptr_t n = 0; { code } }
    { const uintptr_t n = 1; { code } }
  }
/* backslash-region-end */

/***********************************************************************
 *      root access function
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ROOT_FUNC(attr, name, algo, prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISORPHAN(name, prefix)
{
  return !GPCT_CONT_ORPHAN_CHK(algo, name, GPCT_CONT_GET_ENTRY(name, item));
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ORPHAN(name, prefix)
{
  GPCT_CONT_ORPHAN_SET(algo, name, GPCT_CONT_GET_ENTRY(name, item));
}

attr GPCT_CONT_PROTO_ISNULL(name, prefix)
{
  return index == NULL;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISEMPTY(name, prefix)
{
  return root->root == NULL;
}

attr GPCT_CONT_PROTO_GET(name, prefix)
{
  return index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_COUNT(name, prefix)
{
  return root->count;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SIZE(name, prefix)
{
  return -1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  root->root = NULL;
  root->count = 0;

  return GPCT_LOCK_INIT(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  GPCT_LOCK_DESTROY(lockname, &root->lock);
}
/* backslash-region-end */


/***********************************************************************
 *      balance independent, parent link independent item access function
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_HEADTAIL_CODE(name, algo, prefix, lockname, istail)
{
  name##_entry_t *e = root->root;

  if (e == NULL)
    return NULL;

  while (e->child[istail] != NULL)
    e = e->child[istail];

  return GPCT_CONT_GET_ITEM(name, e);
}
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ACCESS_FUNC(attr, name, algo, prefix, lockname, keyfield)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_HEAD(name, prefix)
GPCT_CONT_BINTREES_HEADTAIL_CODE(name, BINTREE, prefix, lockname, 0)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_TAIL(name, prefix)
GPCT_CONT_BINTREES_HEADTAIL_CODE(name, BINTREE, prefix, lockname, 1)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSH(name, prefix)
{
  name##_entry_t *entry;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  entry = GPCT_CONT_GET_ENTRY(name, item);

  if (root->root == NULL)
    root->root = entry;
  else
    {
      name##_entry_t    *node = root->root;
      uint_fast8_t      dir;

      while (1)
        {
          name##_item_t node_i = GPCT_CONT_GET_ITEM(name, node);
          intmax_t res = GPCT_CONT_COMPARE_FIELDS(name, keyfield, node_i, item);
          dir = res < 0;

          if (node->child[dir] == NULL)
            break;

          node = node->child[dir];
        }

      node->child[dir] = entry;
      prefix##_gpct_set_parent(entry, node);
    }

  root->count++;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return 1;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PUSHBACK(name, prefix)
{
  return prefix##_push(root, item);
}
/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ACCESS_UP_FUNC(attr, name, algo, prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_NEXT(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, BINTREE, prefix, lockname, 1)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PREV(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, BINTREE, prefix, lockname, 0)

/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ACCESS_NOUP_FUNC(attr, name, algo, prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_NEXT(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, BINTREE, prefix, lockname, 1) /* FIXME */

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_PREV(name, prefix)
GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, BINTREE, prefix, lockname, 0) /* FIXME */

/* backslash-region-end */

#endif

