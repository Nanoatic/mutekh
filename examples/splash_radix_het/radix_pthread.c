
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/*  Integer radix sort of non-negative integers.                         */
/*                                                                       */
/*  Command line options:                                                */
/*                                                                       */
/*  -pP : P = number of processors.                                      */
/*  -rR : R = radix for sorting.  Must be power of 2.                    */
/*  -nN : N = number of keys to sort.                                    */
/*  -mM : M = maximum key value.  Integer keys k will be generated such  */
/*        that 0 <= k <= M.                                              */
/*  -s  : Print individual processor timing statistics.                  */
/*  -t  : Check to make sure all keys are sorted correctly.              */
/*  -o  : Print out sorted keys.                                         */
/*  -h  : Print out command line options.                                */
/*                                                                       */
/*  Default: RADIX -p1 -n262144 -r1024 -m524288                          */
/*                                                                       */
/*  Note: This version works under both the FORK and SPROC models        */
/*                                                                       */
/*************************************************************************/

#include <mutek/printk.h>
//#include <stdio.h>
//#include <math.h>
#include <unistd.h>

#define DEFAULT_P                    4
#define DEFAULT_N               64//262144
#define DEFAULT_R                   16
#define DEFAULT_M               524288
#define MAX_PROCESSORS              4    
#define RADIX_S                8388608
#define RADIX           70368744177664
#define SEED                 314159265
#define RATIO               1220703125
#define PAGE_SIZE                 4096
#define PAGE_MASK     (~(PAGE_SIZE-1))
#define MAX_RADIX                 4096


#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
//#include <malloc.h>
#define MAX_THREADS 32
pthread_t PThreadTable[MAX_THREADS];


struct prefix_node {
  int densities[MAX_RADIX];
  int ranks[MAX_RADIX];
   
  struct {
    pthread_mutex_t	Mutex;
    pthread_cond_t	CondVar;
    unsigned int	Flag;
  } done;

  char pad[PAGE_SIZE];
};

struct global_memory {
  int Index;                             /* process ID */
  pthread_mutex_t (lock_Index);                    /* for fetch and add to get ID */
  pthread_mutex_t (rank_lock);                     /* for fetch and add to get ID */
  /*   pthread_mutex_t section_lock[MAX_PROCESSORS];*/  /* key locks */
   
  pthread_barrier_t	(barrier_rank);
  /* for ranking process */
   
  pthread_barrier_t	(barrier_key);
  /* for key sorting process */
  int final;
  unsigned int rs;
  unsigned int rf;
  struct prefix_node prefix_tree[2 * MAX_PROCESSORS];
} *global;

struct global_private {
  char pad[PAGE_SIZE];
  int *rank_ff;         /* overall processor ranks */
} gp[MAX_PROCESSORS];

int *key[2];            /* sort from one index into the other */
int **rank_me;          /* individual processor ranks */
int *key_partition;     /* keys a processor works on */
int *rank_partition;    /* ranks a processor works on */

int max_num_digits;
int radix = DEFAULT_R;
int num_keys = DEFAULT_N;
int max_key = DEFAULT_M;
int log2_radix;
int log2_keys;
int dostats = 0;
int test_result = 1;
int doprint = 0;

static void slave_sort(void);
static int get_max_digits(int max_key);
static int log_2(int number);
static void test_sort(int final);
static void printout(void);
static volatile int start = 0;

void app_start()
{
  int i;
  int p;
  int quotient;
  int remainder;
  int sum_i; 
  int sum_f;
  int size;
  int **temp;
  int **temp2;
  int *a;

  if (cpu_id() >= MAX_PROCESSORS)
    return;

  if (cpu_id() == 0)
    {
      log2_radix = log_2(radix); 
      log2_keys = log_2(num_keys);
      global = (struct global_memory *) malloc(sizeof(struct global_memory));;
      if (global == NULL) {
	printk("ERROR: Cannot %d malloc enough memory for global \n", sizeof(struct global_memory));
	abort();
      }
      key[0] = (int *) malloc(num_keys*sizeof(int));;
      key[1] = (int *) malloc(num_keys*sizeof(int));;
      key_partition = (int *) malloc((MAX_PROCESSORS+1)*sizeof(int));;
      rank_partition = (int *) malloc((MAX_PROCESSORS+1)*sizeof(int));;
      size = MAX_PROCESSORS*(radix*sizeof(int)+sizeof(int *));
      rank_me = (int **) malloc(size);;
      if ((key[0] == NULL) || (key[1] == NULL) || (key_partition == NULL)
	  || (rank_partition == NULL) || (rank_me == NULL)) {
	printk("ERROR: Cannot malloc enough memory\n");
	abort(); 
      }

      for (i=0; i < num_keys; i++)
	key[0][i] = rand() % DEFAULT_M;

      temp = rank_me;
      temp2 = temp + MAX_PROCESSORS;
      a = (int *) temp2;
      for (i=0;i<MAX_PROCESSORS;i++) {
	*temp = (int *) a;
	temp++;
	a += radix;
      }

      for (i=0;i<MAX_PROCESSORS;i++)
	gp[i].rank_ff = (int *) malloc(radix*sizeof(int)+PAGE_SIZE);;

      pthread_mutex_init(&(global->lock_Index), NULL);
      pthread_mutex_init(&(global->rank_lock), NULL);

      pthread_barrier_init(&(global->barrier_rank), NULL, MAX_PROCESSORS);
      pthread_barrier_init(&(global->barrier_key), NULL, MAX_PROCESSORS);

      for (i=0; i<2*MAX_PROCESSORS; i++) {
	pthread_mutex_init(&global->prefix_tree[i].done.Mutex, NULL);
	pthread_cond_init(&global->prefix_tree[i].done.CondVar, NULL);
	global->prefix_tree[i].done.Flag = 0;
      }

      global->Index = 0;
      global->final = 0;
      max_num_digits = get_max_digits(max_key);
      printk("\n");
      printk("Integer Radix Sort\n");
      printk("     %d Keys\n",num_keys);
      printk("     %d Processors\n",MAX_PROCESSORS);
      printk("     Radix = %d\n",radix);
      printk("     Max key = %d\n",max_key);
      printk("\n");

      quotient = num_keys / MAX_PROCESSORS;
      remainder = num_keys % MAX_PROCESSORS;
      sum_i = 0;
      sum_f = 0;
      p = 0;

      while (sum_i < num_keys) {
	key_partition[p] = sum_i;
	p++;
	sum_i = sum_i + quotient;
	sum_f = sum_f + remainder;
	sum_i = sum_i + sum_f / MAX_PROCESSORS;
	sum_f = sum_f % MAX_PROCESSORS;
      }
      key_partition[p] = num_keys;

      quotient = radix / MAX_PROCESSORS;
      remainder = radix % MAX_PROCESSORS;
      sum_i = 0;
      sum_f = 0;
      p = 0;

      while (sum_i < radix) {
	rank_partition[p] = sum_i;
	p++;
	sum_i = sum_i + quotient;
	sum_f = sum_f + remainder;
	sum_i = sum_i + sum_f / MAX_PROCESSORS;
	sum_f = sum_f % MAX_PROCESSORS;
      }
      rank_partition[p] = radix;

      printout();
      start = 1;
    }

  /* every processor execute the app_start function due to CONFIG_MUTEK_SMP_APP_START */
  pthread_create(&PThreadTable[cpu_id()], NULL, (void * (*)(void *))(slave_sort), NULL);
}

static void slave_sort()
{
  int i;
  int MyNum;
  int this_key;
  int tmp;
  int loopnum;
  int shiftnum;
  int bb;
  int my_key;
  int key_start;
  int key_stop;
  int rank_start;
  int rank_stop;
  int from=0;
  int to=1;
  int *key_density;       /* individual processor key densities */
  int *key_from;
  int *key_to;
  int *rank_me_mynum;
  int *rank_ff_mynum;
  int stats;
  struct prefix_node* n;
  struct prefix_node* r;
  struct prefix_node* l;
  struct prefix_node* my_node;
  struct prefix_node* their_node;
  int index;
  int level;
  int base;
  int offset;

  while (!start)
    ;

  stats = dostats;

  pthread_mutex_lock(&(global->lock_Index));
  printk("Thread started on cpu %i (%s)"
#ifdef CONFIG_MUTEK_SCHEDULER_MIGRATION
	 " but threads migration is enabled"
#endif
	 "\n", cpu_id(), cpu_type_name());
  MyNum = global->Index;
  global->Index++;
  pthread_mutex_unlock(&(global->lock_Index));

  /* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
     processors to avoid migration */

  key_density = (int *) malloc(radix*sizeof(int));;

  /* Fill the random-number array. */

  key_start = key_partition[MyNum];
  key_stop = key_partition[MyNum + 1];
  rank_start = rank_partition[MyNum];
  rank_stop = rank_partition[MyNum + 1];

  if (rank_stop == radix)
    rank_stop--;

  pthread_barrier_wait(&(global->barrier_key));

  /* POSSIBLE ENHANCEMENT:  Here is where one might reset the
     statistics that one is measuring about the parallel execution */

  pthread_barrier_wait(&(global->barrier_key));

  /* Do 1 iteration per digit.  */

  rank_me_mynum = rank_me[MyNum];
  rank_ff_mynum = gp[MyNum].rank_ff;

  for (loopnum=0;loopnum<max_num_digits;loopnum++) {
    shiftnum = (loopnum * log2_radix);
    bb = (radix-1) << shiftnum;

    /* generate histograms based on one digit */

    for (i = 0; i < radix; i++)
      rank_me_mynum[i] = 0;

    key_from = (int *) key[from];
    key_to = (int *) key[to];

    for (i=key_start;i<key_stop;i++) {
      my_key = key_from[i] & bb;
      my_key = my_key >> shiftnum;  
      rank_me_mynum[my_key]++;
    }

    key_density[0] = rank_me_mynum[0]; 

    for (i=1;i<radix;i++) {
      key_density[i] = key_density[i-1] + rank_me_mynum[i];  
    }

    pthread_barrier_wait(&(global->barrier_rank));

    n = &(global->prefix_tree[MyNum]);
    for (i = 0; i < radix; i++) {
      n->densities[i] = key_density[i];
      n->ranks[i] = rank_me_mynum[i];
    }

    offset = MyNum;
    level = MAX_PROCESSORS >> 1;
    base = MAX_PROCESSORS;

    if ((MyNum & 0x1) == 0) {
      {
	pthread_mutex_lock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);
	global->prefix_tree[base + (offset >> 1)].done.Flag = 1;
	pthread_cond_broadcast(&global->prefix_tree[base + (offset >> 1)].done.CondVar);
	pthread_mutex_unlock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);}
      ;
    }

    while ((offset & 0x1) != 0) {
      offset >>= 1;
      r = n;
      l = n - 1;
      index = base + offset;
      n = &(global->prefix_tree[index]);

      pthread_mutex_lock(&n->done.Mutex);

      if (n->done.Flag == 0) {
	pthread_cond_wait(&n->done.CondVar, &n->done.Mutex);
      }
      n->done.Flag = 0;

      pthread_mutex_unlock(&n->done.Mutex);

      if (offset != (level - 1)) {
	for (i = 0; i < radix; i++) {
	  n->densities[i] = r->densities[i] + l->densities[i];
	  n->ranks[i] = r->ranks[i] + l->ranks[i];
	}
      } else {
	for (i = 0; i < radix; i++) {
	  n->densities[i] = r->densities[i] + l->densities[i];
	}
      }

      base += level;
      level >>= 1;

      if ((offset & 0x1) == 0) {
	  pthread_mutex_lock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);
	  global->prefix_tree[base + (offset >> 1)].done.Flag = 1;
	  pthread_cond_broadcast(&global->prefix_tree[base + (offset >> 1)].done.CondVar);
	  pthread_mutex_unlock(&global->prefix_tree[base + (offset >> 1)].done.Mutex);
      }

    }

    pthread_barrier_wait(&(global->barrier_rank));

    if (MyNum != (MAX_PROCESSORS - 1)) {
      offset = MyNum;
      level = MAX_PROCESSORS;
      base = 0;
      while ((offset & 0x1) != 0) {
	offset >>= 1;
	base += level;
	level >>= 1;
      }
      my_node = &(global->prefix_tree[base + offset]);
      offset >>= 1;
      base += level;
      level >>= 1;
      while ((offset & 0x1) != 0) {
	offset >>= 1;
	base += level;
	level >>= 1;
      }
      their_node = &(global->prefix_tree[base + offset]);

      pthread_mutex_lock(&my_node->done.Mutex);
      if (my_node->done.Flag == 0) {
	pthread_cond_wait(&my_node->done.CondVar, &my_node->done.Mutex);
      }

      my_node->done.Flag = 0;
      pthread_mutex_unlock(&my_node->done.Mutex);

      for (i = 0; i < radix; i++) {
	my_node->densities[i] = their_node->densities[i];
      }
    } else {
      my_node = &(global->prefix_tree[(2 * MAX_PROCESSORS) - 2]);
    }

    offset = MyNum;
    level = MAX_PROCESSORS;
    base = 0;

    while ((offset & 0x1) != 0) {
      {
	pthread_mutex_lock(&global->prefix_tree[base + offset - 1].done.Mutex);
	global->prefix_tree[base + offset - 1].done.Flag = 1;
	pthread_cond_broadcast(&global->prefix_tree[base + offset - 1].done.CondVar);
	pthread_mutex_unlock(&global->prefix_tree[base + offset - 1].done.Mutex);}
      ;
      offset >>= 1;
      base += level;
      level >>= 1;
    }

    offset = MyNum;
    level = MAX_PROCESSORS;
    base = 0;

    for(i = 0; i < radix; i++) {
      rank_ff_mynum[i] = 0;
    }

    while (offset != 0) {
      if ((offset & 0x1) != 0) {
	/* Add ranks of node to your left at this level */
	l = &(global->prefix_tree[base + offset - 1]);
	for (i = 0; i < radix; i++) {
	  rank_ff_mynum[i] += l->ranks[i];
	}
      }
      base += level;
      level >>= 1;
      offset >>= 1;
    }
    for (i = 1; i < radix; i++) {
      rank_ff_mynum[i] += my_node->densities[i - 1];
    }

    pthread_barrier_wait(&(global->barrier_rank));

    /* put it in order according to this digit */

    for (i = key_start; i < key_stop; i++) {  
      this_key = key_from[i] & bb;
      this_key = this_key >> shiftnum;  
      tmp = rank_ff_mynum[this_key];
      key_to[tmp] = key_from[i];
      rank_ff_mynum[this_key]++;
    }   /*  i */  

    if (loopnum != max_num_digits-1) {
      from = from ^ 0x1;
      to = to ^ 0x1;
    }

    pthread_barrier_wait(&(global->barrier_rank));

  } /* for */

  pthread_barrier_wait(&(global->barrier_rank));

  if (MyNum == 0) {
    printout();
    test_sort(global->final);  
  }

}

static int get_max_digits(int max_key)
{
  int done = 0;
  int temp = 1;
  int key_val;

  key_val = max_key;
  while (!done) {
    key_val = key_val / radix;
    if (key_val == 0) {
      done = 1;
    } else {
      temp ++;
    }
  }
  return temp;
}

static int log_2(int number)
{
  int cumulative = 1;
  int out = 0;
  int done = 0;

  while ((cumulative < number) && (!done) && (out < 50)) {
    if (cumulative == number) {
      done = 1;
    } else {
      cumulative = cumulative * 2;
      out ++;
    }
  }

  if (cumulative == number) {
    return(out);
  } else {
    return(-1);
  }
}

static void test_sort(int final)
{
  int i;
  int mistake = 0;
  int *key_final;

  printk("\n");
  printk("                  TESTING RESULTS\n");
  key_final = key[final];
  for (i = 0; i < num_keys-1; i++) {
    if (key_final[i] > key_final[i + 1]) {
      printk("error with key %d, value %ld %d \n",
	     i,key_final[i],key_final[i + 1]);
      mistake++;
    }
  }

  if (mistake) {
    printk("FAILED: %d keys out of place.\n", mistake);
  } else {
    printk("PASSED: All keys in place.\n");
  }
  printk("\n");
}

static void printout()
{
  int i;
  int *key_final;

  key_final = (int *) key[global->final];
  printk("\n");
  printk("                 SORTED KEY VALUES\n");
  printk("%8d ",key_final[0]);
  for (i = 0; i < num_keys-1; i++) {
    printk("%8d ",key_final[i+1]);
    if ((i+2) % 5 == 0) {
      printk("\n");
    }
  }
  printk("\n");
}

