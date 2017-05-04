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

    Generic lookup functions set based on container FOREACH iteration

*/

#ifndef GPCT_CONT_LOOKUP_H_
#define GPCT_CONT_LOOKUP_H_

/* backslash-region-begin */
#define GPCT_CONT_REMOVE_KEY_FUNC(attr, name, algo, prefix,
			          lockname, keyfield)
/* backslash-region-end */

#if 0
attr GPCT_CONT_PROTO_REMOVE_KEY(name, prefix, keyfield)
{
  name##_index_t idx = prefix##_lookup(root, value); /* FIXME should not use prefix here */
  gpct_error_t err = -1;

  if (idx != name##_gpct_null_index)
    {
/*      prefix##_remove(root, idx); FIXME */
      GPCT_CONT_REFDROP(name, prefix##_get(root, idx)); /* FIXME should not use prefix here */
      err = 0;
    }

  return err;
}
#endif

/* backslash-region-begin */
#define   GPCT_CONT_LOOKUP_FUNC(attr, name, algo, prefix,
                                lockname, keyfield)

attr GPCT_CONT_PROTO_LOOKUP(name, prefix, keyfield)
{
  name##_index_t        res = name##_gpct_null_index;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  { GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH)(name, root,
  {
    if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
      {
        GPCT_CONT_REFNEW(name, item);
        res = index;
        goto found;
      }
  }); }

 found:
  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_LOOKUP_LAST(name, prefix, keyfield)
{
  name##_index_t        res = name##_gpct_null_index;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  { GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH_REVERSE)(name, root,
  {
    if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
      {
        GPCT_CONT_REFNEW(name, item);
        res = index;
        goto found;
      }
  }); }

 found:
  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_LOOKUP_NEXT(name, prefix, keyfield)
{
  name##_index_t        res = name##_gpct_null_index;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  { GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH_FROM)(name, root, index,
  {
    if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
      {
        GPCT_CONT_REFNEW(name, item);
        res = index;
        goto found;
      }
  }); }

 found:
  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

attr GPCT_CONT_PROTO_LOOKUP_PREV(name, prefix, keyfield)
{
  name##_index_t        res = name##_gpct_null_index;

  GPCT_LOCK_RDLOCK(lockname, &root->lock);

  { GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH_REVERSE_FROM)(name, root, index,
  {
    if (!GPCT_CONT_COMPARE_FIELD_ARG(name, keyfield, item, value))
      {
        GPCT_CONT_REFNEW(name, item);
        res = index;
        goto found;
      }
  }); }

 found:
  GPCT_LOCK_UNLOCK(lockname, &root->lock);

  return res;
}

GPCT_CONT_REMOVE_KEY_FUNC(attr, name, algo, prefix,
                                lockname, keyfield)
/* backslash-region-end */


#endif

