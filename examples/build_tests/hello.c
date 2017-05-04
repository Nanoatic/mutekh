
#include <mutek/printk.h>

#if defined(CONFIG_PTHREAD)
#include <pthread.h>

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
  return NULL;
}
#endif

void app_start()
{
#if defined(CONFIG_PTHREAD)
  pthread_mutex_init(&m, NULL);
  pthread_create(&a, NULL, f, "Hello world\n");
  pthread_create(&b, NULL, f, "Hello world\n");
#endif
  printk("Hello, world!\n");
}

