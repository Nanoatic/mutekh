
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <mutek/printk.h>
#include <hexo/lock.h>
#include <crypto/crc32.h>

#define POOL_SIZE 256		/* number of max different block allocated at the same time */
#define MAX_SIZE 16384		/* maximum single malloc size */
#define INTER_COUNT 100000
#define THREAD_COUNT 8

struct block_s
{
  uint32_t size;
  uint8_t *data;
  uint8_t hash[4];
  pthread_mutex_t lock;
};

struct block_s pool[POOL_SIZE] = { };

size_t errors = 0;

static void hash_set(const void *data, size_t len, uint8_t *hash)
{
  struct crypto_crc32_ctx_s crc;

  crypto_crc32_init(&crc);
  crypto_crc32_update(&crc, data, len);
  crypto_crc32_get(&crc, hash);
}

static void hash_check(const void *data, size_t len, const uint8_t *hash)
{
  struct crypto_crc32_ctx_s crc;
  uint8_t result[4];

  crypto_crc32_init(&crc);
  crypto_crc32_update(&crc, data, len);
  crypto_crc32_get(&crc, result);
  
  if (memcmp(result, hash, 4)) {
    printk("CRC ERROR at %p\n", data);
    errors++;
  }
}

void * thread(void *id_)
{
  size_t id = (size_t)id_;
  size_t i, ac = 0, rc = 0, fc = 0, nc = 0;

  printk("thread %i started\n", id);

  for (i = 0; i < INTER_COUNT; i++) {

    uint_fast16_t e = rand() % POOL_SIZE;
    struct block_s *b = pool + e;

    pthread_mutex_lock(&b->lock);

    switch (rand() % 3) {

    case 0:			/* switch */
      pthread_yield();
      break;

    case 1: {			/* free/malloc */

      if (b->data) {
	hash_check(b->data, b->size, b->hash);
	fc++;
	free(b->data);
      }

      size_t size = rand() % MAX_SIZE;
      void *data = malloc(size);
      if (data) {
	b->data = data;
	b->size = size;
	memset(data, rand(), size);
	hash_set(data, size, b->hash);
	ac++;
      } else {
	nc++;
      }

      break;
    }

    case 2: {			/* realloc */

      if (b->data)
	hash_check(b->data, b->size, b->hash);

      size_t size = rand() % MAX_SIZE;
      void *data = realloc(b->data, size);
      if (data || size == 0) {
	b->data = data;
	b->size = size;
	memset(data, rand(), size);
	hash_set(data, size, b->hash);
	rc++;
      } else {
	nc++;
      }

      break;
    }

    }

    pthread_mutex_unlock(&b->lock);
  }

  printk("cpu %i thread %i terminated: %i crc errors, %i alloc, %i free, %i realloc, %i alloc fail\n",
	 cpu_id(), id, errors, ac, fc, rc, nc);
  assert(!errors);

  return NULL;
}

pthread_t threads[THREAD_COUNT];

void app_start()
{
  size_t i;

  for (i = 0; i < POOL_SIZE; i++) {
    pthread_mutex_init(&pool[i].lock, NULL);
  }

  for (i = 0; i < THREAD_COUNT; i++)
    pthread_create(threads + i, NULL, thread, (void*)i);
}

