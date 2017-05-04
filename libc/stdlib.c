
#include <stdlib.h>
#include <assert.h>
#include <mutek/printk.h>

void exit(uint_fast8_t status)
{
  printk("called exit() with %i. aborting ...", status);
  abort();
}

error_t atexit(void (*function)(void))
{
  /* FIXME */
  return -1;
}

void abort(void)
{
  cpu_trap();

  while (1)
    ;
}

void *bsearch(
  const void *key,
  const void *_base,
  size_t max,
  size_t width,
  __compiler_sint_t (*compar) (const void *, const void *)
  )
{
    const char *base = _base;
    size_t min = 0;
    size_t cur = (max + min) / 2;
        
    while ( cur < max ) {
        const void *ptr = base + cur * width;
        __compiler_sint_t cmp = compar(key, ptr);

        if ( cmp == 0 )
            return (void*)ptr;

        if ( cmp <= 0 )
            max = cur;
        else
            min = cur + 1;

        size_t tmp = (max + min) / 2;
        if ( cur == tmp )
            break;
        cur = tmp;
    }
    return NULL;
}

char *getenv(const char *key)
{
  return NULL;
}

error_t system(const char *cmd)
{
  return -1;
}
