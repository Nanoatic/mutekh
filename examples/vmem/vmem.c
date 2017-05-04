
#include <pthread.h>
#include <stdlib.h>

pthread_mutex_t m;
pthread_t a;

uint32_t size = 0x1000, i=0, total_size = 0;
uint32_t *addr = NULL;

void *f(void *param)
{
  while (1)
    { 
      pthread_mutex_lock(&m);
      printk("\nstart malloc of %X byte\n", size);
      addr = malloc(size);
      total_size += size;
      printk("end malloc of %x byte\n", size);
      printk("Total of %x byte allocate\n", total_size);
      if (addr)
	{
	  printk("Write 0x55 to %x\n", addr);
	  for( i=0; i++; i<size/4)
	    {
	      addr[i] = 0x55;
	    }
	  printk("end of writing\n", size);
	}
      else
	{
	  printk("ERRRRRRRRRRRRggggg!!!\n", addr);
	  exit(0);
	}
      //size*=2;

      pthread_mutex_unlock(&m);
      pthread_yield();
    }
}

uint32_t main()
{
  pthread_mutex_init(&m, NULL);
  printk("\nYOYO from main\n");
  pthread_create(&a, NULL, f, NULL);
  return 0;
}

