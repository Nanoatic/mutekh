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

    This file defines common helper macros for pointer based containers.

*/

#ifndef GPCT_CONT_ENTRY_H_
#define GPCT_CONT_ENTRY_H_

#include <gpct/_platform.h>

/* "item from entry" and "entry from item" offset macros for pointer
   based containers */

#define GPCT_CONT_GET_ITEM(name, entry)                         \
 ((name##_item_t)(((uint8_t*)entry) - name##_gpct_offsetof))

#define GPCT_CONT_GET_NULLITEM(name, entry)                     \
 (entry != NULL ? (name##_item_t)(((uint8_t*)entry) - name##_gpct_offsetof) : NULL)

#define GPCT_CONT_GET_ENTRY(name, item)                         \
 ((name##_entry_t*)(((uint8_t*)item) + name##_gpct_offsetof))

/* item orphan status shortcuts */

#define GPCT_CONT_ORPHAN_SET_(algo, name, entry) GPCT_CPP_CONCAT(gpct_orphan_##algo##_, name, _set)(entry)
#define GPCT_CONT_ORPHAN_SET(algo, name, entry) GPCT_CONT_ORPHAN_SET_(algo, CONTAINER_ORPHAN_CHK_##name, entry)

#define GPCT_CONT_ORPHAN_CHK_(algo, name, entry) GPCT_CPP_CONCAT(gpct_orphan_##algo##_, name, _chk)(entry)
#define GPCT_CONT_ORPHAN_CHK(algo, name, entry) GPCT_CONT_ORPHAN_CHK_(algo, CONTAINER_ORPHAN_CHK_##name, entry)

#define GPCT_CONT_ORPHAN_EMPTY_FUNC(algo, name, prefix)                               \
GPCT_INTERNAL void gpct_orphan_##algo##_##prefix##_set(name##_entry_t *e) {}          \
GPCT_INTERNAL gpct_bool_t gpct_orphan_##algo##_##prefix##_chk(name##_entry_t *e) { return 1; }

/* container counter shortcuts */

#define GPCT_CONT_COUNTER_T_(name, prefix) GPCT_CPP_CONCAT(gpct_counter_##name##_, prefix, _t)
#define GPCT_CONT_COUNTER_T(name) GPCT_CONT_COUNTER_T_(name, CONTAINER_COUNTER_##name)

#define GPCT_CONT_COUNTER_ADD_(name, prefix, root, val) GPCT_CPP_CONCAT(gpct_counter_##name##_, prefix, _add)(root, val)
#define GPCT_CONT_COUNTER_ADD(name, root, val) GPCT_CONT_COUNTER_ADD_(name, CONTAINER_COUNTER_##name, root, val)

#define GPCT_CONT_COUNTER_SET_(name, prefix, root, val) GPCT_CPP_CONCAT(gpct_counter_##name##_, prefix, _set)(root, val)
#define GPCT_CONT_COUNTER_SET(name, root, val) GPCT_CONT_COUNTER_SET_(name, CONTAINER_COUNTER_##name, root, val)

#define GPCT_CONT_COUNTER_GET_(name, prefix, root) GPCT_CPP_CONCAT(gpct_counter_##name##_, prefix, _get)(root)
#define GPCT_CONT_COUNTER_GET(name, root) GPCT_CONT_COUNTER_GET_(name, CONTAINER_COUNTER_##name, root)

#define GPCT_CONT_COUNTER_ENABLED_(name, prefix) GPCT_CPP_CONCAT(gpct_counter_##name##_, prefix, _enabled)
#define GPCT_CONT_COUNTER_ENABLED(name) GPCT_CONT_COUNTER_ENABLED_(name, CONTAINER_COUNTER_##name)

/* backslash-region-begin */
#define GPCT_CONT_COUNTER_TYPE(name)

typedef gpct_empty_t gpct_counter_##name##_CONTAINER_COUNTER_##name##_t;
typedef size_t gpct_counter_##name##__t;

/* backslash-region-end */


/* backslash-region-begin */
#define GPCT_CONT_COUNTER_FUNC(name)

static const gpct_bool_t gpct_counter_##name##_CONTAINER_COUNTER_##name##_enabled = 0;
static const gpct_bool_t gpct_counter_##name##__enabled = 1;

GPCT_INTERNAL void
gpct_counter_##name##_CONTAINER_COUNTER_##name##_add(name##_root_t *root, ssize_t diff)
{
}

GPCT_INTERNAL void
gpct_counter_##name##__add(name##_root_t *root, ssize_t diff)
{
	const void *foo = ((const void*)&root->counter);
  *((size_t*)foo) += diff;
}

GPCT_INTERNAL void
gpct_counter_##name##_CONTAINER_COUNTER_##name##_set(name##_root_t *root, size_t val)
{
}

GPCT_INTERNAL void
gpct_counter_##name##__set(name##_root_t *root, size_t val)
{
	const void *foo = ((const void*)&root->counter);
	*((size_t*)foo) = val;
}

GPCT_INTERNAL size_t
gpct_counter_##name##_CONTAINER_COUNTER_##name##_get(name##_root_t *root)
{
  return 0;
}

GPCT_INTERNAL size_t
gpct_counter_##name##__get(name##_root_t *root)
{
	const void *foo = ((const void*)&root->counter);
	return *((const size_t*)foo);
}
/* backslash-region-end */


#endif

