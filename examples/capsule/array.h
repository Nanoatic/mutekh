/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright (c) 2010, Nicolas Pouillon <nipo@ssji.net>
*/

#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdint.h>

typedef uint32_t elem_t;

/*
  Array tools
 */

struct array_s {
    size_t size;
    elem_t sum;
    elem_t *array;
};

error_t array_check_sum(struct array_s *array);

ssize_t array_first_unordered(struct array_s *array);

void array_copy(struct array_s *dst, const struct array_s *src);

bool_t array_cmp(const struct array_s *a, const struct array_s *b);

void array_create(struct array_s *array, size_t size, elem_t seed);

void array_dump(struct array_s * array, size_t min, size_t max);

#endif
