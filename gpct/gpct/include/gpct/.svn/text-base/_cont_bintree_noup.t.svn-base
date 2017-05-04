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

    Binary trees w/o parent link common function and macro templates
*/

#ifndef GPCT_CONT_BINTREES_NOUP_H_
#define GPCT_CONT_BINTREES_NOUP_H_

/***********************************************************************
 *      foreach macros
 */

/* backslash-region-begin */  /* In order iterative binary tree traversal using stack */
#define GPCT_CONT_BINTREES_NOUP_FOREACH_BASE(algo, name, root_, order, ...)

  GPCT_CONT_LABEL_DECL(gpct_break);
  GPCT_CONT_LABEL_DECL(gpct_continue);
  name##_entry_t *gpct_stack[name##_gpct_stacksize];
  name##_entry_t *gpct_node = (root_)->root;
  uintptr_t i = 0;

  while (gpct_node != NULL)
    {
      while (gpct_node != NULL)
        {
          if (gpct_node->child[!order] != NULL)
            {
              GPCT_ASSERT(i < name##_gpct_stacksize);
              gpct_stack[i++] = gpct_node->child[!order];
            }
          GPCT_ASSERT(i < name##_gpct_stacksize);
          gpct_stack[i++] = gpct_node;
          gpct_node = gpct_node->child[order];
        }

      do
        {
          GPCT_CONT_LABEL_DECL(gpct_continue);

          GPCT_ASSERT(i > 0);
          gpct_node = gpct_stack[--i];

          {
            name##_item_t item = GPCT_CONT_GET_ITEM(name, gpct_node);
            GPCT_UNUSED name##_index_t index = item;

            { __VA_ARGS__ }
          }
          GPCT_CONT_LABEL(gpct_continue);
        }
      while (i && gpct_node->child[!order] == NULL);

      if (!i)
        break;

      GPCT_ASSERT(i > 0);
      gpct_node = gpct_stack[--i];
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */


/* backslash-region-begin */  /* preorder iterative binary tree traversal using stack */
#define GPCT_CONT_BINTREES_NOUP_FOREACH_UNORDERED_BASE(algo, name, root_, ...)

  GPCT_CONT_LABEL_DECL(gpct_break);
  GPCT_CONT_LABEL_DECL(gpct_continue);

  if ((root_)->root != NULL)
    {
      name##_entry_t    *gpct_stack[name##_gpct_stacksize];
      gpct_index_t      gpct_i = 1;

      gpct_stack[0] = (root_)->root;

      while (gpct_i)
        {
          name##_entry_t        *gpct_node = gpct_stack[--gpct_i];

          do
            {
              name##_entry_t *gpct_next = gpct_node->child[0];
              name##_item_t item = GPCT_CONT_GET_ITEM(name, gpct_node);
              GPCT_UNUSED name##_index_t index = item;

              /* only push right links */
              if (gpct_node->child[1] != NULL)
                {
                  GPCT_ASSERT(gpct_i < name##_gpct_stacksize);
                  gpct_stack[gpct_i++] = gpct_node->child[1];
                }

              { __VA_ARGS__ }

              GPCT_CONT_LABEL(gpct_continue);

              gpct_node = gpct_next;
            }
          while (gpct_node != NULL);
        }
    }

  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */

/***********************************************************************
 *      internal functions for binary trees
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_NOUP_INTERNAL_FUNC(attr, name, prefix, keyfield)

GPCT_INTERNAL name##_entry_t *
prefix##_gpct_get_parent(name##_root_t *root,
                     name##_entry_t *node)
{
  name##_item_t node_i = GPCT_CONT_GET_ITEM(name, node);
  name##_entry_t *prev, *entry = root->root;

  while (1)
    {
      GPCT_ASSERT(entry != NULL);
      {
        name##_item_t item = GPCT_CONT_GET_ITEM(name, entry);
        intmax_t res = GPCT_CONT_COMPARE_FIELDS(name, keyfield, item, node_i);

        if (!res)
          return prev;

        prev = entry;
        entry = entry->child[res <= 0];
      }
    }
}

GPCT_INTERNAL void
prefix##_gpct_set_parent(name##_entry_t *node,
                         name##_entry_t *parent)
{
}
/* backslash-region-end */


/***********************************************************************
 *      root access function
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_ROOT_NOUP_FUNC(attr, name, algo,
                                          prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  {
    GPCT_CONT_BINTREES_NOUP_FOREACH_UNORDERED_BASE(algo, name, root, {
      GPCT_CONT_ORPHAN_SET(algo, name, gpct_node);
      GPCT_CONT_REFDROP(name, item);
    });
  }

  root->root = NULL;
  root->count = 0;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */


/***********************************************************************
 *      item access function
 */

#endif

