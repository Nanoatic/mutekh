/* Test and benchmark of a couple of parallel sorting algorithms.
   Copyright (C) 2008 Free Software Foundation, Inc.

   GCC is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   GCC is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include <limits.h>
#include <omp.h>
#include <stdbool.h>
#include <mutek/printk.h>
#include <stdlib.h>
#include <string.h>

int failures;

#define THRESHOLD 100

static void
verify (const char *name, double stime, int *array, int count)
{
  int i;
  double etime = omp_get_wtime ();

  printk ("%s: time=%g\n", name, etime - stime);
  for (i = 1; i < count; i++)
    if (array[i] < array[i - 1])
      {
	printk ("%s: incorrectly sorted\n", name);
	failures = 1;
      }

  if (!failures)
    printk ("%s: correctly sorted\n", name);
}

static void
insertsort (int *array, int s, int e)
{
  int i, j, val;
  for (i = s + 1; i <= e; i++)
    {
      val = array[i];
      j = i;
      while (j-- > s && val < array[j])
	array[j + 1] = array[j];
      array[j + 1] = val;
    }
}

struct int_pair
{
  int lo;
  int hi;
};

struct int_pair_stack
{
  struct int_pair *top;
#define STACK_SIZE 4 * CHAR_BIT * sizeof (int)
  struct int_pair arr[STACK_SIZE];
};

static inline void
init_int_pair_stack (struct int_pair_stack *stack)
{
  stack->top = &stack->arr[0];
}

static inline void
push_int_pair_stack (struct int_pair_stack *stack, int lo, int hi)
{
  stack->top->lo = lo;
  stack->top->hi = hi;
  stack->top++;
}

static inline void
pop_int_pair_stack (struct int_pair_stack *stack, int *lo, int *hi)
{
  stack->top--;
  *lo = stack->top->lo;
  *hi = stack->top->hi;
}

static inline int
size_int_pair_stack (struct int_pair_stack *stack)
{
  return stack->top - &stack->arr[0];
}

static inline void
swap (int *array, int a, int b)
{
  int val = array[a];
  array[a] = array[b];
  array[b] = val;
}

static inline int
choose_pivot (int *array, int lo, int hi)
{
  int mid = (lo + hi) / 2;

  if (array[mid] < array[lo])
    swap (array, lo, mid);
  if (array[hi] < array[mid])
    {
      swap (array, mid, hi);
      if (array[mid] < array[lo])
	swap (array, lo, mid);
    }
  return array[mid];
}

static inline int
partition (int *array, int lo, int hi)
{
  int pivot = choose_pivot (array, lo, hi);
  int left = lo;
  int right = hi;

  for (;;)
    {
      while (array[++left] < pivot);
      while (array[--right] > pivot);
      if (left >= right)
	break;
      swap (array, left, right);
    }
  return left;
}

static void
sort1 (int *array, int count)
{
  omp_lock_t lock;
  struct int_pair_stack global_stack;
  int busy = 1;
  int num_threads;

  omp_init_lock (&lock);
  init_int_pair_stack (&global_stack);
  #pragma omp parallel firstprivate (array, count)
  {
    int lo = 0, hi = 0, mid, next_lo, next_hi;
    bool idle = true;
    struct int_pair_stack local_stack;

    init_int_pair_stack (&local_stack);
    if (omp_get_thread_num () == 0)
      {
	num_threads = omp_get_num_threads ();
	hi = count - 1;
	idle = false;
      }

    for (;;)
      {
	if (hi - lo < THRESHOLD)
	  {
	    insertsort (array, lo, hi);
	    lo = hi;
	  }
	if (lo >= hi)
	  {
	    if (size_int_pair_stack (&local_stack) == 0)
	      {
	      again:
		omp_set_lock (&lock);
		if (size_int_pair_stack (&global_stack) == 0)
		  {
		    if (!idle)
		      busy--;
		    if (busy == 0)
		      {
			omp_unset_lock (&lock);
			break;
		      }
		    omp_unset_lock (&lock);
		    idle = true;
		    while (size_int_pair_stack (&global_stack) == 0
			   && busy)
		      cpu_interrupt_process ();
		    goto again;
		  }
		if (idle)
		  busy++;
		pop_int_pair_stack (&global_stack, &lo, &hi);
		omp_unset_lock (&lock);
		idle = false;
	      }
	    else
	      pop_int_pair_stack (&local_stack, &lo, &hi);
	  }

	mid = partition (array, lo, hi);
	if (mid - lo < hi - mid)
	  {
	    next_lo = mid;
	    next_hi = hi;
	    hi = mid - 1;
	  }
	else
	  {
	    next_lo = lo;
	    next_hi = mid - 1;
	    lo = mid;
	  }

	if (next_hi - next_lo < THRESHOLD)
	  insertsort (array, next_lo, next_hi);
	else
	  {
	    if (size_int_pair_stack (&global_stack) < num_threads - 1)
	      {
		int size;

		omp_set_lock (&lock);
		size = size_int_pair_stack (&global_stack);
		if (size < num_threads - 1 && size < STACK_SIZE)
		  push_int_pair_stack (&global_stack, next_lo, next_hi);
		else
		  push_int_pair_stack (&local_stack, next_lo, next_hi);
		omp_unset_lock (&lock);
	      }
	    else
	      push_int_pair_stack (&local_stack, next_lo, next_hi);
	  }
      }
    }
  omp_destroy_lock (&lock);
}

int
main (int argc, char **argv)
{
  int i, count = 100000;
  double stime;
  int *unsorted, *sorted, num_threads;

  if (argc >= 2)
    count = strtoul (argv[1], NULL, 0);

  unsorted = malloc (count * sizeof (int));
  sorted = malloc (count * sizeof (int));
  if (unsorted == NULL || sorted == NULL)
    {
      printk ("allocation failure");
      exit (1);
    }

  srand (0xdeadbeef);
  for (i = 0; i < count; i++)
    unsorted[i] = rand ();

  omp_set_nested (1);
  omp_set_dynamic (0);
  #pragma omp parallel
    #pragma omp single nowait
      num_threads = omp_get_num_threads ();
  printk ("Threads: %d\n", num_threads);

  memcpy (sorted, unsorted, count * sizeof (int));
  stime = omp_get_wtime ();
  sort1 (sorted, count);
  verify ("sort", stime, sorted, count);

  printk("Done.\n");

  return 0;
}
