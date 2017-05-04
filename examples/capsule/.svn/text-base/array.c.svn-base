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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "array.h"

error_t array_check_sum(struct array_s *array)
{
    size_t i;
    elem_t sum = 0;

    for ( i=0; i<array->size; ++i )
        sum += array->array[i];

    return (array->sum == sum) ? 0 : -EINVAL;
}

ssize_t array_first_unordered(struct array_s *array)
{
    size_t i;

    for ( i=1; i<array->size; ++i )
        if (array->array[i-1] > array->array[i])
            return i-1;

    return -1;
}

void array_copy(struct array_s *dst, const struct array_s *src)
{
    assert(dst->size == src->size);

    memcpy(dst->array, src->array, sizeof(elem_t) * src->size);
    dst->sum = src->sum;
}

bool_t array_cmp(const struct array_s *a, const struct array_s *b)
{
    if (a->size != b->size)
        return 1;

    return memcmp(a->array, b->array, sizeof(elem_t) * a->size);
}

void array_create(struct array_s *array, size_t size, elem_t seed)
{
    size_t i;
    elem_t sum = 0;

    array->array = malloc(sizeof(elem_t) * size);
    array->size = size;

    if ( seed ) {
        srand(seed);
        for ( i=0; i<size; ++i ) {
            elem_t r = rand();
            array->array[i] = r;
            sum += r;
        }

        array->sum = sum;
    }
}

void array_dump(struct array_s * array, size_t min, size_t max)
{
    size_t i;
    if ( !max )
        max = array->size;

    printf("array[%d:%d] =", min, max);
    for ( i=0; i<array->size; ++i ) {
        printf(" ");
        if ( min == i )
            printf("[");
        printf("%d", array->array[i]);
        if ( max-1 == i )
            printf("]");
    }
    printf("\n");
}
