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

#ifndef GPCT_CONT_BINTREES_ITER_H_
#define GPCT_CONT_BINTREES_ITER_H_

/***********************************************************************
 *      foreach macros
 */

/* In order iterative binary tree traversal using stack */
/* backslash-region-begin */
#define GPCT_CONT_BINTREES_NOUP_FOREACH_BASE(algo, name, root_,
                                             order, ...)

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




/* 
 * preorder iterative binary tree traversal using stack
 */

/* backslash-region-begin */
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





/*
 * In order iterative binary tree traversal using parent link
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_NOSTACK_FOREACH_BASE(algo, name, root_, order, ...)
      GPCT_CONT_LABEL_DECL(gpct_break);
      GPCT_CONT_LABEL_DECL(gpct_continue);
      name##_entry_t *gpct_node;

      if ((gpct_node = (root_)->root))
        while (gpct_node->child[!order])
          gpct_node = gpct_node->child[!order];

      while (gpct_node)
        {
          {
            GPCT_CONT_LABEL_DECL(gpct_continue);
            name##_item_t item = GPCT_CONT_GET_ITEM(name, gpct_node);
            GPCT_UNUSED name##_index_t index = item;

            { __VA_ARGS__ }
            GPCT_CONT_LABEL(gpct_continue);
          }

          if (gpct_node->child[order] != NULL)
            {
              gpct_node = gpct_node->child[order];
              while (gpct_node->child[!order] != NULL)
                gpct_node = gpct_node->child[!order];
            }
          else
            {
              name##_entry_t *parent;

              while ((parent = gpct_node->parent) != NULL)
                {
                  if (parent->child[!order] == gpct_node)
                    break;
                  gpct_node = parent;
                }
              gpct_node = parent;
            }
        }

      GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_NOSTACK_FOREACH_UNORDERED_BASE(algo, name, root, ...)
 GPCT_CONT_BINTREES_UP_FOREACH_BASE(algo, name, root, 0, __VA_ARGS__) /* FIXME */
/* backslash-region-end */





/*
 *  In order iterative binary tree traversal using parent link
 */

/* backslash-region-begin */
#define GPCT_CONT_BINTREES_UP_FOREACH_BASE(algo, name, root, order, ...)

  if (name##_gpct_stacksize)
    {
      /* prefer stack if available */
      GPCT_CONT_BINTREES_NOUP_FOREACH_BASE(algo, name, root, __VA_ARGS__);
    }
  else
    {
      GPCT_CONT_BINTREES_NOSTACK_FOREACH_BASE(algo, name, root, order, __VA_ARGS__);
    }
/* backslash-region-end */





/* backslash-region-begin */
#define GPCT_CONT_BINTREES_UP_FOREACH_UNORDERED_BASE(algo, name, root, ...)

  if (name##_gpct_stacksize)
    {
      /* prefer stack if available */
      GPCT_CONT_BINTREES_NOUP_FOREACH_UNORDERED_BASE(algo, name, root, __VA_ARGS__);
    }
  else
    {
      GPCT_CONT_BINTREES_NOSTACK_FOREACH_UNORDERED_BASE(algo, name, root, __VA_ARGS__);
    }

/* backslash-region-end */

#endif

