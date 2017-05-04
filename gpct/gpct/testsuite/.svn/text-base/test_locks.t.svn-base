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

*/

#include "testsuite.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

m4_changecom
#include <INCLUDE>

typedef GPCT_LOCK_TYPE(TESTLOCK) lock_t;

#define THREAD_COUNT 4

#if LOCK_FEATURE(TESTLOCK, STATIC_INIT)
static lock_t slock = GPCT_LOCK_INITIALIZER(TESTLOCK);
#endif

lock_t lock;

volatile int done = 0;
volatile int state[THREAD_COUNT] = { 0 };
pthread_t threads[THREAD_COUNT];
pthread_barrier_t barrier;

int err = 0;

void *thread_func(void *id_)
{
  long id = (long)id_;
  int i, wrcount = 0, rdcount = 0, rdlcount = 0, wrlcount = 0;
  pthread_barrier_wait(&barrier);

  while (!done)
    {
      switch (rand() % 10)
	{
	case 0:			/* writelock */
	  GPCT_LOCK_WRLOCK(TESTLOCK, &lock);
	  state[id]++;
	  usleep(rand() % 100);
	  wrcount++;
	  for (i = 0; i < THREAD_COUNT; i++)
	    {
	      wrlcount += state[i];
	      err += id != i && state[i];
	    }
	  usleep(rand() % 100);
	  state[id]--;
	  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);
	  usleep(rand() % 100);
	  break;

	case 1:			/* writelock with delay */
	  GPCT_LOCK_WRLOCK(TESTLOCK, &lock);
	  state[id]++;
	  wrcount++;
	  for (i = 0; i < THREAD_COUNT; i++)
	    {
	      wrlcount += state[i];
	      err += id != i && state[i];
	    }
	  state[id]--;
	  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);
	  break;

	case 2:
	case 3:
	case 4:
	case 5:			/* readlock */
	  GPCT_LOCK_RDLOCK(TESTLOCK, &lock);
	  state[id]++;
	  rdlcount++;
	  for (i = 0; i < THREAD_COUNT; i++)
	    rdcount += state[i];
	  state[id]--;
	  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);
	  break;

	case 6:
	case 7:
	case 8:
	case 9:			/* readlock with delay */
	  GPCT_LOCK_RDLOCK(TESTLOCK, &lock);
	  state[id]++;
	  usleep(rand() % 100);
	  rdlcount++;
	  for (i = 0; i < THREAD_COUNT; i++)
	    rdcount += state[i];
	  usleep(rand() % 100);
	  state[id]--;
	  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);
	  usleep(rand() % 100);
	  break;
	}
    }

  GPCT_LOCK_WRLOCK(TESTLOCK, &lock);
  printf("thread: %i, wrlock count: %i/%i, rdlock count: %i/%i\n",
	 id, wrlcount, wrcount, rdlcount, rdcount);
  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);

  err += wrcount != wrlcount;
#if !LOCK_FEATURE(TESTLOCK, READ_WRITE)
  err += rdcount != rdlcount;
#endif

  return NULL;
}

int main()
{
  int i, delay;

#if LOCK_FEATURE(TESTLOCK, STATIC_INIT)
  puts("Statically initialized lock test (" NAME_STR(TESTLOCK) ")");

  GPCT_LOCK_WRLOCK(TESTLOCK, &slock);
  GPCT_LOCK_UNLOCK(TESTLOCK, &slock);

  GPCT_LOCK_RDLOCK(TESTLOCK, &slock);
  GPCT_LOCK_UNLOCK(TESTLOCK, &slock);
#endif

  puts("Dynamically initialized lock test (" NAME_STR(TESTLOCK) ")");

  GPCT_LOCK_INIT(TESTLOCK, &lock);

  GPCT_LOCK_WRLOCK(TESTLOCK, &lock);
  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);

  GPCT_LOCK_RDLOCK(TESTLOCK, &lock);
  GPCT_LOCK_UNLOCK(TESTLOCK, &lock);

  {
    const char *env = getenv("GPCT_TEST_LOCK_DELAY");
    delay = env ? atoi(env) : 1;
  }

  pthread_barrier_init(&barrier, NULL, THREAD_COUNT + 1);
  for (i = 0; i < THREAD_COUNT; i++)
    pthread_create(threads + i, NULL, thread_func, (void*)i);

  pthread_barrier_wait(&barrier);
  sleep(delay);
  done = 1;

  for (i = 0; i < THREAD_COUNT; i++)
    pthread_join(threads[i], NULL);
  pthread_barrier_destroy(&barrier);

  GPCT_LOCK_DESTROY(TESTLOCK, &lock);

  assert(!err);

  return 0;
}

