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

#include <stdio.h>
#include <stdlib.h>

#include <capsule_api.h>
#include "array.h"

//#define VERBOSE

/*
  Common utility function
 */

static inline
void swap(elem_t *a, elem_t *b)
{
    elem_t tmp = *a;
    *a = *b;
    *b = tmp;
}

#if 0
/* partition of array [left,right[ */
static
ssize_t qs_partition(struct array_s *array, ssize_t left, ssize_t right)
{
    elem_t *data = array->array;
    const ssize_t oleft = left, oright = right;

    right--;

    if ( left >= right )
        return -1;

    ssize_t pivot_idx = left;
    elem_t pivot = data[pivot_idx];

#ifdef VERBOSE
    printf("\n%s: %p %d -> %d; pivot: %d\n", __FUNCTION__, array, oleft, oright, pivot);
#endif

    for (;;) {

#ifdef VERBOSE
        array_dump(array, left, right+1);
#endif
    
        while ( (data[left] <= pivot) && (left < oright) )
            left++;

        while ( (data[right] >= pivot) && (right > oleft) )
            right--;

#ifdef VERBOSE
        printf("%s: .......... %d >< %d | %d %d\n", __FUNCTION__, left, right, data[left], data[right]);
#endif

        if ( left >= right )
            break;

        swap(&data[left], &data[right]);

        left++;
        right--;
    }

#ifdef VERBOSE
    printf("%s: ---------- %d == %d\n", __FUNCTION__, left, right);
#endif

    if ( right <= oleft )
        return oleft+1;

#ifdef VERBOSE
    array_dump(array, right, pivot_idx+1);
#endif
        
    swap(&data[pivot_idx], &data[right]);

#ifdef VERBOSE
    array_dump(array, right, pivot_idx+1);
#endif

    return right;
}
#else

#define idx(array, x) ((x)-(array)->array)

/* partition of array [left,right[ */
static
ssize_t qs_partition(struct array_s *array, const ssize_t oleft, const ssize_t oright)
{
    elem_t *data = array->array;

    elem_t *data_left = &data[oleft];
    elem_t *data_right = &data[oright];
    elem_t * const data_oleft = &data[oleft];
    elem_t * const data_oright = &data[oright];

    data_right--;

    if ( data_left >= data_right )
        return -1;

    ssize_t pivot_idx = oleft;
    elem_t *data_pivot_idx = &data[pivot_idx];

    elem_t pivot = *data_pivot_idx;

#ifdef VERBOSE
    printf("\n%s: %p %d -> %d; pivot: %d\n", __FUNCTION__, array, oleft, oright, pivot);
#endif

    for (;;) {

#ifdef VERBOSE
        array_dump(array, idx(array, data_left), idx(array, data_right)+1);
#endif
    
        while ( (*data_left <= pivot) && (data_left < data_oright) )
            data_left++;

        while ( (*data_right >= pivot) && (data_right > data_oleft) )
            data_right--;

#ifdef VERBOSE
        printf("%s: .......... %d >< %d | %d %d\n", __FUNCTION__, idx(array, data_left), idx(array, data_right), *data_left, *data_right);
#endif

        if ( data_left >= data_right )
            break;

        swap(data_left, data_right);

        data_left++;
        data_right--;
    }

#ifdef VERBOSE
    printf("%s: ---------- %d == %d\n", __FUNCTION__, idx(array, data_left), idx(array, data_right));
#endif

    if ( data_right <= data_oleft )
        return oleft+1;

#ifdef VERBOSE
    array_dump(array, idx(array, data_right), pivot_idx+1);
#endif
        
    swap(data_pivot_idx, data_right);

#ifdef VERBOSE
    array_dump(array, idx(array, data_right), pivot_idx+1);
#endif

    return idx(array, data_right);
}

#endif

/*
  Capsule implementation
 */

#define CONTAINER_LOCK_qsort_param_queue HEXO_SPIN

typedef void qs_func_t(struct array_s *array, ssize_t left, ssize_t right);

struct qsort_param_s
{
    union {
        CONTAINER_ENTRY_TYPE(SLIST) list_entry;
        struct {
            qs_func_t *func;
            struct array_s *array;
            size_t left;
            size_t right;
        };
    };
};

CONTAINER_TYPE(qsort_param_queue, SLIST, struct qsort_param_s, list_entry);
CONTAINER_FUNC(qsort_param_queue, SLIST, static inline, qsort_param_queue, list_entry);

qsort_param_queue_root_t qsort_param_queue;

static inline struct qsort_param_s *qsort_param_alloc()
{
    struct qsort_param_s *r = qsort_param_queue_pop(&qsort_param_queue);
    assert(r);
    return r;
}

static
void qsort_param_release(struct qsort_param_s * elem)
{
    qsort_param_queue_push(&qsort_param_queue, elem);
}

void qsort_param_init()
{
    size_t i;
    qsort_param_queue_init(&qsort_param_queue);
    for ( i=0; i<100; ++i )
        qsort_param_release(malloc(sizeof(struct qsort_param_s)));
}

static
void capsule_wrapper(void * arg)
{
    struct qsort_param_s *param = arg;
    param->func(param->array, param->left, param->right);
    qsort_param_release(param);
}

static inline
void qs_probe_run(qs_func_t *func, struct array_s *array, ssize_t left, ssize_t right)
{
    if ( right - left < 8 ) {
        func(array, left, right);
        return;
    }

    capsule_ctxt_t *ctxt;
    capsule_probe(capsule_wrapper, &ctxt);

    if (ctxt) {
        struct qsort_param_s *param = qsort_param_alloc();
        param->func = func;
        param->array = array;
        param->left = left;
        param->right = right;
        capsule_divide(ctxt, param);
    } else {
        func(array, left, right);
    }
}

/*
  Capsule basic implementation
 */

static
void qs_capsule(struct array_s *array, ssize_t left, ssize_t right)
{
    ssize_t pivot = qs_partition(array, left, right);

    if ( pivot < 0 )
        return;

    qs_probe_run(qs_capsule, array, left, pivot);
    qs_capsule(array, pivot, right);
}

/*
  Capsule joining implementation
 */

static
void qs_capsule_grouped(struct array_s *array, ssize_t left, ssize_t right)
{
    ssize_t pivot = qs_partition(array, left, right);

    if ( pivot < 0 )
        return;

    capsule_group_split();
    qs_probe_run(qs_capsule_grouped, array, left, pivot);
    qs_capsule_grouped(array, pivot, right);
    capsule_group_join();
}

/*
 Sequential implementation
 */

static
void qs_seq(struct array_s *array, ssize_t left, ssize_t right)
{
    ssize_t pivot = qs_partition(array, left, right);

#ifdef VERBOSE
    printf("%s: %p %d -> %d, pivot = %d\n", __FUNCTION__, array, left, right, pivot);
#endif

    if ( pivot < 0 )
        return;

    qs_seq(array, left, pivot);
    qs_seq(array, pivot, right);
}

/*
  Entry points
 */

void do_capsule(struct array_s *array)
{
    qs_capsule(array, 0, array->size);
    capsule_group_join();
}

void do_capsule_grouped(struct array_s *array)
{
    qs_capsule_grouped(array, 0, array->size);
    capsule_group_join();
}

void do_seq(struct array_s *array)
{
    qs_seq(array, 0, array->size);
}
