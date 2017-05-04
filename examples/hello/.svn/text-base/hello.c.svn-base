
#include <pthread.h>
#include <mutek/printk.h>

pthread_mutex_t m;
pthread_t a, b;

void *f(void *param)
{
  while (1)
    { 
      pthread_mutex_lock(&m);
      printk("(%s:%i) %s", cpu_type_name(), cpu_id(), param);
      pthread_mutex_unlock(&m);
      pthread_yield();
    }
}

void app_start()
{
  pthread_mutex_init(&m, NULL);
  pthread_create(&a, NULL, f, "Hello world\n");
  pthread_create(&b, NULL, f, "Hello world\n");
}

