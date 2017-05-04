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

#include <stdlib.h>
#include <capsule_api.h>

#include "qsort_capsule.h"
#include "qsort_libc.h"
#include "array.h"

typedef void test_func_t(struct array_s *array);

static
uint32_t test(
    size_t loops,
    test_func_t *func,
    const char *mode,
    struct array_s *array,
    const struct array_s *base_array);

struct test_s {
    const char *name;
    test_func_t *func;
    bool_t is_capsule;
    bool_t is_blocked;
};

/*
  Bench selection
 */

const struct test_s tests[] = {
    { "Capsule normal ", do_capsule,         1, 0 },
    { "Capsule grouped", do_capsule_grouped, 1, 0 },
    { "Capsule blocked", do_capsule,         1, 1 },
    { "Sequential     ", do_seq,             0, 0 },
    { "Libc           ", do_libc,            0, 0 },
};

const size_t tests_count = sizeof(tests)/sizeof(*tests);

/*
  Main bench
 */

int_fast8_t main(int_fast8_t argc, char ** argv)
{
    struct array_s base_array, array;

    size_t size = 7000000;
    elem_t seed = 27758;
    size_t loops = 1000;

    if ((argc != 1) && (argc != 4))
    {
        fprintf(stderr, "Usage: %s [elems_nb iter_nb random_seed].\n", argv[0]);
        abort();
    }

    if (argc == 4)
    {
        size =  atoi(argv[1]);
        loops = atoi(argv[2]);
        seed =  atoi(argv[3]);
    }

    printf("CAPSULE quicksort test and benchmark.\n\n");
    printf("Tests are repeated %u times.\n", loops);
    printf("Random seed: %i.\n", seed);
    printf("\n");
 
    // Capsule runtime initialization
    capsule_sys_init_warmup();
    qsort_param_init();
    
    printf("Nb of elements: %d, init of array...\n", size);

    // Allocations and array preparation
    array_create(&base_array, size, seed);
    array_create(&array, size, 0);

    size_t i;
    uint32_t times[tests_count];

    for ( i=0; i<tests_count; ++i ) {
        const struct test_s *t = tests+i;
        if ( t->is_blocked )
            capsule_sys_block();

        times[i] = test(loops,
                        t->func,
                        t->name,
                        &array, &base_array);

        if ( t->is_capsule ) {
            capsule_sys_dump_all_stats(stdout);
            capsule_sys_reset_all_stats();
        }
        if ( t->is_blocked )
            capsule_sys_unblock();
    }

    printf("\n");
    for ( i=0; i<tests_count; ++i ) {
        const struct test_s *t = tests+i;
        printf("%s time: %d\n", t->name, times[i]);
    }
    printf("\n");

//    printf("1->n cpu: %d %%.\n", (seq_time-normal_time) * 100 / seq_time);
    
    return 0;
}

#if defined(__MUTEK__)
/*
  As we have no command line to launch the tests, but we want to be
  able to run them without modification, if we are in MutekH,
  libcapsule implementation calls run_main() and user has to call
  main() with needed arguments.
 */

void run_main()
{
    char *argv[] = {"app", "10000", "2", "12345", NULL};
    main(sizeof(argv)/sizeof(char*)-1, argv);
}
#endif

/*
  Common test loop
 */

uint32_t test(size_t loops, test_func_t *func, const char *mode, struct array_s *array, const struct array_s *base_array)
{
    size_t i;
    uint32_t begin, end, sum = 0;

    for (i = 0; i < loops; ++i)
    {
        printf("%s iter = %d... ", mode, i);
        array_copy(array, base_array);

        begin = cpu_cycle_count();
	 
        func(array);
        capsule_group_join();

        end = cpu_cycle_count();

        ssize_t index = array_first_unordered(array);
        if ( index >= 0 ) {
            printf("Error: array unordered at %d:\n", index);
            printf("array[%d] = %d\n", index, array->array[index]);
            printf("array[%d] = %d\n", index+1, array->array[index+1]);
            abort();
        }
        
        printf("%d cycles\n", end-begin);
        sum += (end-begin) / loops;
    }
    return sum;
}
