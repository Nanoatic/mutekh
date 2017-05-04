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

    Default array_push*, array_pop* implementation functions

*/

#ifndef GPCT_PUSHPOP_ARRAY_H_
#define GPCT_PUSHPOP_ARRAY_H_

/* backslash-region-begin */
#define GPCT_CONT_PUSHPOP_ARRAY_FUNC(attr, name, prefix,
                                     prefixback, lockname)

attr GPCT_CONT_PROTO_PUSH_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    prefix##_push(root, item[i]);

  return size;
}

attr GPCT_CONT_PROTO_POP_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    item[i] = prefix##_pop(root);

  return size;
}

attr GPCT_CONT_PROTO_PUSHBACK_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    prefixback##_pushback(root, item[i]);

  return size;
}

attr GPCT_CONT_PROTO_POPBACK_ARRAY(name, prefix)
{
  uintptr_t     i;

  for (i = 0; i < size; i++)
    item[i] = prefixback##_popback(root);

  return size;
}
/* backslash-region-end */


#endif

