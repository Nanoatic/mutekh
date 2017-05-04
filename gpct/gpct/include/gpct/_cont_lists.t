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

    Linked lists common function and macro templates
*/

#ifndef GPCT_CONT_LISTS_H_
#define GPCT_CONT_LISTS_H_

#define GPCT_CONT_LIST_ISEND(algo, root, entry)                         \
 GPCT_CPP_CONCAT(GPCT_CONT_, algo, _ISEND)(root, entry)

#define GPCT_CONT_LIST_EMPTY(algo, root)                                \
 GPCT_CPP_CONCAT(GPCT_CONT_, algo, _EMPTY)(root)




/* 
 * singly linked list previous entry and tail entry seek functions
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_INTERNAL_FUNC(attr, name, prefix)

GPCT_INTERNAL name##_entry_t *
prefix##_gpct_prev(name##_root_t *root,
                   name##_entry_t *ref)
{
  name##_entry_t        *entry = root->head;

  if (entry == ref)
    return NULL;

  for (; GPCT_ASSERT(entry != NULL), entry->next != ref;
       entry = entry->next)
         ;

  return entry;
}

GPCT_INTERNAL name##_entry_t *
prefix##_gpct_tail(name##_root_t *root)
{
  name##_entry_t        *tail;

  for (tail = root->head;
       tail && tail->next;
       tail = tail->next)
        ;

  return tail;
}

GPCT_INTERNAL name##_entry_t **
prefix##_gpct_pprev(name##_root_t *root,
                    name##_entry_t *ref)
{
  name##_entry_t        **entry;

  for (entry = &root->head; *entry != ref; entry = &(*entry)->next)
    ;

  return entry;
}

GPCT_INTERNAL name##_entry_t **
prefix##_gpct_ptail(name##_root_t *root)
{
  name##_entry_t        **entry;

  for (entry = &root->head;
       (*entry)->next != NULL;
       entry = &(*entry)->next)
    ;

  return entry;
}
/* backslash-region-end */





/*
 * linked lists init common functions
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_INIT_FUNC(attr, name, algo, prefix, lockname)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_INIT(name, prefix)
{
  GPCT_CONT_LIST_EMPTY(algo, root);

  return GPCT_LOCK_INIT(lockname, &root->lock);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_DESTROY(name, prefix)
{
  GPCT_LOCK_DESTROY(lockname, &root->lock);
}
/* backslash-region-end */






/*
 * linked lists container root common functions
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_ROOT_FUNC(attr, name, algo,
                                  prefix, lockname, head)

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
  return GPCT_CONT_LIST_ISEND(algo, root, root->head);
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_ISFULL(name, prefix)
{
  return 0;
}

attr GPCT_CONT_PROTO_GET(name, prefix)
{
  return index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETPTR(name, prefix)
{
  return index;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_GETINDEX(name, prefix)
{
  return ptr;
}
/* backslash-region-end */





/* 
 * direct access next/head and prev/tail functions
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_NEXT_HEAD_FUNC(attr, name, algo,
                                       prefix, lockname,
                                       NEXT, HEAD, next, head)

GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_##NEXT(name, prefix)
{
  name##_index_t                item;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  {
    name##_entry_t *next = GPCT_CONT_GET_ENTRY(name, index)->next;

    item = !GPCT_CONT_LIST_ISEND(algo, root, next)
         ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, next))
         : NULL;
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_##HEAD(name, prefix)
{
  name##_index_t                item;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  item = !GPCT_CONT_LIST_ISEND(algo, root, root->head)
       ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, root->head))
       : NULL;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}
/* backslash-region-end */





/* 
 * indirect prev/tail access function for simple linked lists
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_PREV_TAIL_FUNC(attr, name, algo,
                                       prefix, lockname)

GPCT_DEPRECATED
GPCT_NONNULL(1, 2)
attr GPCT_CONT_PROTO_PREV(name, prefix)
{
  name##_index_t        item;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  {
    name##_entry_t *index_e = GPCT_CONT_GET_ENTRY(name, index);

    index_e = prefix##_gpct_prev(root, index_e);

    item = !GPCT_CONT_LIST_ISEND(algo, root, index_e)
         ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, index_e))
         : NULL;
  }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}

GPCT_DEPRECATED
GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_TAIL(name, prefix)
{
  name##_entry_t        *tail;
  name##_index_t        item;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  tail = prefix##_gpct_tail(root);

  item = !GPCT_CONT_LIST_ISEND(algo, root, tail)
       ? GPCT_CONT_REFNEW(name, GPCT_CONT_GET_ITEM(name, tail))
       : NULL;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return item;
}
/* backslash-region-end */





/*
 * linked list entry count function
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_COUNT_FUNC(attr, name, algo,
                                   prefix, lockname, head)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_COUNT(name, prefix)
{
  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  name##_entry_t        *entry = root->head;
  size_t                i;

  for (i = 0; !GPCT_CONT_LIST_ISEND(algo, root, entry); i++)
    entry = entry->next;

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return i;
}

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_SIZE(name, prefix)
{
  return -1;
}
/* backslash-region-end */





/*
 * linked list clear functions
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_CLEAR_FUNC(attr, name, algo,
                                   prefix, lockname, head)

GPCT_NONNULL(1)
attr GPCT_CONT_PROTO_CLEAR(name, prefix)
{
  name##_entry_t        *entry, *next;

  GPCT_LOCK_WRLOCK(lockname, &root->lock);

  for (entry = root->head;
       !GPCT_CONT_LIST_ISEND(algo, root, entry);
       entry = next)
    {
      next = entry->next;
      GPCT_CONT_ORPHAN_SET(algo, name, entry);
      GPCT_CONT_REFDROP(name, GPCT_CONT_GET_ITEM(name, entry));
    }

  GPCT_CONT_LIST_EMPTY(algo, root);

  GPCT_LOCK_UNLOCK(lockname, &root->lock);
}
/* backslash-region-end */





/*
 * forward/backward iteration template function code
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_FOREACH_FUNCCODE(attr, name, algo, prefix,
                                         lockname, head, next)

{
  name##_entry_t        *entry;
  intptr_t              res = 0;
  va_list               ap;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (entry = root->head;
       !res && !GPCT_CONT_LIST_ISEND(algo, root, entry);
       entry = entry->next)
    {
      va_start(ap, fcn);
      res = fcn(GPCT_CONT_GET_ITEM(name, entry), ap);
      va_end(ap);
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */





/* 
 * backward iteration template function code for singly linked lists
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_FOREACH_REVERSE_FUNCCODE(attr, name, algo,
                                                 prefix, lockname)

{
  name##_entry_t        *entry, *next;
  intptr_t              res = 0;
  va_list               ap;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (next = NULL;
       next != (root)->head;
       next = entry)
    {
      for (entry = (root)->head;
           entry->next != next;
           entry = entry->next)
        ;
      {
        va_start(ap, fcn);
        res = fcn(GPCT_CONT_GET_ITEM(name, entry), ap);
        va_end(ap);
      }
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */





/*
 * forward/backward iteration template macro
 */


/* backslash-region-begin */
#define GPCT_CONT_LISTS_FOREACH_BASE(algo, name, root, head, next, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  name##_entry_t        *gpct_entry;
  name##_entry_t        *gpct_next;

  for (gpct_entry = head;
       !GPCT_CONT_LIST_ISEND(algo, root, gpct_entry);
       gpct_entry = gpct_next)
    {
      GPCT_CONT_LABEL_DECL(gpct_continue);
      name##_item_t item = GPCT_CONT_GET_ITEM(name, gpct_entry);
      GPCT_UNUSED name##_index_t index = item;
      gpct_next = gpct_entry->next;

      { __VA_ARGS__ }
      GPCT_CONT_LABEL(gpct_continue);
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */





/* 
 * backward iteration template macro for singly linked lists
 */


/* backslash-region-begin */
#define GPCT_CONT_LISTS_FOREACH_REVERSE_BASE(name, root, last, ...)
  GPCT_CONT_LABEL_DECL(gpct_break);
  name##_entry_t        *gpct_entry;
  name##_entry_t        *gpct_next;

  for (gpct_next = last;
       gpct_next != (root)->head;
       gpct_next = gpct_entry)
    {
      for (gpct_entry = (root)->head;
           gpct_entry->next != gpct_next;
           gpct_entry = gpct_entry->next)
        ;
      {
        GPCT_CONT_LABEL_DECL(gpct_continue);
        name##_item_t item = GPCT_CONT_GET_ITEM(name, gpct_entry);
        GPCT_UNUSED name##_index_t index = item;

        { __VA_ARGS__ }
        GPCT_CONT_LABEL(gpct_continue);
      }
    }
  GPCT_CONT_LABEL(gpct_break);
/* backslash-region-end */





/*
 * efficient, stack based, linked list in-place merge sort (stable)
 */
/* FIXME may be optionnaly moved in a library */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_SORT_FUNC(algo, entry_type)

GPCT_INTERNAL entry_type *
gpct_##algo##_merge(entry_type *a, entry_type *b,
                    gpct_bool_t (*cmp)(entry_type *a, entry_type *b))
{
  entry_type *first, *last;

  if (cmp(a, b))
    a = (last = first = a)->next;
  else
    b = (last = first = b)->next;

  while (a != NULL && b != NULL)
    if (cmp(a, b))
      a = (last = last->next = a)->next;
    else
      b = (last = last->next = b)->next;

  last->next = a != NULL ? a : b;
  return first;
}

GPCT_INTERNAL entry_type *
gpct_##algo##_sort(entry_type *tail,
                   gpct_bool_t (*cmp)(entry_type *a, entry_type *b))
{
    size_t n = 0;
    entry_type *stack[32];      /* we are able to sort 2^31 nodes */
    entry_type **s = stack;

    while (tail != NULL)
      {
        size_t idx, tmp;
        entry_type *a = tail;
        entry_type *b = tail->next;

        if (b == NULL)
          {
            *s++ = a;
            break;
          }

        tail = b->next;

        if (cmp(a, b))
          ((*s = a)->next = b)->next = 0;
        else
          ((*s = b)->next = a)->next = 0;
        s++;

        tmp = n++;
        for (idx = n ^ tmp; idx &= idx - 1; s--)
          s[-2] = gpct_##algo##_merge(s[-2], s[-1], cmp);
      }

    while (s-- > stack + 1)
      s[-1] = gpct_##algo##_merge(s[-1], s[0], cmp);

    return stack[0];
}
/* backslash-region-end */





/*
 * Comparaison functions for use with merge and sorting algorithm
 */

/* backslash-region-begin */
#define GPCT_CONT_LISTS_CMP_FUNC(name, prefix, algo, keyfield)

GPCT_INTERNAL gpct_bool_t
prefix##_gpct_cmp_ascend(gpct_##algo##_entry_t *a,
                       gpct_##algo##_entry_t *b)
{
  return (GPCT_CONT_COMPARE_FIELDS(name, keyfield,
           GPCT_CONT_GET_ITEM(name, a),
           GPCT_CONT_GET_ITEM(name, b)) <= 0);
}

GPCT_INTERNAL gpct_bool_t
prefix##_gpct_cmp_descend(gpct_##algo##_entry_t *a,
                        gpct_##algo##_entry_t *b)
{
  return (GPCT_CONT_COMPARE_FIELDS(name, keyfield,
           GPCT_CONT_GET_ITEM(name, a),
           GPCT_CONT_GET_ITEM(name, b)) >= 0);
}
/* backslash-region-end */





/* backslash-region-begin */
#define GPCT_CONT_LISTS_SORTED_FUNCCODE(algo, name, lockname, cmp, head)
{
  name##_entry_t        *entry;
  gpct_bool_t		res = 1;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  for (entry = head;
       !GPCT_CONT_LIST_ISEND(algo, root, entry) &&
       !GPCT_CONT_LIST_ISEND(algo, root, entry->next);
       entry = entry->next)
    {
      if (!cmp(entry, entry->next))
	{
	  res = 0;
	  break;
	}
    }

  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}
/* backslash-region-end */


#endif

