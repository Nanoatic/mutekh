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

    Binary trees with parent link common function and macro templates
*/

#ifndef GPCT_CONT_BINTREES_UP_H_
#define GPCT_CONT_BINTREES_UP_H_

/***********************************************************************
 *      internal functions for binary trees
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_UP_INTERNAL_FUNC(attr, name, prefix, keyfield)

GPCT_INTERNAL name##_entry_t *
prefix##_gpct_get_parent(name##_root_t *root,
                     name##_entry_t *node)
{
  return node->parent;
}

GPCT_INTERNAL void
prefix##_gpct_set_parent(name##_entry_t *node,
                         name##_entry_t *parent)
{
  node->parent = parent;
}
/* backslash-region-end */


/***********************************************************************
 *      root access function
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ROOT_UP_FUNC(attr, name, algo,
                                        prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  name##_entry_t        *node;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  /* destructive traversal */
  for (node = root->root; node != NULL; )
    {
      if (node->child[0] != NULL)
        node = node->child[0];
      else if (node->child[1] != NULL)
        node = node->child[1];
      else
        {
          name##_entry_t *parent = node->parent;

          GPCT_CONT_ORPHAN_SET(algo, name, node);
          GPCT_CONT_REFDROP(name, GPCT_CONT_GET_ITEM(name, node));
          if (parent)
            parent->child[0] = parent->child[1] = NULL;
          node = parent;
        }
    }

  root->root = NULL;
  root->count = 0;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */


/***********************************************************************
 *      item access function
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_NEXTPREV_UP_CODE(name, algo, prefix, lockname, isnext)
{
  name##_entry_t *index_e = GPCT_CONT_GET_ENTRY(name, index);
  name##_entry_t *parent;

  if (index_e->child[!isnext] != NULL)
    {
      index_e = index_e->child[!isnext];
      while (index_e->child[isnext] != NULL)
        index_e = index_e->child[isnext];
      return GPCT_CONT_GET_ITEM(name, index_e);
    }

  while (((parent = index_e->parent)) != NULL)
    {
      if (parent->child[isnext] == index_e)
        break;
      index_e = parent;
    }

  return GPCT_CONT_GET_NULLITEM(name, parent);
}
/* backslash-region-end */

#endif

