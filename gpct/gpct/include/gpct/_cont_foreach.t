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

    This file defines container iteration macros

*/

#ifndef GPCT_CONT_FOREACH_H_
#define GPCT_CONT_FOREACH_H_

/* CONTAINER_FOREACH break and continue macros */

#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC)

#define GPCT_CONT_LABEL_DECL(x)         __label__ x
#define GPCT_CONT_LABEL(x)              goto x; x:
#define GPCT_CONT_GOTO(x)               goto x
#define CONTAINER_FOREACH_BREAK         goto gpct_break
#define CONTAINER_FOREACH_CONTINUE      goto gpct_continue

#else

#define GPCT_CONT_LABEL_DECL(x)
#define GPCT_CONT_LABEL(x)
#define GPCT_CONT_GOTO(x)
#define CONTAINER_FOREACH_BREAK         GPCT_STR_ERROR("CONTAINER_FOREACH_BREAK depends on missing GNU C extension");
#define CONTAINER_FOREACH_CONTINUE      GPCT_STR_ERROR("CONTAINER_FOREACH_CONTINUE depends on missing GNU C extension");

#endif

/* Iterate over the whole container and execute usercode for each
   item */

/* backslash-region-begin */
#define CONTAINER_FOREACH_NOLOCK(name, algo, root, ...)
{
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH)
                 (name, root, __VA_ARGS__)
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_UNORDERED_NOLOCK(name, algo, root, ...)
{
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH_UNORDERED)
                 (name, root, __VA_ARGS__)
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_REVERSE_NOLOCK(name, algo, root, ...)
{
  GPCT_CPP_CONCAT(GPCT_CONT_, algo, _FOREACH_REVERSE)
                 (name, root, __VA_ARGS__)
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH(name, algo, root, ...)
{
  GPCT_LOCK_RDLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_UNORDERED(name, algo, root, ...)
{
  GPCT_LOCK_RDLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_UNORDERED_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_REVERSE(name, algo, root, ...)
{
  GPCT_LOCK_RDLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_REVERSE_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_WRLOCK(name, algo, root, ...)
{
  GPCT_LOCK_WRLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_UNORDERED_WRLOCK(name, algo, root, ...)
{
  GPCT_LOCK_WRLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_UNORDERED_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */


/* backslash-region-begin */
#define CONTAINER_FOREACH_REVERSE_WRLOCK(name, algo, root, ...)
{
  GPCT_LOCK_WRLOCK(CONTAINER_LOCK_##name, &(root)->lock);
  CONTAINER_FOREACH_REVERSE_NOLOCK(name, algo, root, __VA_ARGS__)
  GPCT_LOCK_UNLOCK(CONTAINER_LOCK_##name, &(root)->lock);
}
/* backslash-region-end */

#endif

