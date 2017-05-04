
#include <pthread.h>
#include <mutek/printk.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
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
  /* every processor execute the app_start function due to CONFIG_MUTEK_SMP_APP_START */

  switch (cpu_id() % 2)
    {
    case 0:
      pthread_create(&a, NULL, f, "Hello\n");
      break;

    case 2:
      pthread_create(&b, NULL, f, "World\n");
      break;
    }
}

