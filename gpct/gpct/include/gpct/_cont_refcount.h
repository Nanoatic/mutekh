/*

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

    Container item refcount stuff

*/

#ifndef GPCT_CONT_REFCOUNT_H_
#define GPCT_CONT_REFCOUNT_H_

#define GPCT_CONT_REFNEW_(objprefix, item) GPCT_CPP_CONCAT2(objprefix, _refnew)(item)
#define GPCT_CONT_REFNEW(name, item) GPCT_CONT_REFNEW_(CONTAINER_OBJ_##name, item)

#define GPCT_CONT_REFDROP_(objprefix, item) GPCT_CPP_CONCAT2(objprefix, _refdrop)(item)
#define GPCT_CONT_REFDROP(name, item) GPCT_CONT_REFDROP_(CONTAINER_OBJ_##name, item)

#define GPCT_CONT_OBJECT_EMPTY_FUNC(name, prefix)                                \
GPCT_INTERNAL name##_item_t prefix##_refnew(name##_item_t item) { return item; } \
GPCT_INTERNAL void prefix##_refdrop(name##_item_t item) {}

#endif

